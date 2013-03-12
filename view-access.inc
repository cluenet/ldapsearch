<!-- BEGIN single/access -->
<?php
$More = ($View == "access");

const P_IMPORTANT	= 1;
const P_OBSOLETE	= 2;
const P_UNKNOWN	= 4;

# Privileges only shown in extended view
# 0: display in info
# 1: display in "Access" tab (default)
# 2: do not display
$AllPrivileges = array(
	// services that apply both to server ACLs and globally
	'atd@'			=> [0,		"use of <strong class=\"service\">'at'</strong> daemon"],
	'chfn@'			=> [P_OBSOLETE,	"use of <strong class=\"service\">'chfn'</strong> command",
						"This is mostly useless as the information is stored in LDAP,"
						." and 'chfn' does not know how to change it."],
	'chsh@'			=> [P_OBSOLETE,	"use of <strong class=\"service\">'chsh'</strong> command",
						"This is mostly useless as the information is stored in LDAP,"
						." and 'chsh' does not know how to change it."],
	'cron@'			=> [0,		"use of <strong class=\"service\">crontab</strong>"],
	'ftp@'			=> [0,		"access to <strong class=\"service\">FTP</strong> file transfer"],
	'login@'		=> [0,		"console and 'telnet' login"],
	'passwd@'		=> [P_OBSOLETE,	"use of <strong class=\"service\">'passwd'</strong> command",
						"This is mostly useless as passwords are changed directly"
						." using Kerberos."],
	'samba@'		=> [0,		"access to <strong class=\"service\">Samba</strong> file server"],
	'ssh@'			=> [P_OBSOLETE,	"<strong class=\"service\">SSH</strong> login, for Debian Etch"],
	'sshd@'			=> [0,		"<strong class=\"service\">SSH</strong> (shell) login"],
	// global services
	'bugs'			=> [P_OBSOLETE,	"Cluenet Bugzilla"],
	'cacti'			=> [P_OBSOLETE,	"Cacti – server stats"],
	'forum'			=> [P_OBSOLETE,	"Cluenet <a href=\"http://forum.cluenet.org/\">forum</a>"],
	'mail'			=> [P_OBSOLETE,	"Cluenet.org <strong class=\"service\">mail</strong>"],
	'vps'			=> [P_OBSOLETE,	"Cobi's <strong class=\"service\">VPS</strong> service"],
	'wiki'			=> [0,		"edit access on <strong class=\"service\">Cluewiki</strong>"],
	// acctshell v2 abilities
	'createacct'		=> [P_OBSOLETE,	"approval of new accounts – acctshell v2"],
	'sshvote'		=> [P_OBSOLETE,	"voting on shell applications – acctshell v2"],
	'sshapprove'		=> [P_OBSOLETE,	"approval of shell applications – acctshell v2"],
	'sshvoteoverride'	=> [P_OBSOLETE,	"overriding shell application votes – acctshell v2"],
	// acctshell v3 abilities
	'initialvouch'		=> [P_OBSOLETE,	"vouching for new accounts – acctshell v3"],
	'acctvote'		=> [P_OBSOLETE,	"voting for new account creation – acctshell v3"],
	// acctshell v4 abilities -- accounts
	'acct^initvote'		=> [P_OBSOLETE,	"voting for new account creation – acctshell v4"],
	'acct^acctvote'		=> [P_OBSOLETE,	"voting on access applications – acctshell v4"],
	// acctshell v4 abilities -- misc
	'dns^'			=> [P_OBSOLETE,	"control of <code>{}</code> <strong class=\"service\">DNS domain</strong> – acctshell v4"],
	'tokengen^'		=> [P_OBSOLETE,	"access token generation for <code>{}</code> – acctshell v4"],
	'vpnpanel^'		=> [P_OBSOLETE,	"<strong class=\"service\">ClueVPN BNL</strong> entry for <code>{}</code> – acctshell v4"],
);

function describe_service($service) {
	global $AllPrivileges;

	/* SERVICE	DESCRIBED BY
	 *
	 * foo@bar	foo@bar, foo@
	 * foo^bar	foo^bar, foo^
	 * foo		foo, foo@
	 */
	
	$p = strrpos($service, "@");
	if ($p !== false) {
		$host = substr($service, $p);
		$service = substr($service, 0, $p);
	} else {
		$host = "";
	}

	$svc_type = strtok($service, "^")."^";
	$svc_value = strtok(null);

	if (strlen($host)) {
		if (isset($AllPrivileges["$service@$host"]))
			$descr = $AllPrivileges["$service@$host"];
		elseif (isset($AllPrivileges["$service@"]))
			$descr = $AllPrivileges["$service@"];
		else
			$descr = null;
	} else {
		if (isset($AllPrivileges["$service"]))
			$descr = $AllPrivileges["$service"];
		elseif (isset($AllPrivileges["$svc_type"]))
			$descr = $AllPrivileges["$svc_type"];
		elseif (isset($AllPrivileges["$service@"]))
			$descr = $AllPrivileges["$service@"];
		elseif (isset($AllPrivileges["$svc_type@"]))
			$descr = $AllPrivileges["$svc_type@"];
		else
			$descr = null;
	}

	if ($descr) {
		@list ($level, $name, $title) = $descr;
	} else {
		$level = P_UNKNOWN;
		$name = "<i>(unknown service)</i> <strong class=\"service\">$service</strong>";
		$title = null;
	}

	if (strlen($svc_value))
		$name = str_replace('{}', htmlspecialchars($svc_value), $name);

	return [$level, $name, $title];
}

function find_acls($user_dn) {
	global $ldapConn;
	$search = ldap_search($ldapConn, "ou=servers,dc=cluenet,dc=org",
		"(&(objectClass=groupOfNames)(member=$user_dn))", array());

	$acls = array();
	for ($entry = ldap_first_entry($ldapConn, $search);
		$entry;
		$entry = ldap_next_entry($ldapConn, $entry)) {

		# Whoever decided that $with_attrib==0 be taken as
		# "yes, I want attributes" should be shot.
		$dn = ldap_explode_dn(ldap_get_dn($ldapConn, $entry), true);
		if ($dn["count"] != 6) continue; # sanity check; failed once

		$acls[trim_domain($dn[2])][] = $dn[0];
	}
	ldap_free_result($search);
	return $acls;
}

// retrieve and sort server ACLs
$acl_servers = find_acls($Entry["dn"]);
ksort($acl_servers);

// non-server
$acl_misc = array();
if (has("authorizedservice"))
	$acl_misc = array_merge($acl_misc, $Entry["authorizedservice"]);
if (has("clueauthorizedability"))
	$acl_misc = array_merge($acl_misc, $Entry["clueauthorizedability"]);
sort($acl_misc);

// merge common "shell access" entries
$acl_shell = array();
$acl_server_misc = array();
$services_shell = array("atd", "cron", "login", "passwd", "sshd", "su", "sudo");
foreach ($acl_servers as $host => &$services) {
	sort($services);
	if (array_intersect($services_shell, $services) === $services_shell) {
		$acl_shell[] = $host;
		$acl_server_misc[$host] = array_diff($services, $services_shell);
	}
}

if (has("host"))
	$acl_host = $Entry["host"];
else
	$acl_host = array();
sort($acl_host);

//// prepare everything for display
$Access = array();
$AccessHidden = 0;

$AccessUnknown = array();
$AccessObsolete = array();

$visibleGroups = array();
foreach ($Groups as $dn) {
	if (preg_match("/,cn=svcAccess,cn=[^,]*,ou=servers,/", $dn))
		continue;
	else
		$visibleGroups[] = $dn;
}

if (!$More)
	$AccessHidden += count($visibleGroups);

// host-based access

if (count($acl_host)) {
	$tmp_local = array();
	$tmp_other = array();
	foreach ($acl_host as $host) {
		$short = trim_domain($host);
		if ($host === $short)
			$tmp_other[] = $host;
		else
			$tmp_local[] = $short;
	}
	sort($tmp_local);
	sort($tmp_other);
	$acl_host = array_merge($tmp_local, $tmp_other);
	$hosts_s = implode_e(strprefix_a($acl_host, "<code class=\"host\"><u>", "</u></code>"));
	if ($More) {
		$Access[] = ["<strong class=\"service\">host access</strong> to $hosts_s",
			     "\"host\" attribute",
			     "Granted by the \"host\" attribute on the user's account."];
	} else {
		$Access[] = ["host access to $hosts_s"];
	}
}

// per-server summarized shell access

if (count($acl_shell)) {
	$hosts_s = implode_e(strprefix_a($acl_shell, "<code class=\"host\"><u>", "</u></code>"));
	$services_s = implode(", ", $services_shell);
	if ($More) {
		$Access[] = ["<strong class=\"service\">shell access</strong> on $hosts_s",
			     "ACL entries",
			     "Granted by multiple ACL entries [$services_s] on the server's ACL subtree."];
	} else {
		$Access[] = ["shell access on $hosts_s"];
	}
}

// old-style shell access

if (!$More && in_array("sshd", $acl_misc)) {
	$Access[] = ["shell access on servers which use 'authorizedService'",
		     "<code>\"sshd\"</code> service",
		     "Granted by the \"authorizedService\" attribute on the user's account. Only some servers use this attribute."];
}

// per-server non-"shell" services
// also "shell" services when the "shell access" set is incomplete

if (count($acl_server_misc)) {
	foreach ($acl_server_misc as $host => $services) {
		if ($More && count($services)) {
			foreach ($services as $service) {
				list ($level, $name, $title) = describe_service("$service@$host");
				$text = "$name on <code class=\"host\"><u>$host</u></code>";
				if (strlen($title))
					$text = "<span title=\"".H($title)."\">".$text."</span>";
				$entry = [$text,
					  "<code>\"$service\"</code>",
					  "Granted by ACL entry \"$service\" in the server's ACL subtree."];
				if ($level & P_UNKNOWN)
					$AccessUnknown[] = $entry;
				else
					$Access[] = $entry;
			}
		} else $AccessHidden += count($services) ? 1 : 0;
	}
}

// global privileges

foreach ($acl_misc as $service) {
	list ($level, $name, $title) = describe_service($service);

	if (@in_array($service, $Entry["clueauthorizedability"]))
		$attr = "clueAuthorizedAbility";
	else
		$attr = "authorizedService";

	$note = "<code>\"$service\"</code>";
	$comment = "Granted by \"$service\" in the \"$attr\" attribute on the user's account.";

	if ($level & P_IMPORTANT || $More) {
		if ($service == "mail" and isset($Entry["mailquota"])) {
				$name = "$name <em>(quota: ".formatQuota($Entry["mailquota"]).")</em>";
		}

		if ($level & P_OBSOLETE)
			$name = "<small>(historical)</small> ".$name;

		if (strlen($title))
			$name = "<span title=\"".H($title)."\">".$name."</span>";

		if ($level & P_OBSOLETE)
			$AccessObsolete[] = [$name, $note, $comment];
		elseif ($level & P_UNKNOWN)
			$AccessUnknown[] = [$name, $note, $comment];
		else
			$Access[] = [$name, $note, $comment];
	} else {
		$AccessHidden++;
	}
}

foreach ($AccessObsolete as $item)
	$Access[] = $item;
if ($More)
	foreach ($AccessUnknown as $item)
		$Access[] = $item;

if (count($Access) || $More) {
	print "<div class=\"box content\">\n";
	print "<h2>Access rights</h2>\n";
	if (count($Access)) {
		print "<ul>\n";
		foreach ($Access as $item) {
			@list ($text, $note, $comment) = $item;
			$line = $text;
			if ($More && strlen($note)) {
				$span = "span class=\"comment\"";
				if (strlen($comment))
					$span .= " title=\"".H($comment)."\"";
				$line .= " <$span>(".$note.")</span>";
			}
			print "\t<li>$line</li>\n";
		}
		print "</ul>\n";
	} else {
		print "<p>Nothing to display.</p>\n";
	}
	if ($AccessHidden) {
		if ($More)
			$url = mangle_query(array("more" => null));
		else
			$url = mangle_query(array("view" => "access"));
		print "<p class=\"more\"><a href=\"?".H($url)."\">details – ".plural_s($AccessHidden, "entry")." not shown</a></p>\n";
	}
	print "</div>\n";
}

if ($More) {
	echo "<div class=\"box content\">\n";
	echo "<h3>Group memberships</h3>\n";
	if (count($visibleGroups) > 0) {
		echo "<ul>\n";
		foreach ($visibleGroups as $dn)
			echo "\t<li class=\"pre\">$dn</li>\n";
		echo "</ul>\n";
	} else {
		echo "<p>No groups.</p>\n";
	}
	$numHiddenGroups = count($Groups) - count($visibleGroups);
	if ($numHiddenGroups > 0) {
		echo "<p class=\"comment\">".plural_s($numHiddenGroups, "other group")." are already summarized above.</p>";
	}
	echo "</div>\n";
}

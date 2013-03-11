<!-- BEGIN single/access -->
<?php
$More = ($View == "access");

const PRIV_IMPORTANT	= 1;
const PRIV_OBSOLETE	= 2;
const PRIV_UNKNOWN	= 4;

# Privileges only shown in extended view
# 0: display in info
# 1: display in "Access" tab (default)
# 2: do not display
$AllPrivileges = array(
	'ssh'			=> [PRIV_OBSOLETE,	"<strong class=\"service\">SSH</strong> login (Debian Etch)"],
	'sshd'			=> [0,			"<strong class=\"service\">SSH</strong> login"],
	'login'			=> [0,			"console and 'telnet' login"],
	'cron'			=> [2,			"use of <strong class=\"service\">crontab</strong>"],
	'atd'			=> [0,			"use of <strong class=\"service\">'at'</strong> daemon"],
	'chfn'			=> [PRIV_OBSOLETE,	"use of <strong class=\"service\">'chfn'</strong> command"],
	'chsh'			=> [PRIV_OBSOLETE,	"use of <strong class=\"service\">'chsh'</strong> command"],

	'mail'			=> [PRIV_OBSOLETE,	"Cluenet.org <strong class=\"service\">mail</strong>"],
	'wiki'			=> [0,			"edit access on <strong class=\"service\">Cluewiki</strong>"],
	'forum'			=> [PRIV_OBSOLETE,	"Cluenet <a href=\"http://forum.cluenet.org/\">forum</a>"],
	'bugs'			=> [PRIV_OBSOLETE,	"Cluenet Bugzilla"],
	'cacti'			=> [PRIV_OBSOLETE,	"Cacti – server stats"],
	'vps'			=> [PRIV_OBSOLETE,	"Cobi's <strong class=\"service\">VPS</strong> service"],

	'createacct'		=> [PRIV_OBSOLETE,	"approval of new accounts (acctshell v2)"],
	'sshvote'		=> [PRIV_OBSOLETE,	"voting on shell applications – acctshell v2"],
	'sshapprove'		=> [PRIV_OBSOLETE,	"approval of shell applications – acctshell v2"],
	'sshvoteoverride'	=> [PRIV_OBSOLETE,	"overriding shell application votes – acctshell v2"],

	'initialvouch'		=> [PRIV_OBSOLETE,	"vouching for new accounts – acctshell v3"],
	'acctvote'		=> [PRIV_OBSOLETE,	"voting for new account creation – acctshell v3"],

	'acct^initvote'		=> [PRIV_OBSOLETE,	"voting for new account creation – acctshell v4"],
	'acct^acctvote'		=> [PRIV_OBSOLETE,	"voting on access applications – acctshell v4"],

	'dns^'			=> [PRIV_OBSOLETE,	"control of <code>{}</code> <strong class=\"service\">DNS domain</strong> – acctshell v4"],
	'tokengen^'		=> [PRIV_OBSOLETE,	"access token generation for <code>{}</code> – acctshell v4"],
	'vpnpanel^'		=> [PRIV_OBSOLETE,	"<strong class=\"service\">ClueVPN BNL</strong> entry for <code>{}</code> – acctshell v4"],
);

function describe_service($service) {
	global $AllPrivileges;
	// split type^value ("acct^acctvote") for labelling purposes
	$svc_type = strtok($service, "^")."^";
	$svc_value = strtok(null);

	// get importance level and display name
	// for type^value, fall back to just type
	if (isset($AllPrivileges[$service])) {
		list ($level, $name) = $AllPrivileges[$service];
		if ($name === null and isset($AllPrivileges[$svc_type])) {
			$name = $AllPrivileges[$svc_type][1];
		}
	} elseif (isset($AllPrivileges[$svc_type])) {
		list ($level, $name) = $AllPrivileges[$svc_type];
	} else {
		$level = PRIV_UNKNOWN;
		$name = "(unknown service) <strong class=\"service\">$service</strong>";
	}

	$name = str_replace('{}', htmlspecialchars($svc_value), $name);

	return [$level, $name];
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
				list ($level, $name) = describe_service($service);
				$Access[] = ["$name on <code class=\"host\"><u>$host</u></code>",
					     "<code>\"$service\"</code>",
					     "Granted by ACL entry \"$service\" in the server's ACL subtree."];
			}
			//$services_s = implode(", ", $services);
			//$services_hs = implode_e(strprefix_a($services, "<code>\"", "\"</code>"));
			//$Access[] = [plural($services, "unknown service")." $services_hs on <i>$host</i>",
			//	     "ACL entries",
			//	     "Granted by ACL ".plural($services, "entry")." [$services_s] in the server's ACL subtree."];
		} else $AccessHidden += count($services) ? 1 : 0;
	}
}

// global privileges

$AccessUnknown = array();
$AccessObsolete = array();

foreach ($acl_misc as $service) {
	list ($level, $name) = describe_service($service);

	if (@in_array($service, $Entry["clueauthorizedability"]))
		$attr = "clue­Authorized­Ability";
	else
		$attr = "authorized­Service";

	$note = "<code>\"$service\"</code>";
	$comment = "Service \"$service\" in\nthe \"$attr\" attribute\non the user's account.";

	// display entries based on their importance
	if ($level & PRIV_IMPORTANT || $More) {
		if ($service == "mail" and isset($Entry["mailquota"])) {
				$name = "$name <em>(quota: ".formatQuota($Entry["mailquota"]).")</em>";
		}

		if ($level & PRIV_OBSOLETE) {
			$name = "<small>(historical)</small> ".$name;
			$AccessObsolete[] = [$name, $note, $comment];
			continue;
		}
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

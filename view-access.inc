<!-- BEGIN single/access -->
<?php
$More = ($View == "access");
$EvenMore = $More && isset($_GET["more"]);

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

$acl_servers = find_acls($Entry["dn"]);
ksort($acl_servers);

$acl_misc = array();
if (has("authorizedservice"))
	$acl_misc = array_merge($acl_misc, $Entry["authorizedservice"]);
if (has("clueauthorizedability"))
	$acl_misc = array_merge($acl_misc, $Entry["clueauthorizedability"]);
sort($acl_misc);

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

$Access = array();

if (count($acl_shell)) {
	$Access[] = "<abbr title=\"".implode(", ", $services_shell)."\">shell access</abbr>"
		." on ".implode_e(array_map(function($x) {return "<i>$x</i>";}, $acl_shell));
}

if ($More and in_array("sshd", $acl_misc)) {
	$Access[] = PRIV_DEPRECATED."shell access on servers still using <code>authorizedService</code>";
}

if ($More and count($acl_server_misc)) {
	foreach ($acl_server_misc as $host => $services) {
		if (count($services))
			$Access[] = implode_e($services)." on <i>$host</i>";
	}
}

const PRIV_SHOW_ALWAYS		= 0;
const PRIV_SHOW_TAB			= 1;
const PRIV_SHOW_DETAILED	= 2;
const PRIV_UNKNOWN			= 3;

$AccessUnknown = array();
$AccessHidden = 0;

foreach ($acl_misc as $service) {
	$svc_type = strtok($service, "^")."^";
	$svc_value = strtok(null);

	if (isset($AllPrivileges[$service])) {
		list ($level, $name) = $AllPrivileges[$service];
		if ($name === null and isset($AllPrivileges[$svc_type])) {
			$name = $AllPrivileges[$svc_type];
		}
	} elseif (isset($AllPrivileges[$svc_type])) {
		list ($level, $name) = $AllPrivileges[$svc_type];
	} else {
		$level = PRIV_UNKNOWN;
	}

	if ($level == PRIV_UNKNOWN) {
		# unknown; show only in extended view of 'access'
		if ($EvenMore)
			$AccessUnknown[] = $service;
		else
			$AccessHidden++;
	}
	elseif ($level >= PRIV_SHOW_DETAILED and !$EvenMore) {
		# show only in extended view of 'access'
		if ($More) $AccessHidden++;
	}
	elseif ($level >= PRIV_SHOW_TAB and !$More) {
		# show only in 'access' tab
		$AccessHidden++;
	} else {

	switch ($service) {
	case 'mail':
		# show quota
		if ($More and isset($Entry["mailquota"]))
			$name = "$name <em>(quota: ".formatQuota($Entry["mailquota"]).")</em>";
		break;
	default:
		$name = str_replace('{}', htmlspecialchars($svc_value), $name);
	}

	if ($More)
		$name .= " <i>(<code>$service</code>)</i>";

	$Access[] = $name;
	}
}

foreach ($AccessUnknown as $service) {
	$Access[] = "<em>Unknown service: <code>$service</code></em>";
}

if (count($Access)) {
	print "<div class=\"box content\">\n";
	print "<h2>Access rights</h2>\n";
	print "<ul>\n";
	foreach ($Access as $line)
		print "\t<li>$line</li>\n";
	print "</ul>\n";
	if ($AccessHidden) {
		if ($More) {
			$url = mangle_query(array("more" => null));
		} else {
			$url = mangle_query(array("view" => "access"));
		}
		print "<p><small><a href=\"".H($url)."\">$AccessHidden entries not shown</small></p>\n";
	}
	print "</div>\n";
}


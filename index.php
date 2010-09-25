<?php
# (c) 2009 grawity <grawity@gmail.com>
# This software licensed under...
$LICENSE = <<<EOF

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2004 Sam Hocevar
  14 rue de Plaisance, 75014 Paris, France
 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.

  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

The source code of this application is available at:
    git://github.com/grawity/cluenet-ldapsearch.git
    http://github.com/grawity/cluenet-ldapsearch

EOF;

error_reporting(E_ALL);
ini_set('display_errors', 1);
date_default_timezone_set("UTC");
@include "config.inc";
$Errors = array();
$SysErrors = array();

# Display full information? (false for 'info' view)
$Moar = false;

# Display module to show in single result view
switch (!empty($_GET["view"]) ? $_GET["view"] : "simple") {
	case "raw":
	case "ldif":
		$View = "ldif";
		break;

	case "sshkey":
	case "sshkeys":
		$View = "sshkeys";
		break;
	
	case "access":
	case "account":
	case "person":
	case "contact":
		$View = $_GET["view"];
		break;
	
	case "server":
	case "servers":
	case "serveraccess":
		$View = "servaccess";
		break;

	# 'info' displays basic information from all other modules
	case "simple":
		$View = "info";
		break;
	
	default:
		$View = $_GET["view"];
}

$Stylesheets = array(
	# zeroth entry is the default
	'simple',
	'pretty',
	);

@include "functions.inc";

$Style = $Stylesheets[0];
if (isset($_GET["style"])) {
	if (in_array($_GET["style"], $Stylesheets)) {
		$Style = $_GET["style"];
		setcookie("style", $Style, strtotime("+1 year"));
	}
	else {
		setcookie("style", null, 0);
	}
	header("Location: "
		.parse_url($_SERVER["REQUEST_URI"], PHP_URL_PATH)
		."?".mangle_query(null, array("style")));
}
elseif (isset($_COOKIE["style"])) {
	if (in_array($_COOKIE["style"], $Stylesheets)) {
		$Style = $_COOKIE["style"];
	}
	else {
		setcookie("style", null, 0);
	}
}

// Construct LDAP filter from query...
if (isset($_GET["q"])) {
	$Query = trim($_GET["q"]);
	// user entered LDAP filter
	if (!empty($Query) and $Query[0] == "(") {
		$Filter = $Query;
	}
	// googley 'key:value' query
	elseif (strpos($Query, ':') !== false) {
		list ($prefix, $q) = explode(':', $Query, 2);
		switch (strtolower($prefix)) {
		case 'irc':
			$Filter = "(clueIrcNick={$q})"; break;
		case 'xmpp':
		case 'jabber':
		case 'gtalk':
			$Filter = "(xmppUri={$q})"; break;
		case 'msn':
		case 'msnim':
		case 'live':
		case 'liveim':
		case 'wlm':
			$Filter = "(msnSn={$q})"; break;
		case 'aim':
			$Filter = "(aimSn={$q})"; break;
		case 'pgp':
		case 'pgpkey':
		case 'gpg':
		case 'gpgkey':
			$Filter = "(pgpKeyId={$q})"; break;
		default:
			$Filter = "({$prefix}=${q})"; break;
		}
	}
	// username
	else {
		$Filter = "(uid={$Query})";
	}
}
// ...or show the welcome page.
else {
	$Query = false;
}

if ($Query) {
	$NumResults = -1;
	$ldapConn = ldap_connect_and_do_things();
	if ($ldapConn) {
		$lsearch = @ldap_list($ldapConn, "ou=people,dc=cluenet,dc=org", $Filter, Array(), null, 0);
		if ($lsearch === false) {
			$SysErrors[] = "LDAP query failed.";
			$NumResults = 0;
		} else {
			$NumResults = ldap_count_entries($ldapConn, $lsearch);
		}
	}
} else {
	$NumResults = -1;
}

switch ($NumResults) {
case -1:
	require "welcome.inc";
	break;
case 0:
	require "results-none.inc";
	break;
case 1:
	require "results-single.inc";
	break;
default:
	require "results-list.inc";
	break;
}

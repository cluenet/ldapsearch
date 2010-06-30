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

error_reporting(E_ALL | E_NOTICE);
ini_set('display_errors', 1);
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

$Style = $Stylesheets[0];
if (isset($_GET["style"])) {
	if (in_array($_GET["style"], $Stylesheets)) {
		$Style = $_GET["style"];
		setcookie("style", $Style, strtotime("+1 year"));
	}
	else {
		setcookie("style", null, 0);
	}

	parse_str($_SERVER["QUERY_STRING"], $request);
	unset($request["style"]);
	$request = http_build_query($request);
	header("Location: {$request}");
	unset($request);
}
elseif (isset($_COOKIE["style"])) {
	if (in_array($_COOKIE["style"], $Stylesheets)) {
		$Style = $_COOKIE["style"];
	}
	else {
		setcookie("style", null, 0);
	}
}

@include "functions.inc";

// Construct LDAP filter from query...
if (isset($_GET["q"])) {
	$Query = trim($_GET["q"]);
	// has filter!
	if (!empty($Query) and $Query[0] == "(") {
		$Filter = $Query;
	}
	// has googley query!
	elseif (strpos($Query, ':') !== false) {
		$prefix = substr($Query, 0, strpos($Query, ':'));
		$q = trim(substr($Query, strpos($Query, ':')+1));
		switch (rtrim(strtolower($prefix), ':')) {
		case 'irc':
			$Filter = "(clueIrcNick=".$q.")"; break;
		case 'xmpp':
		case 'jabber':
		case 'gtalk':
			$Filter = "(xmppUri=".$q.")"; break;
		case 'msn':
		case 'msnim':
		case 'live':
		case 'liveim':
		case 'wlm':
			$Filter = "(msnSn=".$q.")"; break;
		case 'aim':
		case 'aol':
		case 'aolim':
			$Filter = "(aimSn=".$q.")"; break;
		case 'pgp':
		case 'pgpkey':
		case 'gpg':
		case 'gpgkey':
			$Filter = "(pgpKeyId=".$q.")"; break;
		default:
			$Filter = "(".$prefix."=".$q.")"; break;
		}
	}
	// has username!
	else {
		$Filter = "(uid=".$Query.")";
	}
}
// ...or show the welcome page.
else {
	$Query = false;
}

do if ($Query) {
	$NumResults = -1;
	// and get results
	$ldapConn = ldap_connect_and_do_things();
	if ($ldapConn) {
		$ldapSH = @ldap_search($ldapConn, BASE_DN, $Filter, Array(), null, 0);
		if ($ldapSH === false) {
			$SysErrors[] = "LDAP query failed.";
			$NumResults = 0;
			break;
		}
		$NumResults = ldap_count_entries($ldapConn, $ldapSH);
		$Entry = ldap_get_entries($ldapConn, $ldapSH);
		ldap_unbind($ldapConn);
	}
	
	/*
	for (
		$E = ldap_first_entry($ldapConn, $ldapSH);
		$A = ldap_get_attributes($ldapConn, $E);
		$E = ldap_next_entry($ldapConn, $E)
	) {
		$Photo[$
	*/
	/* if ($NumResults == 1) {
		$E = ldap_first_entry($ldapConn, $ldapSH);
		$Photo = ldap_get_values_len($ldapConn, $E, "jpegphoto");
		$Photo = $Photo[0];
	} */
}
else {
	$NumResults = -1;
} while (false);

switch ($NumResults) {
	case -1:
		$Title = "";
		break;
	case 0:
		$Title = "Nothing found";
		break;
	case 1:
		$Title = htmlspecialchars($Entry[0]["uid"][0]);
		break;
	default:
		$Title = $NumResults." results";
}
if ($Title != "") $Title .= " - ";

// THE (XHTML) TIME IS NOW ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<?php echo "<!--\n{$LICENSE}\n-->\n"; ?>
<head>
	<meta http-equiv="Content-Type" content="application/xhtml+xml; charset=utf-8" />
	<title><?php echo $Title; ?>ClueLDAP</title>
	<meta http-equiv="X-UA-Compatible" content="IE=8" />
	<meta name="author" content="Mantas Mikulėnas" />
	<style type="text/css">
	pre, code {
		font-family: Consolas, "DejaVu Mono", monospace;
	}

	.value.important {
		font-weight: bold;
	}
	.comment {
		font-style: italic;
	}
	.value.error {
		color: red;
	}
	.value.empty {
		font-style: italic;
	}
	</style>
<?php foreach ($Stylesheets as $s):
	$rel = ($s == $Style)? "stylesheet" : "alternate stylesheet";
	echo "\t <link rel=\"{$rel}\" href=\"{$s}.css\" title=\"{$s}\" />\n";
endforeach; ?>
</head>
<body>

<?php if ($NumResults > -1): ?>
<!-- SEARCH BAR -->
<div class="box">
<form id="searchbar" method="get" action="<?php echo $_SERVER["PHP_SELF"]; ?>">
	<div>
		<input type="text" name="q" id="q" value="<?php echo $Query? htmlspecialchars($Query) : ""; ?>" />
		<input type="submit" value="search &gt;&gt;" />
	</div>
</form>

<?php if (isset($Auth) and $Auth): ?>
<p style="font-size: small; font-style: italic;">Logged in as <strong><?php echo substr($Auth, 0, strpos($Auth, "@")); ?></strong></p>
<?php endif; ?>
</div>
<?php endif; ?>


<?php if (count($SysErrors) > 0): ?>
<div class="box">
<h2 class="error">Bad things happened</h2>
<ul>
<?php foreach ($SysErrors as $msg) echo "\t<li>{$msg}</li>\n"; ?>
</ul>
</div>
<?php endif; ?>

<?php if (isset($_GET["chewy"])): ?>
<div class="box">
<pre>
$View = <?php var_dump($View); ?>
$Auth = <?php var_dump($Auth); ?>

C:\&gt;<blink>_</blink>
</pre>
</div>
<?php endif; ?>

<?php
if ($NumResults == -1) {
	include "welcome.inc";
}

elseif ($NumResults == 0) {
?>
<div class="box">
<h1>403½ Not Found</h1>
<p>Sorry, try again.</p>
</div>
<?php
}

elseif ($NumResults == 1) {
	include "results-single.inc";
}

else {
	include "results-list.inc";
}
?>

<p class="footer"><?php
$nextstyle = array_search($Style, $Stylesheets, true);
if ($nextstyle === false)
	$nextstyle = 1;
else
	$nextstyle++;

parse_str($_SERVER["QUERY_STRING"], $request);
$request["style"] = $Stylesheets[$nextstyle % count($Stylesheets)];
$request = http_build_query($request);
?>
<a href="?<?php echo htmlspecialchars($request) ?>" style="color: black">a small clicky link</a> | grawity, 2009</p>

</body>
</html>

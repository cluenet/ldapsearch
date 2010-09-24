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

	$request = mangle_query(null, array('style'));
	header("Location: /?{$request}");
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
	// and get results
	$ldapConn = ldap_connect_and_do_things();
	if ($ldapConn) {
		$ldapSH = @ldap_list($ldapConn, "ou=people,dc=cluenet,dc=org", $Filter, Array(), null, 0);
		if ($ldapSH === false) {
			$SysErrors[] = "LDAP query failed.";
			$NumResults = 0;
			break;
		}
		$NumResults = ldap_count_entries($ldapConn, $ldapSH);
		$Entry = ldap_get_entries($ldapConn, $ldapSH);
	}
} else {
	$NumResults = -1;
}

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

?>
<!DOCTYPE html>
<html>
<?php echo "<!--\n{$LICENSE}\n-->\n"; ?>
<head>
	<meta http-equiv="Content-Type" content="text/plain; charset=utf-8">
	<title><?php echo $Title; ?>ClueLDAP</title>
	<meta name="author" content="Mantas Mikulėnas">
	<style>
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
	echo "\t <link rel=\"{$rel}\" href=\"{$s}.css\" title=\"{$s}\">\n";
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
<h1>Not found.</h1>
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

<p class="footer"><a href="http://github.com/cluenet/ldapsearch">source</a> | <?php
$nextstyle = array_search($Style, $Stylesheets, true);
$nextstyle++;

$request = mangle_query(array(
	'style' => $Stylesheets[$nextstyle % count($Stylesheets)],
	));
?>
<a href="?<?php echo htmlspecialchars($request) ?>">switch style</a> | <a href="http://www.cluenet.org/">Cluenet</a></p>

</body>
</html>

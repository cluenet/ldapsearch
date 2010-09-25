<?php
$res = ldap_first_entry($ldapConn, $lsearch);
$Entry = ldap_get_attributes($ldapConn, $res);
// do some preparations on the result
$Entry = clean_result($Entry);
#$Entry = htmlize($Entry);
$Entry["dn"] = ldap_get_dn($ldapConn, $res);

// retrieve operational attributes
$lsearch = @ldap_read($ldapConn, $Entry["dn"], "objectClass=*",
	array("createTimestamp"), null, 0);
$res = ldap_first_entry($ldapConn, $lsearch);
$entry_op = ldap_get_attributes($ldapConn, $res);
$Entry["createTimestamp"] = ldif_parse_time($entry_op["createTimestamp"][0]);

// build an array of privileges
$Privileges = array();
if (isset($Entry['authorizedservice']))
	foreach ($Entry['authorizedservice'] as $k => $v)
		$Privileges[$v] = true;
if (isset($Entry['clueauthorizedability']))
	foreach ($Entry['clueauthorizedability'] as $k => $v)
		$Privileges[$v] = true;
ksort($Privileges);

$HasShell = isset($Privileges['ssh']) or isset($Privileges['sshd']);

$IsSuspended = array_search('suspendedUser', $Entry['objectclass']) !== false;

///// Check for errors in LDAP entry
$Errors = array();
$Invalid = array();

$Name = ($Entry["cn"] != $Entry["uid"]) ? $Entry["cn"] : false;
$Entry['uidnumber'] = intval($Entry['uidnumber']);
$Entry['gidnumber'] = intval($Entry['gidnumber']);

if (!has('krb5principalname'))
	$Errors[] = "is missing a Kerberos principal (<code>krb5PrincipalName</code>)";

if ($HasShell and !has('clueircnick'))
	$Errors[] = "has SSH, but didn't set a clueIrcNick";

if (has('pgpkeyid') and !preg_match("/^(0x)?([0-9a-z]{8}){1,2}$/i", $Entry['pgpkeyid']))
	$Errors[] = "incorrect PGP key ID format";

if (has('openid') and
	!(substr($Entry['openid'], 0, 7) == "http://"
	or substr($Entry['openid'], 0, 8) == "https://")) {
	$Errors[] = "invalid OpenID URL - must start with <code>http://</code> or <code>https://</code>";
	$Invalid["openid"] = true;
}

$Title = H($Entry["uid"]);
require "html-header.inc";
?>

<!-- page header - name, uid, picture -->
<div id="title">
<span style="float: left; margin-right: 5px">
<a href="getphoto.php?uid=<?php echo H($Entry["uid"]) ?>" style="border: none">
	<img src="getphoto.php?uid=<?php echo H($Entry["uid"]) ?>&amp;s=56"
		alt="Photo of <?php echo H($Entry["uid"]) ?>"
		style="width: 56px; height: 56px; border: none">
</a>
</span>

<?php if ($Name) {
	echo "<h1>".H($Name)."</h1>\n";
	if (strtolower($Name) != strtolower($Entry["uid"]))
		echo "<div id=\"realname\">".H($Entry["uid"])."</div>\n";
} else {
	echo "<h1>".H($Entry["uid"])."</h1>\n";
} ?>
</div>
<div style="clear: both;"></div>

<!-- view bar -->
<div class="box navigation">
<p>
View:
<?php
echo condLink($View == "info", "info", "?q=".urlencode($Query))."\n";
echo "| ".condLink($View == "account", "account", '?q='.urlencode($Query)."&view=account")."\n";
echo "| ".condLink($View == "person", "personal", '?q='.urlencode($Query)."&view=person")."\n";
echo "| ".condLink($View == "contact", "contact", '?q='.urlencode($Query)."&view=contact")."\n";
echo "| ".condLink($View == "access", "access", '?q='.urlencode($Query)."&view=access")."\n";
if (has('cluesshpubkey'))
	echo "| ".condLink($View == "sshkeys", "SSH keys", '?q='.urlencode($Query)."&view=sshkeys")
		." (".count($Entry['cluesshpubkey']).")\n";
echo "| ".condLink($View == "ldif", "raw entry", '?q='.urlencode($Query)."&view=ldif");
?>
</p>
</div>

<?php if (count($Errors) > 0): ?>
<!-- error box -->
<div class="box content">
<h2 class="error">Errors, warnings and other unimportant stuff</h2>
<ul>
<?php foreach ($Errors as $msg)
	echo "<li>".$msg."</li>\n"; ?>
</ul>
</div>
<?php endif; ?>

<?php
switch ($View) {
case "info":
case "account":
case "person":
case "contact":
case "access":
case "serveraccess":
case "ldif":
case "sshkeys":
	require "view-{$View}.inc";
	break;
}

require "html-footer.inc";
?>

<?php
$res = ldap_first_entry($ldapConn, $lsearch);
$Entry = ldap_get_attributes($ldapConn, $res);
// do some preparations on the result
$Entry = clean_result($Entry);
#$Entry = htmlize($Entry);
$Entry["dn"] = ldap_get_dn($ldapConn, $res);

if (isset($Entry["clueircnick"]))
	$Entry["cluepoints"] = getCluePoints($Entry["clueircnick"]);

// retrieve operational attributes
$lsearch = @ldap_read($ldapConn, $Entry["dn"], "objectClass=*",
	array("createTimestamp", "authzTo"), null, 0);
$res = ldap_first_entry($ldapConn, $lsearch);
$entry_op = ldap_get_attributes($ldapConn, $res);
$Entry["authzTo"] = @$entry_op["authzTo"];
$Entry["createTimestamp"] = ldif_parse_time($entry_op["createTimestamp"][0]);

$IsSuspended = array_search('suspendedUser', $Entry['objectclass']) !== false;

$Groups = array();
$filter = "(|".
	"(&(objectClass=groupOfNames)(member={$Entry["dn"]}))".
	"(&(objectClass=groupOfUniqueNames)(uniqueMember={$Entry["dn"]}))".
	")";
$lsearch = ldap_search($ldapConn, "dc=cluenet,dc=org",
	$filter, array("dn"), null, 0);
for ($entry = ldap_first_entry($ldapConn, $lsearch);
     $entry !== false;
     $entry = ldap_next_entry($ldapConn, $entry))
	$Groups[] = ldap_get_dn($ldapConn, $entry);

$GroupDescs = array();
#foreach ($Groups as $dn) {
#	$lsearch = ldap_search($ldapC

///// Check for errors in LDAP entry
$Errors = array();

$Name = ($Entry["cn"] != $Entry["uid"]) ? $Entry["cn"] : false;
$Entry['uidnumber'] = intval($Entry['uidnumber']);
$Entry['gidnumber'] = intval($Entry['gidnumber']);

function isAttrVisible($attr, $default=false) {
	// TODO TODO: make $attr case-insensitive
	global $Entry;
	return isset($Entry["options"])
		? (in_array("public=y", $Entry["options"])
		   || in_array("display-{$attr}=y", $Entry["options"]))
		: $default;
}

$displayMail = isAttrVisible("mail", false);

$Title = H($Entry["uid"]);
require "html-header.inc";
?>

<!-- page header - name, uid, picture -->
<div id="title">
<span style="float: left; margin-right: 5px">
<?php
if (has("jpegphoto")) {
	$img_url = "getphoto.php?uid=".$Entry["uid"];
} elseif (has("mail")) {
	$img_url = gravatar($Entry["mail"], 56, "mm");
} else {
	#$img_url = "http://wiki.xkcd.com/wirc/images/Bucket.png";
	$img_url = gravatar("", 56, "mm");
}
?>
	<img src="<?php echo H($img_url) ?>"
		alt="Photo of <?php echo H($Entry["uid"]) ?>"
		style="width: 56px; height: 56px">
</span>

<?php if ($Name) {
	echo "<h1>".H($Name)."</h1>\n";
	if ($Name != $Entry["uid"])
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
echo "| ".condLink($View == "access", "access", '?q='.urlencode($Query)."&view=access")."\n";
if (has('sshpublickey'))
	echo "| ".condLink($View == "sshkeys", "SSH keys", '?q='.urlencode($Query)."&view=sshkeys")
		." (".count($Entry['sshpublickey']).")\n";
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
case "access":
case "ldif":
case "sshkeys":
	require "view-{$View}.inc";
	break;
}

require "html-footer.inc";
?>

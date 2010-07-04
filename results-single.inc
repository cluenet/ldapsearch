<?php
///// Do some preparation on the results
$Entry = $Entry[0];
// de-array-ize attributes which are always single-instance
$Entry = fixEntry($Entry);
// htmlspecialchars everything
$Entry = htmlize($Entry);

///// Begin doing things

$IsSuspended = array_search('suspendedUser', $Entry['objectclass']) !== false;

// build an array of privileges (already flipped)
$Privileges = array();
if (isset($Entry['authorizedservice']))
	foreach ($Entry['authorizedservice'] as $k => $v)
		$Privileges[$v] = true;
if (isset($Entry['clueauthorizedability']))
	foreach ($Entry['clueauthorizedability'] as $k => $v)
		$Privileges[$v] = true;
ksort($Privileges);

$HasShell = isset($Privileges['ssh']) or isset($Privileges['sshd']);

///// Check for errors in LDAP entry
$Errors = array();
$Invalid = array();

if (has('uid')) $uid = $Entry['uid'];
else {
	$uid = "Anonymous Coward";
	$Errors[] = "<strong>user doesn't have a user name</strong> (<code>uid</code>)";
}

if (isset($uid) and strtolower($uid) == "fahad") {
	;# $Errors[] = "<strong>user is an asshat</strong>";
}

if (has('uidnumber'))
	$Entry['uidnumber'] = intval($Entry['uidnumber']);
else
	$Errors[] = "<strong>doesn't have an UID</strong> (<code>uidNumber</code>)";

if (has('uidnumber') and $Entry['uidnumber'] < 25000)
	$Errors[] = "<strong>UID is less than 25000</strong> (<code>uidNumber</code>)";

if (has('gidnumber'))
	$Entry['gidnumber'] = intval($Entry['gidnumber']);
else
	$Errors[] = "<strong>doesn't have a group ID</strong> (<code>gidNumber</code>)";

if (!has('homedirectory'))
	$Errors[] = "doesn't have a homedir (<code>homeDirectory</code>)";
	
if (isset($Entry['cn']))
	$Name = ($Entry['cn'] != $uid)? $Entry['cn'] : false;
else
	$Errors[] = "doesn't have a common name (<code>cn</code>)";

if (isset($Privileges['ssh']) xor isset($Privileges['sshd']))
	$Errors[] = "is missing one SSH authorizedService (should have <code>ssh</code> and <code>sshd</code>";

if (!has('krb5principalname'))
	$Errors[] = "is missing a Kerberos principal (<code>krb5PrincipalName</code>)";

if ($HasShell and !has('clueircnick'))
	$Errors[] = "has SSH, but didn't set a clueIrcNick";

if (has('pgpkeyid') and !preg_match("/^(0x)?([0-9a-z]{8}){1,2}$/i", $Entry['pgpkeyid']))
	$Errors[] = "incorrect PGP key ID format";

if (!isset($Privileges['wiki']))
	$Errors[] = "doesn't have ClueWiki access";

if (has('openid') and
	!(substr($Entry['openid'], 0, 7) == "http://"
	or substr($Entry['openid'], 0, 8) == "https://")) {
	$Errors[] = "invalid OpenID URL - must start with <code>http://</code> or <code>https://</code>";
	$Invalid["openid"] = true;
}



?>

<div id="title">
<span style="float: left; margin-right: 5px">
        <img src="getphoto.php?uid=<?php echo $uid ?>"
                alt="Photo of <?php echo $uid ?>"
                style="width: 64px; height: 64px">
</span>
<?php if ($Name) {
	echo "<h1>{$Name}</h1>\n";
	if (strtolower($Name) != strtolower($uid))
		echo "<div id=\"realname\">{$uid}</div>\n";
}
else {
	echo "<h1>{$uid}</h1>\n";
} ?>
</div>
<div style="clear: both;"></div>
<div class="box navigation"> <!-- view bar -->
<p>
view:
   <?php echo condLink($View == "info", "info", '?q='.urlencode($Query));?>
 | <?php echo condLink($View == "account", "account", '?q='.urlencode($Query)."&view=account");?>
 | <?php echo condLink($View == "person", "personal", '?q='.urlencode($Query)."&view=person");?>
 | <?php echo condLink($View == "contact", "contact", '?q='.urlencode($Query)."&view=contact");?>
 | <?php echo condLink($View == "access", "access", '?q='.urlencode($Query)."&view=access");?>
<?php if (has('cluesshpubkey')): ?>
 | <?php echo condLink($View == "sshkeys", "SSH public keys", '?q='.urlencode($Query)."&view=sshkeys"); ?> (<?php echo count($Entry['cluesshpubkey']); ?>)
<?php endif; ?>
 | <?php echo condLink($View == "ldif", "LDIF", '?q='.urlencode($Query)."&view=ldif");?>
</p>
</div>

<?php if (count($Errors) > 0): ?>
<div class="box content"> <!-- errors -->
<h2 class="error">Errors, warnings and other unimportant stuff</h2>
<ul>
<?php foreach ($Errors as $msg) echo "<li>".$msg."</li>\n"; ?>
</ul>
</div>
<?php endif; ?>

<?php
if ($View == "info") { include "view-info.inc"; }
if ($View == "account") { include "view-account.inc"; }
if ($View == "person") { include "view-person.inc"; }
if ($View == "contact") { include "view-contact.inc"; }
if ($View == "access") { include "view-access.inc"; }
if ($View == "servaccess") { include "view-serveraccess.inc"; }
if ($View == "ldif") { include "view-ldif.inc"; }
if ($View == "sshkeys") { include "view-sshkeys.inc"; }
?>

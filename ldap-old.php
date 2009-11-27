<?php
// (c) 2008 Mantas Mikulėnas <grawity@gmail.com>

// Licensed under WTFPL 2.0:

//            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
//                    Version 2, December 2004
//
// Copyright (C) 2004 Sam Hocevar
//  14 rue de Plaisance, 75014 Paris, France
// Everyone is permitted to copy and distribute verbatim or modified
// copies of this license document, and changing it is allowed as long
// as the name is changed.
//
//            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
//   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
//
//  0. You just DO WHAT THE FUCK YOU WANT TO.

// See http://sam.zoy.org/wtfpl/COPYING for more details.

error_reporting(E_ALL ^ E_NOTICE);
ini_set('display_errors', 1);
#ini_set('log_errors', 1);
#ini_set('error_log', dirname(__FILE__) . '/../error.log'); 

define("USE_SSL", True);
define("LDAP_SERVER", "ldap.cluenet.org");
define("LDAP_PORT", USE_SSL? 636 : 389);
define("BASE_DN", "dc=cluenet,dc=org");
define("BIND_DN", False);
define("BIND_PASS", False);


define("LDAP_URI", (USE_SSL?"ldaps":"ldap")."://".LDAP_SERVER.":".LDAP_PORT."/");

$showFace = isset($_GET['face']);

$query = $_GET["q"];

$rawDump = isset($_GET['raw']);
$sshKeyDump = isset($_GET['sshkeys']);
$infoView = !($rawDump or $sshKeyDump);
$moar = isset($_GET['moar']);

function grab($what) {
	$ldapConn = ldap_connect(LDAP_URI);
	ldap_set_option($ldapConn, LDAP_OPT_PROTOCOL_VERSION, 3);
	if (BIND_DN == False || BIND_PASS == False) {
		ldap_bind($ldapConn);
	}
	else {
		ldap_bind($ldapConn, BIND_DN, BIND_PASS);
	}
	$ldapSearch = ldap_search($ldapConn, BASE_DN, $what);
	$results = ldap_get_entries($ldapConn, $ldapSearch);
	ldap_unbind($ldapConn);
	return $results;
}

function getCluePoints($nick) {
	if (strtolower($nick) == 'grawity') return '∞';
	
	$cbURL = "http://ws.cluenet.org/cluebot_points.php?nick=".$nick;
	$cH = curl_init();

	curl_setopt($cH, CURLOPT_URL, $cbURL);
	curl_setopt($cH, CURLOPT_HEADER, 0);
	curl_setopt($cH, CURLOPT_RETURNTRANSFER, 1);
	
	$out = curl_exec($cH);

	curl_close($cH);

	if (preg_match("/[^0-9]+/", $out) === 0) return "Umm...brb.";
	return $out;
}

function c($c) {
	if (isset($_GET['alt'])) switch ($_GET['alt']) {
		case '':
		case 0: echo strrev($c); break;
		case 1: echo $c[0].$c[2].$c[1]; break;
		case 2: echo $c[1].$c[0].$c[2]; break;
		case 3: echo $c[1].$c[2].$c[0]; break;
		case 4: echo $c[2].$c[0].$c[1]; break;
		case 5: echo $c[2].$c[1].$c[0]; break;
		default: echo "f00"; break;
	}
	else echo $c;
}

function formatQuota($q) {
	$size = False;
	$count = False;
	
	$q = explode(',', $q);
	
	foreach ($q as $k) {
		switch ($k[strlen($k)-1]) {
			case 'S': $size = intval(substr($k, 0, strlen($k)-1)); break;
			case 'C': $count = intval(substr($k, 0, strlen($k)-1)); break;
		}
	}
	
	return ($size?niceSize($size,true):"").($size and $count?", ":"").($count?$count." messages":"");
}

function niceSize($bytes, $d = false) {
	$symbol = array('B', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y');
	$exp = 0;
	$converted_value = 0;
	if ($bytes > 0) {
	  $exp = floor(log($bytes)/log($d?1000:1024));
	  $converted_value = ($bytes/pow($d?1000:1024,floor($exp)));
	}
	return sprintf( '%.0f '.$symbol[$exp].($d?'':'i').'B', $converted_value );
}

?>
<?php echo '<'.'?xml'; ?> version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<!--

&alt - alternate colour scheme
&face - turnyournameintoaface.com
&cobi - show -∞ as points
&raw - show raw LDIF

-->
<head>
	<title>ClueLDAP</title>
	<meta http-equiv="Content-Type" content="application/xhtml+xml" />
	<meta http-equiv="X-UA-Compatible" content="IE=8" />
	<meta name="author" content="Mantas Mikulėnas" />
	<style type="text/css">
	body {
		background: #<?php c('dcb'); ?>;
		margin-top: 0px;
	}
	div#top {
		background: #<?php c('cba'); ?>;
		border: 1px solid #<?php c('ba9'); ?>;
		border-top: none;
		width: 100%;
		text-align: left;
	}
	#top form {
		padding: 3px;
	}
	a:link, a:visited {
		color: #<?php c('765'); ?>;
	}
	.head .bigUsername {
		font-size: xx-large;
	}
	.head .someinfo {
		font-family: "Trebuchet MS", sans-serif;
		font-style: italic;
	}
	.someinfo .realName {
		color: #<?php c('654'); ?>;
	}
	.someinfo .elite {
		color: #<?php c('987'); ?>;
	}
	.contentTable {
		width: 100%;
	}
	.contentTable th, .contentTable td {
		vertical-align: top;
		width: 50%;
	}
	.contentBox {
		border: 1px solid #<?php c('ba9'); ?>;
		background: #<?php c('edc'); ?>;
		padding: 3px;
		font-family: "Trebuchet MS", sans-serif;
		/*margin: 3px;*/
		margin-bottom: 3px;
	}
	.contentBox .boxTitle {
		font-size: larger;
	}
	.contentBox .entry {
	}
	ul {
		font-family: "Trebuchet MS", sans-serif;
	}
	ul.smallList {
		list-style-type: none;
		margin-top: 0em;
		margin-bottom: 0em;
	}
	ul.smallList li {
		font-size: xx-small;
	}
	h1:target, h2:target, span:target {
		background: #fff;
		outline: 2px solid #<?php c('ba9'); ?>;
	}
	.warning {
		color: #f00;
		font-weight: bold;
	}
	.empty {
		/*color: #f00;*/
		font-style: italic;
	}
	a img {
		border: none;
	}
	a:hover img {
		outline: 2px solid #<?php c('ba9'); ?>;
	}
	.label {
		color: #<?php c('654'); ?>;
	}
	.value.important {
		font-weight: bold;
	}
	.comment {
		font-style: italic;
		color: #<?php c('765'); ?>;
	}
	</style>
</head>
<body>

<div id="top">
	<form method="get">
		<input type="text" name="q" id="q" value="<?php echo htmlspecialchars($query); ?>" />
		<input type="submit" value="search &gt;&gt;" />
	</form>
</div>

<?php if (empty($query)) { ?>
<p>Please enter an username or a LDAP filter into the above box.</p>
<?php } else { // !empty($query) ?>
<?php
	if ($query[0] == '(') {
		$filter = $query;
	}
	else {
		$filter = '(uid='.$query.')';
	}
	$res = grab($filter);
	$numResults = $res['count'];
?>
<?php if ($numResults > 1) { ?>
<p>Found <?php echo $numResults; ?> users:</p>
<ul>
<?php for ($n = 0; $n < $numResults; $n++) { ?>
	<li><a href="?q=<?php echo $res[$n]['uid'][0]; ?>"><?php echo $res[$n]['uid'][0]; ?></a> <?php if ($res[$n]['uid'][0] != $res[$n]['cn'][0]): ?><em>- <?php echo $res[$n]['cn'][0]; ?></em><?php endif; ?></li>
<?php } // for ?>
</ul>
<?php } elseif ($numResults == 0) { ?>
<p>Nothing found.</p>
<?php } else { // $res['count'] == 1 ?>
<?php for ($n = 0; $n < $numResults; $n++): ?>
<?php
$p = $res[$n];
$userName = $p['uid'][0];
$uid = intval($p['uidnumber'][0]);
if (isset($p['cn']) and $p['cn'][0] != $userName) {
	$commonName = $p['cn'][0];
}
$isSuspended = array_search('suspendedUser', $p['objectclass']) !== false;
$sshKeyCount = isset($p['cluesshpubkey']) ? $p['cluesshpubkey']['count'] : 0;

$access = array();
if (isset($p['clueauthorizedability'])) {
	unset($p['clueauthorizedability']['count']);
	foreach($p['clueauthorizedability'] as $v) $access[$v] = 1;
}
if (isset($p['authorizedservice'])) {
	unset($p['authorizedservice']['count']);
	foreach($p['authorizedservice'] as $v) $access[$v] = 1;
}
if ($uid == 25002) $access['cobi'] = 1;
if ($uid == 25001) $access['crispy'] = 1;

?>

<?php if (isset($_GET['alt'])) { ?>
<p>Also try alt= 1 2 3 4 5</p>
<?php } ?>

<div class="head">
	<span class="bigUsername" id="uid=<?php echo $userName; ?>"><?php echo htmlspecialchars($userName); ?></span>
		<br />
	
	<span class="someinfo">
	
	<?php if (isset($commonName)) { ?>
	<span class="realName"><?php echo htmlspecialchars($commonName); ?></span><br />
	<?php } ?>
	
	<?php if (
		($uid <= 25005 or $uid == 25193)
		and !$isSuspended
	) { ?>
	<span class="elite">is elite</span><br />
	<?php } ?>
	<?php if ($isSuspended) { ?>
	<span clas="elite warning">is suspended</span><br />
	<?php } ?>
	</span>

	<br />
</div>

<table class="contentTable">
<tbody>

<tr>
	<td colspan="2">
		<div class="contentBox">
			view:
			<?php if ($infoView and !$moar) { ?>
				<strong>normal</strong>
			<?php } else { ?>
				<a href="?q=<?php echo $userName; ?>">normal</a>
			<?php } ?>
			|
			<?php if ($infoView and $moar) { ?>
				<strong>more info</strong>
			<?php } else { ?>
				<a href="?q=<?php echo $userName; ?>&amp;moar">more info</a>
			<?php } ?>
			|
			<?php if ($rawDump) { ?>
				<strong>raw LDIF</strong>
			<?php } else { ?>
				<a href="?q=<?php echo $userName; ?>&amp;raw">raw LDIF</a>
			<?php } ?>
			|
			<?php if ($sshKeyDump) { ?>
				<strong>SSH keys</strong>
			<?php } else { ?>
				<a href="?q=<?php echo $userName; ?>&amp;sshkeys">SSH keys</a>
			<?php } ?>
			(<?php echo $sshKeyCount; ?>)
		</div>
	</td>
</tr>

<?php if ($rawDump) { ?>

<tr>
	<td>
		<div class="contentBox">
			<pre><?php system("ldapsearch -x -P 3 -H ".LDAP_URI." '(uid=".$userName.")'"); ?></pre>
		</div>
	</td>
</tr>

<?php } elseif ($sshKeyDump) { ?>

<tr>
	<td>
		<div class="contentBox">
			<span class="boxTitle">SSH public keys</span><br />
			<?php if (isset($p['cluesshpubkey']) and $p['cluesshpubkey']['count'] > 0) { ?>
			<?php unset($p['cluesshpubkey']['count']); foreach ($p['cluesshpubkey'] as $key) { ?>
			<span class="label"><?php echo htmlspecialchars(implode(' ', array_slice(explode(' ', $key), 2))); ?></span><br />
			<textarea class="value" style="width: 100%; text-wrap: unrestricted; word-wrap: break-word;" readonly="readonly" rows="5"><?php echo htmlspecialchars($key); ?></textarea>
			<?php } // foreach ?>
			<?php } else { // no keys ?>
			<span class="comment">No SSH public keys are entered. Well, the script isn't used anyway...</span>
			<?php } ?>
		</div>
	</td>
</tr>

<?php } else { // not $rawDump ?>

<tr>
	<td>
		<div class="contentBox">
			<span class="boxTitle">Account</span><br />
			
			<?php if ($isSuspended) { ?>
			<span class="label">Status:</span>
			<span class="value warning">suspended</span>
			<br />
			<?php } ?>
			
			<span class="label">UID:</span>
			<span class="value"><?php echo $uid; ?></span>
			<br />
			
			<?php if ($p['gidnumber'] != 25000 or $moar) { ?>
			<span class="label">GID:</span>
			<span class="value"><?php echo $p['gidnumber'][0]; ?></span>
			<br />
			<?php } ?>
			
			<span class="label">Shell:</span>
			<?php if (!empty($p['loginshell'])) { ?>
			<span class="value"><?php echo $p['loginshell'][0]; ?>
			<?php if ($p['loginshell'][0] == '/bin/bash') { ?>
			<span class="comment">(what else?)</span>
			<?php } ?>
			</span>
			<?php } else {?>
			<span class="value empty">not set - usually defaults to /bin/sh</span>
			<?php } ?>
			<br />
			
			<?php if (empty($p['homedirectory'])) { ?>
			<span class="label">Homedir:</span>
			<span class="value warning">not set</span>
			<br />
			<?php } elseif ($p['homedirectory'][0] != '/home/'.$userName or $moar) { ?>
			<span class="label">Homedir:</span>
			<span class="value"><?php echo $p['homedirectory'][0]; ?></span>
			<br />
			<?php } ?>
			

			<?php /* ?>
			<span class="label">Allowed services:</span>
			<span class="value"><small><?php
				unset($p['authorizedservice']['count']);
				sort($p['authorizedservice'], SORT_STRING);
				echo implode(", ", $p['authorizedservice']);
			?></small></span>
			<br />
			<?php */ ?>
			
			<?php if (!empty($p['host'])) { ?>
			<span class="label">Accessible hosts:</span>
			<span class="value"><?php
				unset($p['host']['count']);
				sort($p['host'], SORT_STRING);
				foreach ($p['host'] as &$host) $host = str_replace('.cluenet.org', '', $host);
				echo implode(", ", $p['host']);
			?></span>
			<br />
			<?php } ?>
			
			<?php /* if (!empty($p['clueauthorizedability'])) { ?>
			<span class="label">Privileges:</span>
			<span class="value"><small><?php
				unset($p['clueauthorizedability']['count']);
				sort($p['clueauthorizedability'], SORT_STRING);
				echo implode(", ", $p['clueauthorizedability']);
			?></small></span>
			<br />
			<?php } */ ?>
			
			<?php if (!empty($p['mailquota'])) { ?>
			<span class="label">Mail quota:</span>
			<span class="value"><?php echo formatQuota($p['mailquota'][0]); ?></span>
			<br />
			<?php } ?>
			
			<?php if (empty($p['krb5principalname'])) { ?>
			<span class="value warning">user has no Kerberos principal attached</span>
			<br />
			<?php } elseif ($p['krb5principalname'][0] != $userName.'@CLUENET.ORG' or $moar) { ?>
			<span class="label">Kerberos principal:</span>
			<span class="value"><?php echo $p['krb5principalname'][0]; ?></span>
			<br />
			<?php } ?>
			
		</div>
	</td>
	
	<td>
		<div class="contentBox">
			<span class="boxTitle">User</span><br />
			
			<?php if (isset($commonName)) { ?>
			<span class="label">Real name:</span>
			<span class="value important"><?php echo htmlspecialchars($commonName); ?></span>
			<br />
			<?php } ?>
	
			<?php if (empty($p['clueircnick'])) { ?>
			<em class="comment">This user hasn't set his IRC nickname yet.</em>
			<?php } else { ?>
			<span class="label">Cluepoints:</span>
			<span class="value important"><?php echo getCluePoints($p['clueircnick'][0]); ?></span>
			<br />
			
			<span class="label">IRC nick:</span>
			<span class="value important"><?php echo htmlspecialchars($p['clueircnick'][0]); ?></span>
			<br />
			<?php } ?>
			
			<?php if (!empty($p['pgpkeyid'])) { ?>
			<?php $pgpKeyId = htmlspecialchars($p['pgpkeyid'][0]); ?>
			<span class="label">PGP key:</span>
			<?php if (preg_match("/^(0x)?([0-9a-f]{8}){1,2}$/i", $pgpKeyId) < 1) { ?>
			<span class="value warning">invalid</span>
			<?php } else { ?>
			<span class="value"><?php echo $pgpKeyId; ?></span>
			<?php } ?>
			<br />
			<?php } ?>
			
			<?php if (!empty($p['cluevoipuri'])) { ?>
			<?php $voipUri = htmlspecialchars($p['cluevoipuri'][0]); ?>
			<span class="label">VoIP:</span>
			<span class="value"><a href="<?php echo $voipUri; ?>"><?php echo $voipUri; ?></a></span>
			<br />
			<?php } ?>
			
			<!-- BEGIN ACCESS -->
			<br />
			
			<span class="label">Access:</span>
			<?php $accessCount = 0; ?>
			<ul>
			<?php
			$fcount = 0;
			define('A_WARNING', 1 << $fcount++);
			define('A_UNKNOWN', 1 << $fcount++);
			define('A_DESCONLY', 1 << $fcount++);
			define('A_NOINCREMENT', 1 << $fcount++);
			function pac($desc, $flags = 0) {
				global $accessCount;
				$style = "value";
				if (($flags & A_WARNING) == A_WARNING) $style .= " warning";
				if (($flags & A_UNKNOWN) == A_UNKNOWN) $style .= " comment";
				echo "<li class=\"".$style."\">";
				
				echo $desc;
				echo "</li>\n";
				if (($flags & A_NOINCREMENT) != A_NOINCREMENT) $accessCount++;
			}
			?>
			<?php
			
			if ($access['crispy']) pac("lots of things");
			
			if ($access['cobi']) pac("lots of things");
			
			if ($access['ssh'] or $access['sshd'])
				pac("SSH (shell) access");

			if ($access['marriage']) {
				if ($uid == 25001) pac("lamia");
				else pac("marriage");
			}

			if ($access['ssh'] xor $access['sshd'])
				pac("is missing a SSH attribute (should have ssh and sshd)", A_WARNING);
			
			if (!$access['wiki']) pac("doesn't have wiki access =O", A_WARNING);
			
			# print known entries
			$accessUnknown = array();
			foreach ($access as $k => $v) switch (strtolower($k)) {
				case 'crispy': case 'cobi':
				case 'ssh': case 'sshd':
				case 'marriage':
					# handled above
					break;
				case 'mail':
					pac("ClueMail".(!empty($p['mailquota'])?
						' <span class="comment">(quota: '.formatQuota($p['mailquota'][0]).')</span>':''
						)
					);
					break;
				case 'createacct':
					pac("<em>createacct</em> - can activate new accounts");
					break;
				case 'sshvote':
					pac("<em>sshvote</em> - can vote on SSH applications");
					break;
				case 'sshapprove':
					pac("<em>sshapprove</em> - can approve SSH applications");
					break;
				case 'vpn':
					pac("routable VPN (10.156.x.x range)");
					break;
				case 'wiki':
					if ($moar) pac('<a href="https://wiki.cluenet.org/">ClueWiki</a>');
					else $aHidden++;
					break;
				case 'eyeos':
					if ($moar) pac('<a href="http://eyeos.cluenet.org/">EyeOS</a>');
					else $aHidden++;
					break;
				case 'gallery':
					pac('<a href="http://gallery.cluenet.org/">CluePics</a>');
					break;
				case 'forum':
					if ($moar) pac('<a href="http://forum.cluenet.org/">Forum</a>');
					else $aHidden++;
					break;
				case 'sudo':
					if ($moar) pac('sudo (not really)');
					else $aHidden++;
					break;
				case 'cbtrain':
					pac('ClueBot training interface');
					break;
				default: $accessUnknown[$k] = 1;
			}
			
			# print unknown entries
			if ($moar) foreach ($accessUnknown as $k => $v) 
				pac('<em>Unknown service <strong>'.$k.'</strong></em>', A_DESCONLY | A_UNKNOWN);
			?>
			
			<?php if ($accessCount == 0) { ?>
			<li class="value comment">none :&quot;(</li>
			<?php } ?>

			</ul>
			<!-- END ACCESS -->
			
			<?php if (isset($p['cluesshpubkey']) and $p['cluesshpubkey']['count'] > 0) { ?>
			<span class="label">SSH public keys:</span>
			<span class="value"><?php echo $p['cluesshpubkey']['count']; ?> (<a href="?q=<?php echo $userName; ?>&amp;sshkeys">view</a>)</span>
			<?php } ?>
		</div>
	</td>
</tr>

<?php } // $rawDump ?>

</tbody>
</table>

<?php endfor; ?>

<?php } // $res['count'] ?>

<?php } // empty($query) ?>

</body>
</html>

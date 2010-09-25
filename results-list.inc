<?php
$Title = "$NumResults results";
require "html-header.inc";
?>

<div class="box content">
<h1>Found <?php echo $NumResults; ?> entries.</h1>
<ul>
<?php
$list = array();

for ($entry = ldap_first_entry($ldapConn, $lsearch);
	$entry !== false;
	$entry = ldap_next_entry($ldapConn, $entry)) {

	$uid = ldap_get_values($ldapConn, $entry, "uid");
	$cn = ldap_get_values($ldapConn, $entry, "cn");

	$list[$uid[0]] = $cn[0];
}

ksort($list);

foreach ($list as $uid => $cn) {
	echo "\t<li><a href=\"?q=".H($uid)."\">".H($uid)."</a>";
	if ($uid != $cn)
		echo " <em>(".H($cn).")</em>";
	echo "</li>\n";
}
?>
</div>

<?php require "html-footer.inc"; ?>

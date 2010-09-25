<?php
$Title = "$NumResults results";
require "html-header.inc";
?>

<div class="box content">
<h1>Found <?php echo $NumResults; ?> entries.</h1>
<ul>
<?php
for ($entry = ldap_first_entry($ldapConn, $lsearch);
	$entry;
	$entry = ldap_next_entry($ldapConn, $entry)) {

	$uid = ldap_get_values($ldapConn, $entry, "uid");
	$cn = ldap_get_values($ldapConn, $entry, "cn");

	echo "\t<li><a href=\"?q=".H($uid[0])."\">".H($uid[0])."</a>";
	if ($uid[0] != $cn[0])
		echo " <em>(".H($cn[0]).")</em>";
	echo "</li>\n";
}
?>
</div>

<?php require "html-footer.inc"; ?>

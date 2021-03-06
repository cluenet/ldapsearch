<!-- One result, LDIF view -->
<div class="box content">
<?php
$show_props = isset($_GET["properties"]);
$command = implode(" ", array(
	"ldapsearch",
	"-x",
	"-LLL",
	"-b", escapeshellarg($Entry["dn"]),
	"-s", "base",
	escapeshellarg($show_props? "+" : "*"),
	));
?>

<h2><?php echo H($Entry["dn"]); ?></h2>

<pre><?php echo H(`$command`); ?></pre>

<?php
if ($show_props) {
	$new_req = mangle_query(null, array("properties"));
	$link_text = "Back to entry view";
} else {
	$new_req = mangle_query(array("properties" => null));
	$link_text = "View operational attributes";
}
?>
<p><small>
	<a href="?<?php echo H($new_req) ?>"><?php echo $link_text ?></a>
</small></p>

</div>

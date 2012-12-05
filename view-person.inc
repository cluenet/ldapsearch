<!-- BEGIN view/person -->
<?php $Moar = ($View == "person"); ?>
<div class="box content">
<h2>Personal</h2>
<p>
	<span class="label">Real name:</span>
	<?php
	if ($Name)
		echo "\t<span class=\"value important\">{$Name}</span>\n";
	else
		echo "\t<span class=\"value comment\">not set</span>\n";
	?>
	<br />

	<span class="label">IRC nick:</span>
	<?php
	if (has("clueIrcNick"))
		echo "\t<span class=\"value important\">{$Entry["clueircnick"]}</span>\n";
	else
		echo "\t<span class=\"value comment\">this user is too lazy to set one</span>\n";
	?>
</p>
</div>

<?php if ($Moar and has("cluegeneralcontact")): ?>
<div class="box content">
<?php
foreach ($Entry["cluegeneralcontact"] as $e)
	foreach (explode("\n", $e) as $line)
		echo "<p>{$line}</p>\n";
?>
</div>
<?php endif; ?>

<!-- Begin single/person/crap -->
<?php
list ($attributes, ,) = checkDisplaySection(
	array("spokenlanguage")
);

if (count($attributes) > 0) {
	echo "<div class=\"box content\">\n";
	include "box-singlesection.inc";
	echo "</div>\n";
}
?>

<!-- Begin single/person/skills -->
<?php if ($Moar and has("skill")): ?>
<div class="box content">
<h3>Skills</h3>
<ul>
<?php
foreach (htmlize($Entry["skill"]) as $skill)
	echo "\t<li>{$skill}</li>\n";
?>
</ul>
</div>
<?php endif; // $Moar and has(skill) ?>

<!-- END single/person -->

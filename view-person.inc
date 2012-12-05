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
	echo "\t<span class=\"value comment\">unknown</span>\n";
?>
	<br />

	<span class="label">IRC nick:</span>
<?php
if (has("clueIrcNick"))
	echo "\t<span class=\"value important\">{$Entry["clueircnick"]}</span>\n";
else
	echo "\t<span class=\"value comment\">unknown</span>\n";
?>
	</p>
<?php if ($Moar) { ?>
</div>
<?php } ?>


<?php
// contact info box

list ($sections, $numHidden, $numDisplayed) = checkDisplaySections(array(
	// basic contact information
	array("mail", "pgpkeyid", "altemail"),
	// websites and other URLs
	array("labeleduri"),
	// IM addresses, &c.
	array("contactinfo"),
));

if ($numDisplayed + $numHidden > 0) {
	if ($Moar) {
		echo "<div class=\"box content\">\n";
		echo "<h3>Contact information</h3>\n";
	}
	foreach ($sections as $section => $attributes) {
		if (count($attributes) == 0)
			continue;
		include "box-singlesection.inc";
	}
	if (!$Moar and has("contactinfo"))
		$numHidden++;
	if ($numHidden > 0)
		moreLink($numHidden, "person");
	if ($Moar) echo "</div>\n";
}

// postal addresses box

list ($attributes, $numHidden, $numDisplayed) = checkDisplaySection(
	array("postaladdress", "c")
);

if (count($attributes) > 0) {
	if ($Moar) {
		echo "<div class=\"box content\">\n";
		echo "<h3>Postal address</h3>\n";
	}
	include "box-singlesection.inc";
	if ($numHidden > 0)
		moreLink($numHidden, "person");
	if ($Moar) echo "</div>\n";
}

if (!$Moar) echo "</div>\n";

?>

<!-- END single/person -->

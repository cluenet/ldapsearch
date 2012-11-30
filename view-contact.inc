<!-- Begin single/contact -->
<?php $Moar = ($View == "contact"); ?>
<div class="box content">
<h2>Contact</h2>
<?php
list ($sections, $numHidden, $numDisplayed) = checkDisplaySections(array(
	array("mail", "pgpkeyid", "altemail"),
	array("labeleduri"),
	array("contactinfo"),
));

if ($numDisplayed + $numHidden > 0) {
	foreach ($sections as $section => $attributes) {
		if (count($attributes) == 0) continue;
		include "box-singlesection.inc";
	}

	if (!$Moar and has("contactinfo")) $numHidden++;

	if ($numHidden > 0) moreLink($numHidden, "contact");
}
else {
	echo "<p style=\"font-style: italic;\">This box intentionally left blank.</p>\n";
}
?>
</div>

<!-- Begin view/person/address -->
<?php
list ($attributes, ,) = checkDisplaySection(
	array("postaladdress", "c")
);

if (count($attributes) > 0) {
	echo "<div class=\"box content\">\n";
	include "box-singlesection.inc";
	echo "</div>\n";
}
?>

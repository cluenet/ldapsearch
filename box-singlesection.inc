<?php
echo "<p>\n";
foreach ($attributes as $attr) {
	if (isset($AllAttributes[$attr]))
		$name = $AllAttributes[$attr][1];
	else
		$name = $attr;
		
	echo "\t<span class=\"label\">{$name}:</span>\n";
	
	# htmlspecialchars is done in results-single.inc
	$value = $Entry[$attr];

	switch ($attr):
	case "mail":
	case "altemail":
		$value = linkify($value, "mail");
		break;
	case "openid":
		if (preg_match("|^https?://|", $value))
			$value = linkify($value, "www");
		break;
	endswitch;
	
	if (is_array($value)) $value = implode(", ", $value);
	
	echo "\t<span class=\"value\">{$value}</span>\n";
	echo "<br />\n";
}
echo "</p>\n";

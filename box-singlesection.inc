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

	if ($attr == "labeleduri") {
		$urls = $value;
		sort($urls);
		$value = "<ul>\n";
		foreach ($urls as $v) {
			@list ($uri, $label) = explode(" ", $v, 2);
			if (!strlen($label))
				$label = "<i>$uri</i>";
			$value .= "\t<li><a href=\"{$uri}\">{$label}</a></li>\n";
		}
		$value .= "</ul>\n";
	} elseif ($attr == "contactinfo") {
		$value = "<ul>\n\t<li>".implode("</li>\n\t<li>", $value)."</li>\n</ul>\n";
	}

	switch ($attr):
	case "mail":
	case "altemail":
		$value = linkify($value, "mail");
		break;
	endswitch;
	
	if (is_array($value)) $value = implode(", ", $value);
	
	echo "\t<span class=\"value\">{$value}</span>\n";
	echo "<br />\n";
}
echo "</p>\n";

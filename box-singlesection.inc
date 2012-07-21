<?php
foreach ($attributes as $attr) {
	if (isset($AllAttributes[$attr]))
		$name = $AllAttributes[$attr][1];
	else
		$name = $attr;

	echo "<p>\n";
	echo "\t<span class=\"label\">{$name}:</span>\n";
	
	# htmlspecialchars is done in results-single.inc
	$value = $Entry[$attr];

	switch ($attr):
	case "contactinfo":
		$value = "<ul>\n\t<li>".implode("</li>\n\t<li>", $value)."</li>\n</ul>\n";
		break;
	case "c":
		$code = strtoupper($value);
		$country = lookup_country($code);
		if ($country)
			$value = $country;
		$file = "flags/".strtolower($code).".png";
		if (file_exists($file))
			$value = "<img src=\"{$file}\"> $value";
		else
			$value = "$value ($code)";
		break;
	case "labeleduri":
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
		break;
	case "mail":
	case "altemail":
		$value = linkify($value, "mail");
		break;
	case "pgpkeyid":
		@list($id, $uri) = explode(" ", $value, 2);
		if (strlen(@$uri))
			$value = "<a href=\"{$uri}\">{$id}</a>";
		break;
	endswitch;
	
	if (is_array($value)) $value = implode(", ", $value);
	
	echo "\t<span class=\"value\">{$value}</span>\n";
	echo "</p>\n";
}

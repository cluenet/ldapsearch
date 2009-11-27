<!-- One result, SSH key view -->
<div class="box">
<h2>SSH public keys</h2>
<?php

function snipOptions($key) {
	if (preg_match("/([^\\\\]\" )()/", $key, $matches, PREG_OFFSET_CAPTURE) >= 1) {
		$key = htmlspecialchars(substr($key, $matches[2][1]));
		unset($matches);
	}
	
	if (preg_match("/([a-z-]+) (.+?)(?: (.*))\$/", $key, $matches))
		return $matches;
	else
		return false;
}

foreach ($Entry["cluesshpubkey"] as $e):
$key = snipOptions($e);

if ($key === false)
	echo "<h3 class=\"error\">Unrecognized key</h3>\n";
else
	echo "<h3>{$key[3]}</h3>\n";

echo "<input type=\"text\" class=\"sshkey\" readonly=\"readonly\" value=\"" . htmlspecialchars($e) . "\" /><br />\n";

endforeach; ?>
</div>

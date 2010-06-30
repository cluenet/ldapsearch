<!-- One result, SSH key view -->
<?php ?>
<div class="box">
<h2>SSH public keys</h2>
<?php

function snipOptions($key) {
	if (preg_match("/([^\\\\]\" )()/", $key, $matches, PREG_OFFSET_CAPTURE) >= 1) {
		$key = htmlspecialchars(substr($key, $matches[2][1]));
		unset($matches);
	}
	
	if (preg_match("/([a-z-]+?) ([A-Za-z0-9\\-+\\/=]+?)(?: (.*))?\$/", $key, $matches))
		return $matches;
	else
		return false;
}

foreach ($Entry["cluesshpubkey"] as $key):
$info = snipOptions($e);

if ($info === false)
	echo "<h3 class=\"error\">invalid key</h3>\n";
elseif (!isset($info[3]))
	echo "<h3><i>unnamed key</i></h3>\n";
else
	echo "<h3>{$info[3]}</h3>\n";

echo "<input type=\"text\" class=\"sshkey\" readonly value=\"" . htmlspecialchars($key) . "\" onclick=\"\" /><br />\n";

endforeach; ?>
</div>

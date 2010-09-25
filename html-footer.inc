<?php
$nextstyle = array_search($Style, $Stylesheets, true);
$nextstyle++;

$new_url = mangle_query(array(
	'style' => $Stylesheets[$nextstyle % count($Stylesheets)],
	));
?>
<p class="footer">
<a href="http://github.com/cluenet/ldapsearch">source</a>
| <a href="?<?php echo htmlspecialchars($new_url) ?>">switch style</a>
| <a href="http://www.cluenet.org/">Cluenet</a>
</p>

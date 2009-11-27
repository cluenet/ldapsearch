<!-- Multiple results -->
<p>Found <?php echo $NumResults; ?> entries.</p>
<ul>
<?php
for ($i = 0; $i < $NumResults; $i++) {
	$uid = htmlspecialchars($Entry[$i]["uid"][0]);
	$commonName = htmlspecialchars($Entry[$i]["cn"][0]);
	echo '<li>';
		echo '<a href="?q='.$uid.'">'.$uid.'</a>';
		if ($uid != $commonName) echo ' <em>- '.$commonName.'</em>';
	echo '</li>'.N;
}
?>

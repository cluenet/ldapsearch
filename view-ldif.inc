<!-- One result, LDIF view -->
<div class="box">
<?php
$command = implode(" ", array(
	"ldapsearch",
	"-x",
	"-b", escapeshellarg("ou=people,".BASE_DN),
	escapeshellarg($Filter),
	));
?>
<h2><?php echo htmlspecialchars($command); ?></h2>
<pre><?php echo htmlspecialchars(`$command`); ?></pre>
</div>

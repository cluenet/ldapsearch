<!-- One result, LDIF view -->
<div class="box">
<?php $command = "ldapsearch -x ".escapeshellarg($Filter); ?>
<h2><?php echo htmlspecialchars($command); ?></h2>
<pre><?php echo htmlspecialchars(`$command`); ?></pre>
</div>

<!-- BEGIN single/access -->
<?php $Moar = ($View == "access"); ?>
<?php if ($View == "access" and isset($Entry['host'])): ?>
<div class="box content">
<h2>Accessible hosts</h2>
<ul>
<?php
sort($Entry['host']);
foreach ($Entry['host'] as $host) {
	# snip off the domain
	if (substr($host, -strlen(".cluenet.org")) == ".cluenet.org")
		$host = substr($host, 0, -strlen(".cluenet.org"));

	echo "<li>{$host}</li>\n";
}
?>
</ul>
</div>
<?php endif; ?>

<div class="box content">
<h2>Access rights</h2>
<ul>
<?php
function level($name) {
	$name = strtolower($name);
	global $AllPrivileges, $Moar;
	return isset($AllPrivileges[$name])? $AllPrivileges[$name][0] : 1;
}

if (!$Moar and isset($Privileges['ssh']) and isset($Privileges['sshd'])) {
	# info view - show only one
	unset($Privileges['sshd']);
}

$numDisplay = 0;
$unknown = array();
$numHidden = 0;
$numVeryHidden = 0;

$EvenMoar = isset($_GET["evenmoar"]);

foreach (array_keys($Privileges) as $privilege) {
	if (level($privilege) >= 2 and !$EvenMoar) {
		# hide completely
		$numVeryHidden++; continue;
	}
	elseif (level($privilege) >= 1 and !$Moar) {
		# hide only in info view
		$numHidden++; continue;
	}

	if (!isset($AllPrivileges[$privilege])) {
		$unknown[] = $privilege; continue;
	}	
	$numDisplay++;
	
	$name = $AllPrivileges[$privilege][1];
	
	switch ($privilege) {
	case 'mail':
		# show quota
		if ($Moar and isset($Entry["mailquota"]))
			$name = "{$name} <em>(quota: ".formatQuota($Entry["mailquota"]).")</em>";
		break;

	case 'marriage':
		# show name
		if ($Entry["uidnumber"] == 25001) $name = "lamia";
		if ($Entry["uidnumber"] == 25055) $name = "Crispy";
		break;
	}
	
	if ($Moar) $name = "<code>{$privilege}</code>: {$name}";
	
	echo "\t<li>{$name}</li>\n";	
}

#if ($Moar) {
foreach ($unknown as $privilege) {
	$numDisplay++;
	echo "\t<li><em>Unknown service <code>{$privilege}</code></em></li>\n";
}
#}

if ($numDisplay == 0)
	echo "\t<li><em>None :(</em></li>\n";

echo "</ul>\n";

if ($numHidden > 0) moreLink($numHidden, "access");

if ($Moar and $numVeryHidden > 0)
	echo "<p><small>Also ",
		"<a href=\"?q=".htmlspecialchars($_GET["q"])."&amp;view=access&amp;evenmoar\" class=\"more\">",
		($numVeryHidden == 1)? "one entry" : "{$numVeryHidden} entries",
		"</a> that you shouldn't care about.",
		"</small></p>\n";

?>
</div>


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
	global $AllPrivileges;
	if (isset($AllPrivileges[$name]))
		return $AllPrivileges[$name][0];
	else
		return -1;
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

foreach (array_keys($Privileges) as $privEntry) {
	$privType = strtok($privEntry, "^")."^";
	$privValue = strtok(null);
	
	if (isset($AllPrivileges[$privEntry])) {
		list ($level, $name) = $AllPrivileges[$privEntry];
		
		if ($name === null and isset($AllPrivileges[$privType])) {
			$name = $AllPrivileges[$privType][1];
		}
	}
	elseif (isset($AllPrivileges[$privType])) {
		list ($level, $name) = $AllPrivileges[$privType];
	}
	else {
		$level = -1;
	}
	
	if ($level >= 2 and !$EvenMoar) {
		# hide completely
		$numVeryHidden++;
		continue;
	}
	elseif ($level >= 1 and !$Moar) {
		# show only in 'access' tab
		$numHidden++;
		continue;
	}
	elseif ($level == -1) {
		if ($Moar) $unknown[] = $privEntry;
		continue;
	}
	else {
		$numDisplay++;
	}
	
	switch ($privEntry) {
	case 'mail':
		# show quota
		if ($Moar and isset($Entry["mailquota"]))
			$name = "{$name} <em>(quota: ".formatQuota($Entry["mailquota"]).")</em>";
		break;
		
	default:
		$name = str_replace('{}', htmlspecialchars($privValue), $name);
	}
	
	#if ($Moar) $name = "<code>{$privEntry}</code>: {$name}";
	
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


<?php $More = ($View == "account"); ?>
<div class="box content">
<h2>Account</h2>

<?php if ($IsSuspended): ?>
<p>
	<span class="value important">Suspended user</span>
</p>
<?php endif; ?>

<p>
	<span class="label">User name:</span>
	<span class="value important"><?= $Entry["uid"] ?></span>
</p>

<?php if (ENABLE_CLUEPOINTS && has("clueircnick")): ?>
<p>
	<span class="label">Cluepoints:</span>
	<span class="value important"><?= ($points = getCluePoints($Entry["clueircnick"])) === false ? 'unknown' : $points ?></span>
</p>
<?php endif; ?>

<?php
if (!$More)
	expandLink("details", "account");
?>
</div>

<?php if ($More): ?>
<div class="box content">
<p>
	<span class="label">Login shell:</span>
	<?php if (isset($Entry["loginshell"])): ?>
	<span class="value pre"><?= $Entry["loginshell"] ?></span>
	<?php else: ?>
	<span class="value empty">not set - usually defaults to <code>/bin/sh</code></span>
	<?php endif; ?>
</p>

<p>
	<span class="label">Home directory:</span>
	<span class="value pre"><?= $Entry["homedirectory"] ?></span>
</p>

<p>
	<span class="label">UID:</span>
	<span class="value pre important"><?= $Entry["uidnumber"] ?></span>,
	<span class="label">GID:</span>
	<span class="value pre"><?= $Entry["gidnumber"] ?></span>
</p>

<?php if (isset($Entry["krbprincipalname"])): ?>
<p>
	<span class="label">Kerberos principal:</span>
	<span class="value pre"><?= $Entry["krbprincipalname"] ?></span>
</p>
<?php endif; ?>

<?php if (isset($Entry["timezone"])): ?>
<p>
	<span class="label">Timezone:</span>
	<span class="value"><?= $Entry["timezone"] ?></span>
</p>
<?php endif; ?>
</div>
<?php endif; ?>

<?php
/*
const RWHO_LIB = "/cluenet/rwho";
const RWHO_URL = "http://equal.cluenet.org/rwho/";
do if (@include RWHO_LIB."/lib-php/librwho.php") {
	$rwho = RWho\retrieve($Entry["uid"], null);
	$num_logins = count($rwho);
	if (!$num_logins and !$More)
		break;

	print "<div class=\"box content\">\n";
	print "<p>\n";
	print "\t<span class=\"label\"><a href=\"".H(RWHO_URL)."?user=".H($Entry["uid"])."\">rwho</a>:</span>\n";
	do if ($num_logins == 0) {
		print "\t<span class=\"value\">No active sessions.</span>\n";
	} else {
		$num_boxen = count(array_unique(array_map(function($a) {return $a["host"];}, $rwho)));
		print "\t<span class=\"value\">$num_logins active ".plural($num_logins, "session")." on $num_boxen ".plural($num_boxen, "server")."</span>\n";
		if (!$More) break;
		$rwho = RWho\summarize($rwho);
		print "\t<ul>\n";
		foreach ($rwho as $ent) {
			foreach ($ent as $k => $v)
				$ent["{$k}_h"] = htmlspecialchars($v);
			$line = "<strong>".$ent["host_h"]."</strong>: ";
			if ($ent["is_summary"])
				$line .= $ent["line_h"]." sessions";
			else
				$line .= "on <code>".$ent["line_h"]."</code>";

			if ($ent["rhost"] === "")
				$line .= " (console login)";
			else
				$line .= " from <code>".$ent["rhost_h"]."</code>";

			print "\t\t<li>$line</li>\n";
		}
		print "\t</ul>\n";
	} while (false);
	print "</p>\n";
	print "</div>\n";
} while (false);
*/
?>

<?php if ($More): ?>
<div class="box content">
<p>
	<span class="label">Entry created:</span>
<?php if ($Entry["createTimestamp"] > 1207008000): ?>
	<span class="value important"><?= strftime("%Y-%m-%d", $Entry["createTimestamp"]) ?></span>
<?php else: ?>
	<span class="value"><?= strftime("%Y-%m-%d", $Entry["createTimestamp"]) ?></span>
	<strong>(mass-import; actual date unknown)</strong>
<?php endif; ?>
	</p>

<?php if (isset($Entry["seealso"])) { ?>
<p>
	<span class="label">See also:</span>
<?php
$islist = count($Entry["seealso"]) > 1;
echo $islist
	? "</p>\n<ul class=\"value\">\n"
	: "<span class=\"value\">";
foreach ($Entry["seealso"] as $val) {
	$valH = H($val);
	if (preg_match('/^uid=([^,]+),ou=people,dc=cluenet,dc=org$/', $val, $m)) {
		$uidH = H($m[1]);
		$line = "<code><a href=\"?q=$uidH\">$valH</a></code>";
	} else {
		$line = "<code>$valH</code>";
	}
	echo $islist
		? "\t<li>$line</li>\n"
		: $line;
}
echo $islist
	? "</ul>\n"
	: "</span>\n</p>\n";
?>
<?php } ?>
</div>
<?php endif; ?>

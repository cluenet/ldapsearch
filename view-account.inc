<?php $More = ($View == "account"); ?>
<div class="box">
<h2>Account</h2>

<?php if ($IsSuspended): ?>
<p>
	<span class="value important">Suspended user</span>
</p>
<?php endif; ?>

<p>	
	<span class="label">User name:</span>
	<span class="value important"><?php echo H($Entry["uid"]) ?></span>
</p>

<?php if (has("clueircnick")): ?>
<p>
	<span class="label">Cluepoints:</span>
	<span class="value important"><?php echo ($points = getCluePoints($Entry["clueircnick"])) === false ? 'unknown' : $points; ?></span>
</p>
<?php endif;

if ($More): ?>
<p>	
	<span class="label">Login shell:</span>
	<?php if (isset($Entry["loginshell"])): ?>
	<span class="value"><?php echo $Entry["loginshell"]; ?></span>
	<?php else: ?>
	<span class="value empty">not set - usually defaults to <code>/bin/sh</code></span>
	<?php endif; ?>
</p>
<?php endif; ?>

<?php if ($More and isset($Entry["homedirectory"])): ?>
<p>
	<span class="label">Home directory:</span>
	<span class="value"><?php echo $Entry["homedirectory"]; ?></span>
</p>
<?php endif; ?>

<?php if ($More and isset($Entry["uidnumber"])): ?>
<p>	
	<span class="label">UID:</span>
	<span class="value important"><?php echo $Entry["uidnumber"]; ?></span>
</p>
<?php endif; ?>
	
<?php if ($More and isset($Entry["gidnumber"])): ?>
<p>
	<span class="label">GID:</span>
	<span class="value"><?php echo $Entry["gidnumber"]; ?></span>
</p>
<?php endif; ?>
	
<?php if ($More and isset($Entry["krb5principalname"])): ?>
<p>
	<span class="label">Kerberos principal:</span>
	<span class="value"><?php echo $Entry["krb5principalname"]; ?></span>
</p>
<?php endif; ?>
	
<?php if ($More and isset($Entry["timezone"])): ?>
<p>
	<span class="label">Timezone:</span>
	<span class="value"><?php echo $Entry["timezone"]; ?></span>
</p>
<?php endif; ?>
	
<?php if ($More and isset($Entry["mailquota"])): ?>
<p>
	<span class="label">Mail quota:</span>
	<span class="value"><?php echo formatQuota($Entry["mailquota"]); ?></span>
</p>
<?php endif; ?>
</div>

<?php
const RWHO_LIB = "/home/grawity/code/useless/rwho";
const RWHO_URL = "http://equal.cluenet.org/rwho/";
do if (include RWHO_LIB."/rwho.lib.php") {
	$rwho = RWho\retrieve($Entry["uid"], null);
	$num_logins = count($rwho);
	if (!$num_logins and !$More)
		break;

	print "<div class=\"box\">\n";
	print "<p>\n";
	print "\t<span class=\"label\"><a href=\"".H(RWHO_URL)."\">rwho</a>:</span>\n";
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
?>

<?php if ($More): ?>
<div class="box">
<p>
	<span class="label">Entry created:</span>
<?php if ($Entry["createTimestamp"] > 1207008000): ?>
	<span class="value important"><?php echo strftime("%Y-%m-%d %H:%M", $Entry["createTimestamp"]); ?></span>
<?php else: ?>
	<span class="value"><?php echo strftime("%Y-%m-%d %H:%M", $Entry["createTimestamp"]); ?></span>
	<strong>(mass-import; actual date joined unknown)</strong>
<?php endif; ?>
</p>
</div>
<?php endif; ?>

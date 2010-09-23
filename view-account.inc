<?php $Moar = ($View == "account"); ?>
<div class="box">
<h2>Account</h2>

<?php if ($IsSuspended): ?>
<p>
	<span class="value important">Suspended user</span>
</p>
<?php endif; ?>

<p>	
	<span class="label">User name:</span>
	<span class="value important"><?php echo $uid; ?></span>
</p>

<?php /*
<?php if (has("clueircnick")): ?>
<p>
	<span class="label">Cluepoints:</span>
	<span class="value important"><?php echo $points = getCluePoints($Entry["clueircnick"]); ?></span>
	<?php if ($points > 9000): ?>
	(OVER 9000!)
	<?php endif; ?>
</p>
<?php endif; ?>
*/ ?>

<?php if ($Moar and $HasShell): ?>
<p>	
	<span class="label">Login shell:</span>
	<?php if (isset($Entry["loginshell"])): ?>
	<span class="value"><?php echo $Entry["loginshell"]; ?></span>
	<?php else: ?>
	<span class="value empty">not set - usually defaults to <code>/bin/sh</code></span>
	<?php endif; ?>
</p>
<?php endif; ?>

<?php if ($Moar and isset($Entry["homedirectory"])): ?>
<p>
	<span class="label">Home directory:</span>
	<span class="value"><?php echo $Entry["homedirectory"]; ?></span>
</p>
<?php endif; ?>

<?php if ($Moar and isset($Entry["uidnumber"])): ?>
<p>	
	<span class="label">UID:</span>
	<span class="value important"><?php echo $Entry["uidnumber"]; ?></span>
</p>
<?php endif; ?>
	
<?php if ($Moar and isset($Entry["gidnumber"])): ?>
<p>
	<span class="label">GID:</span>
	<span class="value"><?php echo $Entry["gidnumber"]; ?></span>
</p>
<?php endif; ?>
	
<?php if ($Moar and isset($Entry["krb5principalname"])): ?>
<p>
	<span class="label">Kerberos principal:</span>
	<span class="value"><?php echo $Entry["krb5principalname"]; ?></span>
</p>
<?php endif; ?>
	
<?php if ($Moar and isset($Entry["timezone"])): ?>
<p>
	<span class="label">Timezone:</span>
	<span class="value"><?php echo $Entry["timezone"]; ?></span>
</p>
<?php endif; ?>
	
<?php if ($Moar and isset($Entry["mailquota"])): ?>
<p>
	<span class="label">Mail quota:</span>
	<span class="value"><?php echo formatQuota($Entry["mailquota"]); ?></span>
</p>
<?php endif; ?>
</div>

<div class="box">
<p>
	<span class="label">Created:</span>
	<span class="value important"><?php echo strftime("%Y-%m-%d %H:%M", $Entry["createTimestamp"]); ?></span>
</p>

<p>
	<span class="label">Modified:</span>
	<span class="value"><?php echo strftime("%Y-%m-%d %H:%M", $Entry["modifyTimestamp"]); ?>
		by <code><?php echo htmlspecialchars($OpEntry["modifiersName"][0]); ?></code></span>
</p>
</div>

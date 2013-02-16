<?php
$Title = "";
require "html-header.inc";
?>

<!-- big search box -->
<div class="box">
<h1>ldapsearch<sup>2¾</sup></h1>

<form id="searchbar" method="get" action="./">
	<p><input type="search" name="q" id="q"
		style="font-size: x-large; width: 99%;"
		value="<?php echo H($Query) ?>"></p>
	<p><input type="submit" value="search &gt;&gt;"></p>
</form>
</div>

<!-- introductory text -->
<div class="box content">
<p>You can also enter LDAP filters, e.g. <samp>(authorizedService=vpn)</samp></p>
</p>
</div>

<?php require "html-footer.inc"; ?>

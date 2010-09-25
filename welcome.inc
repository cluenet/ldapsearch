<?php
$Title = "";
require "html-header.inc";
?>

<!-- big search box -->
<div class="box">
<h1>ldapsearch<sup>2¾</sup></h1>

<form id="searchbar" method="get" action="./">
	<p><input type="text" name="q" id="q"
		style="font-size: x-large; width: 99%;"
		value="<?php echo H($Query) ?>"></p>
	<p><input type="submit" value="search &gt;&gt;"></p>
</form>
</div>

<!-- introductory text -->
<div class="box content">
<p>If you want to search for an IRC nick, prefix it with "<samp>irc:</samp>" This also works for IM addreses and pretty much all LDAP attributes.</p>

<p>You can also enter LDAP filters:<br>
<samp>(authorizedService=vpn)</samp></p>
</p>
</div>

<?php require "html-footer.inc"; ?>

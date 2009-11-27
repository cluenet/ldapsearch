<div class="box">
<h1>ldapsearch<sup>2½</sup></h1>

<p>
<form id="searchbar" method="get" action="./">
	<p><input type="text" style="font-size: x-large; width: 99%;" name="q" id="q" value="<?php echo $Query? $Query : ""; ?>" /></p>
	<p><input type="submit" value="search &gt;&gt;" /></p>
</form>
</div>

<div class="box content">
<p>If you want to search for an IRC nick, prefix it with &quot;irc:&quot;.<br />
Example: <code>irc:methecooldude</code></p>

<p>This also works for XMPP/MSN/AIM addreses and some other things.</p>

<p>You can also enter LDAP filters like this:<br />
<code>(authorizedService=vpn)</code></p>
</p>
</div>

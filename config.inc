<?php
define("N", "\n");
define("T", "\t");

define("USE_SSL", false);
define("LDAP_SERVER", "ldap.cluenet.org");
define("LDAP_PORT", USE_SSL? 636 : 389);
define("BASE_DN", "dc=cluenet,dc=org");
define("SASL_REALM", "CLUENET.ORG");
define("BIND_DN", False);
define("BIND_PASS", False);


define("LDAP_URI", (USE_SSL?"ldaps":"ldap")."://".LDAP_SERVER.":".LDAP_PORT."/");

$AllWebServices = array(
	"twitter"    => "Twitter",
	"digg"       => "Digg",
	"slashdot"   => "Slashdot",
	"googlecode" => "Google Code",
	"github"     => "GitHub",
	"freshmeat"  => "Freshmeat",
	"wikipedia"  => "Wikipedia",
);

$SingleAttributes = array(
	'cluesshprivkey',
	'cluesignupinfo',
	'clueircnick',
	'cluegender',
	'cluebirthyear',
	'suspendedpassword',
	'pgpkeyid',
	'aimsn',
	'xmppuri',
	'msnsn',
	'scheduleinfo',
	'twitteruser',
	'digguser',
	'slashdotuser',
	'githubuser',
	'freshmeatuser',
	'wikipediauser',
	'occupation',
	'timezone',
	'school',
	'wikipediauser',
	'cluenotes',
	'openid',
	'givenname',
	'c',
	'st', ## ?
	'mail',
	'sn',
	'uid',
	'uidnumber',
	'gidnumber',
	'homedirectory',
	'krb5principalname',
	'mailquota',
	'gecos',
	'loginshell',
	'cn',
	'altemail', ## ?
);

# Privileges only shown in extended view
# 0: display in info
# 1: display in "Access" tab (default)
# 2: do not display
$AllPrivileges = array(
	'marriage'   => array( 1, "marriage" ),
	'jackass'    => array( 1, "being a jackass" ),
	
	'ssh'        => array( 1, "<em>old</em> shell access [ssh]" ),
	'sshd'       => array( 1, "<em>old</em> shell access [sshd]" ),
	'sudo'       => array( 2, "<em>old</em> sudo" ),
	'login'      => array( 2, "<em>old</em> console login" ),
	'su'         => array( 2, "<em>old</em> su" ),
	'cron'       => array( 2, "<em>old</em> crontab" ),
	'atd'        => array( 2, "<em>old</em> atd" ),
	'chfn'       => array( 2, "<em>old</em> chfn" ),
	'chsh'       => array( 2, "<em>old</em> chsh" ),
	
	'mail'       => array( 0, "Cluenet.org mail" ),
	'wiki'       => array( 2, "wiki" ),
	'forum'      => array( 2, "<a href=\"http://forum.cluenet.org/\">forum</a>" ),
	'bugs'       => array( 2, "Bugzilla" ),
	'cacti'      => array( 2, "Cacti stats" ),
	'vps'        => array( 1, "CobiVPS" ),
		
	'acct^acctvote'	=> array( 0, "voting on account applications" ),
	'dns^'		=> array( 1, "DNS control of <code>{}</code>" ),
	'vpnpanel^'	=> array( 1, "VPN control of <code>{}</code>" ),
	
	
);

# Ignority level
# 0: display in info
# 1: display only in the full tab
$AllAttributes = array(
    'xmppuri'            => array( 0, "XMPP" ),
	'msnsn'              => array( 1, "<abbr title=\"Windows Live Messenger\">WLM</abbr>" ),
	'aimsn'              => array( 1, "<abbr title=\"AOL Instant Messenger\">AIM</abbr>" ),
	'mail'               => array( 0, "Email" ),
	'altemail'           => array( 1, "Alternate email" ),
	'pgpkeyid'           => array( 0, "PGP key ID" ),
	'telephonenumber'    => array( 1, "Phone" ),
	'uri'                => array( 1, "URI" ),
	'openid'             => array( 1, "OpenID" ),
	'cluegeneralcontact' => array( 1, "Other" ),
	
	'c'                  => array( 1, "Country" ),
	'postaladdress'      => array( 1, "Address" ),
	
	'spokenlanguage'     => array( 1, "Spoken languages" ),
);

?>

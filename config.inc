<?php
const LDAP_URI = "ldap://ldap.cluenet.org:389";
const LDAP_STARTTLS = false;

const BASE_DN = "dc=cluenet,dc=org";
const SASL_REALM = "CLUENET.ORG";
const BIND_DN = false;
const BIND_PASS = false;

const CB_SERVER = "lindelof.cluenet.org";
const CB_PORT = 58945;

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
);

define("PRIV_DEPRECATED", "<em>(old)</em> ");
define("PRIV_OBSOLETE", "<strong>(obsolete)</strong> ");

# Privileges only shown in extended view
# 0: display in info
# 1: display in "Access" tab (default)
# 2: do not display
$AllPrivileges = array(
	'marriage'   => array( 0, "marriage" ),
	
	'ssh'        => array( 2, PRIV_OBSOLETE."SSH access (Debian Etch)" ),
	'sshd'       => array( 2, PRIV_OBSOLETE."SSH access" ),
	'sudo'       => array( 2, PRIV_OBSOLETE."sudo" ),
	'login'      => array( 2, PRIV_OBSOLETE."console login" ),
	'su'         => array( 2, PRIV_OBSOLETE."su" ),
	'cron'       => array( 2, PRIV_OBSOLETE."crontab" ),
	'atd'        => array( 2, PRIV_OBSOLETE."atd" ),
	'chfn'       => array( 2, PRIV_OBSOLETE."chfn" ),
	'chsh'       => array( 2, PRIV_OBSOLETE."chsh" ),
	
	'mail'       => array( 2, PRIV_OBSOLETE."Cluenet.org mail" ),
	'wiki'       => array( 2, "wiki" ),
	'forum'      => array( 2, PRIV_OBSOLETE."<a href=\"http://forum.cluenet.org/\">forum</a>" ),
	'bugs'       => array( 2, PRIV_OBSOLETE."Bugzilla" ),
	'cacti'      => array( 2, PRIV_OBSOLETE."Cacti stats" ),
	'vps'        => array( 1, "CobiVPS" ),
	
	'createacct'		=> array( 2, PRIV_OBSOLETE."approval of new accounts" ),
	'sshvote'			=> array( 2, PRIV_OBSOLETE."voting on shell applications" ),
	'sshapprove'		=> array( 2, PRIV_OBSOLETE."approval of shell applications" ),
	'sshvoteoverride'	=> array( 2, PRIV_OBSOLETE."overriding shell application votes" ),
	
	'acct^acctvote'		=> array( 0, "voting on access applications" ),
	'dns^'				=> array( 1, "DNS control of <code>{}</code>" ),
	'dns^cluenet.org.'	=> array( 0, null ),
	'dns^in-addr.arpa.'	=> array( 0, null ),
	'tokengen^'			=> array( 2, "access token generation for <code>{}</code>" ),
	'vpnpanel^'			=> array( 1, "VPN control of <code>{}</code>" ),
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

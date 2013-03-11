<?php
const LDAP_URI = "ldap://ldap.cluenet.org";
#const LDAP_URI = "ldap://localhost:3890";
const LDAP_STARTTLS = false;

const BASE_DN = "dc=cluenet,dc=org";
const SASL_REALM = "CLUENET.ORG";
#const BIND_DN = false;
#const BIND_PASS = false;
require "config.inc.local";

const ENABLE_CLUEPOINTS = true;
const CB_SERVER = "virgule.cluenet.org";
const CB_PORT = 58945;

$SingleAttributes = array(
	'cluesignupinfo',
	'clueircnick',
	'cluegender',
	'cluebirthyear',
	'suspendedpassword',
	'pgpkeyid',
	'occupation',
	'timezone',
	'school',
	'cluenotes',
	'givenname',
	'c',
	'st', ## ?
	'mail',
	'sn',
	'uid',
	'uidnumber',
	'gidnumber',
	'homedirectory',
	'krbprincipalname',
	'gecos',
	'loginshell',
	'cn',
);

# Ignority level
# 0: display in info
# 1: display only in the full tab

const DISPLAY_ALWAYS = 0;
const DISPLAY_EXTENDED = 1;

$AllAttributes = array(
	'labeleduri'		=> array( 1, "Internet addresses" ),
	'mail'			=> array( 0, "Email" ),
	'altemail'		=> array( 1, "Alternate email" ),
	'pgpkeyid'		=> array( 1, "PGP key ID" ),
	'telephonenumber'	=> array( 1, "Phone" ),
	'contactinfo'		=> array( 1, "Other" ),

	'c'			=> array( 0, "Country" ),
	'st'			=> array( 1, "State" ),
	'l'			=> array( 1, "Location" ),
	'postaladdress'		=> array( 1, "Address" ),

	'spokenlanguage'	=> array( 1, "Spoken languages" ),
);

?>

<?php
include "config.inc";
$what = "(uid=".$_GET["uid"].")";
$ldapConn = ldap_connect(LDAP_URI);
ldap_set_option($ldapConn, LDAP_OPT_PROTOCOL_VERSION, 3);
if (BIND_DN == False || BIND_PASS == False) {
	ldap_bind($ldapConn);
}
else {
	ldap_bind($ldapConn, BIND_DN, BIND_PASS);
}
$ldapSearch = ldap_search($ldapConn, BASE_DN, $what, Array("jpegphoto"), null, 0);
if (ldap_count_entries($ldapConn, $ldapSearch) != 1) {
	header("Location: http://wiki.xkcd.com/wirc/bucket.png");
	die();
}
$results = ldap_first_entry($ldapConn, $ldapSearch);
$photo = ldap_get_values_len($ldapConn, $results, "jpegphoto");
ldap_unbind($ldapConn);
header("Content-Type: image/jpeg");
echo $photo[0];

?>

<?php
include "config.inc";
include "functions.inc";

$conn = ldap_connect_and_do_things();
if (!$conn) {
	header("Content-Type: text/plain");
	print_r($SysErrors);
	die();
}

$uid = $_GET["uid"];

$search = ldap_search($conn,
	"ou=people,".BASE_DN,
	"(uid={$uid})",
	array("jpegphoto"),
	false,
	0);

if (ldap_count_entries($conn, $search) != 1) {
	header("{$_SERVER["SERVER_PROTOCOL"]} 404");
	header("Location: http://wiki.xkcd.com/wirc/images/Bucket.png");
	die("User not found");
}

$entry = ldap_first_entry($conn, $search);
$values = ldap_get_values_len($conn, $entry, "jpegphoto");
if ($values === false) {
	header("{$_SERVER["SERVER_PROTOCOL"]} 404");
	header("Location: http://wiki.xkcd.com/wirc/images/Bucket.png");
	die("User has no photo");
}

ldap_unbind($conn);

header("Content-Type: image/jpeg");
print $values[0];

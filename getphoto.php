<?php
include "config.inc";
include "functions.inc";

function do_jpegphoto(&$photo) {
	$tag = sprintf("\"%08x\"", crc32($photo));

	// check if client has the ETag
	if (isset($_SERVER['HTTP_IF_NONE_MATCH'])
		and $tag == $_SERVER['HTTP_IF_NONE_MATCH']) {
		header("{$_SERVER["SERVER_PROTOCOL"]} 304");
		header("Status: 304");
		header("Content-Length: 0");
	}
	else {
		header("Etag: {$tag}");
		header("Content-Type: image/jpeg");
		header("Content-Length: ".strlen($photo));
		print $photo;
	}
}

function do_bucket() {
	header("{$_SERVER["SERVER_PROTOCOL"]} 404");
	header("Location: http://wiki.xkcd.com/wirc/images/Bucket.png");
}

function do_gravatar($mail) {
	$hash = md5(strtolower(trim($mail)));
	$url = "http://www.gravatar.com/avatar/{$hash}?d=identicon";
	header("Location: $url");
}

$conn = ldap_connect_and_do_things();
if (!$conn) {
	header("Content-Type: text/plain");
	print_r($SysErrors);
	die();
}

$uid = $_GET["uid"];

$search = @ldap_read($conn, "uid={$uid},ou=people,dc=cluenet,dc=org",
	"(objectClass=*)", array("jpegphoto", "mail", "modifyTimestamp"), false, 0);

if (!$search) {
	// user entry for $uid not found
	die("User not found\n");
}

$entry = ldap_first_entry($conn, $search);

// time the LDAP entry was modified
// no per-attribute mod times, but close enough
$modtime = ldap_get_values($conn, $entry, "modifyTimestamp");
$modtime = ldif_parse_time($modtime[0]);
header("Last-Modified: ".date(DateTime::RFC2822, $modtime));

$photo = ldap_get_values_len($conn, $entry, "jpegphoto");
if ($photo !== false) {
	ldap_unbind($conn);
	do_jpegphoto($photo[0]);
	die;
}

$mail = ldap_get_values($conn, $entry, "mail");
if ($mail !== false) {
	ldap_unbind($conn);
	do_gravatar($mail[0]);
	die;
}

ldap_unbind($conn);
do_bucket();
die;

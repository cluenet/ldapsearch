<?php
include "config.inc";
include "functions.inc";

function no_jpegphoto($jpegphotoerror) {
	$search = ldap_search($conn,
		"ou=people,".BASE_DN,
		"(uid={$uid})",
		array("mail"),
		false,
		0);

	if (ldap_count_entries($conn, $search) != 1) {
		header("{$_SERVER["SERVER_PROTOCOL"]} 404");
		header("Location: http://wiki.xkcd.com/wirc/images/Bucket.png");
		die("jpegphoto not found: {$jpegphotoerror}. mail not found: ".ldap_error($conn).".");
	}

	$entry = ldap_first_entry($conn, $search);
	$values = ldap_get_values($conn, $entry, "mail");
	if ($values === false) {
		header("{$_SERVER["SERVER_PROTOCOL"]} 404");
		header("Location: http://wiki.xkcd.com/wirc/images/Bucket.png");
		die("jpegphoto not found: {$jpegphotoerror}. mail is false: ".ldap_error($conn).".");
	}
	
	//$values[0] is an email address
	//try getting a gravatar/identicon, SO-style
	
	$email = trim($values[0]);
	$email = strtolower($email);
	$email = md5($email);
	$url = 'http://www.gravatar.com/avatar/' . $email . '?d=identicon';
	
	header("{$_SERVER["SERVER_PROTOCOL"]} 302");
	header("Location: $url");
	die("Sending gravatar");
}

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
	no_jpegphoto(ldap_error($conn));
}

$entry = ldap_first_entry($conn, $search);
$values = ldap_get_values_len($conn, $entry, "jpegphoto");
if ($values === false) {
	no_jpegphoto(ldap_error($conn));
}

ldap_unbind($conn);

$tag = sprintf("\"%08x\"", crc32($values[0]));

if (isset($_SERVER['HTTP_IF_NONE_MATCH'])
	and $tag == $_SERVER['HTTP_IF_NONE_MATCH']) {
	header("HTTP/1.0 304");
	header("Status: 304");
	header("Content-Length: 0");
	die;
}
else {
	header("Etag: {$tag}");
	header("Content-Type: image/jpeg");
	header("Content-Length: ".strlen($values[0]));
	print $values[0];
}

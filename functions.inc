<?php
# vim: ft=php

// todo: handle arrays?
function my_build_query($items) {
	$query = array();
	foreach ($items as $key => $value) {
		if ($value === null)
			$query[] = urlencode($key);
		else
			$query[] = urlencode($key)."=".urlencode($value);
	}
	return implode("&", $query);
}

function mangle_query($add, $remove=null) {
	parse_str($_SERVER["QUERY_STRING"], $query);
	
	if ($add !== null)
		foreach ($add as $key => $value)
			$query[$key] = $value;
	
	if ($remove !== null)
		foreach ($remove as $key)
			unset($query[$key]);
	
	return my_build_query($query);
}

function strptime_sane($date, $format) {
	$time_t = (object) strptime($date, $format);
	return mktime(
		$time_t->tm_hour,
		$time_t->tm_min,
		$time_t->tm_sec,
		$time_t->tm_mon + 1,
		$time_t->tm_mday,
		$time_t->tm_year + 1900
		);
}

function ldif_parse_time($str) {
	return strptime_sane($str, "%Y%m%d%H%M%SZ");
}

function clean_result($entry) {
	$new = array();
	global $SingleAttributes;

	# make it a purely associative array
	for ($i = 0; $i < $entry["count"]; $i++)
		unset($entry[$i]);

	unset($entry["count"]);

	# flatten value arrays if exactly one value is guaranteed	
	foreach ($entry as $key => $values) {
		// array_change_key_case is needed after ldap_get_attributes
		$key = strtolower($key);

		if ($key == "dn")
			continue;
		elseif (in_array($key, $SingleAttributes))
			$new[$key] = $values[0];
		else {
			unset($values["count"]);
			$new[$key] = $values;
		}
	}
	return $new;
}


function getCluePoints($nick) {
	if (!ENABLE_CLUEPOINTS)
		return "unknown";

	$conn = @fsockopen(CB_SERVER, CB_PORT, $errno, $errstr, 10);
	if (!$conn)
		return false;
	
	fwrite($conn, "points $nick\r\n");
	$data = fgets($conn);
	if (!strlen($data))
		return false;

	$data = explode(":", trim($data));
	return intval($data[1]);
}

function formatQuota($q) {
	$size = False;
	$count = False;
	$q = explode(',', $q);
	foreach ($q as $k) {
		switch ($k[strlen($k)-1]) {
			case 'S': $size = intval(substr($k, 0, strlen($k)-1)); break;
			case 'C': $count = intval(substr($k, 0, strlen($k)-1)); break;
		}
	}
	return ($size? formatSize($size,true) : "").
		($size and $count? ", " : "").
		($count? $count." messages" : "");
}


function formatSize($bytes, $decimal=false) {
	$symbol = 'kMGTPEZY';
	$exp = 0;
	$converted_value = 0;
	if ($bytes > 0) {
		$exp = floor(log($bytes)/log($decimal?1000:1024));
		$converted_value = $bytes/pow($decimal?1000:1024, floor($exp));
	}
	if ($exp) {
		return sprintf('%.0f %s%sB', $converted_value, $symbol[$exp-1], $decimal?'':'i');
	} else {
		return sprintf('%d B', $converted_value);
	}
}


function urlify($addr, $scheme) {
	$templates = array(
		"www" => "%s",
		"xmpp" => "xmpp://%s",
		"aim" => "aim:goim?screenname=%s",
		"msn" => "msnim:chat?contact=%s",
		"yahooim" => "ymsgr:sendIM?%s",

		"mail" => "mailto:%s",

		"twitter" => "https://twitter.com/%s",
		"digg" => "http://digg.com/users/%s",
		"slashdot" => "http://slashdot.org/~%s",
		"freshmeat" => "http://freshmeat.net/~%s",
		"github" => "https://github.com/%s",
		"googlecode" => "http://code.google.com/p/%s/",
		"lastfm" => "http://last.fm/user/%s",
		"wikipedia" => "http://en.wikipedia.org/wiki/User:%s",
	);

	if (array_key_exists($scheme, $templates))
		return sprintf($templates[$scheme], $addr);
	else
		return $addr;
}

function linkify($addr, $site) {
	if (is_array($addr)) {
		foreach ($addr as &$k)
			$k = linkify($k, $site);
		return $addr;
	}

	$url = urlify($addr, $site);
	$link = '<a href="'.htmlspecialchars($url).'" class="ext">'.$addr.'</a>';
	return $link;
}


function condLink($cond, $title, $url) {
	$Link = "";
	$Link .= $cond? '<strong>' : '<a href="'.htmlspecialchars($url).'" class="more">';
	$Link .= $title;
	$Link .= $cond? '</strong>' : '</a>';
	return $Link;
}

function gravatar($mail, $size=80, $default="mm") {
	$hash = md5(strtolower(trim($mail)));
	if (@$_SERVER["HTTPS"]) {
		$url = "https://secure.gravatar.com";
	} else {
		$url = "http://www.gravatar.com";
	}
	$url .= "/avatar/$hash?".http_build_query(array("s" => $size, "d" => $default));
	return $url;
}

function H($str) {
	return htmlspecialchars($str, ENT_COMPAT, "UTF-8");
}

// escape all HTML (recursive)
function htmlize($in) {
	if (is_array($in))
		foreach ($in as $k => $v) $in[$k] = htmlize($in[$k]);
	elseif (is_string($in))
		$in = htmlspecialchars($in);
	return $in;
}

function ldap_connect_and_do_things() {
	global $SysErrors;
	global $Auth;

	$ldapH = @ldap_connect(LDAP_URI);
	if (!$ldapH) {
		$SysErrors[] = "Can't connect to LDAP server. Blargh.";
		return false;
	}

	$R = @ldap_set_option($ldapH, LDAP_OPT_PROTOCOL_VERSION, 3);
	if (!$R) {
		$SysErrors[] = "ldap_set_option: ".ldap_errno($ldapH)." - ".ldap_error($ldapH);
		return false;
	}

	if (LDAP_STARTTLS) {
		$R = @ldap_start_tls($ldapH);
		if (!$R) {
			$SysErrors[] = "ldap_start_tls: ".ldap_errno($ldapH)." - ".ldap_error($ldapH);
		}
	}
	
	if (isset($_SERVER["KRB5CCNAME"])) {
		putenv("KRB5CCNAME=".$_SERVER["KRB5CCNAME"]);
		
		$R = @ldap_sasl_bind($ldapH, null, null, "GSSAPI", SASL_REALM);
		if (!$R) {
			$SysErrors[] = "ldap_sasl_bind: ".ldap_errno($ldapH)." - ".ldap_error($ldapH);
			#return false;
		}
		else {
			$Auth = $_SERVER['REMOTE_USER'];
		}
	}
	else {
		// doing anonymous bind
		if (BIND_DN == False || BIND_PASS == False) {
			$R = @ldap_bind($ldapH);
			
			if (!$R) {
				$SysErrors[] = "ldap_bind (anon): ".ldap_errno($ldapH)." - ".ldap_error($ldapH);
				#return false;
			}
		}
		else {
			$R = @ldap_bind($ldapH, BIND_DN, BIND_PASS);
			
			if (!$R) {
				$SysErrors[] = "ldap_bind (dn=".BIND_DN."): ".ldap_errno($ldapH)." - ".ldap_error($ldapH);
				#return false;
			}
		}
		$Auth = false;
	}
	return $ldapH;
}

function has($what) {
	global $Entry;
	if (is_array($what))
		return hasAny($what);
	return isset($Entry[strtolower($what)]);
}

function hasAny($what) {
	foreach ($what as $attr)
		if (has($attr)) return true;
	return false;
}

function hasAll($what) {
	foreach ($what as $attr)
		if (!has($attr)) return false;
	return true;
}

function moreLink($count, $destView) {
	echo "<p class=\"more\">",
		"<a href=\"?q=".htmlspecialchars($_GET["q"])."&amp;view={$destView}\" class=\"more\">",
		($count == 1)? "one more" : "{$count} more",
		"</a>",
		"</p>\n";
}

function attrlevel($name) {
	$name = strtolower($name);
	global $AllAttributes, $Moar;
	return isset($AllAttributes[$name])? $AllAttributes[$name][0] : 1;
}

function checkDisplaySection($attributes) {
	global $Moar;
	$displayed = array();
	$numHidden = 0;
	$numDisplayed = 0;
	foreach ($attributes as $attr) {
		if (has($attr)) {
			if (attrlevel($attr) > 1)
				# permanently hidden
				;
			elseif (!$Moar and attrlevel($attr) > 0)
				$numHidden++;
			else {
				$numDisplayed++;
				$displayed[] = $attr;
			}
		}
	}
	#echo "<pre>"; print_r($attributes); echo "</pre>\n";
	#echo "<pre>"; print_r($section); echo "</pre>\n";
	return array($displayed, $numHidden, $numDisplayed);
}

function checkDisplaySections($attributes) {
	$sections = array();
	$numHidden = 0;
	$numDisplayed = 0;
	foreach ($attributes as $section => $secAttrs) {
		$res = checkDisplaySection($secAttrs);
		$sections[$section] = $res[0];
		
		$numHidden += $res[1];
		$numDisplayed += $res[2];
	}
	return array($sections, $numHidden, $numDisplayed);
}

function implode_e($array) {
	if (count($array) >= 2)
		$last = array_pop($array);
	$s = implode(", ", $array);
	if (isset($last))
		$s .= " and $last";
	return $s;
}

# Damn useful.
function plural($count, $noun) {
	return $count == 1 ? $noun : $noun."s";
}

function trim_domain($fqdn, $domain=".cluenet.org") {
	$domain_len = strlen($domain);
	if (substr($fqdn, -$domain_len) === $domain)
		return substr($fqdn, 0, -$domain_len);
	else
		return $fqdn;
}

?>

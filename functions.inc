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

function fixEntry($entry) {
	$new = array();
	global $SingleAttributes;
	# unset unneeded vars
	unset($entry["count"]);
	for ($i = 0; isset($entry[$i]); $i++) unset($entry[$i]);
	# do st00f
	foreach ($entry as $key => $values) {
		if ($key == "dn") continue;
		elseif (in_array($key, $SingleAttributes)) {
			$new[$key] = $values[0];
		}
		else {
			unset($values["count"]);
			$new[$key] = $values;
		}
	}
	return $new;
}


function getCluePoints($nick) {
	#if (strtolower($nick) == 'grawity') return '∞';
	$nick = escapeshellarg($nick);
	
	# create a temporary cache for anonymous connections
	$ccache = tempnam("/tmp", "krb5cc_ldapsearch_");
	putenv("KRB5CCNAME={$ccache}");
	
	$principal = "anon/scalar.cluenet.org";
	$keytab = "/home/grawity/anon.scalar.cluenet.org.keytab";
	exec("kinit -kt {$keytab} {$principal}");
	
	$out = exec("remctl irc.api.cluenet.org irc cluebot points {$nick}");
	
	@unlink($ccache);
	
	strtok($out, ":"); $points = strtok("");
	$points = (int)$points;
	
	srand(); switch (rand(0, 7)):
	case 1:
		return "0x".str_pad(strtoupper(dechex($points)), 8, "0", 0);
	case 7:
		return base_convert($points, 10, 2);
	default;
		return $points;
	endswitch;
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


function formatSize($bytes, $decimal = false) {
	$symbol = array('B', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y');
	$exp = 0;
	$converted_value = 0;
	if ($bytes > 0) {
	  $exp = floor(log($bytes)/log($decimal?1000:1024));
	  $converted_value = ($bytes/pow($decimal?1000:1024,floor($exp)));
	}
	return sprintf( '%.0f '.$symbol[$exp].($decimal?'':'i').'B', $converted_value );
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


// escape all HTML (recursive)
function htmlize($in) {
	if (is_array($in))
		foreach ($in as $k => $v) $in[$k] = htmlize($in[$k]);
	else
		$in = htmlspecialchars($in);
	return $in;
}


function gravatar($email, $maxSize = 80) {
	return "http://www.gravatar.com/avatar/".md5($email).".jpg?s=".$maxSize;
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

	ldap_start_tls($ldapH);
	
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

?>

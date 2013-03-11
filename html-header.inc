<?php ?>
<!DOCTYPE html>
<?php echo "<!--\n{$LICENSE}\n-->\n"; ?>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	<title><?php if (strlen($Title)) echo "$Title - "; ?>ClueLDAP</title>
	<meta name="author" content="Mantas Mikulėnas">
	<style>
	pre, code {
		font-family: Consolas, "DejaVu Mono", monospace;
	}

	.value.important {
		font-weight: bold;
	}
	.value.error {
		color: red;
	}
	.value.empty {
		font-style: italic;
	}
	</style>
<?php foreach ($Stylesheets as $s):
	$rel = ($s == $Style)? "stylesheet" : "alternate stylesheet";
	echo "\t <link rel=\"{$rel}\" href=\"{$s}.css\" title=\"{$s}\">\n";
endforeach; ?>
</head>
<body>

<?php if ($NumResults >= 0): ?>
<!-- search bar -->
<div class="box">
<form id="searchbar" method="get" action="<?php echo $_SERVER["PHP_SELF"]; ?>">
	<div>
		<input type="search" name="q" id="q" value="<?php echo H($Query) ?>" />
		<input type="submit" value="search" />
	</div>
</form>
</div>
<?php endif; ?>

<?php if (count($SysErrors)): ?>
<!-- errors -->
<div class="box">
<h2 class="error">Something bad happened.</h2>
<ul>
<?php foreach ($SysErrors as $msg)
	echo "\t<li>$msg</li>\n"; ?>
</ul>
</div>
<?php endif; ?>

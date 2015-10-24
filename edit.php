<?php
header("Content-Type: text/html; charset=utf-8");
require_once("classes/CoreKit/Core.inc");
require_once("classes/CoreKit/DBConnection.inc");

$core = Core::getInstance();
$DBConnection = new DBConnection();

if (isset($_GET['delegate'])) {
	$delegate = intval($_GET['delegate']);
	$SQLQuery = "select * from deltagare where id='".$delegate."';";
	$SQLResult = $DBConnection->runQuery($SQLQuery);
	$row = $SQLResult->getRow();
	$name = $row['namn'];
	$troop = $row['kar'];
}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" type="text/css" href="style.css" />
<title>Distriktsstämman 2011</title>
</head>
<body>
<div id="list">
	<h1>Delegat <?=$delegate?></h1>
	<form action="index.php" method="POST">
	  <input type="hidden" name="edit" id="edit" value="<?=$delegate?>" />
	  <table style="width: 600px">
	    <tr>
			<td style="font-weight: bold; width: 300px;">Namn</td>
			<td><input type="text" name="name" id="name" value="<?=$name?>" /></td>
		</tr>
		<tr>
			<td style="font-weight: bold; width: 300px;">Scoutkår</td>
			<td><input type="text" name="troop" id="troop" value="<?=$troop?>" /></td>
	    </tr>
	  </table>
	  <input type="submit" value="Uppdatera" />
	  <input type="button" value="Avbryt" onClick="document.location='index.php'" />
	</form>
</div>
</body>
</html>

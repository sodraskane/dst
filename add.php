<?php
header("Content-Type: text/html; charset=utf-8");
require_once("classes/CoreKit/Core.inc");
require_once("classes/CoreKit/DBConnection.inc");

$core = Core::getInstance();
$DBConnection = new DBConnection();

if (isset($_POST['add'])) {
	// Uppdatera talare
	$delegate = intval($_POST['delegate']);
	$name = addslashes(trim($_POST['name']));
	$troop = addslashes(trim($_POST['troop']));
	if ($speaker > 0 && strlen($name) > 0 && strlen($troop) > 0) {
		$SQLQuery = "update deltagare set "
				  . "namn = '$name', kar = '$troop' where id = $speaker;";
		$SQLResult = $DBConnection->runQuery($SQLQuery);
		if ($SQLResult->getNbrOfAffectedRows() < 1) {
			$SQLQuery = "insert into deltagare values "
					  . "($speaker, '$name', '$troop');";
			$DBConnection->runQuery($SQLQuery);
		}
	}
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
	<h1>Delegat</h1>
	<form action="add.php" method="POST">
	  <input type="hidden" name="add" id="add" value="" />
	  <table style="width: 600px">
	    <tr>
			<td style="font-weight: bold; width: 300px;">Delegatnummer</td>
			<td><input type="text" name="delegate" id="delegate" value="" /></td>
		</tr>
	    <tr>
			<td style="font-weight: bold; width: 300px;">Namn</td>
			<td><input type="text" name="name" id="name" value="" /></td>
		</tr>
		<tr>
			<td style="font-weight: bold; width: 300px;">Scoutkår</td>
			<td><input type="text" name="troop" id="troop" value="<?=$troop?>" /></td>
	    </tr>
	  </table>
	  <input type="submit" value="Lägg till" />
	</form>
</div>
</body>
</html>

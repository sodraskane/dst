<?php
header("Content-Type: text/html; charset=utf-8");
require_once("classes/CoreKit/Core.inc");
require_once("classes/CoreKit/DBConnection.inc");

$core = Core::getInstance();
$DBConnection = new DBConnection();

if (isset($_POST)) {
	// Uppdatera talare
	for ($i = 1; $i < count($_POST['row']); $i++) {
		$name = addslashes(trim($_POST['row'][$i]['name']));
		$troop = addslashes(trim($_POST['row'][$i]['troop']));
		if (strlen($name) > 0 && strlen($troop) > 0) {
			$SQLQuery = "update deltagare set "
					  . "namn = '$name', kar = '$troop' where id = $i;";
			$DBConnection->runQuery($SQLQuery);
		}
	}
}
// Lista talare
$SQLQuery = "select * from deltagare";
$SQLResult = $DBConnection->runQuery($SQLQuery);
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
<form name="fix" id="fix" method="post" action="fixa.php">
<div id="list">
	<h1>Fixa listan</h1>
	<form action="index.php" method="POST">
	  <input type="hidden" name="edit" id="edit" value="<?=$delegate?>" />
	  <table style="width: 600px">
	    <tr>
			<td>Id</td>
			<td>Namn</td>
			<td>Scoutkår</td>
		</tr>
<?php
while ($row = $SQLResult->getRow()) {
?>
		<tr>
			<td><?=$row['id']?></td>
			<td><input type="text" name="row[<?=$row['id']?>][name]" id="name" value="<?=$row['namn']?>" /></td>
			<td><input type="text" name="row[<?=$row['id']?>][troop]" id="troop" value="<?=$row['kar']?>" /></td>
	    </tr>
<?php
}
?>
	  </table>
	  <input type="submit" value="Uppdatera" />
	  <input type="button" value="Avbryt" onClick="document.location='index.php'" />
	</form>
</div>
</form>
</body>
</html>

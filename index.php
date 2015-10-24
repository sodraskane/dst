<?php
header('Content-type: text/html; charset=utf-8');
require_once("classes/CoreKit/Core.inc");
require_once("classes/CoreKit/DBConnection.inc");

$core = Core::getInstance();
$DBConnection = new DBConnection();

if (isset($_POST['speaker'])) {
	// L�gg till nya talare i talarlistan
	$speaker = intval($_POST['speaker']);
	$SQLQuery = "insert into talarlista values (null, $speaker);";
	$DBConnection->runQuery($SQLQuery);
} else if (isset($_GET['remove'])) {
	// Ta bort talare ur listan
	$remove = intval($_GET['remove']);
	$SQLQuery = "delete from talarlista where plats = $remove;";
	$DBConnection->runQuery($SQLQuery);
} else if (isset($_POST['edit'])) {
	// Uppdatera talare
	$speaker = intval($_POST['edit']);
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
<title>Distriktsstämman 2014</title>
</head>
<body>
<div id="new_speaker">
	<form action="index.php" method="post" name="new_speaker">
		Ny talare: <input type="text" name="speaker" style="width: 100px" />
		<input type="submit" name="submit_speaker" value="Lägg till" />
	</form>
</div>
<div style="height: 20px;"></div>
<div id="list">
	<h1>Talarlista</h1>
	<table style="width: 900px;" cellpadding="0" cellspacing="0">
		<tr>
			<td style="font-weight: bold; width: 200px;">Delegatnummer</td>
			<td style="font-weight: bold; width: 250px;">Namn</td>
			<td style="font-weight: bold; width: 250px;">Scoutkår</td>
			<td style="width: 200px;"></td>
		</tr>
<?php
$SQLQuery = "select * from talarlista t left join deltagare d "
		.	"on (t.talare = d.id) order by t.plats asc;";
$SQLResult = $DBConnection->runQuery($SQLQuery);
while ($row = $SQLResult->getRow()) {
?>
		<tr>
			<td><?=$row['talare']?></td>
			<td><?=$row['namn']?></td>
			<td><?=$row['kar']?></td>
			<td>
			  <a href="edit.php?delegate=<?=$row['talare']?>">[Ändra]</a> |
			  <a href="index.php?remove=<?=$row['plats']?>">[Ta bort]</a>
			</td>
		</tr>
<?php
}
		// Visa hela talarlistan, med borttagning
?>
	</table>
</div>
</body>
</html>

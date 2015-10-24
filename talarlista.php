<?php
header("Content-Type: text/html; charset=utf-8");
require_once("classes/CoreKit/Core.inc");
require_once("classes/CoreKit/DBConnection.inc");

$core = Core::getInstance();
$DBConnection = new DBConnection();
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="refresh" content="1" />
<link rel="stylesheet" type="text/css" href="style.css" />
<title>Distriktsstämman 2014</title>
</head>
<body>
<div style="height: 20px;"></div>
<div id="list">
	<h1>Talarlista</h1>
	<table style="width: 900px;" cellpadding="0" cellspacing="0">
		<tr>
			<td style="font-weight: bold; width: 130px;">Delegat</td>
			<td style="font-weight: bold; width: 230px;">Namn</td>
			<td style="font-weight: bold; width: 400px;">Scoutkår</td>
			<td style="width: 100px;"></td>
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
			<td></td>
		</tr>
<?php
}
		// Visa hela talarlistan, med borttagning
?>
	</table>
</div>
</body>
</html>

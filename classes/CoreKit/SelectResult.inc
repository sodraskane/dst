<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/SQLResult.inc");

/**
 *	Klassen SelectResult inneh�ller den information som ges av databashanteraren
 *	d� en fr�ga av typen SELECT, SHOW, DESCRIBE, EXPLAIN eller liknande.
 *
 *	@version	Version 		0.1.0\n
 *				Skapad: 		2005-12-06\n
 *				Senast �ndrad: 	2005-12-06
 *	@author 	Johan Holmberg, johan@yamborii.net
 */
class SelectResult extends SQLResult {
	private $_resultSet;

	/**
	 *	Skapar en instans av SelectResult.
	 *
	 *	@throws Kastar ett Y_IllegalValueException om angivna parametrar var
	 *			ogiltiga.
	 *
	 *	@param	$connection �r en referens till det DBConnection som genererade
	 *			detta SQLResult.
	 *	@param	$SQLQuery �r den fr�ga som st�lldes till databashanteraren.
	 *	@param	$resultSet �r den resurs som skickades tillbaka fr�n
	 *			databashanteraren.
	 */
	public function __construct($connection, $SQLQuery, $resultSet) {
		if (!is_resource($resultSet))
			throw new Y_IllegalValueException('A resultset was expected');
		
		parent::__construct($connection, $SQLQuery);
		
		$this->_resultSet = $resultSet;
	}
      
    /**
     *	Ta fram n�sta rad fr�n databasfr�gan. P� varje v�rde
     *	i den resulterande vektorn kommer stripslashes() att k�ras.
	 *
	 *	@return Returnerar en vektor inneh�llandes datan fr�n en rad ur
	 *			svaret fr�n en SQL-fr�ga. Om slutet av resultatet �r n�tt,
	 *			eller om resultatet �r tomt, resulteras falskt.
	 */
	public function getRow() {
		$returnResult = false;
		
		$returnResult = mysql_fetch_array($this->_resultSet);
		if ($returnResult) {
			array_map("stripslashes", $returnResult);
		}
		
		return $returnResult;
	}
	
	/**
	 *	G�r om resultatet fr�n databasf�rfr�gningen till en
	 *	anv�ndarbar datastruktur.
	 *
	 *	@return	Returnerar en vektor av vektorer eller null.
	 */
	public function getRowsAsArray() {
		$returnResult = null;
		
		while ($row = mysql_fetch_array($this->_resultSet)) {
			$returnResult[] =& array_map("stripslashes", $row);
		}
		
		return $returnResult;
	}

	/**
	 *	Tar reda p� antalet rader som returnerades fr�n databashanteraren.
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getNbrOfRows() {
		return intval(mysql_num_rows($this->_resultSet));
	}
}
?>
<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/SQLResult.inc");

/**
 *	Klassen SelectResult innehåller den information som ges av databashanteraren
 *	då en fråga av typen SELECT, SHOW, DESCRIBE, EXPLAIN eller liknande.
 *
 *	@version	Version 		0.1.0\n
 *				Skapad: 		2005-12-06\n
 *				Senast ändrad: 	2005-12-06
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
	 *	@param	$connection är en referens till det DBConnection som genererade
	 *			detta SQLResult.
	 *	@param	$SQLQuery är den fråga som ställdes till databashanteraren.
	 *	@param	$resultSet är den resurs som skickades tillbaka från
	 *			databashanteraren.
	 */
	public function __construct($connection, $SQLQuery, $resultSet) {
		if (!is_resource($resultSet))
			throw new Y_IllegalValueException('A resultset was expected');
		
		parent::__construct($connection, $SQLQuery);
		
		$this->_resultSet = $resultSet;
	}
      
    /**
     *	Ta fram nästa rad från databasfrågan. På varje värde
     *	i den resulterande vektorn kommer stripslashes() att köras.
	 *
	 *	@return Returnerar en vektor innehållandes datan från en rad ur
	 *			svaret från en SQL-fråga. Om slutet av resultatet är nått,
	 *			eller om resultatet är tomt, resulteras falskt.
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
	 *	Gör om resultatet från databasförfrågningen till en
	 *	användarbar datastruktur.
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
	 *	Tar reda på antalet rader som returnerades från databashanteraren.
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getNbrOfRows() {
		return intval(mysql_num_rows($this->_resultSet));
	}
}
?>
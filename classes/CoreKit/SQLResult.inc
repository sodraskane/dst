<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");

/**
 *	Klassen SQLResult agerar superklass till klasserna SelectResult och
 *	UpdateResult, vilka ges som svar av DBConnection n�r en fr�ga st�lls
 *	till databashanteraren.
 *
 *	@version	Version 		0.0.2\n
 *				Skapad: 		2005-12-06\n
 *				Senast �ndrad: 	2011-10-17
 *	@author 	Johan Holmberg, johan@yamborii.net
 */
abstract class SQLResult {
	protected $_connection;
	protected $_query;
	
	/**
	 *	Skapar en instans av SQLResult.
	 *
	 *	@throws Kastar ett Y_IllegalValueException om angivna parametrar var
	 *			ogiltiga.
	 *
	 *	@param	$connection �r en referens till det DBConnection som genererade
	 *			detta SQLResult.
	 *	@param	$SQLQuery �r den fr�ga som st�lldes till databashanteraren.
	 */
	public function __construct($connection, $SQLQuery) {
		if (!is_a($connection, 'DBConnection'))
			throw new Y_IllegalValueException('A DBConnection was expected');
		if (!is_string($SQLQuery))
			throw new Y_IllegalValueException('A string was expected');
			
		$this->_connection = $connection;
		$this->_query = $SQLQuery;
	}
	
	/**
	 *	Skickar tillbaka en referens till det DBConnection-objekt som skapade
	 *	detta SQLResult-objekt.
	 *
	 *	@return	Returnerar ett DBConnection-objekt.
	 */
	public function getConnection() {
		return $this->_connection;
	}
	
	/**
	 *	Skickar tillbaka den fr�ga som st�lldes till databasen.
	 *
	 *	@return	Returnerar SQL-fr�gan som en str�ng.
	 */
	public function getQuery() {
		return $this->_query;
	}
	
		/**
	 *	Tar reda p� antalet rader som p�verkades av runQuery().
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getNbrOfAffectedRows() {
		return mysql_affected_rows($this->_conn);
	}
}
?>
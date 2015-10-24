<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/SQLResult.inc");

/**
 *	Klassen ModifyResult innehåller den information som ges av databashanteraren
 *	då en fråga av typen INSERT, UPDATE, DELETE eller liknande.
 *
 *	@version	Version 		0.1.1\n
 *				Skapad: 		2005-12-06\n
 *				Senast ändrad: 	2005-12-12
 *	@author 	Johan Holmberg, johan@yamborii.net
 */
class ModifyResult extends SQLResult {
	private $_id;
	private $_rows;

	/**
	 *	Skapar en instans av UpdateResult.
	 *
	 *	@param	$connection är en referens till det DBConnection som genererade
	 *			detta SQLResult.
	 *	@param	$SQLQuery är den fråga som ställdes till databashanteraren.
	 */
	public function __construct($connection, $SQLQuery) {
		parent::__construct($connection, $SQLQuery);
		
		$this->_id = mysql_insert_id($connection->getConnection());
		$this->_rows = mysql_affected_rows($connection->getConnection());
	}
	
	/**
	 *	Tar reda på vilket ID-nummer den senast insatta raden fick.
	 *	Om frågan inte var av INSERT-slaget, returneras istället 0.
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getID() {
		return intval($this->_id);
	}
	  
	/**
	 *	Tar reda på antalet rader som påverkades av den senaste frågan.
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getNbrOfAffectedRows() {
		return intval($this->_rows);
	}
}
?>
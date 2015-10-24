<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/SQLResult.inc");

/**
 *	Klassen ModifyResult inneh�ller den information som ges av databashanteraren
 *	d� en fr�ga av typen INSERT, UPDATE, DELETE eller liknande.
 *
 *	@version	Version 		0.1.1\n
 *				Skapad: 		2005-12-06\n
 *				Senast �ndrad: 	2005-12-12
 *	@author 	Johan Holmberg, johan@yamborii.net
 */
class ModifyResult extends SQLResult {
	private $_id;
	private $_rows;

	/**
	 *	Skapar en instans av UpdateResult.
	 *
	 *	@param	$connection �r en referens till det DBConnection som genererade
	 *			detta SQLResult.
	 *	@param	$SQLQuery �r den fr�ga som st�lldes till databashanteraren.
	 */
	public function __construct($connection, $SQLQuery) {
		parent::__construct($connection, $SQLQuery);
		
		$this->_id = mysql_insert_id($connection->getConnection());
		$this->_rows = mysql_affected_rows($connection->getConnection());
	}
	
	/**
	 *	Tar reda p� vilket ID-nummer den senast insatta raden fick.
	 *	Om fr�gan inte var av INSERT-slaget, returneras ist�llet 0.
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getID() {
		return intval($this->_id);
	}
	  
	/**
	 *	Tar reda p� antalet rader som p�verkades av den senaste fr�gan.
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getNbrOfAffectedRows() {
		return intval($this->_rows);
	}
}
?>
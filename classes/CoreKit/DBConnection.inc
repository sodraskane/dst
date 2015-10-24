<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/main/init.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_DBConnectionException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_SQLException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/ModifyResult.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/SelectResult.inc");

/**
 *	Klassen DBConnection �r en del av Y2-systemets abstraktionslager f�r
 *	databashantering. Denna klass ska alltid anv�ndas n�r ett php-script
 *	pratar med databasen. Alla anrop till en given databas sker genom en
 *	instans av klassen: en f�r varje databas-server.
 *
 *	@version	Version 		0.2.1\n
 *				Skapad: 		2005-12-06\n
 *				Senast �ndrad: 	2005-12-21
 *	@author 	Johan Holmberg, johan@yamborii.net
 */
class DBConnection {
	private $_username;   	// Login f�r anslutning till servern
	private $_password;    	// L�sen f�r anslutning till servern
	private $_hostname;    	// SQL-serverns placering
	private $_connection;  	// Koppling till databasservern
	private $_schema;		// Databasens namn

	/**
	 *	Skapar en instans av DBConnection. Inga parametrar �r n�dv�ndiga,
	 *	men g�r att ge. Ges inga parametrar, kommer default-v�rdena att
	 *	anv�ndas.
	 *
	 *	@throws Y_IllegalValueException om indatan var felaktig.
	 *
	 *	@param	$username �r anv�ndarnamnet som anv�nds f�r att logga in p�
	 *			databasservern. En str�ng.
	 *	@param	$password �r l�senordet som anv�nds f�r att logga in p�
	 *			databasservern. En str�ng.
	 *	@param	$hostname �r v�rddatorns adress. En str�ng.
	 *	@param	$schema �r namnet p� det databasschema vi vill anv�nda. En str�ng.
	 */
	public function __construct($username = '', $password = '',
								$hostname = '', $schema = '') {
		if (!is_string($username))
			throw new Y_IllegalValueException('Expected a valid username');
		if (!is_string($password))
			throw new Y_IllegalValueException('Expected a valid password');
		if (!is_string($hostname))
			throw new Y_IllegalValueException('Expected a valid hostname');
		if (!is_string($schema))
			throw new Y_IllegalValueException('Expected a valid schema');
		
		global $_YINIT;
		
		if (strlen($username) > 0) {
			$this->_username = $username;
			$this->_password = $password;
			$this->_hostname = $hostname;
			$this->_schema = $schema;
		} else {
			$this->_username = $_YINIT['core_db_login'];
			$this->_password = $_YINIT['core_db_password'];
			$this->_hostname = $_YINIT['core_db_host'];
			$this->_schema = $_YINIT['core_db_database'];
		}
		$this->_connection = $this->_accessDB();
	}

	/**
	 *	St�nger kopplingen mot servern.
	 */
	public function __destruct() {
		//mysql_close();
	}
	
	/**
	 *	Skickar tillbaka namnet p� databasservern.
	 *
	 *	@return	Returnerar servernamnet som en str�ng.
	 */
	public function getHostname() {
		return $this->_hostname;
	}
	
	/**
	 *	Skickar tillbaka namnet p� aktuellt schema.
	 *
	 *	@return	Returnerar schemats namn som en str�ng.
	 */
	public function getSchema() {
		return $this->_schema;
	}
	
	/**
	 *	Skickar tillbaka kopplingen.
	 *
	 *	@return	Returnerar kopplingens id-nummer.
	 */
	public function getConnection() {
		return $this->_connection;
	}

	/**
	 *	G�r en f�rfr�gan till databasen.\n
	 *
	 *	Kom ih�g att k�ra addslashes() p� alla str�ngar som ska skrivas
	 *	till databasen. Detta f�r att f�rhindra SQL-injektioner.
	 *
	 *	@throws	Y_SQLException om fr�gan var ogilitig.
	 *
	 *	@param	$SQLQuery �r en giltig SQL-fr�ga.
	 *
	 *	@return Returnerar ett SQLResult-objekt. Detta objekt �r egentligen
	 *			subklassat som SelectResult och ModifyResult, och vilket
	 *			objekt du f�r, beror p� vilken typ av fr�ga du st�llde.
	 */
	public function runQuery($SQLQuery) {
		$returnResult = null;
		$result = false;
		
		if (is_string($SQLQuery) && strlen(trim($SQLQuery)) > 0) {
			$SQLQuery = trim($SQLQuery);
			$result = mysql_query($SQLQuery, $this->_connection);
			if (!$result) {
				// Fel i fr�gan! Kasta undantag!
				throw new Y_SQLException('Illegal query!');
			} else if (is_resource($result)) {
				// En "select"-fr�ga. Skapa ett SelectResult.
				$returnResult = new SelectResult($this, $SQLQuery, $result);
			} else {
				// En "modify"-fr�ga. Skapa ett ModifyResult.
				$returnResult = new ModifyResult($this, $SQLQuery);
			}
		} else {
			throw new Y_SQLException('Illegal query!');
		}
		
		return $returnResult;
	}
	  
	  ############################################
	 #				Privata funktioner			#
	############################################
	  

	/**
	 *	Skapar en koppling till en MySQL-server.
	 *	@private
	 *
	 *	@throws Kastar ett Y_DBConnectionException om uppkopplingen misslyckades.
	 */
	private function _accessDB(){
		$result = null;    //Den resulterande SQL-kopplingen

		$result = mysql_pconnect($this->_hostname, $this->_username,
								 $this->_password);
		mysql_query('SET NAMES utf8');
		if (!$result)
			throw new Y_DBConnectionException("Unable to connect to server.");
		if (!mysql_select_db($this->_schema, $result))
			throw new Y_DBConnectionException("Unable to connect to server.");

		return $result;
	}
}
?>
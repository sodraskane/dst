<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_DBConnectionException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/Singleton.inc");

/**
 *	Klassen DBHandler �r Y2-systemets abstraktionslager f�r
 *	databashantering. Denna klass ska alltid anv�ndas n�r ett
 *	php-script pratar med databasen.
 *
 *	Klassen �r en Singleton-klass, och man n�r den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		2.2.0\n
 *				Skapad: 		2002-05-15\n
 *				Senast �ndrad: 	2005-07-17
 *	@author 	Johan Holmberg, johan@yamborii.net
 */
class DBHandler implements Singleton {
	private static $_instance;
	
	private $_login;   	// Login f�r anslutning till MySQL
	private $_pass;    	// L�sen f�r anslutning till MySQL
	private $_host;    	// MySQL-serverns placering
	private $_conn;    	// Koppling till MySQL-servern
	private $_database;	// Databasens namn
	private $_data;		// Resultatet fr�n den senaste f�rfr�gan

	/**
	 *	Skapar ett objekt av DBHandler. Inga parametrar �r
	 *	n�dv�ndiga, men g�r att ge.\n
	 *	Om inga tidigare instanser skapats av denna klass,
	 *	s�tts den globala instansen h�r.
	 */
	private function __construct() {
		$this->setUp();
	}
	
	/**
	 *	Returnera instansen av DBHandler.
	 *
	 *	@return Singleton-instansen av DBHandler.
	 */
	public static function getInstance() {
       if (!isset(self::$_instance)) {
           $c = __CLASS__;
           self::$_instance = new $c;
       }

       return self::$_instance;
	}
	
	/**
	 *	S�tter upp en uppkoppling mot en server.
	 *
	 *	@param $login �r ett loginnamn p� databasservern.
	 *	@param $pass �r ett l�senord p� databasservern.
	 *	@param $host �r ip-adressen till databasservern.
	 *	@param $database �r den databas vi vill arbeta mot.
	 */
	public function setUp($login = '', $pass = '',
						  $host = '', $database = '') {
		global $_YINIT;
		mysql_set_charset('utf8');
		
		if (is_string($login) && strlen($login) > 0) {
			$this->_login = $login;
			$this->_pass = $pass;
			$this->_host = $host;
			$this->_database = $database;
		} else {
			$this->_login = $_YINIT['core_db_login'];
			$this->_pass = $_YINIT['core_db_password'];
			$this->_host = $_YINIT['core_db_host'];
			$this->_database = $_YINIT['core_db_database'];
		}
		$this->_conn = $this->_accessDB();
	}

	/**
	 *	G�r en f�rfr�gan till databasen. Metoden sparar undan giltiga
	 *	resultat lokalt i DBHandler-instansen f�r att enklare kunna
	 *	hantera resultatet. Detta �r det normala s�ttet att anv�nda
	 *	klassen.\n
	 *	Det g�r �ven att spara undan resultatet utanf�r instansen,
	 *	vilket �r l�mpligt om man vill arbeta med flera SQL-fr�gor
	 *	samtidigt. Detta �r dock inte att rekommendera, eftersom
	 *	koden d� blir mer sv�rl�st.\n
	 *	Kom ih�g att k�ra addslashes() p� alla str�ngar som ska skrivas
	 *	till databasen. Detta f�r att f�rhindra SQL-injektioner.
	 *
	 *	@param	$SQLQuery �r en giltig SQL-fr�ga.
	 *
	 *	@return Returnerar \n
	 *			x ett resurs-id om fr�gan �r av typen SELECT, SHOW,
	 *			DESCRIBE, EXPLAIN och giltig\n
	 *			x sant om fr�gan �r av typen INSERT, UPDATE, DELETE
	 *			eller liknande och giltig\n
	 *			x annars falskt.
	 */
	public function runQuery($SQLQuery) {
		$returnResult = false;
		
		if (is_string($SQLQuery) && strlen(trim($SQLQuery)) > 0) {
			unset($this->_data);
			$SQLQuery = trim($SQLQuery);
			$returnResult = mysql_query($SQLQuery, $this->_conn);
			if ($returnResult) {
				$this->_data = $returnResult;
			}
		}
		
		return $returnResult;
	}
      
	/**
	 *	Tar reda p� vilket ID-nummer den senast insatta raden fick.
	 *	Om den senaste fr�gan inte var av INSERT-slaget, returneras
	 *	ist�llet 0.
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getID() {
		$id = mysql_insert_id($this->_conn);
		
		return $id;
	}
      
    /**
     *	Ta fram n�sta rad fr�n den senaste databasfr�gan. P� varje v�rde
     *	i den resulterande vektorn kommer stripslashes() att k�ras.
	 *
	 *	@param	$data �r den resurs som returnerades fr�n runQuery().
	 *			Skickas ingen parameter, anv�nds det senaste resultatet.
	 *
	 *	@return Returnerar en vektor inneh�llandes datan fr�n en rad ur
	 *			svaret fr�n en SQL-fr�ga. Om slutet av resultatet �r n�tt,
	 *			eller om resultatet �r tomt, resulteras falskt.
	 */
	public function getRow($data = false) {
		$returnResult = false;
		
		if (is_resource($data)) {
			$returnResult = mysql_fetch_array($data);
		} else if (!$data) {
			$returnResult = mysql_fetch_array($this->_data);
		}
		if ($returnResult) {
			array_map("stripslashes", $returnResult);
		}
		
		return $returnResult;
	}
	
	/**
	 *	G�r om resultatet fr�n databasf�rfr�gningen till en
	 *	anv�ndarbar datastruktur.
	 *
	 *	@param	$data �r det heltal som returnerades fr�n runQuery().
	 *
	 *	@return	Returnerar en vektor av vektorer eller null.
	 */
	public function getRowsAsArray($data = false) {
		$returnResult = null;
		
		if ($data) {
			while ($row = mysql_fetch_array($data)) {
				$returnResult[] =& array_map("stripslashes", $row);
			}
		} else {
			while ($row = mysql_fetch_array($this->_data)) {
				$returnResult[] =& array_map("stripslashes", $row);
			}
		}
		
		return $returnResult;
	}

	/**
	 *	St�nger kopplingen mot servern.
	 */
	public function closeDB() {
		mysql_close();
	}

	/**
	 *	Tar reda p� antalet rader som returnerades fr�n runQuery().
	 *
	 *	@param $data �r det heltal som �r resultatet fr�n runQuery().
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getNbrOfRows($data = false) {
		$returnResult = 0;
		if ($data) {
			if (is_resource($data)) {
				$returnResult = intval(mysql_num_rows($data));
			}
		} else {
			$returnResult = intval(mysql_num_rows($this->_data));
		}
		
		return $returnResult;
	}
	  
	/**
	 *	Tar reda p� antalet rader som p�verkades av runQuery().
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getNbrOfAffectedRows() {
		return mysql_affected_rows($this->_conn);
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

		$result = mysql_pconnect($this->_host, $this->_login, $this->_pass);
		mysql_query('SET NAMES utf8');
		if (!$result)
			throw new Y_DBConnectionException("Unable to connect to server.");
		if (!mysql_select_db($this->_database, $result))
			throw new Y_DBConnectionException("Unable to connect to server.");

		return $result;
	}
}
?>
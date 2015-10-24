<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_DBConnectionException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/Singleton.inc");

/**
 *	Klassen DBHandler är Y2-systemets abstraktionslager för
 *	databashantering. Denna klass ska alltid användas när ett
 *	php-script pratar med databasen.
 *
 *	Klassen är en Singleton-klass, och man når den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		2.2.0\n
 *				Skapad: 		2002-05-15\n
 *				Senast ändrad: 	2005-07-17
 *	@author 	Johan Holmberg, johan@yamborii.net
 */
class DBHandler implements Singleton {
	private static $_instance;
	
	private $_login;   	// Login för anslutning till MySQL
	private $_pass;    	// Lösen för anslutning till MySQL
	private $_host;    	// MySQL-serverns placering
	private $_conn;    	// Koppling till MySQL-servern
	private $_database;	// Databasens namn
	private $_data;		// Resultatet från den senaste förfrågan

	/**
	 *	Skapar ett objekt av DBHandler. Inga parametrar är
	 *	nödvändiga, men går att ge.\n
	 *	Om inga tidigare instanser skapats av denna klass,
	 *	sätts den globala instansen här.
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
	 *	Sätter upp en uppkoppling mot en server.
	 *
	 *	@param $login är ett loginnamn på databasservern.
	 *	@param $pass är ett lösenord på databasservern.
	 *	@param $host är ip-adressen till databasservern.
	 *	@param $database är den databas vi vill arbeta mot.
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
	 *	Gör en förfrågan till databasen. Metoden sparar undan giltiga
	 *	resultat lokalt i DBHandler-instansen för att enklare kunna
	 *	hantera resultatet. Detta är det normala sättet att använda
	 *	klassen.\n
	 *	Det går även att spara undan resultatet utanför instansen,
	 *	vilket är lämpligt om man vill arbeta med flera SQL-frågor
	 *	samtidigt. Detta är dock inte att rekommendera, eftersom
	 *	koden då blir mer svårläst.\n
	 *	Kom ihåg att köra addslashes() på alla strängar som ska skrivas
	 *	till databasen. Detta för att förhindra SQL-injektioner.
	 *
	 *	@param	$SQLQuery är en giltig SQL-fråga.
	 *
	 *	@return Returnerar \n
	 *			x ett resurs-id om frågan är av typen SELECT, SHOW,
	 *			DESCRIBE, EXPLAIN och giltig\n
	 *			x sant om frågan är av typen INSERT, UPDATE, DELETE
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
	 *	Tar reda på vilket ID-nummer den senast insatta raden fick.
	 *	Om den senaste frågan inte var av INSERT-slaget, returneras
	 *	istället 0.
	 *
	 *	@return Returnerar ett heltal.
	 */
	public function getID() {
		$id = mysql_insert_id($this->_conn);
		
		return $id;
	}
      
    /**
     *	Ta fram nästa rad från den senaste databasfrågan. På varje värde
     *	i den resulterande vektorn kommer stripslashes() att köras.
	 *
	 *	@param	$data är den resurs som returnerades från runQuery().
	 *			Skickas ingen parameter, används det senaste resultatet.
	 *
	 *	@return Returnerar en vektor innehållandes datan från en rad ur
	 *			svaret från en SQL-fråga. Om slutet av resultatet är nått,
	 *			eller om resultatet är tomt, resulteras falskt.
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
	 *	Gör om resultatet från databasförfrågningen till en
	 *	användarbar datastruktur.
	 *
	 *	@param	$data är det heltal som returnerades från runQuery().
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
	 *	Stänger kopplingen mot servern.
	 */
	public function closeDB() {
		mysql_close();
	}

	/**
	 *	Tar reda på antalet rader som returnerades från runQuery().
	 *
	 *	@param $data är det heltal som är resultatet från runQuery().
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
	 *	Tar reda på antalet rader som påverkades av runQuery().
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
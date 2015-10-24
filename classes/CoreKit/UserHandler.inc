<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchUserException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/Singleton.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/DBConnection.inc");

/**
 *	UserHandler har hand om den grundl�ggande hanteringen av anv�ndarna.
 *	Icke-grundl�ggande anv�ndarhantering sk�ts av klasser i SupportKit.
 *
 *	Klassen anv�nder sig av tabellen core_users_users
 *
 *	Klassen �r en Singleton-klass, och man n�r den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		0.6.1\n
 *				Skapad: 		2005-06-13\n
 *				Senast �ndrad: 	2005-12-12
 *	@author		Johan Holmberg, johan@yamborii.net
 */
class UserHandler implements Singleton {
	private static $_instance;
	
	private $_DBConnection;
	
	/**
	 *	Skapar en instans av UserHandler
	 */
	private function __construct() {
		global $_YINIT;
		if (isset($_YINIT['DBConnection'])) {
			$this->_DBConnection = $_YINIT['DBConnection'];
		} else {
			$this->_DBConnection = new DBConnection();
		}
	}
	
	/**
	 *	Returnerar instansen av UserHandler.
	 *
	 *	@return Singleton-instansen av UserHandler.
	 */
	public static function getInstance() {
       if (!isset(self::$_instance)) {
           $c = __CLASS__;
           self::$_instance = new $c;
       }

       return self::$_instance;
	}

	/**
	 *	L�gger till en anv�ndare om ingen anv�ndare med �nskat namn finns.
	 *
	 *	@param	$username �r det �nskade namnet. En str�ng p� minst ett och h�gst
	 *			45 tecken, exklusive inledande och avslutande whitespaces. Tab och
	 *			newlines �r ej till�tna.
	 *	@param	$password �r det �nskade l�senordet. En str�ng p� minst fem och h�gst
	 *			45 tecken, exklusive inledande och avslutande whitespaces. Tab och
	 *			newlines �r ej till�tna.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar den nya anv�ndarens id-nummer om anv�ndaren lades till,
	 *			annars falskt.
	 */
	public function addUser($username, $password) {
		$returnResult = false;
		
		if (is_string($username) && !strpos($username, "\n") &&
				!strpos($username, "\t") &&
			is_string($password) && !strpos($password, "\n") &&
				!strpos($password, "\t")) {
			$username = addslashes(trim($username));
			$password = addslashes(trim($password));
			if (strlen($username) > 0 && strlen($username) < 46 &&
				strlen($password) > 4 && strlen($password) < 46) {
				$SQLQuery = 'INSERT INTO core_users_users '
						.	"VALUES (NULL, '$username', '$password', 'y');";
				$result = $this->_DBConnection->runQuery($SQLQuery);
				if ($result->getNbrOfAffectedRows() > 0) {
					$returnResult = $result->getId();
				}
			} else {
				throw new Y_IllegalValueException('Parameters of wrong size');
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	�ndrar en anv�ndares anv�ndarnamn.
	 *
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *	@param	$newUsername �r det nya anv�ndarnamnet. En str�ng p� minst ett och
	 *			h�gst 45 tecken, exklusive inledande och avslutande whitespaces.
	 *			Tab och newlines �r ej till�tna.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om anv�ndarnamnet �ndrades, annars falskt.
	 */
	public function alterUsername($userid, $newUsername) {
		$returnResult = false;
		
		if (is_int($userid) && $userid > 0 &&
			is_string($newUsername) && !strpos($newUsername, "\n") &&
				!strpos($newUsername, "\t")) {
			$newUsername = addslashes(trim($newUsername));
			if (strlen($newUsername) > 0 && strlen($newUsername) < 46) {
				$SQLQuery = 'UPDATE core_users_users '
						.	"SET username = '$newUsername' "
						.	"WHERE id = $userid;";
				$result = $this->_DBConnection->runQuery($SQLQuery);
				$returnResult = $result->getNbrOfAffectedRows() > 0;
				if (!$returnResult) {
					throw new Y_NoSuchUserException('No such user in database.');
				}
			} else {
				throw new Y_IllegalValueException('Username of wrong size');
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	�ndrar en anv�ndares l�senord.
	 *
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *	@param	$newPassword �r det nya l�senordet. En str�ng p� minst fem och h�gst
	 *			45 tecken, exklusive inledande och avslutande whitespaces. Tab och
	 *			newlines �r ej till�tna.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om l�senordet �ndrades, annars falskt.
	 */
	public function alterPassword($userid, $newPassword) {
		$returnResult = false;
		
		if (is_int($userid) && $userid > 0 &&
			is_string($newPassword) && !strpos($newPassword, "\n") &&
				!strpos($newPassword, "\t")) {
			$newPassword = addslashes(trim($newPassword));
			if (strlen($newPassword) > 4 && strlen($newPassword) < 46) {
				$SQLQuery = 'UPDATE core_users_users '
						.	"SET password = '$newPassword' "
						.	"WHERE id = $userid;";
				$result = $this->_DBConnection->runQuery($SQLQuery);
				$returnResult = $result->getNbrOfAffectedRows() > 0;
				if (!$returnResult) {
					throw new Y_NoSuchUserException('No such user in database.');
				}
			} else {
				throw new Y_IllegalValueException('Password of wrong size');
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar bort en anv�ndare.
	 *
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om anv�ndaren togs bort, annars falskt.
	 */
	public function removeUser($userid) {
		$returnResult = false;
		
		if (is_int($userid) && $userid > 0) {
			$SQLQuery = 'DELETE FROM core_users_users '
					.	"WHERE id = $userid;";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = $result->getNbrOfAffectedRows() > 0;
			if (!$returnResult) {
				throw new Y_NoSuchUserException('No such user in database.');
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda p� en anv�ndares anv�ndarnamn.
	 *
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar anv�ndarens anv�ndarnamn om anv�ndaren fanns, annars
	 *			falskt.
	 */
	public function getUsername($userid) {
		$returnResult = false;
		
		if (is_int($userid) && $userid > 0) {
			$SQLQuery = 'SELECT username '
					.	'FROM core_users_users '
					.	"WHERE id = $userid "
					.	'LIMIT 1;';
			$result = $this->_DBConnection->runQuery($SQLQuery);
			if ($result->getNbrOfRows() == 1) {
				$row = $result->getRow();
				$returnResult = stripslashes($row['username']);
			} else {
				throw new Y_NoSuchUserException('No such user in database.');
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda p� anv�ndarens id-nummer.
	 *
	 *	@param	$username �r anv�ndarens anv�ndarnamn. En str�ng.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar anv�ndarens id-nummer om namnet fanns, annars falskt.
	 */
	public function getUserId($username) {
		$returnResult = false;
		
		if (is_string($username) && !strpos($username, "\n") &&
				!strpos($username, "\t")) {
			$username = addslashes(trim($username));
			if (strlen($username) > 0 && strlen($username) < 46) {
				$SQLQuery = 'SELECT id '
						.	'FROM core_users_users '
						.	"WHERE username = '$username' "
						.	'LIMIT 1;';
				$result = $this->_DBConnection->runQuery($SQLQuery);
				if ($result->getNbrOfRows() == 1) {
					$row = $result->getRow();
					$returnResult = intval($row['id']);
				} else {
					throw new Y_NoSuchUserException('No such user in database.');
				}
			} else {
				throw new Y_IllegalValueException('Illegal username');
			}
		} else {
			throw new Y_IllegalValueException('Illegal username');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Aktiverar en anv�ndare.
	 *
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om anv�ndaren aktiverades, annars falskt.
	 */
	public function activateUser($userid) {
		$returnResult = false;
		
		if (is_int($userid) && $userid > 0) {
			$SQLQuery = 'UPDATE core_users_users '
					.	"SET active = 'y' "
					.	"WHERE id = $userid;";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult =$result->getNbrOfAffectedRows() == 1;
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Deaktiverar en anv�ndare.
	 *
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om anv�ndaren deaktiverades, annars falskt.
	 */
	public function deactivateUser($userid) {
		$returnResult = false;
		
		if (is_int($userid) && $userid > 0) {
			$SQLQuery = 'UPDATE core_users_users '
					.	"SET active = 'n' "
					.	"WHERE id = $userid;";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult =$result->getNbrOfAffectedRows() == 1;
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda p� om en anv�ndare existerar.
	 *
	 *	@param	$userid �r anv�ndarens id-nummer eller anv�ndarnamn. Ett heltal
	 *			st�rre �n noll eller en str�ng.
	 *	@param	$onlyActive anger om anv�ndaren i fr�ga ska s�kas bland aktiva
	 *			eller inte. Anges sant, returneras sant enbart om anv�ndaren finns
	 *			och �r aktiv. Anges falskt, returneras sant oavsett om anv�ndaren
	 *			�r aktiv eller inte, s� l�nge den finns i databasen.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om anv�ndaren fanns i databasen, annars falskt.
	 */
	public function userExists($userid, $onlyActive = true) {
		// TODO fixa enhetstesterna s� att dessa avspeglar �ndringarna!
		$returnResult = false;
		
		if (is_int($userid) && $userid > 0 && is_bool($onlyActive)) {
			$SQLQuery = 'SELECT id '
					.	'FROM core_users_users '
					.	"WHERE id = $userid";
			if ($onlyActive) {
				$SQLQuery .= " AND active = 'y'";
			}
			$SQLQuery .= ' LIMIT 1;';
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult =$result->getNbrOfRows() == 1;
		} else if (is_string($userid) && strlen(trim($userid)) > 0
					&& is_bool($onlyActive)) {
			$userid = addslashes(trim($userid));
			$SQLQuery = 'SELECT id '
					.	'FROM core_users_users '
					.	"WHERE username = '$userid'";
			if ($onlyActive) {
				$SQLQuery .= " AND active = 'y'";
			}
			$SQLQuery .= ' LIMIT 1;';
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult =$result->getNbrOfRows() == 1;
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Validerar en anv�ndares anv�ndarnamn och l�senord.
	 *
	 *	@param	$username �r anv�ndarens anv�ndarnamn. En str�ng.
	 *	@param	$password �r anv�ndarens l�senord. En str�ng.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar anv�ndarens id-nummer om namn och l�sen st�mmer,
	 *			annars falskt.
	 */
	public function validateUser($username, $password) {
		$returnResult = false;
		
		if (is_string($username) && !strpos($username, "\n") &&
				!strpos($username, "\t") &&
			is_string($password) && !strpos($password, "\n") &&
				!strpos($password, "\t")) {
			$username = addslashes(trim($username));
			$password = addslashes(trim($password));
			if (strlen($username) > 0 && strlen($username) < 46 &&
				strlen($password) > 4 && strlen($password) < 46) {
				$SQLQuery = 'SELECT id '
						.	'FROM core_users_users '
						.	"WHERE username = '$username' "
						.		"AND password = '$password' AND active = 'y';";
				$result = $this->_DBConnection->runQuery($SQLQuery);
				if ($row = $result->getRow()) {
					$returnResult = intval($row['id']);
				}
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
}
?>
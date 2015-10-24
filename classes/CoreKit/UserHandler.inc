<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchUserException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/Singleton.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/DBConnection.inc");

/**
 *	UserHandler har hand om den grundläggande hanteringen av användarna.
 *	Icke-grundläggande användarhantering sköts av klasser i SupportKit.
 *
 *	Klassen använder sig av tabellen core_users_users
 *
 *	Klassen är en Singleton-klass, och man når den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		0.6.1\n
 *				Skapad: 		2005-06-13\n
 *				Senast ändrad: 	2005-12-12
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
	 *	Lägger till en användare om ingen användare med önskat namn finns.
	 *
	 *	@param	$username är det önskade namnet. En sträng på minst ett och högst
	 *			45 tecken, exklusive inledande och avslutande whitespaces. Tab och
	 *			newlines är ej tillåtna.
	 *	@param	$password är det önskade lösenordet. En sträng på minst fem och högst
	 *			45 tecken, exklusive inledande och avslutande whitespaces. Tab och
	 *			newlines är ej tillåtna.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar den nya användarens id-nummer om användaren lades till,
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
	 *	Ändrar en användares användarnamn.
	 *
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *	@param	$newUsername är det nya användarnamnet. En sträng på minst ett och
	 *			högst 45 tecken, exklusive inledande och avslutande whitespaces.
	 *			Tab och newlines är ej tillåtna.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om användarnamnet ändrades, annars falskt.
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
	 *	Ändrar en användares lösenord.
	 *
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *	@param	$newPassword är det nya lösenordet. En sträng på minst fem och högst
	 *			45 tecken, exklusive inledande och avslutande whitespaces. Tab och
	 *			newlines är ej tillåtna.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om lösenordet ändrades, annars falskt.
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
	 *	Tar bort en användare.
	 *
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om användaren togs bort, annars falskt.
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
	 *	Tar reda på en användares användarnamn.
	 *
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar användarens användarnamn om användaren fanns, annars
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
	 *	Tar reda på användarens id-nummer.
	 *
	 *	@param	$username är användarens användarnamn. En sträng.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar användarens id-nummer om namnet fanns, annars falskt.
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
	 *	Aktiverar en användare.
	 *
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om användaren aktiverades, annars falskt.
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
	 *	Deaktiverar en användare.
	 *
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om användaren deaktiverades, annars falskt.
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
	 *	Tar reda på om en användare existerar.
	 *
	 *	@param	$userid är användarens id-nummer eller användarnamn. Ett heltal
	 *			större än noll eller en sträng.
	 *	@param	$onlyActive anger om användaren i fråga ska sökas bland aktiva
	 *			eller inte. Anges sant, returneras sant enbart om användaren finns
	 *			och är aktiv. Anges falskt, returneras sant oavsett om användaren
	 *			är aktiv eller inte, så länge den finns i databasen.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om användaren fanns i databasen, annars falskt.
	 */
	public function userExists($userid, $onlyActive = true) {
		// TODO fixa enhetstesterna så att dessa avspeglar ändringarna!
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
	 *	Validerar en användares användarnamn och lösenord.
	 *
	 *	@param	$username är användarens användarnamn. En sträng.
	 *	@param	$password är användarens lösenord. En sträng.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar användarens id-nummer om namn och lösen stämmer,
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
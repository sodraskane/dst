<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchGroupException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchUserException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/Singleton.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/DBHandler.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/UserHandler.inc");

/**
 *	GroupHandler har hand om den grundläggande hanteringen av grupperna.
 *	Icke-grundläggande användarhantering sköts av klasser i SupportKit.
 *
 *	Klassen använder sig av tabellerna core_groups_groups och
 *	core_groups_members
 *
 *	Klassen är en Singleton-klass, och man når den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		0.3.2\n
 *				Skapad: 		2005-06-13\n
 *				Senast ändrad: 	2005-12-12
 *	@author		Johan Holmberg, johan@yamborii.net
 */
class GroupHandler implements Singleton {
	private static $_instance;
	
	private $_DBConnection;
	
	/**
	 *	Skapar en instans av GroupHandler.
	 *	@private
	 */
	private function __construct() {
		global $_YINIT;
		$this->_DBConnection = $_YINIT['DBConnection'];
	}
	
	/**
	 *	Returnerar instansen av GroupHandler.
	 *
	 *	@return Singleton-instansen av GroupHandler.
	 */
	public static function getInstance() {
       if (!isset(self::$_instance)) {
           $c = __CLASS__;
           self::$_instance = new $c;
       }

       return self::$_instance;
	}

	/**
	 *	Lägger till en grupp om ingen grupp med önskat namn finns.
	 *
	 *	@param	$groupname är det önskade namnet. En sträng.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar den nya gruppens id-nummer om gruppen lades till,
	 *			annars falskt.
	 */
	public function addGroup($groupname) {
		$returnResult = false;
		
		if (is_string($groupname) && !strpos($groupname, "\n") &&
				!strpos($groupname, "\t")) {
			$groupname = addslashes(trim($groupname));
			if (strlen($groupname) > 0 && strlen($groupname) < 46) {
				$SQLQuery = 'INSERT INTO core_groups_groups '
						.	"VALUES (NULL, '$groupname', 'y');";
				$result = $this->_DBConnection->runQuery($SQLQuery);
				if ($result->getNbrOfAffectedRows() < 1) {
					$returnResult = false;
				} else {
					$returnResult = $result->getID();
				}
			} else {
				throw new Y_IllegalValueException('Name of wrong size');
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameter');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Ändrar en grupps namn.
	 *
	 *	@param	$groupid är användarens id-nummer. Ett heltal större än noll.
	 *	@param	$newGroupname är det nya gruppnamn. En sträng.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om gruppnamnet ändrades, annars falskt.
	 */
	public function alterGroupName($groupid, $newGroupname) {
		$returnResult = false;
		
		if ($this->groupExists($groupid)) {
			if (is_string($newGroupname) && !strpos($newGroupname, "\n") &&
				!strpos($newGroupname, "\t")) {
				$newGroupname = addslashes(trim($newGroupname));
				if (strlen($newGroupname) > 0 && strlen($newGroupname) < 46) {
					$SQLQuery = 'UPDATE core_groups_groups '
							.	"SET name = '$newGroupname' "
							.	"WHERE id = $groupid;";
					$result = $this->_DBConnection->runQuery($SQLQuery);
					$returnResult = $result->getNbrOfAffectedRows() > 0;
				} else {
					throw new Y_IllegalValueException('Name of wrong size');
				}
			} else {
				throw new Y_IllegalValueException('Illegal parameter');
			}
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar bort en grupp.
	 *
	 *	@param	$groupid är grupps id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *
	 *	@return	Returnerar sant om gruppen togs bort, annars falskt.
	 */
	public function removeGroup($groupid) {
		$returnResult = false;
		
		if ($this->groupExists($groupid)) {
			$SQLQuery = 'DELETE FROM core_groups_groups '
					.	"WHERE id = $groupid;";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = $result->getNbrOfAffectedRows() > 0;
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda på en grupps namn.
	 *
	 *	@param	$groupid är gruppens id-nummer. Ett heltal större än noll
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *
	 *	@return	Returnerar gruppens namn om gruppen fanns, annars
	 *			falskt.
	 */
	public function getGroupName($groupid) {
		$returnResult = false;
		
		if ($this->groupExists($groupid)) {
			$SQLQuery = 'SELECT name '
					.	'FROM core_groups_groups '
					.	"WHERE id = $groupid "
					.	'LIMIT 1;';
			$result = $this->_DBConnection->runQuery($SQLQuery);
			if ($result->getNbrOfRows() == 1) {
				$row = $result->getRow();
				$returnResult = stripslashes($row['name']);
			}
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda på gruppens id-nummer.
	 *
	 *	@param	$groupname är gruppens namn. En sträng.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar gruppens id-nummer om namnet fanns, annars falskt.
	 */
	public function getGroupId($groupname) {
		$returnResult = false;
		
		if (is_string($groupname) && !strpos($groupname, "\n") &&
				!strpos($groupname, "\t")) {
			$groupname = addslashes(trim($groupname));
			if (strlen($groupname) > 0 && strlen($groupname) < 46) {
				$SQLQuery = 'SELECT id '
						.	'FROM core_groups_groups '
						.	"WHERE name = '$groupname' "
						.	'LIMIT 1;';
				$result = $this->_DBConnection->runQuery($SQLQuery);
				if ($result->getNbrOfRows() == 1) {
					$row = $result->getRow();
					$returnResult = intval($row['id']);
				} else {
					throw new Y_NoSuchGroupException('No such group in database');
				}
			} else {
				throw new Y_IllegalValueException('Name of wrong size');
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Aktiverar en grupp.
	 *
	 *	@param	$groupid är gruppens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *
	 *	@return	Returnerar sant om gruppen aktiverades, annars falskt.
	 */
	public function activateGroup($groupid) {
		$returnResult = false;
		
		if ($this->groupExists($groupid)) {
			$SQLQuery = 'UPDATE core_groups_groups '
					.	"SET active = 'y' "
					.	"WHERE id = $groupid;";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult =$result->getNbrOfAffectedRows() == 1;
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Deaktiverar en grupp.
	 *
	 *	@param	$groupid är gruppens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *
	 *	@return	Returnerar sant om gruppen deaktiverades, annars falskt.
	 */
	public function deactivateGroup($groupid) {
		$returnResult = false;
		
		if ($this->groupExists($groupid)) {
			$SQLQuery = 'UPDATE core_groups_groups '
					.	"SET active = 'n' "
					.	"WHERE id = $groupid;";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult =$result->getNbrOfAffectedRows() == 1;
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda på om en grupp existerar.
	 *
	 *	@param	$groupid är gruppens id-nummer. Ett heltal större än noll.
	 *	@param	$onlyActive anger om gruppen i fråga ska sökas bland aktiva
	 *			eller inte. Anges sant, returneras sant enbart om gruppen finns
	 *			och är aktiv. Anges falskt, returneras sant oavsett om gruppen
	 *			är aktiv eller inte, så länge den finns i databasen.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om gruppen fanns i databasen, annars falskt.
	 */
	public function groupExists($groupid, $onlyActive = true) {
		$returnResult = null;
		
		if (is_int($groupid) && $groupid > 0 && is_bool($onlyActive)) {
			$SQLQuery = 'SELECT id '
					.	'FROM core_groups_groups '
					.	"WHERE id = $groupid ";
			if ($onlyActive) {
				$SQLQuery .= "AND active = 'y' ";
			}
			$SQLQuery .= 'LIMIT 1;';
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult =$result->getNbrOfRows() == 1;
		} else {
			throw new Y_IllegalValueException('Illegal parameters');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Lägger till en användare i en grupp.
	 *
	 *	@param	$groupid är gruppens id-nummer. Ett heltal större än noll.
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *
	 *	@return	Returnerar sant om användaren fanns och lades till, annars falskt.
	 */
	public function addUserToGroup($groupid, $userid) {
		$returnResult = false;
		
		if ($this->groupExists($groupid)) {
			$UserHandler = UserHandler::getInstance();
			if ($UserHandler->userExists($userid)) {
				$SQLQuery = 'INSERT INTO core_groups_members '
						.	"VALUES ($groupid, $userid);";
				$result = $this->_DBConnection->runQuery($SQLQuery);
				$returnResult = $result->getNbrOfAffectedRows() == 1;
			} else {
				throw new Y_NoSuchUserException('No such user in database');
			}
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda på om en användare är medlem i en grupp eller ej.
	 *
	 *	@param	$groupid är gruppens id-nummer. Ett heltal större än noll.
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *
	 *	@return	Returnerar sant om användaren var medlem i gruppen, annars falskt.
	 */
	public function userIsMember($groupid, $userid) {
		$returnResult = false;
		
		if ($this->groupExists($groupid)) {
			$UserHandler = UserHandler::getInstance();
			if ($UserHandler->userExists($userid)) {
				$SQLQuery = 'SELECT COUNT(*) '
						.	'FROM core_groups_members '
						.	"WHERE groupId = $groupid AND user = $userid "
						.	'LIMIT 1;';
				$result = $this->_DBConnection->runQuery($SQLQuery);
				$returnResult = $result->getNbrOfRows() == 1;
			} else {
				throw new Y_NoSuchUserException('No such user in database');
			}
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar bort en användare ur en grupp.
	 *
	 *	@param	$groupid är gruppens id-nummer. Ett heltal större än noll.
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *
	 *	@return	Returnerar sant om användaren togs bort ur gruppen, annars falskt.
	 */
	public function removeUserFromGroup($groupid, $userid) {
		$returnResult = false;
		
		if ($this->groupExists($groupid)) {
			$UserHandler = UserHandler::getInstance();
			if ($UserHandler->userExists($userid)) {
				$SQLQuery = 'DELETE FROM core_groups_members '
						.	"WHERE groupId = $groupid AND user = $userid;";
				$result = $this->_DBConnection->runQuery($SQLQuery);
				$returnResult = $result->getNbrOfAffectedRows() == 1;
			} else {
				throw new Y_NoSuchUserException('No such user in database');
			}
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Töm en grupp på användare.
	 *
	 *	@param	$groupid är gruppens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om gruppen är tom efter körning, annars falskt.
	 */
	public function flushGroup($groupid) {
		$returnResult = false;
		
		if ($this->groupExists($groupid)) {
			$SQLQuery = 'DELETE FROM core_groups_members '
					.	"WHERE groupId = $groupid;";
			$this->_DBConnection->runQuery($SQLQuery);
			$SQLQuery = 'SELECT * FROM core_groups_members '
					.	"WHERE groupId = $groupid "
					.	'LIMIT 1;';
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = $result->getNbrOfRows() == 0;
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar bort en användare från alla grupper.
	 *
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om användaren är borttagen från grupperna,
	 *			annars falskt.
	 */
	public function removeUserFromAllGroups($userid) {
		$returnResult = false;
		$UserHandler = UserHandler::getInstance();
		
		if ($UserHandler->userExists($userid)) {
			$SQLQuery = 'DELETE FROM core_groups_members '
					.	"WHERE user = $userid;";
			$this->_DBConnection->runQuery($SQLQuery);
			$SQLQuery = 'SELECT * FROM core_groups_members '
					.	"WHERE user = $userid "
					.	'LIMIT 1;';
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = $result->getNbrOfRows() == 0;
		} else {
			throw new Y_NoSuchUserException('No such user in database.');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda på alla grupper en användare är medlem i.
	 *
	 *	@param	$userid är användarens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar en vektor av vektorer på formen |int id|string name|.
	 *			Finns inte användaren i några grupper, returneras en tom vektor.
	 */
	public function getAllGroupsWithThisMember($userid) {
		$returnResult = array();
		$UserHandler = UserHandler::getInstance();
		
		if ($UserHandler->userExists($userid)) {
			$SQLQuery = 'SELECT id, name '
					.	'FROM core_groups_groups c1, core_groups_members c2 '
					.	"WHERE c2.user = $userid AND c1.id = c2.groupid AND "
					.		"c1.active = 'y';";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			while ($row = $result->getRow() ) {
				$returnResult[] = array('id' => intval($row[0]),
										'name' => $row[1]);
			}
		} else {
			throw new Y_NoSuchUserException('No such user in database.');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda på alla medlemmar i en grupp.
	 *
	 *	@param	$groupid är gruppens id-nummer. Ett heltal större än noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar en vektor på formen |int id|int username|. Finns inte
	 *			några medlemmar, returneras en tom vektor.
	 */
	public function getAllMembersInGroup($groupid) {
		$returnResult = array();
		
		if ($this->groupExists($groupid)) {
			$SQLQuery = 'SELECT id, username '
					.	'FROM core_users_users u, core_groups_members g '
					.	"WHERE g.groupId = $groupid AND u.id = g.user AND "
					.		"u.active = 'y';";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			while ($row = $result->getRow() ) {
				$returnResult[] = array('id' => intval($row[0]),
										'name' => $row[1]);
			}
		} else {
			throw new Y_NoSuchGroupException('No such group in database');
		}
		
		return $returnResult;
	}
}
?>
<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchGroupException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchUserException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/Singleton.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/DBHandler.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/UserHandler.inc");

/**
 *	GroupHandler har hand om den grundl�ggande hanteringen av grupperna.
 *	Icke-grundl�ggande anv�ndarhantering sk�ts av klasser i SupportKit.
 *
 *	Klassen anv�nder sig av tabellerna core_groups_groups och
 *	core_groups_members
 *
 *	Klassen �r en Singleton-klass, och man n�r den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		0.3.2\n
 *				Skapad: 		2005-06-13\n
 *				Senast �ndrad: 	2005-12-12
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
	 *	L�gger till en grupp om ingen grupp med �nskat namn finns.
	 *
	 *	@param	$groupname �r det �nskade namnet. En str�ng.
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
	 *	�ndrar en grupps namn.
	 *
	 *	@param	$groupid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *	@param	$newGroupname �r det nya gruppnamn. En str�ng.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om gruppnamnet �ndrades, annars falskt.
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
	 *	@param	$groupid �r grupps id-nummer. Ett heltal st�rre �n noll.
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
	 *	Tar reda p� en grupps namn.
	 *
	 *	@param	$groupid �r gruppens id-nummer. Ett heltal st�rre �n noll
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
	 *	Tar reda p� gruppens id-nummer.
	 *
	 *	@param	$groupname �r gruppens namn. En str�ng.
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
	 *	@param	$groupid �r gruppens id-nummer. Ett heltal st�rre �n noll.
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
	 *	@param	$groupid �r gruppens id-nummer. Ett heltal st�rre �n noll.
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
	 *	Tar reda p� om en grupp existerar.
	 *
	 *	@param	$groupid �r gruppens id-nummer. Ett heltal st�rre �n noll.
	 *	@param	$onlyActive anger om gruppen i fr�ga ska s�kas bland aktiva
	 *			eller inte. Anges sant, returneras sant enbart om gruppen finns
	 *			och �r aktiv. Anges falskt, returneras sant oavsett om gruppen
	 *			�r aktiv eller inte, s� l�nge den finns i databasen.
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
	 *	L�gger till en anv�ndare i en grupp.
	 *
	 *	@param	$groupid �r gruppens id-nummer. Ett heltal st�rre �n noll.
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *
	 *	@return	Returnerar sant om anv�ndaren fanns och lades till, annars falskt.
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
	 *	Tar reda p� om en anv�ndare �r medlem i en grupp eller ej.
	 *
	 *	@param	$groupid �r gruppens id-nummer. Ett heltal st�rre �n noll.
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *
	 *	@return	Returnerar sant om anv�ndaren var medlem i gruppen, annars falskt.
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
	 *	Tar bort en anv�ndare ur en grupp.
	 *
	 *	@param	$groupid �r gruppens id-nummer. Ett heltal st�rre �n noll.
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *
	 *	@return	Returnerar sant om anv�ndaren togs bort ur gruppen, annars falskt.
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
	 *	T�m en grupp p� anv�ndare.
	 *
	 *	@param	$groupid �r gruppens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om gruppen �r tom efter k�rning, annars falskt.
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
	 *	Tar bort en anv�ndare fr�n alla grupper.
	 *
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om anv�ndaren �r borttagen fr�n grupperna,
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
	 *	Tar reda p� alla grupper en anv�ndare �r medlem i.
	 *
	 *	@param	$userid �r anv�ndarens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om anv�ndaren inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar en vektor av vektorer p� formen |int id|string name|.
	 *			Finns inte anv�ndaren i n�gra grupper, returneras en tom vektor.
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
	 *	Tar reda p� alla medlemmar i en grupp.
	 *
	 *	@param	$groupid �r gruppens id-nummer. Ett heltal st�rre �n noll.
	 *
	 *	@throws	Kastar ett Y_NoSuchGroupException om gruppen inte fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar en vektor p� formen |int id|int username|. Finns inte
	 *			n�gra medlemmar, returneras en tom vektor.
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
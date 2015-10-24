<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchPermissionException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchUserException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/Singleton.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/DBHandler.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/UserHandler.inc");

/**
 *	Klassen PermissionHandler hanterar behörigheter i systemet.
 *
 *	Klassen använder tabellerna core_permission_keys och
 *	core_permissions_data.
 *
 *	Klassen är en Singleton-klass, och man når den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		0.4.3\n
 *				Skapad: 		2005-05-13\n
 *				Senast ändrad: 	2006-04-26
 *	@author 	Johan Holmberg, johan@yamborii.net
 */
class PermissionHandler implements Singleton {
	private static $_instance;
	
	private $_DBConnection;
	private $_UserHandler;

	/**
	 *	Skapar en instans av PermissionHandler.
	 *	@private
	 */
	private function __construct() {
		global $_YINIT;
		if (isset($_YINIT['DBConnection'])) {
			$this->_DBConnection = $_YINIT['DBConnection'];
		} else {
			$this->_DBConnection = new DBConnection();
		}
		$this->_UserHandler = UserHandler::getInstance();
	}
	
	/**
	 *	Returnera instansen av PermissionHandler.
	 *
	 *	@return Singleton-instansen av PermissionHandler.
	 */
	public static function getInstance() {
       if (!isset(self::$_instance)) {
           $c = __CLASS__;
           self::$_instance = new $c;
       }

       return self::$_instance;
	}
	
	/**
	 *	Skapar en ny behörighetskategori. Detta görs genom att namnge en ny
	 *	behörighetsnyckel, som sedan kan användas. Dubletterade namn är ej
	 *	tillåtna. För namngivning, se namngivnigspolicyn.
	 *
	 *	@param	$key är namnet på den nya nyckeln. En sträng. Giltiga tecken är
	 *			[A-Z0-9_]. Nyckeln ska vara minst fem tecken och högst 45 tecken
	 *			lång.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om nyckeln lades till, annars falskt.
	 */
	public function createKey($key) {
		$returnResult = false;
		
		if (is_string($key) && preg_match('/^([A-Z0-9_]){5,45}$/', $key)) {
			$SQLQuery = 'INSERT INTO core_permissions_keys '
					.	"VALUES ('$key', NULL);";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = $result->getNbrOfAffectedRows() > 0;
		} else {
			throw new Y_IllegalValueException('Illegal parameter');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar bort en hehörighetskategori. Metoden tar bort nyckeln för kategorin,
	 *	tillsammans med alla sparade behörigheter för denna kategori.
	 *
	 *	@param	$key är namnet på den nyckel som ska tas bort. En sträng.
	 *
	 *	@throws	Kastar ett Y_NoSuchPermissionException om nyckeln ej fanns.
	 *
	 *	@return	Returnerar sant om kategorin togs bort, annars falskt.
	 */
	public function removeKey($key) {
		$returnResult = false;
		
		if ($this->_getKeyId($key) > 0) {
			$SQLQuery = 'DELETE core_permissions_keys AS pKeys, '
					.		'core_permissions_data AS pData '
					.	'FROM pKeys, pData '
					.	"WHERE pData.id = pKeys.id AND pKeys.keyname = '$key';";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = $result->getNbrOfAffectedRows() > 0;
			if (!$returnResult) {
				$SQLQuery = 'DELETE FROM core_permissions_keys '
					.	"WHERE keyname = '$key';";
				$result = $this->_DBConnection->runQuery($SQLQuery);
				$returnResult = $result->getNbrOfAffectedRows() > 0;
			}
		} 
		
		return $returnResult;
	}
	
	/**
	 *	Ger en användare en angiven behörighet.
	 *
	 *	@param	$user är den användare vi vill ge behörigheten. Ett heltal.
	 *	@param	$key är nyckelnamnet på den behörighet vi vill ge. En sträng.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *	@throws	Kastar ett Y_NoSuchPermissionException om nyckeln ej fanns.
	 *
	 *	@return	Returnerar sant om användaren fick behörighet, annars falskt
	 */
	public function grantPermission($user, $key) {
		$returnResult = false;
		
		if ($this->_UserHandler->userExists($user) &&
			$keyId = $this->_getKeyId($key)) {
			$SQLQuery = 'INSERT INTO core_permissions_data '
					.	"VALUES($keyId, $user, '');";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = $result->getNbrOfAffectedRows() > 0;
		} else {
			throw new Y_NoSuchUserException('No such user in database.');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar bort en användares behörighet för en given behörighetstyp.
	 *
	 *	@param	$user är den användare vars behörighet vi vill ta bort. Ett heltal.
	 *	@param	$key är nyckelnamnet på den behörighet vi vill ta bort. En sträng.
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *	@throws	Kastar ett Y_NoSuchPermissionException om nyckeln ej fanns.
	 *
	 *	@return	Returnerar sant om behörigheten drogs in, annars falskt.
	 */
	public function revokePermission($user, $key) {
		$returnResult = false;
		
		if ($this->_UserHandler->userExists($user) &&
			$keyId = $this->_getKeyId($key)) {
			$SQLQuery = 'DELETE FROM core_permissions_data '
					.	"WHERE id = $keyId AND user = $user;";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = $result->getNbrOfAffectedRows() > 0;
		} else {
			throw new Y_NoSuchUserException('No such user in database.');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Lägger till ett argument till en användares behörighet.
	 *
	 *	@param	$user är den användare vars argument vi vill ändra. Ett heltal.
	 *	@param	$key är nyckelnamnet på den behörighet vi vill ändra. En sträng.
	 *	@param	$argument är eventuellt argument till behörighetstypen. En sträng.
	 *
	 *	@throws	Kastar ett Y_NoSuchPermissionException om nyckeln ej fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om argumentet godtogs, annars falskt.
	 */
	public function addArgument($user, $key, $argument, $debug = false) {
		$returnResult = false;
		$keyId = $this->_getKeyId($key);
		
		if ($this->_UserHandler->userExists($user) &&
			is_int($keyId) && is_string($argument)) {
			$argument = addslashes(trim($argument));
			$SQLQuery = 'SELECT arguments '
					.	'FROM core_permissions_data '
					.	"WHERE id = $keyId AND user = $user;";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			if ($result->getNbrOfRows() > 0) {
				$row = $result->getRow();
				$oldArgs = $row['arguments'];
				if (!preg_match("/(^($argument)$|^($argument),\s|" .
								",\s($argument),\s|,\s($argument)\$)/",
								$oldArgs)) {
					if (strlen($oldArgs) > 0) {
						$argument = $oldArgs . ', ' . $argument;
					}
					$SQLQuery = 'UPDATE core_permissions_data '
							.	"SET arguments = '$argument' "
							.	"WHERE id = $keyId AND user = $user;";
					$result = $this->_DBConnection->runQuery($SQLQuery);
					$returnResult = $result->getNbrOfAffectedRows() > 0;
				}
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameter');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar bort ett argument från en användares behörighet.
	 *
	 *	@param	$user är den användare vars argument vi vill ändra. Ett heltal.
	 *	@param	$key är nyckelnamnet på den behörighet vi vill ändra. En sträng.
	 *	@param	$argument är eventuellt argument till behörighetstypen. En sträng.
	 *
	 *	@throws	Kastar ett Y_NoSuchPermissionException om nyckeln ej fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om argumentet togs bort, annars falskt.
	 */
	public function removeArgument($user, $key, $argument) {
		$returnResult = false;
		$keyId = $this->_getKeyId($key);
		
		if ($this->_UserHandler->userExists($user) &&
			is_int($keyId) && is_string($argument)) {
			$argument = addslashes(trim($argument));
			$SQLQuery = 'SELECT arguments '
					.	'FROM core_permissions_data '
					.	"WHERE id = $keyId AND user = $user";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			if ($result->getNbrOfRows() > 0) {
				$row = $result->getRow();
				$oldArg = $row['arguments'];
				$newArg = preg_replace("/(^$argument$|^$argument,\s|" .
										",\s$argument,\s|,\s$argument\$)/",
										', ', $oldArg);
				if ($oldArg != $newArg) {
					$newArg = preg_replace("/(^,\s|,\s\$)/", '', $newArg);
					$SQLQuery = 'UPDATE core_permissions_data '
							.	"SET arguments = '$newArg' "
							.	"WHERE id = $keyId AND user = $user;";
					$result = $this->_DBConnection->runQuery($SQLQuery);
					$returnResult = $result->getNbrOfAffectedRows() > 0;
				}
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameter');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar reda på om användaren har en given behörighet.
	 *
	 *	@param	$user är den användare vars behörigheter vi vill testa.
	 *			Ett heltal.
	 *	@param	$key är nyckelnamnet på den behörighet vi vill testa. En sträng.
	 *	@param	$argument är eventuellt argument till behörighetstypen. En sträng.
	 *			Argumentet kan utelämnas.
	 *
	 *	@throws	Kastar ett Y_NoSuchPermissionException om nyckeln ej fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar sant om användaren hade behörighet, annars falskt.
	 */
	public function hasPermission($user, $key, $argument = false) {
		$returnResult = false;
		$keyId = $this->_getKeyId($key);
		
		if ($this->_UserHandler->userExists($user) &&
			is_int($keyId)) {
			$SQLQuery = 'SELECT * '
					.	'FROM core_permissions_data '
					.	"WHERE id = $keyId AND user = $user;";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			if ($result->getNbrOfRows() == 1) {
				if (is_string($argument)) {
					$row = $result->getRow();
					$arguments = stripslashes($row['arguments']);
					if (preg_match("/(^$argument$|^$argument,\s|" .
								",\s$argument,\s|,\s$argument\$)/", $arguments)) {
						$returnResult = true;
					}
				} else if (is_bool($argument) && !$argument) {
					$returnResult = true;
				} else {
					throw new Y_IllegalValueException('Illegal arguments');
				}
			}
		} else {
			throw new Y_IllegalValueException('Illegal parameter');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar bort alla behörigheter för en given användare.
	 *
	 *	@param	$user är den användare vars behörigheter vi vill ta bort. Ett
	 *			heltal.
	 *
	 *	@throws	Kastar ett Y_NoSuchPermissionException om nyckeln ej fanns.
	 *
	 *	@return	Returnerar sant om behörigheterna togs bort, annars falskt.
	 */
	public function flushPermissions($user) {
		$returnResult = false;
		
		if ($this->_UserHandler->userExists($user)) {
			$SQLQuery = 'DELETE FROM core_permissions_data '
					.	"WHERE user = $user";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = $result->getNbrOfAffectedRows() > 0;
		} else {
			throw new Y_NoSuchUserException('No such user in database.');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Listar alla användare med given behörighet.
	 *
	 *	@param	$key är namnet på den behörighet vi vill ha information om.
	 *			En sträng.
	 *	@param	$argument är ett eventuellt argument för behörighetstypen, vilket
	 *			används vid sökningen. En sträng. Argumenetet är frivilligt. Anges
	 *			inget, listas alla användare för behörighetstypen.
	 *
	 *	@throws	Kastar ett Y_NoSuchPermissionException om nyckeln ej fanns.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@return	Returnerar en vektor av typen [int] innehållandes alla användare
	 *			som har given behörighet. Finns inga, består vektorn av ett enda
	 *			element, vars värde är -1.
	 */
	public function listPermitted($key, $argument = false) {
		$returnResult[0] = -1;
		$keyId = $this->_getKeyId($key);
		$i = 0;
		
		if (is_int($keyId) &&
			(is_bool($argument) && !$argument || is_string($argument))) {
			if (is_string($argument)) {
				$argument = trim($argument);
			} else if (!is_bool($argument)) {
				throw new Y_IllegalValueException('Illegal arguments');
			}
			$SQLQuery = 'SELECT user, arguments '
					.	'FROM core_permissions_data '
					.	"WHERE id = $keyId "
					.	'ORDER BY user ASC;';
			$result = $this->_DBConnection->runQuery($SQLQuery);
			while ($row = $result->getRow()) {
				if (is_string($argument)) {
					$arguments = stripslashes($row['arguments']);
					if (preg_match("/(^$argument$|^$argument,\s|" .
								",\s$argument,\s|,\s$argument\$)/", $arguments)) {
						$returnResult[$i] = intval($row['user']);
						$i++;
					}
				} else {
					$returnResult[$i] = intval($row['user']);
					$i++;
				}
			}
		}
		
		return $returnResult;
	}
	
	/**
	 *	Listar alla behörigheter för en given användare.
	 *
	 *	@param	$user är användar-id:t för den användare vi vill ha information
	 *			om. Ett heltal
	 *
	 *	@throws	Kastar ett Y_NoSuchUserException om användaren inte fanns.
	 *
	 *	@return	Returnerar en vektor på formen [string key | string arguments].
	 *			Har användaren inga behörigheter, består vektorn av ett enda
	 *			element, vars värde är -1.
	 */
	public function listPermissions($user) {
		$returnResult[0] = -1;
		$i = 0;
		
		if ($this->_UserHandler->userExists($user)) {
			$SQLQuery = 'SELECT k.keyname, d.arguments '
					.	'FROM core_permissions_keys k, core_permissions_data d '
					.	"WHERE d.user = $user AND d.id = k.id "
					.	"ORDER BY k.keyname ASC";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			$returnResult = null;
			while ($row = $result->getRow()) {
				$returnResult[$i]['key'] = stripslashes($row['keyname']);
				$returnResult[$i]['arguments'] = stripslashes($row['arguments']);
				$i++;
			}
			if (is_null($returnResult)) {
				$returnResult[0] = -1;
			}
		} else {
			throw new Y_NoSuchUserException('No such user in database.');
		}
		
		return $returnResult;
	}
	
	  ############################
	 #		Privata metoder		#
	############################
	
	/**
	 *	Tar reda på en nyckels id-nummer.
	 *	@private
	 *
	 *	@param	$key är nyckelns namn. En sträng.
	 *
	 *	@throws	Kastar ett Y_NoSuchPermissionException om nyckeln ej fanns.
	 *
	 *	@return	Returnerar id-numret om nyckeln finns, annars falskt.
	 */
	private function _getKeyId($key) {
		$returnResult = false;
		
		if (is_string($key) && preg_match('/^([A-Z0-9_]){5,45}$/', $key)) {
			$SQLQuery = 'SELECT id '
					.	'FROM core_permissions_keys '
					.	"WHERE keyname = '$key'";
			$result = $this->_DBConnection->runQuery($SQLQuery);
			if ($row = $result->getRow()) {
				$returnResult = intval($row['id']);
			} else {
				throw new Y_NoSuchPermissionException('Key was not found.');
			}
		}
		
		return $returnResult;
	}
}
?>
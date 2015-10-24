<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/Singleton.inc");

/**
 *	Core utg�r k�rnan i Y2-systemet. H�r finns alla grundl�ggande
 *	funktioner som r�r sessions- och cookiehantering. Klassen hanterar
 *	�ven instansiering av objekt.
 *
 *	Klassen �r en Singleton-klass, och man n�r den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		2.2.1\n
 *				Skapad: 		2002-07-18\n
 *				Senast �ndrad: 	2005-12-06
 *	@author		Johan Holmberg, johan@yamborii.net
 */
class Core implements Singleton {
	private static $_instance;
	private static $_instanceIndex;
	
	/**
	 *	Skapar ett objekt av core.
	 */
	private function __construct() {
		session_start();
		session_write_close();
		
		self::$_instanceIndex = 0;
	}
	
	/**
	 *	Returnerar instansen av Core.
	 *
	 *	@return Singleton-instansen av Core.
	 */
	public static function getInstance() {
       if (!isset(self::$_instance)) {
           $c = __CLASS__;
           self::$_instance = new $c;
       }

       return self::$_instance;
	}
	
	/**
	 *	Skickar tillbaka ett id-nummer som anv�nds f�r att
	 *	j�mf�ra objekt. Detta �r ett fulhack f�r PHP4, som inte
	 *	st�der referensj�mf�relser.\n
	 *	Instans-indexet kommer att uppdateras efter varje anrop
	 *	till metoden, vilket garanterar att varje objekt kommer att
	 *	f� ett unikt id-nummer. Detta g�r j�mf�relsen mellan objekt
	 *	mycket snabbare och s�krare, och eliminerar �ven risken f�r
	 *	o�ndliga loopar till f�ljd av korsreferenser.
	 *
	 *	@return Returnerar ett unikt id-nummer, vilket kan ges �t
	 *			anropande objekt.
	 */
	public function getInstanceIndex() {
		self::$_instanceIndex++;
		
		return self::$_instanceIndex;
	}
	
	/**
	 *	Sparar undan ett v�rde till en cookie. Cookien �r giltig
	 *	i hundra dagar.
	 *
	 *	@param	$name anger namnet p� cookien som ska sparas. En
	 *			str�ng best�ende av tecknen a-zA-z_. �r namnet
	 *			ogiltig, kommer ingen cookie att sparas.
	 *	@param	$value �r v�rdet som ska sparas i cookien.
	 */
	public function writeCookie($name, $value) {
		if (is_string($name) && preg_match('/^([a-zA-Z_]+)$/', $name)) {
			setcookie($name, $value, time()+100*24*3600);
		} else {
			throw new Y_IllegalValueException('Illegal Parameter');
		}
	}
	
	/**
	 *	L�ser v�rdet i en cookie. 
	 *
	 *	@param	$name anger namnet p� cookien som ska l�sas. En
	 *			str�ng best�ende av tecknen a-zA-z_.
	 *
	 *	@return	Returnerar cookiens inneh�ll. �r cookien odefinierad,
	 *			returneras null.
	 */
	public function readCookie($name) {
		$returnResult = null;
		
		if (is_string($name)) {
			if (isset($_COOKIE[$name])) {
				$returnResult = $_COOKIE[$name];
			}
		} else {
			throw new Y_IllegalValueException('Illegal Parameter');
		}
		
		return $returnResult;
	}
	
	/**
	 *	Tar bort en cookie.
	 *
	 *	@param	$name anger namnet p� cookien som ska sparas. En
	 *			str�ng best�ende av tecknen a-zA-z_.
	 */
	public function removeCookie($name) {
		if (is_string($name) && preg_match('/^([a-zA-Z_]+)$/', $name)) {
			setcookie($name, '', time()-3600);
		} else {
			throw new Y_IllegalValueException('Illegal Parameter');
		}
	}
	
	/**
	 *	Sparar undan ett v�rde som en sessionsvariabel, s� att denna
	 *	kan anv�ndas �ven p� andra sidor.
	 *
	 *	@param	$name �r namnet p� sessionsvariabeln. En str�ng.
	 *	@param	$value �r sessionsvariabelns v�rde.
	 */
	public function writeSessionVariable($name, $value) {
		if (is_string($name)) {
			session_start();
			$_SESSION[$name] = $value;
			session_write_close();
		} else {
			throw new Y_IllegalValueException('Illegal Parameter');
		}
	}
	
	/**
	 *	L�ser v�rdet i en sessionsvariabel. 
	 *
	 *	@param	$name anger namnet p� sessionsvariabeln som ska l�sas.
	 *			En str�ng.
	 *
	 *	@return	Returnerar sessionsvariabelns inneh�ll. �r cookien
	 *			odefinierad, returneras null.
	 */
	public function readSessionVariable($name) {
		$returnResult = null;
		
		if (is_string($name)) {
			if (isset($_SESSION[$name])) {
				$returnResult = $_SESSION[$name];
			}
		} else {
			throw new Y_IllegalValueException('Illegal Parameter');
		}
		
		return $returnResult;
	}
	
	/**
	 *	L�ser v�rdet i en sessionsvariabel. 
	 *
	 *	@param	$name anger namnet p� sessionsvariabeln som ska
	 *			tas bort. En str�ng.
	 */
	public function removeSessionVariable($name) {
		if (is_string($name)) {
			unset($_SESSION[$name]);
		} else {
			throw new Y_IllegalValueException('Illegal Parameter');
		}
	}
}
?>
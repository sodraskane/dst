<?php
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/Singleton.inc");

/**
 *	Core utgör kärnan i Y2-systemet. Här finns alla grundläggande
 *	funktioner som rör sessions- och cookiehantering. Klassen hanterar
 *	även instansiering av objekt.
 *
 *	Klassen är en Singleton-klass, och man når den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		2.2.1\n
 *				Skapad: 		2002-07-18\n
 *				Senast ändrad: 	2005-12-06
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
	 *	Skickar tillbaka ett id-nummer som används för att
	 *	jämföra objekt. Detta är ett fulhack för PHP4, som inte
	 *	stöder referensjämförelser.\n
	 *	Instans-indexet kommer att uppdateras efter varje anrop
	 *	till metoden, vilket garanterar att varje objekt kommer att
	 *	få ett unikt id-nummer. Detta gör jämförelsen mellan objekt
	 *	mycket snabbare och säkrare, och eliminerar även risken för
	 *	oändliga loopar till följd av korsreferenser.
	 *
	 *	@return Returnerar ett unikt id-nummer, vilket kan ges åt
	 *			anropande objekt.
	 */
	public function getInstanceIndex() {
		self::$_instanceIndex++;
		
		return self::$_instanceIndex;
	}
	
	/**
	 *	Sparar undan ett värde till en cookie. Cookien är giltig
	 *	i hundra dagar.
	 *
	 *	@param	$name anger namnet på cookien som ska sparas. En
	 *			sträng bestående av tecknen a-zA-z_. Är namnet
	 *			ogiltig, kommer ingen cookie att sparas.
	 *	@param	$value är värdet som ska sparas i cookien.
	 */
	public function writeCookie($name, $value) {
		if (is_string($name) && preg_match('/^([a-zA-Z_]+)$/', $name)) {
			setcookie($name, $value, time()+100*24*3600);
		} else {
			throw new Y_IllegalValueException('Illegal Parameter');
		}
	}
	
	/**
	 *	Läser värdet i en cookie. 
	 *
	 *	@param	$name anger namnet på cookien som ska läsas. En
	 *			sträng bestående av tecknen a-zA-z_.
	 *
	 *	@return	Returnerar cookiens innehåll. Är cookien odefinierad,
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
	 *	@param	$name anger namnet på cookien som ska sparas. En
	 *			sträng bestående av tecknen a-zA-z_.
	 */
	public function removeCookie($name) {
		if (is_string($name) && preg_match('/^([a-zA-Z_]+)$/', $name)) {
			setcookie($name, '', time()-3600);
		} else {
			throw new Y_IllegalValueException('Illegal Parameter');
		}
	}
	
	/**
	 *	Sparar undan ett värde som en sessionsvariabel, så att denna
	 *	kan användas även på andra sidor.
	 *
	 *	@param	$name är namnet på sessionsvariabeln. En sträng.
	 *	@param	$value är sessionsvariabelns värde.
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
	 *	Läser värdet i en sessionsvariabel. 
	 *
	 *	@param	$name anger namnet på sessionsvariabeln som ska läsas.
	 *			En sträng.
	 *
	 *	@return	Returnerar sessionsvariabelns innehåll. Är cookien
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
	 *	Läser värdet i en sessionsvariabel. 
	 *
	 *	@param	$name anger namnet på sessionsvariabeln som ska
	 *			tas bort. En sträng.
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

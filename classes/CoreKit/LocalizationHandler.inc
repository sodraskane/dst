<?php
// TODO G�r klart den h�r klassen!

require_once($_SERVER['DOCUMENT_ROOT']."/classes/CoreKit/Core.inc");
require_once($_SERVER['DOCUMENT_ROOT']."/classes/CoreKit/DBHandler.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_MethodLockedException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoLanguageException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchStringException.inc");
/**
 *	LocalizationHandler har hand om spr�khanteringen i systemet.
 *
 *	Klassen anv�nder sig av tabellen core_localization_languages.
 *
 *	Klassen �r en Singleton-klass, och man n�r den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		0.3.1\n
 *				Skapad: 		2005-07-22\n
 *				Senast �ndrad: 	2005-12-12
 *	@author		Johan Holmberg, johan@yamborii.net
 */
class LocalizationHandler {
	private static $_instance;
	private $_core;
	private $_DBConnection;
	private $_strings;
	private $_lang;
	private $_locked;
	
	/**
	 *	Skapar en instans av LocalizationHandler.
	 */
	private function __construct() {
		global $_YINIT;
		
		$this->_core = Core::getInstance();
		$this->_DBConnection = $_YINIT['DBConnection'];
		
		$this->_strings = null;
		$this->_lang = '';
		$this->_locked = false;

//		if (isset($_SESSION['lang'])) {
//			$this->_lang = $_SESSION['lang'];
//		} else if (isset ($_COOKIE) && isset ($_COOKIE['lang'])) {
//			if ($this->_Localizer->validateLanguage($_COOKIE['lang'])) {
//				$this->_lang = $_COOKIE['lang'];
//				$_SESSION['lang'] = $_COOKIE['lang'];
//			} else {
//				$this->setLanguage($this->_Localizer->getDefaultLanguage());
//			}
//		} else {
//			$myLang = ""; //if empty, we get the default language
//			if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
//				$myLang = $_SERVER['HTTP_ACCEPT_LANGUAGE'];
//			}
//			$this->_lang = $this->_Localizer->chooseLanguage($myLang);
//			$_SESSION['lang'] = $this->_lang;
//		}
	}
	
	/**
	 *	Returnerar instansen av Settings.
	 *
	 *	@return Singleton-instansen av Settings.
	 */
	public function getInstance() {
       if (!isset(self::$_instance)) {
           $c = __CLASS__;
           self::$_instance = new $c;
       }

       return self::$_instance;
	}
	
	/**
	 *	Tar fram en lista p� alla inlagda spr�k, sorterade i bokstavsordning.
	 *
	 *	@throws	Kastar ett Y_NoLanguageException om inga spr�k fanns inlagda.
	 *
	 *	@return Returnerar en vektor av listor p� formen\n
	 *			|string code|string name|
	 *			Vektorn sorteras efter nyckeln 'code'
	 */
	public function listLanguages() {
		$returnResult = array();
		
		$SQLQuery = 'SELECT * '
				.	'FROM core_localization_languages '
				.	'ORDER BY language_key ASC;';
		$result = $this->_DBConnection->runQuery($SQLQuery);
		if ($result->getNbrOfRows() > 0) {
			$i = 0;
			while ($row = $result->getRow()) {
				$returnResult[$i]['code'] = $row['language_key'];
				$returnResult[$i++]['name'] = $row['language_name'];
			}
		} else {
			throw new Y_NoLanguageException();
		}
		
		return $returnResult;
	}
	
	/**
	 *	L�ser upp objektet f�r byte av spr�k.
	 */
	public function unlock() {
		$this->_locked = false;
	}

	/**
	 *	Tar reda p� vilket spr�k vi anv�nder.
	 *
	 *	@throws	Kastar ett Y_NoLanguageException om inget spr�k �r valt.
	 *
	 *	@return Returnerar en spr�kkod p� tv� till fyra bokst�ver.
	 */
	public function getLanguage() {
		if ($this->_lang == '') {
			throw new Y_NoLanguageException();
		}
		return $this->_lang;
	}
	
	/**
	 *	Anger vilket spr�k vi vill anv�nda. Metoden �ndrar bara spr�ket om
	 *	addCollection inte anropats, detta f�r att f�rs�kra oss mot att tv�
	 *	spr�k inte dyker upp p� samma sida.
	 *
	 *	@throws	Kastar ett Y_NoLanguageException om spr�ket inte finns inlagt.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametern var
	 *			felaktig.
	 *	@throws	Kastar ett Y_MethodLockedException om addCollection blivit
	 *			anropad.
	 *
	 *	@param $lang �r en spr�kkod p� tv� till fyra bokst�ver.
	 */
	public function setLanguage($lang) {
		if (!$this->_locked) {
			if (is_string($lang) && strlen($lang) >= 2 && strlen($lang) <= 4) {
				if ($this->_languageExists($lang)) {
					$this->_lang = $lang;
				} else {
					throw new Y_NoLanguageException();
				}
			} else {
				throw new Y_IllegalValueException();
			}
		} else {
			throw new Y_MethodLockedException();
		}
	}
	
	/**
	 *	V�ljer ut ett spr�k att anv�nda ur databasen. Metoden matchar
	 *	anv�ndarens f�rvalda spr�k mot de spr�k som finns inlagda i
	 *	databasen. F�rsta tr�ffen anv�nds, och finns inga tr�ffar,
	 *	anv�nds engelska som f�rval.
	 *
	 *	@return Returnerar det spr�k som ska anv�ndas under sessionen. En
	 *			str�ng p� tv�-fyra tecken, vilka �r ISO-koden f�r spr�ket.
	 */
	public function selectLanguage() {
		$returnResult = 'en';
		$accept_lang = $_SERVER['HTTP_ACCEPT_LANGUAGE'];
		if ($accept_lang != '') {
			$active_langs = $this->listLanguages();
			if (count($active_langs) > 0) {
				$langs = explode(',',$accept_lang);
				foreach ($langs as $lang) {
					$lang = substr($lang,0,2);
					foreach ($active_langs as $l) {
						if ($l == $lang) {
							$returnResult = $lang;
							break;
						}
					}
				}
			}
		}
		
		return $returnResult;
	}
	
	/**
	 *	L�ser in en samling spr�kstr�ngar p� angivet spr�k. Metoden l�gger till
	 *	dessa i en lista, som �gs av klassen. Du kan l�gga till hur m�nga
	 *	samlingar du vill.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametern var
	 *			felaktig.
	 *
	 *	@param	$collection �r namnet p� den samling vi vill l�sa in.
	 */
	public function addCollection($collection) {
	}
	
	/**
	 *	H�mtar en textstr�ng p� angivet spr�k.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparameternvar
	 *			felaktig.
	 *	@throws	Kastar ett Y_NoSuchStringException om str�ngen inte fanns.
	 *
	 *	@param	$name �r den spr�kkonstant vi vill ha.
	 *
	 *	@return	Returnerar en str�ng p� valt spr�k.
	 */
	public function getString($name) {
		$returnResult = '';
		
		return $returnResult;
	}
	
	/**
	 *	S�tter in diverse v�rden i en str�ng, vilket g�r att vi kan l�ta
	 *	olika spr�k hantera data p� sina egna s�tt. Detta inneb�r att systemet
	 *	kan skita fullst�ndigt i grammatiska regler i olika spr�k.\n
	 *	\n
	 *	F�r att f� in r�tt v�rde p� r�tt plats, �r det n�dv�ndigt att veta p�
	 *	vilken plats i vektorn varje v�rde f�rekommer. Dessa placeras sedan ut
	 *	p� r�tt plats genom att placera ut dess index (plats i vektorn + 1) p�
	 *	den plats i str�ngen d�r v�rdet ska s�ttas in. Exempel:\n
	 *	\n
	 *	$string = "P� menyn har vi %1\$s, %3\$s och %2\$s.";
	 *	$values = Array("kebab", "falafel", "kokta strumpor");
	 *	$result = insertValues($string, $values);
	 *	echo $result;
	 *	> P� menyn har vi kebab, kokta strumpor och falafel.\n
	 *	\n
	 *	Placeras inga index-markeringar ut, antar metoden att v�rdena ska
	 *	placeras ut i den ordning de f�rekommer i vektorn.\n
	 *	\n
	 *	F�ljande saker kan s�ttas in i str�ngen och vektorn:\n
	 *	Procenttecken	=>	%%		(Ex: insertValues("42%%") => "42%")\n
	 *	Str�ng			=>	%s		(Ex: insertValues("Hello, %s!", "world") 
	 								=> "Hello, world!")\n
	 *	Heltal			=>	%d		(Ex: insertValues("Svaret �r %d", 42)
	 								=> "Svaret �r 42")\n
	 *	Flyttal			=>	%f		(Ex: insertValues("pi: %f", 3.141529)
	 								=> "pi: 3.141529")
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@param	$string �r str�ngen som ska f�r�ndras. En str�ng.
	 *	@param	$values �r de v�rden som ska s�ttas in i str�ngen. En vektor
	 *			med str�ngar eller tal.
	 *
	 *	@return Returnerar en str�ng.
	 */
	public function insertValues($string, $values) {
		$returnResult = "";
		
		if (is_string($string)) {
			$returnResult = $string;
			if (!is_null($values)) {
				$returnResult = vsprintf($string, $values);
			} else {
				$returnResult = vsprintf($string, "");
			}
		}
		
		return $returnResult;
	}
	
	/**
	 *	S�ker efter ett spr�k i databasen.
	 *	@private
	 *
	 *	@param $lang �r en spr�kkod p� tv� till fyra bokst�ver.
	 *
	 *	@return	Returnerar sant om spr�ket fanns i databasen, annars falskt.
	 */
	private function _languageExists($lang) {
		$SQLQuery = "SELECT * "
				.	"FROM core_localization_languages "
				.	"WHERE language_key = '$lang';";
		$result = $this->_DBConnection->runQuery($SQLQuery);
		
		return $result->getNbrOfRows() > 0;
	}
}
?>
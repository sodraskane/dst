<?php
// TODO Gör klart den här klassen!

require_once($_SERVER['DOCUMENT_ROOT']."/classes/CoreKit/Core.inc");
require_once($_SERVER['DOCUMENT_ROOT']."/classes/CoreKit/DBHandler.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_IllegalValueException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_MethodLockedException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoLanguageException.inc");
require_once($_SERVER["DOCUMENT_ROOT"]."/classes/CoreKit/exceptions/Y_NoSuchStringException.inc");
/**
 *	LocalizationHandler har hand om språkhanteringen i systemet.
 *
 *	Klassen använder sig av tabellen core_localization_languages.
 *
 *	Klassen är en Singleton-klass, och man når den globala instansen
 *	genom att anropa metoden getInstance();
 *
 *	@version	Version 		0.3.1\n
 *				Skapad: 		2005-07-22\n
 *				Senast ändrad: 	2005-12-12
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
	 *	Tar fram en lista på alla inlagda språk, sorterade i bokstavsordning.
	 *
	 *	@throws	Kastar ett Y_NoLanguageException om inga språk fanns inlagda.
	 *
	 *	@return Returnerar en vektor av listor på formen\n
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
	 *	Låser upp objektet för byte av språk.
	 */
	public function unlock() {
		$this->_locked = false;
	}

	/**
	 *	Tar reda på vilket språk vi använder.
	 *
	 *	@throws	Kastar ett Y_NoLanguageException om inget språk är valt.
	 *
	 *	@return Returnerar en språkkod på två till fyra bokstäver.
	 */
	public function getLanguage() {
		if ($this->_lang == '') {
			throw new Y_NoLanguageException();
		}
		return $this->_lang;
	}
	
	/**
	 *	Anger vilket språk vi vill använda. Metoden ändrar bara språket om
	 *	addCollection inte anropats, detta för att försäkra oss mot att två
	 *	språk inte dyker upp på samma sida.
	 *
	 *	@throws	Kastar ett Y_NoLanguageException om språket inte finns inlagt.
	 *	@throws	Kastar ett Y_IllegalValueException om inparametern var
	 *			felaktig.
	 *	@throws	Kastar ett Y_MethodLockedException om addCollection blivit
	 *			anropad.
	 *
	 *	@param $lang är en språkkod på två till fyra bokstäver.
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
	 *	Väljer ut ett språk att använda ur databasen. Metoden matchar
	 *	användarens förvalda språk mot de språk som finns inlagda i
	 *	databasen. Första träffen används, och finns inga träffar,
	 *	används engelska som förval.
	 *
	 *	@return Returnerar det språk som ska användas under sessionen. En
	 *			sträng på två-fyra tecken, vilka är ISO-koden för språket.
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
	 *	Läser in en samling språksträngar på angivet språk. Metoden lägger till
	 *	dessa i en lista, som ägs av klassen. Du kan lägga till hur många
	 *	samlingar du vill.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametern var
	 *			felaktig.
	 *
	 *	@param	$collection är namnet på den samling vi vill läsa in.
	 */
	public function addCollection($collection) {
	}
	
	/**
	 *	Hämtar en textsträng på angivet språk.
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparameternvar
	 *			felaktig.
	 *	@throws	Kastar ett Y_NoSuchStringException om strängen inte fanns.
	 *
	 *	@param	$name är den språkkonstant vi vill ha.
	 *
	 *	@return	Returnerar en sträng på valt språk.
	 */
	public function getString($name) {
		$returnResult = '';
		
		return $returnResult;
	}
	
	/**
	 *	Sätter in diverse värden i en sträng, vilket gör att vi kan låta
	 *	olika språk hantera data på sina egna sätt. Detta innebär att systemet
	 *	kan skita fullständigt i grammatiska regler i olika språk.\n
	 *	\n
	 *	För att få in rätt värde på rätt plats, är det nödvändigt att veta på
	 *	vilken plats i vektorn varje värde förekommer. Dessa placeras sedan ut
	 *	på rätt plats genom att placera ut dess index (plats i vektorn + 1) på
	 *	den plats i strängen där värdet ska sättas in. Exempel:\n
	 *	\n
	 *	$string = "På menyn har vi %1\$s, %3\$s och %2\$s.";
	 *	$values = Array("kebab", "falafel", "kokta strumpor");
	 *	$result = insertValues($string, $values);
	 *	echo $result;
	 *	> På menyn har vi kebab, kokta strumpor och falafel.\n
	 *	\n
	 *	Placeras inga index-markeringar ut, antar metoden att värdena ska
	 *	placeras ut i den ordning de förekommer i vektorn.\n
	 *	\n
	 *	Följande saker kan sättas in i strängen och vektorn:\n
	 *	Procenttecken	=>	%%		(Ex: insertValues("42%%") => "42%")\n
	 *	Sträng			=>	%s		(Ex: insertValues("Hello, %s!", "world") 
	 								=> "Hello, world!")\n
	 *	Heltal			=>	%d		(Ex: insertValues("Svaret är %d", 42)
	 								=> "Svaret är 42")\n
	 *	Flyttal			=>	%f		(Ex: insertValues("pi: %f", 3.141529)
	 								=> "pi: 3.141529")
	 *
	 *	@throws	Kastar ett Y_IllegalValueException om inparametrarna var
	 *			felaktiga.
	 *
	 *	@param	$string är strängen som ska förändras. En sträng.
	 *	@param	$values är de värden som ska sättas in i strängen. En vektor
	 *			med strängar eller tal.
	 *
	 *	@return Returnerar en sträng.
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
	 *	Söker efter ett språk i databasen.
	 *	@private
	 *
	 *	@param $lang är en språkkod på två till fyra bokstäver.
	 *
	 *	@return	Returnerar sant om språket fanns i databasen, annars falskt.
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
<?php
// TODO Rensa upp den h�r klassen. Koden �r grisig och s�kert ocks� ineffektiv

/**
 *	Utilities �r en samling praktiska metoder f�r vanligt f�rekommande saker
 *	s�som validering av variabler, konvertering och liknande.
 *
 *	Klassen �r en samling statiska metoder, vilket g�r att en instans
 *	av klassen inte beh�ver (och b�r inte) skapas f�r att metoderna ska
 *	kunna anv�ndas.
 *
 *	@version	Version 		0.1.0\n
 *				Skapad: 		2005-02-17\n
 *				Senast �ndrad: 	2005-07-15
 *	@author		Johan Holmberg, johan@yamborii.net
 */
class Utilities {
	  
	/**
	 *	R�knar ut hur l�ngt det �r till dokument-roten.
	 *
	 *	@return Returnerar en str�ng med den relativa s�kv�gen till det
	 *			aktuella scriptets plats.
	 */
	public static function calcDist() {
		$step = '../';
		$dist = "";
		$occurences = substr_count(getenv("SCRIPT_NAME"), '/') - 1;
		for ($i = 0; $i < $occurences; $i++) {
			$dist .= $step;
		}
    	return $dist;
	}
	
	/**
	 *	Omvandlar ett datum till ett format som HTTP-protokollet f�rst�r.
	 *
	 *	@param	$date �r ett datum p� Yamboriis standardformat. Anges ingen
	 *			parameter, h�mtar metoden sitt datum fr�n den interna klocakn.
	 *
	 *	$return	Returnerar ett HTTP-formatterat datum. �r indatan ogiltig,
	 *			returneras ist�llet false.
	 */
	public static function HTTPDate($date = false) {
		$returnResult = false;
		
		if (!$date) {
			$date = time();
			$returnResult = date("r", $date);
		} else if (Utilities::validateDate($date)) {
			$date = strtotime($date);
			$returnResult = date("r", $date);
		}
		
		return $returnResult;
	}
	  
	/**
	 *	Skapar och formatterar ett datum p� Y2s standarformat. 
	 *
	 *	@param	$skipSecs anger om vi vill slippa sekunderna. Anges sant,
	 *			visas inga sekunder. Ett booleskt v�rde. F�r alla andra
	 *			v�rden och typer returneras datum med sekunder.
	 *
	 *	@return Returnerar dagens datum p� Y2s standardformat.
	 */
	public static function getY2Date($skipSecs = false) {
		if (is_bool($skipSecs) && $skipSecs) {
			$date = date("Y-m-d H:i", time());
		} else {
			$date = date("Y-m-d H:i:s", time());
		}
	  	return $date;
	}
	  
	/**
	 *	Sl�r ett �ga p� om ett datum st�mmer �verens med dagens eller
	 *	g�rdagens.
	 *
	 *	@param $date �r ett datum p� formen som returneras av
	 *			php-systemets date("Y-m-d H:i:s", time()), tillika Y2:s
	 *			standard-format.
	 *	@param $noSecs anger om vi vill slippa sekunderna. Anges sant,
	 *			visas inga sekunder.
	 *
	 *	@return Funktionen returnerar en str�ng. Motsvarar $date dagens
	 *			datum returneras str�ngen "idag hh:mm:ss". Motsvarar $date
	 *			g�rdagens datum returneras str�ngen "ig�r hh:mm:ss", annars en
	 *			str�ng p� formen "yyyy-mm-dd hh:mm:ss". Felaktiga indata ger
	 *			false som svar.
	 */
	public static function decorateDate($date, $skipSecs = false) {
		$returnResult = false;
		$oneDay = 86400;
		
		if (Utilities::validateDate($date) && is_bool($skipSecs)) {
			$unixDate = strtotime($date);
			$diff = time() - $unixDate;
			$date = date('Y-m-d H:i:s', $unixDate);
			
			switch (true) {
				case ($diff < $oneDay):
					// idag
					$returnResult = 'idag' . substr($date, strpos($date, ' '));
					break;
				case ($diff < 2 * $oneDay):
					// ig�r
					$returnResult = 'ig�r' . substr($date, strpos($date, ' '));
					break;
				default:
					$returnResult = $date;
					break;
			}
			
			if ($skipSecs && substr_count($returnResult, ':') == 2) {
				$returnResult = substr($returnResult, 0, -3);
			}
		}
		
		return $returnResult;
	}

	/**
	 *	Funktionen rensar inkommande str�ng fr�n farlig HTML-kod och
	 *	omvandlar radbrytningar till HTML-formatterade diton. Tecken som
	 *	kan misstolkas av webl�saren byts ut mot "s�kra" tecken.
	 *
	 *	@param	$iString �r str�ngen som ska kontrolleras och konverteras.
	 *
	 *	@return	Returnerar en tillr�ttalagd str�ng
	 */
	public static function cleanseString($iString) {
		$returnString = "";
		// Innan vi b�rjar, konverterar vi Mac- och Windows-kodning av radbrytning
		// till den enda r�tta: en newline!
		$iString = str_replace("\r\n", "\n", $iString);
		$iString = str_replace("\r", "\n", $iString);
		
		// F�rst tar vi bort all html och php ur str�ngen, och delar sedan upp den i
		// ett antal otroligt vackra delar, som vi sedan formatterar var f�r sig.
		$splitString = explode("\n", strip_tags(stripslashes(trim($iString))));
		$max = count($splitString);
		for ($i = 0; $i < $max; $i++) {
			$returnString .= htmlentities($splitString[$i], ENT_QUOTES);
			$returnString .= "<br />\n";
		}
		if ($max < 1) {
			$returnString = $iString;
		}
		
		// Nu bygger vi om alla '&amp;' till '&' d�r det beh�vs.
		$find = '/(&amp;)(\#?[a-zA-A0-9]+;)/';
		$replace = "&\$2";
		$returnString =  preg_replace($find, $replace, $returnString);
		
		// Innan vi avslutar v�r metod och returnerar resultatet, tar
		// vi bort v�r allra sista radbrytning, eftersom den �r i v�gen!
		if ($i > 0) {
			$returnString = substr($returnString, 0, -7);
		}
		
		return $returnString;
	}
	
	/**
	 *	Verifierar en str�ng som en giltig URL. Metoden har problem med URL:er
	 *	med tv� p� varandra f�ljande punkter.
	 *
	 *	@param	$url �r den str�ng som ska verifieras.
	 *	@param	$verifyProtocol �r ett booleskt v�rde som anger om funktionen ska
	 *			ta med http-prefixet i verifieringen. Om sant anges, kommer
	 *			http att kollas, i annat fall utel�mnas denna koll. Observera
	 *			att i det fall $verifyPrefix �r satt till falskt, och mailaderssen
	 *			inneh�ller http-prefixet, kommer falskt att returneras.\n
	 *			Det f�rdefinierade v�rdet p� $verifyPrefix �r falskt.
	 *
	 *	@return	Returnerar sant om $url �r en giltig URL, annars falskt.
	 */
	public static function validateURL($url, $verifyProtocol = false) {
		// FixDot garanterar att punkter inte f�ljs av eller f�reg�s av
		// ett bindestreck.
		$fixDot = '((?<!-)\.(?!-))';
		$protocol = '(http|https|ftp):\/\/(?!-)';
		// Subdomain godk�nner bara alfanumeriska tecken, bindestreck och
		// punkter. 
		$subdomain = '([0-9a-zA-Z\-]+'.$fixDot.'?)+';
		$topdomain = $fixDot.'[a-z]{2,4}';
		$path =	'(\/([a-z0-9A-Z\&\?\%\=\,\~\.\-\/]+)*)?';
		if ($verifyProtocol) {
			$urlFormat = '/^'.$protocol.$subdomain.$topdomain.$path.'$/';
		} else {
			$urlFormat = '/^'.$subdomain.$topdomain.$path.'$/';
		}
		
		return is_string($url) && preg_match($urlFormat, $url);
	}
	
	/**
	 *	Verifierar en str�ng som en giltig mailadress.
	 *
	 *	@param	$mail �r den str�ng som ska verifieras.
	 *	@param	$verifyPrefix �r ett booleskt v�rde som anger om funktionen ska
	 *			ta med mailto:-prefixet i verifieringen. Om sant anges, kommer
	 *			mailto: att kollas, i annat fall utel�mnas denna koll. Observera
	 *			att i det fall $verifyPrefix �r satt till falskt, och mailaderssen
	 *			inneh�ller mailto:-prefixet, kommer falskt att returneras.\n
	 *			Det f�rdefinierade v�rdet p� $verifyPrefix �r falskt.
	 *
	 *	@return	Returnerar sant om $mail �r en giltig mailadress, annars falskt.
	 */
	public static function validateMail($mail, $verifyPrefix = false) {
		$mailFormat = '/^';
		if ($verifyPrefix) {
			$mailFormat .= '(mailto:)';
		}
		$mailFormat .= '([0-9a-z]+)([0-9a-z\.\-_]+)@([0-9a-z\.\-_]+)\.([0-9a-z]+)$/';
		
		return is_string($mail) && preg_match($mailFormat, $mail);
	}
	
	/**
	 *	Verifierar att en inkommande str�ng �r ett datum p� Yamboriis standarformat.
	 *
	 *	@param	$date �r den str�ng som ska verifieras.
	 *
	 *	@return	Returnerar sant om $date var giltigt, annars falskt.
	 */
	public static function validateDate($date) {
		$dateFormat = '/^\d{3}[1-9]\-(0[1-9]|1[0-2])\-(0[1-9]|[1-2]\d|3[0-1]) ([0-1]\d|2[0-3]):[0-5]\d(:\d{2})?$/';
		
		return is_string($date) && preg_match($dateFormat, $date);
	}
}
?>
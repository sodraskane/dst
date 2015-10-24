<?php
// TODO Rensa upp den här klassen. Koden är grisig och säkert också ineffektiv

/**
 *	Utilities är en samling praktiska metoder för vanligt förekommande saker
 *	såsom validering av variabler, konvertering och liknande.
 *
 *	Klassen är en samling statiska metoder, vilket gör att en instans
 *	av klassen inte behöver (och bör inte) skapas för att metoderna ska
 *	kunna användas.
 *
 *	@version	Version 		0.1.0\n
 *				Skapad: 		2005-02-17\n
 *				Senast ändrad: 	2005-07-15
 *	@author		Johan Holmberg, johan@yamborii.net
 */
class Utilities {
	  
	/**
	 *	Räknar ut hur långt det är till dokument-roten.
	 *
	 *	@return Returnerar en sträng med den relativa sökvägen till det
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
	 *	Omvandlar ett datum till ett format som HTTP-protokollet förstår.
	 *
	 *	@param	$date är ett datum på Yamboriis standardformat. Anges ingen
	 *			parameter, hämtar metoden sitt datum från den interna klocakn.
	 *
	 *	$return	Returnerar ett HTTP-formatterat datum. Är indatan ogiltig,
	 *			returneras istället false.
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
	 *	Skapar och formatterar ett datum på Y2s standarformat. 
	 *
	 *	@param	$skipSecs anger om vi vill slippa sekunderna. Anges sant,
	 *			visas inga sekunder. Ett booleskt värde. För alla andra
	 *			värden och typer returneras datum med sekunder.
	 *
	 *	@return Returnerar dagens datum på Y2s standardformat.
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
	 *	Slår ett öga på om ett datum stämmer överens med dagens eller
	 *	gårdagens.
	 *
	 *	@param $date är ett datum på formen som returneras av
	 *			php-systemets date("Y-m-d H:i:s", time()), tillika Y2:s
	 *			standard-format.
	 *	@param $noSecs anger om vi vill slippa sekunderna. Anges sant,
	 *			visas inga sekunder.
	 *
	 *	@return Funktionen returnerar en sträng. Motsvarar $date dagens
	 *			datum returneras strängen "idag hh:mm:ss". Motsvarar $date
	 *			gårdagens datum returneras strängen "igår hh:mm:ss", annars en
	 *			sträng på formen "yyyy-mm-dd hh:mm:ss". Felaktiga indata ger
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
					// igår
					$returnResult = 'igår' . substr($date, strpos($date, ' '));
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
	 *	Funktionen rensar inkommande sträng från farlig HTML-kod och
	 *	omvandlar radbrytningar till HTML-formatterade diton. Tecken som
	 *	kan misstolkas av webläsaren byts ut mot "säkra" tecken.
	 *
	 *	@param	$iString är strängen som ska kontrolleras och konverteras.
	 *
	 *	@return	Returnerar en tillrättalagd sträng
	 */
	public static function cleanseString($iString) {
		$returnString = "";
		// Innan vi börjar, konverterar vi Mac- och Windows-kodning av radbrytning
		// till den enda rätta: en newline!
		$iString = str_replace("\r\n", "\n", $iString);
		$iString = str_replace("\r", "\n", $iString);
		
		// Först tar vi bort all html och php ur strängen, och delar sedan upp den i
		// ett antal otroligt vackra delar, som vi sedan formatterar var för sig.
		$splitString = explode("\n", strip_tags(stripslashes(trim($iString))));
		$max = count($splitString);
		for ($i = 0; $i < $max; $i++) {
			$returnString .= htmlentities($splitString[$i], ENT_QUOTES);
			$returnString .= "<br />\n";
		}
		if ($max < 1) {
			$returnString = $iString;
		}
		
		// Nu bygger vi om alla '&amp;' till '&' där det behövs.
		$find = '/(&amp;)(\#?[a-zA-A0-9]+;)/';
		$replace = "&\$2";
		$returnString =  preg_replace($find, $replace, $returnString);
		
		// Innan vi avslutar vår metod och returnerar resultatet, tar
		// vi bort vår allra sista radbrytning, eftersom den är i vägen!
		if ($i > 0) {
			$returnString = substr($returnString, 0, -7);
		}
		
		return $returnString;
	}
	
	/**
	 *	Verifierar en sträng som en giltig URL. Metoden har problem med URL:er
	 *	med två på varandra följande punkter.
	 *
	 *	@param	$url är den sträng som ska verifieras.
	 *	@param	$verifyProtocol är ett booleskt värde som anger om funktionen ska
	 *			ta med http-prefixet i verifieringen. Om sant anges, kommer
	 *			http att kollas, i annat fall utelämnas denna koll. Observera
	 *			att i det fall $verifyPrefix är satt till falskt, och mailaderssen
	 *			innehåller http-prefixet, kommer falskt att returneras.\n
	 *			Det fördefinierade värdet på $verifyPrefix är falskt.
	 *
	 *	@return	Returnerar sant om $url är en giltig URL, annars falskt.
	 */
	public static function validateURL($url, $verifyProtocol = false) {
		// FixDot garanterar att punkter inte följs av eller föregås av
		// ett bindestreck.
		$fixDot = '((?<!-)\.(?!-))';
		$protocol = '(http|https|ftp):\/\/(?!-)';
		// Subdomain godkänner bara alfanumeriska tecken, bindestreck och
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
	 *	Verifierar en sträng som en giltig mailadress.
	 *
	 *	@param	$mail är den sträng som ska verifieras.
	 *	@param	$verifyPrefix är ett booleskt värde som anger om funktionen ska
	 *			ta med mailto:-prefixet i verifieringen. Om sant anges, kommer
	 *			mailto: att kollas, i annat fall utelämnas denna koll. Observera
	 *			att i det fall $verifyPrefix är satt till falskt, och mailaderssen
	 *			innehåller mailto:-prefixet, kommer falskt att returneras.\n
	 *			Det fördefinierade värdet på $verifyPrefix är falskt.
	 *
	 *	@return	Returnerar sant om $mail är en giltig mailadress, annars falskt.
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
	 *	Verifierar att en inkommande sträng är ett datum på Yamboriis standarformat.
	 *
	 *	@param	$date är den sträng som ska verifieras.
	 *
	 *	@return	Returnerar sant om $date var giltigt, annars falskt.
	 */
	public static function validateDate($date) {
		$dateFormat = '/^\d{3}[1-9]\-(0[1-9]|1[0-2])\-(0[1-9]|[1-2]\d|3[0-1]) ([0-1]\d|2[0-3]):[0-5]\d(:\d{2})?$/';
		
		return is_string($date) && preg_match($dateFormat, $date);
	}
}
?>
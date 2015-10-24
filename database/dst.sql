-- phpMyAdmin SQL Dump
-- version 2.11.6
-- http://www.phpmyadmin.net
--
-- Värd: localhost
-- Skapad: 16 februari 2002 kl 00:56
-- Serverversion: 5.0.51
-- PHP-version: 5.2.6

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Databas: `dst`
--

-- --------------------------------------------------------

--
-- Struktur för tabell `deltagare`
--

DROP TABLE IF EXISTS `deltagare`;
CREATE TABLE `deltagare` (
  `id` int(11) NOT NULL auto_increment,
  `namn` varchar(64) character set utf8 collate utf8_bin default NULL,
  `kar` varchar(64) character set utf8 collate utf8_bin default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=431 ;

-- --------------------------------------------------------

--
-- Struktur för tabell `talarlista`
--

DROP TABLE IF EXISTS `talarlista`;
CREATE TABLE `talarlista` (
  `plats` int(11) NOT NULL auto_increment,
  `talare` int(11) NOT NULL,
  PRIMARY KEY  (`plats`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_swedish_ci AUTO_INCREMENT=2 ;

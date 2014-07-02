-- MySQL Administrator dump 1.4
--
-- ------------------------------------------------------
-- Server version	4.1.14-Max


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


--
-- Create schema hvdb
--

CREATE DATABASE IF NOT EXISTS hvdb;
USE hvdb;

--
-- Definition of table `BOARD_types`
--
CREATE TABLE `BOARD_types` (
  `ID` tinyint(3) unsigned NOT NULL default '0',
  `NAME` varchar(10) NOT NULL default '',
  `DESCR` varchar(45) NOT NULL default '',
  `CHANS` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `BOARD_types`
--

/*!40000 ALTER TABLE `BOARD_types` DISABLE KEYS */;
INSERT INTO `BOARD_types` (`ID`,`NAME`,`DESCR`,`CHANS`) VALUES 
 (0,'NONE','None',0),
 (1,'MASTER','Master Board',8),
 (2,'RDB30','Remote Distribution 30',30),
 (3,'RDB36','Remote Distribution 36',36);
/*!40000 ALTER TABLE `BOARD_types` ENABLE KEYS */;


--
-- Definition of table `CALIBRATION`
--
CREATE TABLE `CALIBRATION` (
  `ID` smallint(5) unsigned NOT NULL auto_increment,
  `TYPE` smallint(5) unsigned NOT NULL default '0',
  `DAC` blob NOT NULL,
  `ADC` blob NOT NULL,
  `CUR` mediumblob NOT NULL,
  `VMON` tinyblob NOT NULL,
  PRIMARY KEY  (`ID`,`TYPE`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `CALIBRATION`
--

/*!40000 ALTER TABLE `CALIBRATION` DISABLE KEYS */;
/*!40000 ALTER TABLE `CALIBRATION` ENABLE KEYS */;


--
-- Definition of table `CONFIG`
--
CREATE TABLE `CONFIG` (
  `NAME` varchar(45) NOT NULL default '',
  `HOSTID` int(10) unsigned NOT NULL default '0',
  `DATASLOT` int(10) unsigned NOT NULL default '0',
  `DATA` mediumblob NOT NULL,
  PRIMARY KEY  (`NAME`,`HOSTID`,`DATASLOT`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `CONFIG`
--

/*!40000 ALTER TABLE `CONFIG` DISABLE KEYS */;
/*!40000 ALTER TABLE `CONFIG` ENABLE KEYS */;


--
-- Definition of table `CSC`
--
CREATE TABLE `CSC` (
  `ID` smallint(5) unsigned NOT NULL auto_increment,
  `ENDCAP` char(1) NOT NULL default '0',
  `STATION` tinyint(3) unsigned NOT NULL default '0',
  `RING` tinyint(3) unsigned NOT NULL default '0',
  `POSITION` smallint(5) unsigned NOT NULL default '0',
  `NAME` varchar(12) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `CSC_IDX` (`ENDCAP`,`STATION`,`RING`,`POSITION`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `CSC`
--

/*!40000 ALTER TABLE `CSC` DISABLE KEYS */;
INSERT INTO `CSC` (`ID`,`ENDCAP`,`STATION`,`RING`,`POSITION`,`NAME`) VALUES 
 (1,'-',1,3,1,'ME-1/3/01'),
 (2,'-',1,3,2,'ME-1/3/02'),
 (3,'-',1,3,3,'ME-1/3/03'),
 (4,'-',1,3,4,'ME-1/3/04'),
 (5,'-',1,3,5,'ME-1/3/05'),
 (6,'-',1,3,6,'ME-1/3/06'),
 (7,'-',1,3,7,'ME-1/3/07'),
 (8,'-',1,3,8,'ME-1/3/08'),
 (9,'-',1,3,9,'ME-1/3/09'),
 (10,'-',1,3,10,'ME-1/3/10'),
 (11,'-',1,3,11,'ME-1/3/11'),
 (12,'-',1,3,12,'ME-1/3/12'),
 (13,'-',1,3,13,'ME-1/3/13'),
 (14,'-',1,3,14,'ME-1/3/14'),
 (15,'-',1,3,15,'ME-1/3/15'),
 (16,'-',1,3,16,'ME-1/3/16'),
 (17,'-',1,3,17,'ME-1/3/17'),
 (18,'-',1,3,18,'ME-1/3/18'),
 (19,'-',1,3,19,'ME-1/3/19'),
 (20,'-',1,3,20,'ME-1/3/20'),
 (21,'-',1,3,21,'ME-1/3/21'),
 (22,'-',1,3,22,'ME-1/3/22'),
 (23,'-',1,3,23,'ME-1/3/23'),
 (24,'-',1,3,24,'ME-1/3/24'),
 (25,'-',1,3,25,'ME-1/3/25'),
 (26,'-',1,3,26,'ME-1/3/26'),
 (27,'-',1,3,27,'ME-1/3/27'),
 (28,'-',1,3,28,'ME-1/3/28'),
 (29,'-',1,3,29,'ME-1/3/29'),
 (30,'-',1,3,30,'ME-1/3/30'),
 (31,'-',1,3,31,'ME-1/3/31'),
 (32,'-',1,3,32,'ME-1/3/32'),
 (33,'-',1,3,33,'ME-1/3/33'),
 (34,'-',1,3,34,'ME-1/3/34'),
 (35,'-',1,3,35,'ME-1/3/35'),
 (36,'-',1,3,36,'ME-1/3/36'),
 (37,'-',1,2,1,'ME-1/2/01'),
 (38,'-',1,2,2,'ME-1/2/02'),
 (39,'-',1,2,3,'ME-1/2/03'),
 (40,'-',1,2,4,'ME-1/2/04'),
 (41,'-',1,2,5,'ME-1/2/05'),
 (42,'-',1,2,6,'ME-1/2/06'),
 (43,'-',1,2,7,'ME-1/2/07'),
 (44,'-',1,2,8,'ME-1/2/08'),
 (45,'-',1,2,9,'ME-1/2/09'),
 (46,'-',1,2,10,'ME-1/2/10'),
 (47,'-',1,2,11,'ME-1/2/11'),
 (48,'-',1,2,12,'ME-1/2/12'),
 (49,'-',1,2,13,'ME-1/2/13'),
 (50,'-',1,2,14,'ME-1/2/14'),
 (51,'-',1,2,15,'ME-1/2/15'),
 (52,'-',1,2,16,'ME-1/2/16'),
 (53,'-',1,2,17,'ME-1/2/17'),
 (54,'-',1,2,18,'ME-1/2/18'),
 (55,'-',1,2,19,'ME-1/2/19'),
 (56,'-',1,2,20,'ME-1/2/20'),
 (57,'-',1,2,21,'ME-1/2/21'),
 (58,'-',1,2,22,'ME-1/2/22'),
 (59,'-',1,2,23,'ME-1/2/23'),
 (60,'-',1,2,24,'ME-1/2/24'),
 (61,'-',1,2,25,'ME-1/2/25'),
 (62,'-',1,2,26,'ME-1/2/26'),
 (63,'-',1,2,27,'ME-1/2/27'),
 (64,'-',1,2,28,'ME-1/2/28'),
 (65,'-',1,2,29,'ME-1/2/29'),
 (66,'-',1,2,30,'ME-1/2/30'),
 (67,'-',1,2,31,'ME-1/2/31'),
 (68,'-',1,2,32,'ME-1/2/32'),
 (69,'-',1,2,33,'ME-1/2/33'),
 (70,'-',1,2,34,'ME-1/2/34'),
 (71,'-',1,2,35,'ME-1/2/35'),
 (72,'-',1,2,36,'ME-1/2/36'),
 (73,'-',2,2,1,'ME-2/2/01'),
 (74,'-',2,2,2,'ME-2/2/02'),
 (75,'-',2,2,3,'ME-2/2/03'),
 (76,'-',2,2,4,'ME-2/2/04'),
 (77,'-',2,2,5,'ME-2/2/05'),
 (78,'-',2,2,6,'ME-2/2/06'),
 (79,'-',2,2,7,'ME-2/2/07'),
 (80,'-',2,2,8,'ME-2/2/08'),
 (81,'-',2,2,9,'ME-2/2/09'),
 (82,'-',2,2,10,'ME-2/2/10'),
 (83,'-',2,2,11,'ME-2/2/11'),
 (84,'-',2,2,12,'ME-2/2/12'),
 (85,'-',2,2,13,'ME-2/2/13'),
 (86,'-',2,2,14,'ME-2/2/14'),
 (87,'-',2,2,15,'ME-2/2/15'),
 (88,'-',2,2,16,'ME-2/2/16'),
 (89,'-',2,2,17,'ME-2/2/17'),
 (90,'-',2,2,18,'ME-2/2/18'),
 (91,'-',2,2,19,'ME-2/2/19'),
 (92,'-',2,2,20,'ME-2/2/20'),
 (93,'-',2,2,21,'ME-2/2/21'),
 (94,'-',2,2,22,'ME-2/2/22'),
 (95,'-',2,2,23,'ME-2/2/23'),
 (96,'-',2,2,24,'ME-2/2/24'),
 (97,'-',2,2,25,'ME-2/2/25'),
 (98,'-',2,2,26,'ME-2/2/26'),
 (99,'-',2,2,27,'ME-2/2/27'),
 (100,'-',2,2,28,'ME-2/2/28'),
 (101,'-',2,2,29,'ME-2/2/29'),
 (102,'-',2,2,30,'ME-2/2/30'),
 (103,'-',2,2,31,'ME-2/2/31'),
 (104,'-',2,2,32,'ME-2/2/32'),
 (105,'-',2,2,33,'ME-2/2/33'),
 (106,'-',2,2,34,'ME-2/2/34'),
 (107,'-',2,2,35,'ME-2/2/35'),
 (108,'-',2,2,36,'ME-2/2/36'),
 (109,'-',2,1,1,'ME-2/1/01'),
 (110,'-',2,1,2,'ME-2/1/02'),
 (111,'-',2,1,3,'ME-2/1/03'),
 (112,'-',2,1,4,'ME-2/1/04'),
 (113,'-',2,1,5,'ME-2/1/05'),
 (114,'-',2,1,6,'ME-2/1/06'),
 (115,'-',2,1,7,'ME-2/1/07'),
 (116,'-',2,1,8,'ME-2/1/08'),
 (117,'-',2,1,9,'ME-2/1/09'),
 (118,'-',2,1,10,'ME-2/1/10'),
 (119,'-',2,1,11,'ME-2/1/11'),
 (120,'-',2,1,12,'ME-2/1/12'),
 (121,'-',2,1,13,'ME-2/1/13'),
 (122,'-',2,1,14,'ME-2/1/14'),
 (123,'-',2,1,15,'ME-2/1/15'),
 (124,'-',2,1,16,'ME-2/1/16'),
 (125,'-',2,1,17,'ME-2/1/17'),
 (126,'-',2,1,18,'ME-2/1/18'),
 (127,'-',3,2,1,'ME-3/2/01'),
 (128,'-',3,2,2,'ME-3/2/02'),
 (129,'-',3,2,3,'ME-3/2/03'),
 (130,'-',3,2,4,'ME-3/2/04'),
 (131,'-',3,2,5,'ME-3/2/05'),
 (132,'-',3,2,6,'ME-3/2/06'),
 (133,'-',3,2,7,'ME-3/2/07'),
 (134,'-',3,2,8,'ME-3/2/08'),
 (135,'-',3,2,9,'ME-3/2/09'),
 (136,'-',3,2,10,'ME-3/2/10'),
 (137,'-',3,2,11,'ME-3/2/11'),
 (138,'-',3,2,12,'ME-3/2/12'),
 (139,'-',3,2,13,'ME-3/2/13'),
 (140,'-',3,2,14,'ME-3/2/14'),
 (141,'-',3,2,15,'ME-3/2/15'),
 (142,'-',3,2,16,'ME-3/2/16'),
 (143,'-',3,2,17,'ME-3/2/17'),
 (144,'-',3,2,18,'ME-3/2/18'),
 (145,'-',3,2,19,'ME-3/2/19'),
 (146,'-',3,2,20,'ME-3/2/20'),
 (147,'-',3,2,21,'ME-3/2/21'),
 (148,'-',3,2,22,'ME-3/2/22'),
 (149,'-',3,2,23,'ME-3/2/23'),
 (150,'-',3,2,24,'ME-3/2/24'),
 (151,'-',3,2,25,'ME-3/2/25'),
 (152,'-',3,2,26,'ME-3/2/26'),
 (153,'-',3,2,27,'ME-3/2/27'),
 (154,'-',3,2,28,'ME-3/2/28'),
 (155,'-',3,2,29,'ME-3/2/29'),
 (156,'-',3,2,30,'ME-3/2/30'),
 (157,'-',3,2,31,'ME-3/2/31'),
 (158,'-',3,2,32,'ME-3/2/32'),
 (159,'-',3,2,33,'ME-3/2/33'),
 (160,'-',3,2,34,'ME-3/2/34'),
 (161,'-',3,2,35,'ME-3/2/35'),
 (162,'-',3,2,36,'ME-3/2/36'),
 (163,'-',3,1,1,'ME-3/1/01'),
 (164,'-',3,1,2,'ME-3/1/02'),
 (165,'-',3,1,3,'ME-3/1/03'),
 (166,'-',3,1,4,'ME-3/1/04'),
 (167,'-',3,1,5,'ME-3/1/05'),
 (168,'-',3,1,6,'ME-3/1/06'),
 (169,'-',3,1,7,'ME-3/1/07'),
 (170,'-',3,1,8,'ME-3/1/08'),
 (171,'-',3,1,9,'ME-3/1/09'),
 (172,'-',3,1,10,'ME-3/1/10'),
 (173,'-',3,1,11,'ME-3/1/11'),
 (174,'-',3,1,12,'ME-3/1/12'),
 (175,'-',3,1,13,'ME-3/1/13'),
 (176,'-',3,1,14,'ME-3/1/14'),
 (177,'-',3,1,15,'ME-3/1/15'),
 (178,'-',3,1,16,'ME-3/1/16'),
 (179,'-',3,1,17,'ME-3/1/17'),
 (180,'-',3,1,18,'ME-3/1/18'),
 (181,'-',4,2,1,'ME-4/2/01'),
 (182,'-',4,2,2,'ME-4/2/02'),
 (183,'-',4,2,3,'ME-4/2/03'),
 (184,'-',4,2,4,'ME-4/2/04'),
 (185,'-',4,2,5,'ME-4/2/05'),
 (186,'-',4,2,6,'ME-4/2/06'),
 (187,'-',4,2,7,'ME-4/2/07'),
 (188,'-',4,2,8,'ME-4/2/08'),
 (189,'-',4,2,9,'ME-4/2/09'),
 (190,'-',4,2,10,'ME-4/2/10'),
 (191,'-',4,2,11,'ME-4/2/11'),
 (192,'-',4,2,12,'ME-4/2/12'),
 (193,'-',4,2,13,'ME-4/2/13'),
 (194,'-',4,2,14,'ME-4/2/14'),
 (195,'-',4,2,15,'ME-4/2/15'),
 (196,'-',4,2,16,'ME-4/2/16'),
 (197,'-',4,2,17,'ME-4/2/17'),
 (198,'-',4,2,18,'ME-4/2/18'),
 (199,'-',4,2,19,'ME-4/2/19'),
 (200,'-',4,2,20,'ME-4/2/20'),
 (201,'-',4,2,21,'ME-4/2/21'),
 (202,'-',4,2,22,'ME-4/2/22'),
 (203,'-',4,2,23,'ME-4/2/23'),
 (204,'-',4,2,24,'ME-4/2/24'),
 (205,'-',4,2,25,'ME-4/2/25'),
 (206,'-',4,2,26,'ME-4/2/26'),
 (207,'-',4,2,27,'ME-4/2/27'),
 (208,'-',4,2,28,'ME-4/2/28'),
 (209,'-',4,2,29,'ME-4/2/29'),
 (210,'-',4,2,30,'ME-4/2/30'),
 (211,'-',4,2,31,'ME-4/2/31'),
 (212,'-',4,2,32,'ME-4/2/32'),
 (213,'-',4,2,33,'ME-4/2/33'),
 (214,'-',4,2,34,'ME-4/2/34'),
 (215,'-',4,2,35,'ME-4/2/35'),
 (216,'-',4,2,36,'ME-4/2/36'),
 (217,'-',4,1,1,'ME-4/1/01'),
 (218,'-',4,1,2,'ME-4/1/02'),
 (219,'-',4,1,3,'ME-4/1/03'),
 (220,'-',4,1,4,'ME-4/1/04'),
 (221,'-',4,1,5,'ME-4/1/05'),
 (222,'-',4,1,6,'ME-4/1/06'),
 (223,'-',4,1,7,'ME-4/1/07'),
 (224,'-',4,1,8,'ME-4/1/08'),
 (225,'-',4,1,9,'ME-4/1/09'),
 (226,'-',4,1,10,'ME-4/1/10'),
 (227,'-',4,1,11,'ME-4/1/11'),
 (228,'-',4,1,12,'ME-4/1/12'),
 (229,'-',4,1,13,'ME-4/1/13'),
 (230,'-',4,1,14,'ME-4/1/14'),
 (231,'-',4,1,15,'ME-4/1/15'),
 (232,'-',4,1,16,'ME-4/1/16'),
 (233,'-',4,1,17,'ME-4/1/17'),
 (234,'-',4,1,18,'ME-4/1/18'),
 (235,'+',1,3,1,'ME+1/3/01'),
 (236,'+',1,3,2,'ME+1/3/02'),
 (237,'+',1,3,3,'ME+1/3/03'),
 (238,'+',1,3,4,'ME+1/3/04'),
 (239,'+',1,3,5,'ME+1/3/05'),
 (240,'+',1,3,6,'ME+1/3/06'),
 (241,'+',1,3,7,'ME+1/3/07'),
 (242,'+',1,3,8,'ME+1/3/08'),
 (243,'+',1,3,9,'ME+1/3/09'),
 (244,'+',1,3,10,'ME+1/3/10'),
 (245,'+',1,3,11,'ME+1/3/11'),
 (246,'+',1,3,12,'ME+1/3/12'),
 (247,'+',1,3,13,'ME+1/3/13'),
 (248,'+',1,3,14,'ME+1/3/14'),
 (249,'+',1,3,15,'ME+1/3/15'),
 (250,'+',1,3,16,'ME+1/3/16'),
 (251,'+',1,3,17,'ME+1/3/17'),
 (252,'+',1,3,18,'ME+1/3/18'),
 (253,'+',1,3,19,'ME+1/3/19'),
 (254,'+',1,3,20,'ME+1/3/20'),
 (255,'+',1,3,21,'ME+1/3/21'),
 (256,'+',1,3,22,'ME+1/3/22'),
 (257,'+',1,3,23,'ME+1/3/23'),
 (258,'+',1,3,24,'ME+1/3/24'),
 (259,'+',1,3,25,'ME+1/3/25'),
 (260,'+',1,3,26,'ME+1/3/26'),
 (261,'+',1,3,27,'ME+1/3/27'),
 (262,'+',1,3,28,'ME+1/3/28'),
 (263,'+',1,3,29,'ME+1/3/29'),
 (264,'+',1,3,30,'ME+1/3/30'),
 (265,'+',1,3,31,'ME+1/3/31'),
 (266,'+',1,3,32,'ME+1/3/32'),
 (267,'+',1,3,33,'ME+1/3/33'),
 (268,'+',1,3,34,'ME+1/3/34'),
 (269,'+',1,3,35,'ME+1/3/35'),
 (270,'+',1,3,36,'ME+1/3/36'),
 (271,'+',1,2,1,'ME+1/2/01'),
 (272,'+',1,2,2,'ME+1/2/02'),
 (273,'+',1,2,3,'ME+1/2/03'),
 (274,'+',1,2,4,'ME+1/2/04'),
 (275,'+',1,2,5,'ME+1/2/05'),
 (276,'+',1,2,6,'ME+1/2/06'),
 (277,'+',1,2,7,'ME+1/2/07'),
 (278,'+',1,2,8,'ME+1/2/08'),
 (279,'+',1,2,9,'ME+1/2/09'),
 (280,'+',1,2,10,'ME+1/2/10'),
 (281,'+',1,2,11,'ME+1/2/11'),
 (282,'+',1,2,12,'ME+1/2/12'),
 (283,'+',1,2,13,'ME+1/2/13'),
 (284,'+',1,2,14,'ME+1/2/14'),
 (285,'+',1,2,15,'ME+1/2/15'),
 (286,'+',1,2,16,'ME+1/2/16'),
 (287,'+',1,2,17,'ME+1/2/17'),
 (288,'+',1,2,18,'ME+1/2/18'),
 (289,'+',1,2,19,'ME+1/2/19'),
 (290,'+',1,2,20,'ME+1/2/20'),
 (291,'+',1,2,21,'ME+1/2/21'),
 (292,'+',1,2,22,'ME+1/2/22'),
 (293,'+',1,2,23,'ME+1/2/23'),
 (294,'+',1,2,24,'ME+1/2/24'),
 (295,'+',1,2,25,'ME+1/2/25'),
 (296,'+',1,2,26,'ME+1/2/26'),
 (297,'+',1,2,27,'ME+1/2/27'),
 (298,'+',1,2,28,'ME+1/2/28'),
 (299,'+',1,2,29,'ME+1/2/29'),
 (300,'+',1,2,30,'ME+1/2/30'),
 (301,'+',1,2,31,'ME+1/2/31'),
 (302,'+',1,2,32,'ME+1/2/32'),
 (303,'+',1,2,33,'ME+1/2/33'),
 (304,'+',1,2,34,'ME+1/2/34'),
 (305,'+',1,2,35,'ME+1/2/35'),
 (306,'+',1,2,36,'ME+1/2/36'),
 (307,'+',2,2,1,'ME+2/2/01'),
 (308,'+',2,2,2,'ME+2/2/02'),
 (309,'+',2,2,3,'ME+2/2/03'),
 (310,'+',2,2,4,'ME+2/2/04'),
 (311,'+',2,2,5,'ME+2/2/05'),
 (312,'+',2,2,6,'ME+2/2/06'),
 (313,'+',2,2,7,'ME+2/2/07'),
 (314,'+',2,2,8,'ME+2/2/08'),
 (315,'+',2,2,9,'ME+2/2/09'),
 (316,'+',2,2,10,'ME+2/2/10'),
 (317,'+',2,2,11,'ME+2/2/11'),
 (318,'+',2,2,12,'ME+2/2/12'),
 (319,'+',2,2,13,'ME+2/2/13'),
 (320,'+',2,2,14,'ME+2/2/14'),
 (321,'+',2,2,15,'ME+2/2/15'),
 (322,'+',2,2,16,'ME+2/2/16'),
 (323,'+',2,2,17,'ME+2/2/17'),
 (324,'+',2,2,18,'ME+2/2/18'),
 (325,'+',2,2,19,'ME+2/2/19'),
 (326,'+',2,2,20,'ME+2/2/20'),
 (327,'+',2,2,21,'ME+2/2/21'),
 (328,'+',2,2,22,'ME+2/2/22'),
 (329,'+',2,2,23,'ME+2/2/23'),
 (330,'+',2,2,24,'ME+2/2/24'),
 (331,'+',2,2,25,'ME+2/2/25'),
 (332,'+',2,2,26,'ME+2/2/26'),
 (333,'+',2,2,27,'ME+2/2/27'),
 (334,'+',2,2,28,'ME+2/2/28'),
 (335,'+',2,2,29,'ME+2/2/29'),
 (336,'+',2,2,30,'ME+2/2/30'),
 (337,'+',2,2,31,'ME+2/2/31'),
 (338,'+',2,2,32,'ME+2/2/32'),
 (339,'+',2,2,33,'ME+2/2/33'),
 (340,'+',2,2,34,'ME+2/2/34'),
 (341,'+',2,2,35,'ME+2/2/35'),
 (342,'+',2,2,36,'ME+2/2/36'),
 (343,'+',2,1,1,'ME+2/1/01'),
 (344,'+',2,1,2,'ME+2/1/02'),
 (345,'+',2,1,3,'ME+2/1/03'),
 (346,'+',2,1,4,'ME+2/1/04'),
 (347,'+',2,1,5,'ME+2/1/05'),
 (348,'+',2,1,6,'ME+2/1/06'),
 (349,'+',2,1,7,'ME+2/1/07'),
 (350,'+',2,1,8,'ME+2/1/08'),
 (351,'+',2,1,9,'ME+2/1/09'),
 (352,'+',2,1,10,'ME+2/1/10'),
 (353,'+',2,1,11,'ME+2/1/11'),
 (354,'+',2,1,12,'ME+2/1/12'),
 (355,'+',2,1,13,'ME+2/1/13'),
 (356,'+',2,1,14,'ME+2/1/14'),
 (357,'+',2,1,15,'ME+2/1/15'),
 (358,'+',2,1,16,'ME+2/1/16'),
 (359,'+',2,1,17,'ME+2/1/17'),
 (360,'+',2,1,18,'ME+2/1/18'),
 (361,'+',3,2,1,'ME+3/2/01'),
 (362,'+',3,2,2,'ME+3/2/02'),
 (363,'+',3,2,3,'ME+3/2/03'),
 (364,'+',3,2,4,'ME+3/2/04'),
 (365,'+',3,2,5,'ME+3/2/05'),
 (366,'+',3,2,6,'ME+3/2/06'),
 (367,'+',3,2,7,'ME+3/2/07'),
 (368,'+',3,2,8,'ME+3/2/08'),
 (369,'+',3,2,9,'ME+3/2/09'),
 (370,'+',3,2,10,'ME+3/2/10'),
 (371,'+',3,2,11,'ME+3/2/11'),
 (372,'+',3,2,12,'ME+3/2/12'),
 (373,'+',3,2,13,'ME+3/2/13'),
 (374,'+',3,2,14,'ME+3/2/14'),
 (375,'+',3,2,15,'ME+3/2/15'),
 (376,'+',3,2,16,'ME+3/2/16'),
 (377,'+',3,2,17,'ME+3/2/17'),
 (378,'+',3,2,18,'ME+3/2/18'),
 (379,'+',3,2,19,'ME+3/2/19'),
 (380,'+',3,2,20,'ME+3/2/20'),
 (381,'+',3,2,21,'ME+3/2/21'),
 (382,'+',3,2,22,'ME+3/2/22'),
 (383,'+',3,2,23,'ME+3/2/23'),
 (384,'+',3,2,24,'ME+3/2/24'),
 (385,'+',3,2,25,'ME+3/2/25'),
 (386,'+',3,2,26,'ME+3/2/26'),
 (387,'+',3,2,27,'ME+3/2/27'),
 (388,'+',3,2,28,'ME+3/2/28'),
 (389,'+',3,2,29,'ME+3/2/29'),
 (390,'+',3,2,30,'ME+3/2/30'),
 (391,'+',3,2,31,'ME+3/2/31'),
 (392,'+',3,2,32,'ME+3/2/32'),
 (393,'+',3,2,33,'ME+3/2/33'),
 (394,'+',3,2,34,'ME+3/2/34'),
 (395,'+',3,2,35,'ME+3/2/35'),
 (396,'+',3,2,36,'ME+3/2/36'),
 (397,'+',3,1,1,'ME+3/1/01'),
 (398,'+',3,1,2,'ME+3/1/02'),
 (399,'+',3,1,3,'ME+3/1/03'),
 (400,'+',3,1,4,'ME+3/1/04'),
 (401,'+',3,1,5,'ME+3/1/05'),
 (402,'+',3,1,6,'ME+3/1/06'),
 (403,'+',3,1,7,'ME+3/1/07'),
 (404,'+',3,1,8,'ME+3/1/08'),
 (405,'+',3,1,9,'ME+3/1/09'),
 (406,'+',3,1,10,'ME+3/1/10'),
 (407,'+',3,1,11,'ME+3/1/11'),
 (408,'+',3,1,12,'ME+3/1/12'),
 (409,'+',3,1,13,'ME+3/1/13'),
 (410,'+',3,1,14,'ME+3/1/14'),
 (411,'+',3,1,15,'ME+3/1/15'),
 (412,'+',3,1,16,'ME+3/1/16'),
 (413,'+',3,1,17,'ME+3/1/17'),
 (414,'+',3,1,18,'ME+3/1/18'),
 (415,'+',4,2,1,'ME+4/2/01'),
 (416,'+',4,2,2,'ME+4/2/02'),
 (417,'+',4,2,3,'ME+4/2/03'),
 (418,'+',4,2,4,'ME+4/2/04'),
 (419,'+',4,2,5,'ME+4/2/05'),
 (420,'+',4,2,6,'ME+4/2/06'),
 (421,'+',4,2,7,'ME+4/2/07'),
 (422,'+',4,2,8,'ME+4/2/08'),
 (423,'+',4,2,9,'ME+4/2/09'),
 (424,'+',4,2,10,'ME+4/2/10'),
 (425,'+',4,2,11,'ME+4/2/11'),
 (426,'+',4,2,12,'ME+4/2/12'),
 (427,'+',4,2,13,'ME+4/2/13'),
 (428,'+',4,2,14,'ME+4/2/14'),
 (429,'+',4,2,15,'ME+4/2/15'),
 (430,'+',4,2,16,'ME+4/2/16'),
 (431,'+',4,2,17,'ME+4/2/17'),
 (432,'+',4,2,18,'ME+4/2/18'),
 (433,'+',4,2,19,'ME+4/2/19'),
 (434,'+',4,2,20,'ME+4/2/20'),
 (435,'+',4,2,21,'ME+4/2/21'),
 (436,'+',4,2,22,'ME+4/2/22'),
 (437,'+',4,2,23,'ME+4/2/23'),
 (438,'+',4,2,24,'ME+4/2/24'),
 (439,'+',4,2,25,'ME+4/2/25'),
 (440,'+',4,2,26,'ME+4/2/26'),
 (441,'+',4,2,27,'ME+4/2/27'),
 (442,'+',4,2,28,'ME+4/2/28'),
 (443,'+',4,2,29,'ME+4/2/29'),
 (444,'+',4,2,30,'ME+4/2/30'),
 (445,'+',4,2,31,'ME+4/2/31'),
 (446,'+',4,2,32,'ME+4/2/32'),
 (447,'+',4,2,33,'ME+4/2/33'),
 (448,'+',4,2,34,'ME+4/2/34'),
 (449,'+',4,2,35,'ME+4/2/35'),
 (450,'+',4,2,36,'ME+4/2/36'),
 (451,'+',4,1,1,'ME+4/1/01'),
 (452,'+',4,1,2,'ME+4/1/02'),
 (453,'+',4,1,3,'ME+4/1/03'),
 (454,'+',4,1,4,'ME+4/1/04'),
 (455,'+',4,1,5,'ME+4/1/05'),
 (456,'+',4,1,6,'ME+4/1/06'),
 (457,'+',4,1,7,'ME+4/1/07'),
 (458,'+',4,1,8,'ME+4/1/08'),
 (459,'+',4,1,9,'ME+4/1/09'),
 (460,'+',4,1,10,'ME+4/1/10'),
 (461,'+',4,1,11,'ME+4/1/11'),
 (462,'+',4,1,12,'ME+4/1/12'),
 (463,'+',4,1,13,'ME+4/1/13'),
 (464,'+',4,1,14,'ME+4/1/14'),
 (465,'+',4,1,15,'ME+4/1/15'),
 (466,'+',4,1,16,'ME+4/1/16'),
 (467,'+',4,1,17,'ME+4/1/17'),
 (468,'+',4,1,18,'ME+4/1/18');
/*!40000 ALTER TABLE `CSC` ENABLE KEYS */;


--
-- Definition of table `CSC_DATA`
--
CREATE TABLE `CSC_DATA` (
  `CSC` smallint(5) unsigned NOT NULL auto_increment,
  `TIMESTAMP` datetime NOT NULL default '0000-00-00 00:00:00',
  `CHAN` smallint(5) unsigned NOT NULL default '0',
  `VMON` smallint(5) unsigned NOT NULL default '0',
  `IMON` float NOT NULL default '0',
  `VSET` smallint(5) unsigned NOT NULL default '0',
  `VCUR` smallint(5) unsigned NOT NULL default '0',
  `RAMP_UP` smallint(5) unsigned NOT NULL default '0',
  `RAMP_DOWN` smallint(5) unsigned NOT NULL default '0',
  `VMAX` smallint(5) unsigned NOT NULL default '0',
  `IMAX` smallint(5) unsigned NOT NULL default '0',
  `VTRIP` smallint(5) unsigned NOT NULL default '0',
  `ITRIP` smallint(5) unsigned NOT NULL default '0',
  `TRIPCNT` int(10) unsigned NOT NULL default '0',
  `TRIPDLY` int(10) unsigned NOT NULL default '0',
  `STATE` smallint(5) unsigned NOT NULL default '0',
  `STATUS` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`CSC`,`CHAN`)
) ENGINE=HEAP DEFAULT CHARSET=latin1 COMMENT='InnoDB free: 3072 kB; (`CSC`) REFER `hvdb/CSC`(`ID`)';

--
-- Dumping data for table `CSC_DATA`
--

/*!40000 ALTER TABLE `CSC_DATA` DISABLE KEYS */;
/*!40000 ALTER TABLE `CSC_DATA` ENABLE KEYS */;


--
-- Definition of table `DNS`
--
CREATE TABLE `DNS` (
  `ID` smallint(5) unsigned NOT NULL auto_increment,
  `DIM_DNS_NODE` varchar(45) NOT NULL default '',
  `DIM_DNS_PORT` smallint(5) unsigned NOT NULL default '3605',
  PRIMARY KEY  (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `DNS`
--

/*!40000 ALTER TABLE `DNS` DISABLE KEYS */;
/*!40000 ALTER TABLE `DNS` ENABLE KEYS */;


--
-- Definition of table `HOST`
--
CREATE TABLE `HOST` (
  `ID` smallint(5) unsigned NOT NULL auto_increment,
  `ADDRESS` varchar(45) NOT NULL default '',
  `PRIMARY_DNS` smallint(5) unsigned NOT NULL default '0',
  `BACKUP_DNS` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `FK_PRIMARY_DNS` (`PRIMARY_DNS`),
  KEY `FK_BACKUP_DNS` (`BACKUP_DNS`),
  CONSTRAINT `FK_BACKUP_DNS` FOREIGN KEY (`BACKUP_DNS`) REFERENCES `DNS` (`ID`),
  CONSTRAINT `FK_PRIMARY_DNS` FOREIGN KEY (`PRIMARY_DNS`) REFERENCES `DNS` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `HOST`
--

/*!40000 ALTER TABLE `HOST` DISABLE KEYS */;
/*!40000 ALTER TABLE `HOST` ENABLE KEYS */;


--
-- Definition of table `HVCARD`
--
CREATE TABLE `HVCARD` (
  `ID` smallint(5) unsigned NOT NULL auto_increment,
  `HOST` smallint(5) unsigned NOT NULL default '0',
  `BUS` smallint(5) unsigned NOT NULL default '0',
  `SLOT` smallint(5) unsigned NOT NULL default '0',
  `DATASLOT` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `HVCARD_UNIQ` USING BTREE (`HOST`,`BUS`,`SLOT`),
  KEY `FK_HOST_HC` (`HOST`),
  CONSTRAINT `FK_HOST_HC` FOREIGN KEY (`HOST`) REFERENCES `HOST` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='InnoDB free: 3072 kB; (`HOST`) REFER `hvdb/HOST`(`ID`)';

--
-- Dumping data for table `HVCARD`
--

/*!40000 ALTER TABLE `HVCARD` DISABLE KEYS */;
/*!40000 ALTER TABLE `HVCARD` ENABLE KEYS */;


--
-- Definition of table `MAP`
--
CREATE TABLE `MAP` (
  `CSC` smallint(5) unsigned NOT NULL auto_increment,
  `RDB` smallint(5) unsigned NOT NULL default '0',
  `RDB_PART` tinyint(3) unsigned NOT NULL default '1',
  `MASTER` smallint(5) unsigned NOT NULL default '0',
  `MASTER_CHAN` tinyint(3) unsigned NOT NULL default '0',
  `PRIME` smallint(5) unsigned NOT NULL default '0',
  `HOST` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`CSC`),
  UNIQUE KEY `MAP_UNIQ` (`RDB`,`RDB_PART`,`MASTER`,`MASTER_CHAN`,`PRIME`,`HOST`),
  KEY `FK_RDB` (`RDB`),
  KEY `FK_MASTER` (`MASTER`),
  KEY `FK_PRIME` (`PRIME`),
  KEY `FK_HOST` (`HOST`),
  CONSTRAINT `FK_HOST` FOREIGN KEY (`HOST`) REFERENCES `HOST` (`ID`),
  CONSTRAINT `FK_MASTER` FOREIGN KEY (`MASTER`) REFERENCES `MASTERDB` (`ID`),
  CONSTRAINT `FK_PRIME` FOREIGN KEY (`PRIME`) REFERENCES `PRIME` (`ID`),
  CONSTRAINT `FK_RDB` FOREIGN KEY (`RDB`) REFERENCES `RDB` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='InnoDB free: 3072 kB; (`RDB`) REFER `hvdb/RDB`(`ID`); (`CSC`';

--
-- Dumping data for table `MAP`
--

/*!40000 ALTER TABLE `MAP` DISABLE KEYS */;
/*!40000 ALTER TABLE `MAP` ENABLE KEYS */;


--
-- Definition of table `MASTERDB`
--
CREATE TABLE `MASTERDB` (
  `ID` smallint(5) unsigned NOT NULL auto_increment,
  `HOSTCARD` smallint(5) unsigned NOT NULL default '0',
  `ADDR` tinyint(3) unsigned NOT NULL default '0',
  `TYPE` tinyint(3) unsigned NOT NULL default '1',
  PRIMARY KEY  (`ID`),
  KEY `FK_TYPE_M` (`TYPE`),
  KEY `FK_HVCARD_M` (`HOSTCARD`),
  CONSTRAINT `FK_HVCARD_M` FOREIGN KEY (`HOSTCARD`) REFERENCES `HVCARD` (`ID`),
  CONSTRAINT `FK_TYPE_M` FOREIGN KEY (`TYPE`) REFERENCES `BOARD_types` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `MASTERDB`
--

/*!40000 ALTER TABLE `MASTERDB` DISABLE KEYS */;
/*!40000 ALTER TABLE `MASTERDB` ENABLE KEYS */;


--
-- Definition of table `PRIME`
--
CREATE TABLE `PRIME` (
  `ID` smallint(5) unsigned NOT NULL auto_increment,
  `PORT` smallint(5) unsigned NOT NULL default '0',
  `ADDR` smallint(5) unsigned NOT NULL default '0',
  `HOST` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='InnoDB free: 3072 kB; (`HOST`) REFER `hvdb/HOST`(`ID`)';

--
-- Dumping data for table `PRIME`
--

/*!40000 ALTER TABLE `PRIME` DISABLE KEYS */;
/*!40000 ALTER TABLE `PRIME` ENABLE KEYS */;


--
-- Definition of table `RDB`
--
CREATE TABLE `RDB` (
  `ID` smallint(5) unsigned NOT NULL auto_increment,
  `HOSTCARD` smallint(5) unsigned NOT NULL default '0',
  `ADDR` tinyint(3) unsigned NOT NULL default '0',
  `TYPE` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `RDB_UNIQ` USING BTREE (`HOSTCARD`,`ADDR`,`TYPE`),
  KEY `FK_HVCARD` (`HOSTCARD`),
  KEY `FK_TYPES` (`TYPE`),
  CONSTRAINT `FK_HVCARD` FOREIGN KEY (`HOSTCARD`) REFERENCES `HVCARD` (`ID`),
  CONSTRAINT `FK_TYPES` FOREIGN KEY (`TYPE`) REFERENCES `BOARD_types` (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='InnoDB free: 3072 kB; (`PRIMARY`) REFER `hvdb/PRIME`(`ID`); ';

--
-- Dumping data for table `RDB`
--

/*!40000 ALTER TABLE `RDB` DISABLE KEYS */;
/*!40000 ALTER TABLE `RDB` ENABLE KEYS */;


--
-- Definition of table `TRIP_INFO`
--
CREATE TABLE `TRIP_INFO` (
  `RDB` smallint(5) unsigned NOT NULL default '0',
  `CHAN` smallint(5) unsigned NOT NULL default '0',
  `TRIPTYPE` smallint(5) unsigned NOT NULL default '0',
  `TIMESTAMP` datetime NOT NULL default '0000-00-00 00:00:00',
  `VTRIP` smallint(5) unsigned NOT NULL default '0',
  `ITRIP` float NOT NULL default '0',
  `VTRIP_ADC` smallint(5) unsigned NOT NULL default '0',
  `ITRIP_ADC` smallint(5) unsigned NOT NULL default '0',
  `TRIP_DELAY` smallint(5) unsigned NOT NULL default '0',
  `VSET_DAC` smallint(5) unsigned NOT NULL default '0',
  `IMAX_ADC` smallint(5) unsigned NOT NULL default '0',
  `MARKER` smallint(5) unsigned NOT NULL default '0',
  `DATA` mediumblob NOT NULL,
  PRIMARY KEY  USING BTREE (`RDB`,`CHAN`,`TIMESTAMP`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `TRIP_INFO`
--

/*!40000 ALTER TABLE `TRIP_INFO` DISABLE KEYS */;
/*!40000 ALTER TABLE `TRIP_INFO` ENABLE KEYS */;




/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;

USE hvdb;

DROP TABLE IF EXISTS `TRIP_INFO`;

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


GRANT SELECT ON hvdb.* TO 'hvuser'@'localhost' IDENTIFIED BY 'UFdqm_04';
GRANT SELECT ON hvdb.* TO 'hvuser'@'%' IDENTIFIED BY 'UFdqm_04';
GRANT SELECT,INSERT,UPDATE,DELETE ON hvdb.* TO 'hvadmin'@'localhost' IDENTIFIED BY 'UFdqm_04';
GRANT SELECT,INSERT,UPDATE,DELETE ON hvdb.* TO 'hvadmin'@'%' IDENTIFIED BY 'UFdqm_04';

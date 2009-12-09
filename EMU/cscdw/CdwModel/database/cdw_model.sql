--------------------------------------------------------
--  File created - Tuesday-December-08-2009   
--------------------------------------------------------
  DROP TABLE "CDW_BASE_UNITS" cascade constraints;
  DROP TABLE "CDW_DOMAINS" cascade constraints;
  DROP TABLE "CDW_PARAMETERS" cascade constraints;
  DROP TABLE "CDW_PARAM_VALUES" cascade constraints;
  DROP TABLE "CDW_SEVERITIES" cascade constraints;
  DROP TABLE "CDW_SOURCES" cascade constraints;
  DROP TABLE "CDW_UNITS" cascade constraints;
  DROP SEQUENCE "CDW_PVA_ID_SEQ";

--------------------------------------------------------
--  DDL for Sequence CDW_PVA_ID_SEQ
--------------------------------------------------------

   CREATE SEQUENCE  "CDW_PVA_ID_SEQ"  MINVALUE 1 MAXVALUE 99999999999999999999 INCREMENT BY 1 START WITH 1 CACHE 20 NOORDER  NOCYCLE ;
--------------------------------------------------------
--  DDL for Table CDW_BASE_UNITS
--------------------------------------------------------

  CREATE TABLE "CDW_BASE_UNITS" 
   (	"BUN_ID" VARCHAR2(30)
   ) ;
--------------------------------------------------------
--  DDL for Table CDW_DOMAINS
--------------------------------------------------------

  CREATE TABLE "CDW_DOMAINS" 
   (	"DOM_ID" VARCHAR2(10), 
	"DOM_NAME" VARCHAR2(128)
   ) ;
--------------------------------------------------------
--  DDL for Table CDW_PARAMETERS
--------------------------------------------------------

  CREATE TABLE "CDW_PARAMETERS" 
   (	"PAR_ID" VARCHAR2(30), 
	"PAR_DOM_ID" VARCHAR2(10), 
	"PAR_DESCR" VARCHAR2(512)
   ) ;
--------------------------------------------------------
--  DDL for Table CDW_PARAM_VALUES
--------------------------------------------------------

  CREATE TABLE "CDW_PARAM_VALUES" 
   (	"PVA_ID" NUMBER(20,0), 
	"PVA_SRC_ID" VARCHAR2(10), 
	"PVA_SOURCE_TIME" DATE, 
	"PVA_PAR_ID" VARCHAR2(30), 
	"PVA_CREATE_TIME" DATE, 
	"PVA_BUN_ID" VARCHAR2(30), 
	"PVA_COMPONENT_ID" VARCHAR2(30), 
	"PVA_RUN_NUMBER" NUMBER(10,0), 
	"PVA_STRING_VALUE" VARCHAR2(4000), 
	"PVA_NUMERIC_VALUE" NUMBER, 
	"PVA_BOOLEAN_VALUE" NUMBER(1,0), 
	"PVA_LOWER_THRESHOLD" NUMBER(20,10), 
	"PVA_UPPER_THRESHOLD" NUMBER(20,10), 
	"PVA_FRACTION" NUMBER(10,9), 
	"PVA_SEV_ID" VARCHAR2(10), 
	"PVA_DESCR" CLOB
   ) ;
--------------------------------------------------------
--  DDL for Table CDW_SEVERITIES
--------------------------------------------------------

  CREATE TABLE "CDW_SEVERITIES" 
   (	"SEV_ID" VARCHAR2(10)
   ) ;
--------------------------------------------------------
--  DDL for Table CDW_SOURCES
--------------------------------------------------------

  CREATE TABLE "CDW_SOURCES" 
   (	"SRC_ID" VARCHAR2(10), 
	"SRC_NAME" VARCHAR2(128)
   ) ;
--------------------------------------------------------
--  DDL for Table CDW_UNITS
--------------------------------------------------------

  CREATE TABLE "CDW_UNITS" 
   (	"UNI_ID" VARCHAR2(30), 
	"UNI_RATE" NUMBER DEFAULT 1, 
	"UNI_BUN_ID" VARCHAR2(30)
   ) ;

---------------------------------------------------
--   DATA FOR TABLE CDW_SEVERITIES
--   FILTER = none used
---------------------------------------------------
REM INSERTING into CDW_SEVERITIES
Insert into CDW_SEVERITIES (SEV_ID) values ('DEBUG');
Insert into CDW_SEVERITIES (SEV_ID) values ('ERROR');
Insert into CDW_SEVERITIES (SEV_ID) values ('FATAL');
Insert into CDW_SEVERITIES (SEV_ID) values ('INFO');
Insert into CDW_SEVERITIES (SEV_ID) values ('WARN');

---------------------------------------------------
--   END DATA FOR TABLE CDW_SEVERITIES
---------------------------------------------------

---------------------------------------------------
--   DATA FOR TABLE CDW_SOURCES
--   FILTER = none used
---------------------------------------------------
REM INSERTING into CDW_SOURCES
Insert into CDW_SOURCES (SRC_ID,SRC_NAME) values ('LOCAL_DQM','CSC Local DQM');

---------------------------------------------------
--   END DATA FOR TABLE CDW_SOURCES
---------------------------------------------------

---------------------------------------------------
--   DATA FOR TABLE CDW_UNITS
--   FILTER = none used
---------------------------------------------------
REM INSERTING into CDW_UNITS
Insert into CDW_UNITS (UNI_ID,UNI_RATE,UNI_BUN_ID) values ('METER',1,'METER');
Insert into CDW_UNITS (UNI_ID,UNI_RATE,UNI_BUN_ID) values ('CENTIMETER',0.01,'METER');

---------------------------------------------------
--   END DATA FOR TABLE CDW_UNITS
---------------------------------------------------

---------------------------------------------------
--   DATA FOR TABLE CDW_BASE_UNITS
--   FILTER = none used
---------------------------------------------------
REM INSERTING into CDW_BASE_UNITS
Insert into CDW_BASE_UNITS (BUN_ID) values ('METER');

---------------------------------------------------
--   END DATA FOR TABLE CDW_BASE_UNITS
---------------------------------------------------

---------------------------------------------------
--   DATA FOR TABLE CDW_DOMAINS
--   FILTER = none used
---------------------------------------------------
REM INSERTING into CDW_DOMAINS
Insert into CDW_DOMAINS (DOM_ID,DOM_NAME) values ('DATA','CSC Data Domain');

---------------------------------------------------
--   END DATA FOR TABLE CDW_DOMAINS
---------------------------------------------------

---------------------------------------------------
--   DATA FOR TABLE CDW_PARAMETERS
--   FILTER = none used
---------------------------------------------------
REM INSERTING into CDW_PARAMETERS
Insert into CDW_PARAMETERS (PAR_ID,PAR_DOM_ID,PAR_DESCR) values ('CUMULATIVE_ALCT_COUNTER','DATA','CUMULATIVE_ALCT_COUNTER');
Insert into CDW_PARAMETERS (PAR_ID,PAR_DOM_ID,PAR_DESCR) values ('CUMULATIVE_CLCT_COUNTER','DATA','CUMULATIVE_CLCT_COUNTER');
Insert into CDW_PARAMETERS (PAR_ID,PAR_DOM_ID,PAR_DESCR) values ('CUMULATIVE_LCT_COUNTER','DATA','CUMULATIVE_LCT_COUNTER');
Insert into CDW_PARAMETERS (PAR_ID,PAR_DOM_ID,PAR_DESCR) values ('CUMULATIVE_L1A_COUNTER','DATA','CUMULATIVE_L1A_COUNTER');
Insert into CDW_PARAMETERS (PAR_ID,PAR_DOM_ID,PAR_DESCR) values ('DATA_EVENTS','DATA','Number of events collected by a component');

---------------------------------------------------
--   END DATA FOR TABLE CDW_PARAMETERS
---------------------------------------------------
--------------------------------------------------------
--  Constraints for Table CDW_BASE_UNITS
--------------------------------------------------------

  ALTER TABLE "CDW_BASE_UNITS" ADD CONSTRAINT "CDW_BASE_UNITS_PK" PRIMARY KEY ("BUN_ID") ENABLE;
 
  ALTER TABLE "CDW_BASE_UNITS" MODIFY ("BUN_ID" NOT NULL ENABLE);
--------------------------------------------------------
--  Constraints for Table CDW_DOMAINS
--------------------------------------------------------

  ALTER TABLE "CDW_DOMAINS" ADD CONSTRAINT "CDW_DOM_PK" PRIMARY KEY ("DOM_ID") ENABLE;
 
  ALTER TABLE "CDW_DOMAINS" MODIFY ("DOM_ID" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_DOMAINS" MODIFY ("DOM_NAME" NOT NULL ENABLE);
--------------------------------------------------------
--  Constraints for Table CDW_PARAMETERS
--------------------------------------------------------

  ALTER TABLE "CDW_PARAMETERS" ADD CONSTRAINT "CDW_FTY_PK" PRIMARY KEY ("PAR_ID") ENABLE;
 
  ALTER TABLE "CDW_PARAMETERS" MODIFY ("PAR_ID" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_PARAMETERS" MODIFY ("PAR_DOM_ID" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_PARAMETERS" MODIFY ("PAR_DESCR" NOT NULL ENABLE);
--------------------------------------------------------
--  Constraints for Table CDW_PARAM_VALUES
--------------------------------------------------------

  ALTER TABLE "CDW_PARAM_VALUES" ADD CONSTRAINT "CDW_PVA_PK" PRIMARY KEY ("PVA_ID") ENABLE;
 
  ALTER TABLE "CDW_PARAM_VALUES" MODIFY ("PVA_ID" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_PARAM_VALUES" MODIFY ("PVA_SRC_ID" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_PARAM_VALUES" MODIFY ("PVA_SOURCE_TIME" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_PARAM_VALUES" MODIFY ("PVA_PAR_ID" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_PARAM_VALUES" MODIFY ("PVA_CREATE_TIME" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_PARAM_VALUES" MODIFY ("PVA_COMPONENT_ID" NOT NULL ENABLE);
--------------------------------------------------------
--  Constraints for Table CDW_SEVERITIES
--------------------------------------------------------

  ALTER TABLE "CDW_SEVERITIES" ADD CONSTRAINT "CDW_SEVERITIES_PK" PRIMARY KEY ("SEV_ID") ENABLE;
 
  ALTER TABLE "CDW_SEVERITIES" MODIFY ("SEV_ID" NOT NULL ENABLE);
--------------------------------------------------------
--  Constraints for Table CDW_SOURCES
--------------------------------------------------------

  ALTER TABLE "CDW_SOURCES" ADD CONSTRAINT "CDW_SRC_PK" PRIMARY KEY ("SRC_ID") ENABLE;
 
  ALTER TABLE "CDW_SOURCES" MODIFY ("SRC_ID" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_SOURCES" MODIFY ("SRC_NAME" NOT NULL ENABLE);
--------------------------------------------------------
--  Constraints for Table CDW_UNITS
--------------------------------------------------------

  ALTER TABLE "CDW_UNITS" ADD CONSTRAINT "CDW_UNITS_PK" PRIMARY KEY ("UNI_ID") ENABLE;
 
  ALTER TABLE "CDW_UNITS" MODIFY ("UNI_ID" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_UNITS" MODIFY ("UNI_RATE" NOT NULL ENABLE);
 
  ALTER TABLE "CDW_UNITS" MODIFY ("UNI_BUN_ID" NOT NULL ENABLE);
--------------------------------------------------------
--  DDL for Index CDW_BASE_UNITS_PK
--------------------------------------------------------

  CREATE UNIQUE INDEX "CDW_BASE_UNITS_PK" ON "CDW_BASE_UNITS" ("BUN_ID") 
  ;
--------------------------------------------------------
--  DDL for Index CDW_DOM_PK
--------------------------------------------------------

  CREATE UNIQUE INDEX "CDW_DOM_PK" ON "CDW_DOMAINS" ("DOM_ID") 
  ;
--------------------------------------------------------
--  DDL for Index CDW_FTY_PK
--------------------------------------------------------

  CREATE UNIQUE INDEX "CDW_FTY_PK" ON "CDW_PARAMETERS" ("PAR_ID") 
  ;
--------------------------------------------------------
--  DDL for Index CDW_PVA_PK
--------------------------------------------------------

  CREATE UNIQUE INDEX "CDW_PVA_PK" ON "CDW_PARAM_VALUES" ("PVA_ID") 
  ;
--------------------------------------------------------
--  DDL for Index CDW_SEVERITIES_PK
--------------------------------------------------------

  CREATE UNIQUE INDEX "CDW_SEVERITIES_PK" ON "CDW_SEVERITIES" ("SEV_ID") 
  ;
--------------------------------------------------------
--  DDL for Index CDW_SRC_PK
--------------------------------------------------------

  CREATE UNIQUE INDEX "CDW_SRC_PK" ON "CDW_SOURCES" ("SRC_ID") 
  ;
--------------------------------------------------------
--  DDL for Index CDW_UNITS_PK
--------------------------------------------------------

  CREATE UNIQUE INDEX "CDW_UNITS_PK" ON "CDW_UNITS" ("UNI_ID") 
  ;


--------------------------------------------------------
--  Ref Constraints for Table CDW_PARAMETERS
--------------------------------------------------------

  ALTER TABLE "CDW_PARAMETERS" ADD CONSTRAINT "CDW_DOM_FTY_FK" FOREIGN KEY ("PAR_DOM_ID")
	  REFERENCES "CDW_DOMAINS" ("DOM_ID") ENABLE;
--------------------------------------------------------
--  Ref Constraints for Table CDW_PARAM_VALUES
--------------------------------------------------------

  ALTER TABLE "CDW_PARAM_VALUES" ADD CONSTRAINT "CDW_FTY_FCT_FK" FOREIGN KEY ("PVA_PAR_ID")
	  REFERENCES "CDW_PARAMETERS" ("PAR_ID") ENABLE;
 
  ALTER TABLE "CDW_PARAM_VALUES" ADD CONSTRAINT "CDW_SEV_FCT_FK" FOREIGN KEY ("PVA_SEV_ID")
	  REFERENCES "CDW_SEVERITIES" ("SEV_ID") ON DELETE SET NULL ENABLE;
 
  ALTER TABLE "CDW_PARAM_VALUES" ADD CONSTRAINT "CDW_SRC_FCT_FK" FOREIGN KEY ("PVA_SRC_ID")
	  REFERENCES "CDW_SOURCES" ("SRC_ID") ENABLE;
 
  ALTER TABLE "CDW_PARAM_VALUES" ADD CONSTRAINT "RELATION_12" FOREIGN KEY ("PVA_BUN_ID")
	  REFERENCES "CDW_BASE_UNITS" ("BUN_ID") ON DELETE SET NULL ENABLE;


--------------------------------------------------------
--  Ref Constraints for Table CDW_UNITS
--------------------------------------------------------

  ALTER TABLE "CDW_UNITS" ADD CONSTRAINT "RELATION_10" FOREIGN KEY ("UNI_BUN_ID")
	  REFERENCES "CDW_BASE_UNITS" ("BUN_ID") ENABLE;


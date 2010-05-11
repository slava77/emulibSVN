
drop package CDW_UTILITY
/

drop table CDW_DDL
/

CREATE TABLE CDW_DDL (
    OBJECT_TYPE VARCHAR2(30 BYTE) NOT NULL ENABLE,
    OBJECT_NAME VARCHAR2(30 BYTE) NOT NULL ENABLE,
    DDL CLOB NOT NULL ENABLE,
    GRANTS CLOB,
    CONSTRAINT CDW_DDL_PK PRIMARY KEY (OBJECT_TYPE, OBJECT_NAME)
)
/

CREATE OR REPLACE PACKAGE CDW_UTILITY AS

    procedure disable_foreign_key(p_table_name varchar2, p_column_name varchar2);
    procedure enable_foreign_key(p_table_name varchar2, p_column_name varchar2);
    procedure disable_table_constraints(p_table varchar);
    procedure enable_table_constraints(p_table varchar);
    
    function get_object_grants(p_object_name varchar2) return clob;
    procedure generate_ddl;

END CDW_UTILITY;
/

create or replace PACKAGE BODY CDW_UTILITY AS

    procedure modify_foreign_key(p_table_name varchar2, p_column_name varchar2, p_action varchar2) AS
        l_constraint_name varchar2(30);
    begin

        select
            con.constraint_name
        into
            l_constraint_name
        from
            all_constraints con
                join all_cons_columns col
                    on (con.constraint_name = col.constraint_name)
        where
            con.constraint_type = 'R' and
            con.owner = user and
            con.TABLE_NAME = p_table_name and
            col.column_name = p_column_name;

        execute immediate 'ALTER TABLE ' || user || '.' || p_table_name || ' MODIFY CONSTRAINT ' || l_constraint_name || ' ' || p_action;

    exception when others then
        null;
    end;

    procedure disable_foreign_key(p_table_name varchar2, p_column_name varchar2) AS
    begin
        modify_foreign_key(p_table_name, p_column_name, 'DISABLE');
    end;

    procedure enable_foreign_key(p_table_name varchar2, p_column_name varchar2) AS
    begin
        modify_foreign_key(p_table_name, p_column_name, 'ENABLE');
    end;

    function get_object_grants(p_object_name varchar2) return clob as
    begin
        return DBMS_METADATA.GET_DEPENDENT_DDL('OBJECT_GRANT', p_object_name, user);
    exception when others then
        return null;
    end;

    procedure generate_ddl as
    begin

        delete from CDW_DDL;

        DBMS_METADATA.SET_TRANSFORM_PARAM(DBMS_METADATA.SESSION_TRANSFORM, 'STORAGE', false);
        DBMS_METADATA.SET_TRANSFORM_PARAM(DBMS_METADATA.SESSION_TRANSFORM, 'TABLESPACE', false);
        DBMS_METADATA.SET_TRANSFORM_PARAM(DBMS_METADATA.SESSION_TRANSFORM, 'SEGMENT_ATTRIBUTES', false);

        INSERT INTO
            CDW_DDL (OBJECT_TYPE, OBJECT_NAME, DDL, GRANTS)
        SELECT
            object_type,
            object_name,
            DBMS_METADATA.GET_DDL(object_type, object_name, user),
            DBMS_METADATA.GET_DEPENDENT_DDL('OBJECT_GRANT', object_name, user)
        from
            SYS.user_objects
        where
            object_type in ('SEQUENCE','PACKAGE','TRIGGER','TABLE','VIEW','FUNCTION','SYNONYM') and
            object_name not like '%$%' and
            substr(object_name,1,4) != 'BIN$' and
            substr(object_name,1,3) != 'DR$'
        order by
            object_type, object_name;

        DBMS_METADATA.SET_TRANSFORM_PARAM(DBMS_METADATA.SESSION_TRANSFORM,'DEFAULT');

    end;

    procedure disable_table_constraints(p_table varchar) as
    begin
        for cur in (select owner, constraint_name, table_name, decode(constraint_type, 'R', 0, 'U', 1, 'C', 5, 'P', 100, 10) ord from all_constraints where OWNER = user and TABLE_NAME = p_table order by ord asc) loop
            execute immediate 'ALTER TABLE ' || cur.owner || '.' || cur.table_name || ' MODIFY CONSTRAINT "' || cur.constraint_name || '" DISABLE';
        end loop;
    end;

    procedure enable_table_constraints(p_table varchar) as
    begin
        for cur in (select owner, constraint_name, table_name, decode(constraint_type, 'R', 0, 'U', 1, 'C', 5, 'P', 100, 10) ord from all_constraints where OWNER = user and TABLE_NAME = p_table order by ord desc) loop
            execute immediate 'ALTER TABLE ' || cur.owner || '.' || cur.table_name || ' MODIFY CONSTRAINT "' || cur.constraint_name || '" ENABLE';
        end loop;
    end;

END CDW_UTILITY;
/

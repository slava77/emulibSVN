
create or replace PACKAGE CDW_UTILITY AS

    procedure disable_foreign_key(p_table_name varchar2, p_column_name varchar2);
    procedure enable_foreign_key(p_table_name varchar2, p_column_name varchar2);
    procedure disable_table_constraints(p_table varchar);
    procedure enable_table_constraints(p_table varchar);

    function get_object_grants(p_object_name varchar2) return clob;
    procedure generate_ddl;

    function partition_table_ddl(p_table_name varchar2, p_date_column varchar2, p_months_forward number) return varchar2;
    function max_table_partition(p_table varchar2) return date;
    procedure repartition_tables;

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
            cdw_utility.get_object_grants('OBJECT_GRANT')
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

    procedure modify_table_contraints(p_table varchar, p_action varchar) as
        l_ord_remote_fk number;
        l_ord_local_fk  number;
        l_ord_local_pk  number;
        l_ord_local_ot  number;
    begin

        if p_action = 'DISABLE' then
            l_ord_remote_fk := 1;
            l_ord_local_fk  := 2;
            l_ord_local_ot  := 3;
            l_ord_local_pk  := 4;
        end if;

        if p_action = 'ENABLE' then
            l_ord_local_pk  := 1;
            l_ord_local_ot  := 2;
            l_ord_local_fk  := 3;
            l_ord_remote_fk := 4;
        end if;

        for cur in (
            select owner, constraint_name, table_name, ord from (
                select owner, constraint_name, table_name, l_ord_remote_fk ord from ALL_CONSTRAINTS
                    where r_constraint_name in (select constraint_name from ALL_CONS_COLUMNS where OWNER = user and TABLE_NAME = p_table)
                union
                select owner, constraint_name, table_name, decode(constraint_type, 'R', l_ord_local_fk, 'P', l_ord_local_pk, l_ord_local_ot) ord from ALL_CONSTRAINTS
                    where OWNER = user and TABLE_NAME = p_table)
            order by ord asc) loop

            execute immediate 'ALTER TABLE ' || cur.owner || '.' || cur.table_name || ' MODIFY CONSTRAINT "' || cur.constraint_name || '" ' || p_action;

        end loop;

    end;

    procedure disable_table_constraints(p_table varchar) as
    begin
        modify_table_contraints(p_table, 'DISABLE');
    end;

    procedure enable_table_constraints(p_table varchar) as
    begin
        modify_table_contraints(p_table, 'ENABLE');
    end;

    function partition_table_ddl(p_table_name varchar2, p_date_column varchar2, p_months_forward number) return varchar2 as
      par_sql   varchar2(32000) := null;
      par_now   date := TO_DATE(TO_CHAR(sysdate, 'YYYY-MM') || '-01', 'YYYY-MM-DD');
      par_date  date := par_now;
      par_first number := 1;
    begin

      DBMS_METADATA.SET_TRANSFORM_PARAM(DBMS_METADATA.SESSION_TRANSFORM, 'STORAGE', false);
      DBMS_METADATA.SET_TRANSFORM_PARAM(DBMS_METADATA.SESSION_TRANSFORM, 'TABLESPACE', false);
      DBMS_METADATA.SET_TRANSFORM_PARAM(DBMS_METADATA.SESSION_TRANSFORM, 'SEGMENT_ATTRIBUTES', false);
      par_sql := DBMS_METADATA.GET_DDL('TABLE', p_table_name, user);
      DBMS_METADATA.SET_TRANSFORM_PARAM(DBMS_METADATA.SESSION_TRANSFORM,'DEFAULT');

      par_sql := par_sql || ' partition by range(' || p_date_column || ') (';
      par_date := par_now;

      while par_date < ADD_MONTHS(par_now, 6) loop
        if par_first = 0 then
          par_sql := par_sql || ',';
        end if;
        par_sql := par_sql || '   ';
        par_sql := par_sql || 'partition P' || TO_CHAR(par_date, 'YYYYMM') || ' values less than (to_date(''' || TO_CHAR(ADD_MONTHS(par_date, 1), 'YYYY-MM-DD') || ''', ''YYYY-MM-DD''))';
        par_date := ADD_MONTHS(par_date, 1);
        par_first := 0;
      end loop;

      par_sql := par_sql || ')';

      par_sql := 'drop table ' || p_table_name || ' cascade constraints purge' || CHR(10) || CHR(13) || '/' || par_sql || CHR(10) || CHR(13) || '/';

      return par_sql;

    end;

    function max_table_partition(p_table varchar2) return date as
      l_d date := null;
      l_md date := null;
      l_s varchar2(32000);
    begin
      for r in (select table_name, high_value from USER_TAB_PARTITIONS where table_name = p_table) loop
        l_s := 'select ' || substr(r.high_value, 1, length(r.high_value)) || ' from dual';
        execute immediate l_s into l_d;
        if l_md is null or l_d > l_md then
          l_md := l_d;
        end if;
      end loop;
      return l_md;
    end;

    procedure repartition_tables as
      l_s varchar2(32000);
      l_d date := null;
    begin
      for r in (select table_name from USER_TAB_PARTITIONS where table_name like 'CDW_%' group by table_name) loop
        l_d := cdw_utility.max_table_partition(r.table_name);
        if (l_d - sysdate) < 30 then
          for i in 1..1 loop
            l_s := 'ALTER TABLE ' || r.table_name || ' ADD PARTITION P' || TO_CHAR(l_d, 'YYYYMM');
            l_d := ADD_MONTHS(l_d, 1);
            l_s := l_s || ' values less than (to_date(''' || TO_CHAR(l_d, 'YYYY-MM-DD') || ''', ''YYYY-MM-DD''))';
            execute immediate l_s;
          end loop;
        end if;
      end loop;
    end;

END CDW_UTILITY;
/

ALTER PACKAGE CDW_UTILITY COMPILE BODY
/

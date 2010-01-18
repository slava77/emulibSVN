
drop package CDW_UTILITY
/

CREATE OR REPLACE PACKAGE CDW_UTILITY AS

  procedure disable_foreign_key(p_table_name varchar2, p_column_name varchar2);
  procedure enable_foreign_key(p_table_name varchar2, p_column_name varchar2);

END CDW_UTILITY;
/

create or replace
PACKAGE BODY CDW_UTILITY AS

  procedure modify_foreign_key(p_table_name varchar2, p_column_name varchar2, p_action varchar2) AS
    l_constraint_name varchar2(30);
  begin

    select
      con.constraint_name
    into
      l_constraint_name
    from
      all_constraints con
      join all_cons_columns col on (con.constraint_name = col.constraint_name)
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

END CDW_UTILITY;
/

begin
    cdw_utility.disable_table_constraints('CDW_COMPONENT_CLASS_PARENTS');
    cdw_utility.disable_table_constraints('CDW_COMPONENT_SYNONYMS');
    cdw_utility.disable_table_constraints('CDW_COMPONENT_LINK_CLASSES');
    cdw_utility.disable_table_constraints('CDW_COMPONENTS');
    cdw_utility.disable_table_constraints('CDW_COMPONENT_CLASSES');
end;
/

-- Dropping components and classes table backup (if exists)
begin

    begin
        execute immediate 'drop table CDW_COMPONENTS$OLD';
    exception when others then
        null;
    end;

    begin
        execute immediate 'drop table CDW_COMPONENTS$IDS';
    exception when others then
        null;
    end;

    begin
        execute immediate 'drop table CDW_COMPONENT_CLASSES$OLD';
    exception when others then
        null;
    end;

    begin
        execute immediate 'drop table CDW_COMPONENT_CLASSES$IDS';
    exception when others then
        null;
    end;

end;
/

-- Creating and populating current components and classes table backup
create table
  CDW_COMPONENTS$OLD
as
  select
    *
  from
    CDW_COMPONENTS
/

create table
  CDW_COMPONENT_CLASSES$OLD
as
  select
    *
  from
    CDW_COMPONENT_CLASSES
/

-- Add index
CREATE UNIQUE INDEX CDW_COMPONENT_OLD_IDX ON CDW_COMPONENTS$OLD (CMP_NAME)
/

CREATE UNIQUE INDEX CDW_COMPONENT_CLASS_OLD_IDX ON CDW_COMPONENT_CLASSES$OLD (CCL_NAME)
/

delete from CDW_COMPONENT_CLASS_PARENTS
/

delete from CDW_COMPONENT_SYNONYMS
/

delete from CDW_COMPONENT_LINK_CLASSES
/

delete from CDW_COMPONENTS
/

delete from CDW_COMPONENT_CLASSES
/


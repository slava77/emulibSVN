begin
    cdw_utility.disable_table_constraints('CDW_COMPONENT_CLASS_PARENTS');
    cdw_utility.disable_table_constraints('CDW_COMPONENT_SYNONYMS');
    cdw_utility.disable_table_constraints('CDW_COMPONENT_LINK_CLASSES');
    cdw_utility.disable_table_constraints('CDW_COMPONENTS');
    cdw_utility.disable_table_constraints('CDW_COMPONENT_CLASSES');
end;
/

-- Dropping components table backup (if exists)
begin
    execute immediate 'drop table CDW_COMPONENTS$IMP';
exception when others then
    null;
end;
/

-- Creating and populating current components table backup
create table
  CDW_COMPONENTS$IMP
as
  select
    CMP_ID,
    CMP_NAME
  from
    CDW_COMPONENTS
/

-- Add index
CREATE UNIQUE INDEX CDW_COMPONENT_IMP_IDX ON CDW_COMPONENTS$IMP (CMP_NAME)
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


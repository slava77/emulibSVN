begin
    cdw_utility.disable_table_constraints('CDW_COMPONENT_CLASS_PARENTS');
    cdw_utility.disable_table_constraints('CDW_COMPONENT_SYNONYMS');
    cdw_utility.disable_table_constraints('CDW_COMPONENT_LINK_CLASSES');
    cdw_utility.disable_table_constraints('CDW_COMPONENTS');
    cdw_utility.disable_table_constraints('CDW_COMPONENT_CLASSES');
end;
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

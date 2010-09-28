-- Set all identifiers to NULL
update CDW_COMPONENTS set CMP_ID = null
/

-- Update identifiers to previous id's (where it is possible)
update
  CDW_COMPONENTS c
set
  c.CMP_ID = (
    select
      ci.CMP_ID
    from
      CDW_COMPONENTS$IMP ci
    where
      ci.CMP_NAME = c.CMP_NAME
  )
/

declare
  l_ccl_id number := 0;
  l_cmp_id number := 0;
begin

  -- Insert all components that where removed from current ontology
  for r in (select CMP_ID, CMP_NAME from CDW_COMPONENTS$IMP
              where (CMP_NAME) not in
                (select CMP_NAME from CDW_COMPONENTS)) loop

    -- Create a special component class (REMOVED) and attach all removed components to it
    if l_ccl_id = 0 then
      select max(CCL_ID) + 1 into l_ccl_id from CDW_COMPONENT_CLASSES;
      insert into CDW_COMPONENT_CLASSES (CCL_ID, CCL_NAME, CCL_DESCR) values (l_ccl_id, 'REMOVED', 'Automatically added while importing ontology');
    end if;

    insert into CDW_COMPONENTS (CMP_ID, CMP_NAME, CMP_CCL_ID) values (r.CMP_ID, r.CMP_NAME, l_ccl_id);

  end loop;

  -- Get next component id
  select max(CMP_ID) + 1 into l_cmp_id from CDW_COMPONENTS where CMP_ID is not null;

  -- Update newly created components with appropriate unique id's
  for r in (select CMP_NAME from CDW_COMPONENTS where CMP_ID is null) loop
    update CDW_COMPONENTS set CMP_ID = l_cmp_id where CMP_NAME = r.CMP_NAME;
    l_cmp_id := l_cmp_id + 1;
  end loop;

end;
/

begin

    cdw_utility.enable_table_constraints('CDW_COMPONENT_CLASSES');
    cdw_utility.enable_table_constraints('CDW_COMPONENTS');
    cdw_utility.enable_table_constraints('CDW_COMPONENT_LINK_CLASSES');  
    cdw_utility.enable_table_constraints('CDW_COMPONENT_SYNONYMS');
    cdw_utility.enable_table_constraints('CDW_COMPONENT_CLASS_PARENTS');

end;
/

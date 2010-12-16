-- Creating and populating new components table backup
create table
  CDW_COMPONENTS$IDS
as
  select
    CMP_ID as CMP_ID_NEW,
    CMP_NAME,
    CMP_ID as CMP_ID_FIN
  from
    CDW_COMPONENTS
/

-- Add index
CREATE UNIQUE INDEX CDW_COMPONENT_IDS_IDX ON CDW_COMPONENTS$IDS (CMP_ID_NEW, CMP_ID_FIN)
/

-- Add index
CREATE UNIQUE INDEX CDW_COMPONENT_IDS_NAME_IDX ON CDW_COMPONENTS$IDS (CMP_NAME)
/

-- Set all identifiers to NULL
update CDW_COMPONENTS$IDS set CMP_ID_FIN = null
/

-- Update identifiers to previous id's (where it is possible)
update
  CDW_COMPONENTS$IDS ci
set
  ci.CMP_ID_FIN = (
    select
      co.CMP_ID
    from
      CDW_COMPONENTS$OLD co
    where
      ci.CMP_NAME = co.CMP_NAME
  )
/

declare
  l_ccl_id number := 0;
  l_cmp_id number := 0;
begin

  -- Insert all components that where removed from current ontology
  for r in (select CMP_ID, CMP_NAME from CDW_COMPONENTS$OLD
              where (CMP_NAME) not in
                (select CMP_NAME from CDW_COMPONENTS)) loop

    -- Create a special component class (REMOVED) and attach all removed components to it
    if l_ccl_id = 0 then
      select nvl(max(CCL_ID), 0) + 1 into l_ccl_id from CDW_COMPONENT_CLASSES;
      insert into CDW_COMPONENT_CLASSES (CCL_ID, CCL_NAME, CCL_DESCR) values (l_ccl_id, 'REMOVED', 'Automatically added while importing ontology');
    end if;

    insert into CDW_COMPONENTS (CMP_ID, CMP_NAME, CMP_CCL_ID) values (r.CMP_ID, r.CMP_NAME, l_ccl_id);
    insert into CDW_COMPONENTS$IDS (CMP_ID_NEW, CMP_NAME, CMP_ID_FIN) values (r.CMP_ID, r.CMP_NAME, r.CMP_ID);

  end loop;

  -- Get next component id
  select nvl(max(CMP_ID_FIN), 0) + 1 into l_cmp_id from CDW_COMPONENTS$IDS where CMP_ID_FIN is not null;

  -- Update newly created components with appropriate unique id's
  for r in (select CMP_NAME from CDW_COMPONENTS$IDS where CMP_ID_FIN is null) loop
    update CDW_COMPONENTS$IDS set CMP_ID_FIN = l_cmp_id where CMP_NAME = r.CMP_NAME;
    l_cmp_id := l_cmp_id + 1;
  end loop;

end;
/

begin

    -- Updating synonyms foreign key
    update 
        CDW_COMPONENT_SYNONYMS cs 
    set 
        cs.SYN_CMP_ID = (select ci.CMP_ID_FIN
                         from CDW_COMPONENTS$IDS ci
                         where ci.CMP_ID_NEW = cs.SYN_CMP_ID);

    -- Updating components
    update
        CDW_COMPONENTS c
    set
        c.CMP_ID = (select ci.CMP_ID_FIN
                    from CDW_COMPONENTS$IDS ci
                    where ci.CMP_ID_NEW = c.CMP_ID);
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

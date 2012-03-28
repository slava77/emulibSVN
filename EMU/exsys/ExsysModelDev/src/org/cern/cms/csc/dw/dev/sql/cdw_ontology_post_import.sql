------------------------------------------------------------------------------------
--
-- Handling Component Classes
--
------------------------------------------------------------------------------------

-- Creating and populating new ccl table backup
create table
  CDW_COMPONENT_CLASSES$IDS
as
  select
    CCL_ID as CCL_ID_NEW,
    CCL_NAME,
    CCL_ID as CCL_ID_FIN
  from
    CDW_COMPONENT_CLASSES
/

-- Add index
CREATE UNIQUE INDEX CDW_CCL_IDS_IDX ON CDW_COMPONENT_CLASSES$IDS (CCL_ID_NEW, CCL_ID_FIN)
/

-- Add index
CREATE UNIQUE INDEX CDW_CCL_IDS_NAME_IDX ON CDW_COMPONENT_CLASSES$IDS (CCL_NAME)
/

-- Set all identifiers to NULL
update CDW_COMPONENT_CLASSES$IDS set CCL_ID_FIN = null
/

-- Update identifiers to previous id's (where it is possible)
update
  CDW_COMPONENT_CLASSES$IDS ci
set
  ci.CCL_ID_FIN = (
    select
      co.CCL_ID
    from
      CDW_COMPONENT_CLASSES$OLD co
    where
      ci.CCL_NAME = co.CCL_NAME
  )
/

declare
  l_ccl_id number := 0;
  l_cmp_id number := 0;
begin

  -- Insert all classes that where removed from current ontology
  for r in (select CCL_ID, CCL_NAME from CDW_COMPONENT_CLASSES$OLD
              where (CCL_NAME) not in
                (select CCL_NAME from CDW_COMPONENT_CLASSES)) loop

    insert into CDW_COMPONENT_CLASSES (CCL_ID, CCL_NAME, CCL_DESCR) values (r.CCL_ID * -1, r.CCL_NAME, 'Removed from current update!');
    insert into CDW_COMPONENT_CLASSES$IDS (CCL_ID_NEW, CCL_NAME, CCL_ID_FIN) values (r.CCL_ID * -1, r.CCL_NAME, r.CCL_ID);

  end loop;

  -- Get next component id
  select nvl(max(CCL_ID_FIN), 0) + 1 into l_cmp_id from CDW_COMPONENT_CLASSES$IDS where CCL_ID_FIN is not null;

  -- Update newly created components with appropriate unique id's
  for r in (select CCL_NAME from CDW_COMPONENT_CLASSES$IDS where CCL_ID_FIN is null) loop
    update CDW_COMPONENT_CLASSES$IDS set CCL_ID_FIN = l_cmp_id where CCL_NAME = r.CCL_NAME;
    l_cmp_id := l_cmp_id + 1;
  end loop;

end;
/

-- Updating component class foreign keys

update
    CDW_COMPONENT_CLASS_PARENTS cp
set
    cp.CCP_PARENT_CCL_ID = (select ci.CCL_ID_FIN
                            from CDW_COMPONENT_CLASSES$IDS ci
                            where ci.CCL_ID_NEW = cp.CCP_PARENT_CCL_ID)
/

update
    CDW_COMPONENT_CLASS_PARENTS cp
set
    cp.CCP_CHILD_CCL_ID = (select ci.CCL_ID_FIN
                           from CDW_COMPONENT_CLASSES$IDS ci
                           where ci.CCL_ID_NEW = cp.CCP_CHILD_CCL_ID)
/

update
    CDW_COMPONENTS c
set
    c.CMP_CCL_ID = (select ci.CCL_ID_FIN
                     from CDW_COMPONENT_CLASSES$IDS ci
                     where ci.CCL_ID_NEW = c.CMP_CCL_ID)
/

-- Updating classes
update
    CDW_COMPONENT_CLASSES c
set
    c.CCL_ID = (select ci.CCL_ID_FIN
                from CDW_COMPONENT_CLASSES$IDS ci
                where ci.CCL_ID_NEW = c.CCL_ID)
/

------------------------------------------------------------------------------------
--
-- Handling Component Link Classes
--
------------------------------------------------------------------------------------

-- Creating and populating new lcl table backup
create table
  CDW_COMPONENT_LINK_CLASSES$IDS
as
  select
    LCL_ID as LCL_ID_NEW,
    LCL_NAME,
    LCL_ID as LCL_ID_FIN
  from
    CDW_COMPONENT_LINK_CLASSES
/

-- Add index
CREATE UNIQUE INDEX CDW_LCL_IDS_IDX ON CDW_COMPONENT_LINK_CLASSES$IDS (LCL_ID_NEW, LCL_ID_FIN)
/

-- Add index
CREATE UNIQUE INDEX CDW_LCL_IDS_NAME_IDX ON CDW_COMPONENT_LINK_CLASSES$IDS (LCL_NAME)
/

-- Set all identifiers to NULL
update CDW_COMPONENT_LINK_CLASSES$IDS set LCL_ID_FIN = null
/

-- Update identifiers to previous id's (where it is possible)
update
  CDW_COMPONENT_LINK_CLASSES$IDS lci
set
  lci.LCL_ID_FIN = (
    select
      lco.LCL_ID
    from
      CDW_COMPONENT_LINK_CLASSES$OLD lco
    where
      lci.LCL_NAME = lco.LCL_NAME
  )
/

declare
  l_lcl_id number := 0;
begin

  -- Insert all link classes that where removed from current ontology
  for r in (select LCL_ID, LCL_NAME from CDW_COMPONENT_LINK_CLASSES$OLD
              where (LCL_NAME) not in
                (select LCL_NAME from CDW_COMPONENT_LINK_CLASSES)) loop

    insert into CDW_COMPONENT_LINK_CLASSES (LCL_ID, LCL_NAME, LCL_DESCR, LCL_TRANSITIVE) values (r.LCL_ID * -1, r.LCL_NAME, 'Removed from current update!', 0);
    insert into CDW_COMPONENT_LINK_CLASSES$IDS (LCL_ID_NEW, LCL_NAME, LCL_ID_FIN) values (r.LCL_ID * -1, r.LCL_NAME, r.LCL_ID);

  end loop;

  -- Get next id
  select nvl(max(LCL_ID_FIN), 0) + 1 into l_lcl_id from CDW_COMPONENT_LINK_CLASSES$IDS where LCL_ID_FIN is not null;

  -- Update newly created link classes with appropriate unique id's
  for r in (select LCL_NAME from CDW_COMPONENT_LINK_CLASSES$IDS where LCL_ID_FIN is null) loop
    update CDW_COMPONENT_LINK_CLASSES$IDS set LCL_ID_FIN = l_lcl_id where LCL_NAME = r.LCL_NAME;
    l_lcl_id := l_lcl_id + 1;
  end loop;

end;
/

-- Updating component class foreign keys

update
    RE_REL_CMP_FINDER_LINK_CLASSES finder
set
    finder.RECFL_COMPONENT_LINK_CLASS_ID = (select lci.LCL_ID_FIN
                            from CDW_COMPONENT_LINK_CLASSES$IDS lci
                            where lci.LCL_ID_NEW = finder.RECFL_COMPONENT_LINK_CLASS_ID)
/

update
    CDW_COMPONENT_LINK_CLASSES lc
set
    lc.LCL_INVERSE_LCL_ID = (select lci.LCL_ID_FIN
                            from CDW_COMPONENT_LINK_CLASSES$IDS lci
                            where lci.LCL_ID_NEW = lc.LCL_INVERSE_LCL_ID)
/

update
    CDW_COMPONENT_LINK_CLASSES lc
set
    lc.LCL_PARENT_LCL_ID = (select lci.LCL_ID_FIN
                            from CDW_COMPONENT_LINK_CLASSES$IDS lci
                            where lci.LCL_ID_NEW = lc.LCL_PARENT_LCL_ID)
/

update
    CDW_COMPONENT_LINK_CLASSES lc
set
    lc.LCL_ID = (select lci.LCL_ID_FIN
                 from CDW_COMPONENT_LINK_CLASSES$IDS lci
                 where lci.LCL_ID_NEW = lc.LCL_ID)
/

------------------------------------------------------------------------------------
--
-- Handling Components
--
------------------------------------------------------------------------------------

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

    insert into CDW_COMPONENTS (CMP_ID, CMP_NAME, CMP_CCL_ID) values (r.CMP_ID *  -1, r.CMP_NAME, l_ccl_id);
    insert into CDW_COMPONENTS$IDS (CMP_ID_NEW, CMP_NAME, CMP_ID_FIN) values (r.CMP_ID * -1, r.CMP_NAME, r.CMP_ID);

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

update cdw_facts set fct_cmp_id = null where fct_cmp_id not in (select cmp_id from cdw_components)
/

begin

    cdw_utility.enable_foreign_key('CDW_FACTS', 'FCT_CMP_ID');

    -- Takes quite a long time, needs optimisation
    -- cdw_ontology.update_graph_links;

end;
/

select count(*) as "ORPHAN FACTS LEFT" from cdw_facts where fct_cmp_id is null
/

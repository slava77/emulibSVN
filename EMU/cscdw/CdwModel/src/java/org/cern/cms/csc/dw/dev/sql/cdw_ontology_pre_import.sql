begin
    cdw_ontology.disable_foreign_key('CDW_FACTS', 'FCT_CMP_ID');
end;
/

delete from cdw_component_graph_links
/
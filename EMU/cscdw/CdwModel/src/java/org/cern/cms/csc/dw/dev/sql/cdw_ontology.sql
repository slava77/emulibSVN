drop sequence CDW_CGL_ID_SEQ
/

CREATE SEQUENCE CDW_CGL_ID_SEQ
/

drop package CDW_ONTOLOGY
/

CREATE OR REPLACE PACKAGE CDW_ONTOLOGY AS 

  procedure update_graph_links;

END CDW_ONTOLOGY;
/

create or replace
PACKAGE BODY CDW_ONTOLOGY AS

  procedure update_graph_links AS
    i number := 0;
  BEGIN

    delete from CDW_COMPONENT_GRAPH_LINKS;

    -- Adding direct links; i = 0
    insert into cdw_component_graph_links /*+ append */ g (CGL_ID, CGL_PARENT_CMP_ID, CGL_CHILD_CMP_ID, CGL_LCL_ID, CGL_LEVEL)
    select cdw_cgl_id_seq.nextval, l.lin_cmp_id, l.lin_linked_cmp_id, l.lin_lcl_id, 1 from cdw_component_links l;

    loop

      i := i + 1;

      insert into cdw_component_graph_links /*+ append */ (
        CGL_ID,
        CGL_LEVEL,
        CGL_PARENT_CMP_ID,
        CGL_CHILD_CMP_ID,
        CGL_LCL_ID)
      select
        cdw_cgl_id_seq.nextval,
        i + 1,
        t1.cgl_parent_cmp_id,
        t2.cgl_child_cmp_id,
        t2.cgl_lcl_id
      from (
          select
            cgl_parent_cmp_id,
            cgl_child_cmp_id,
            cgl_lcl_id,
            cgl_level
          from
            cdw_component_graph_links
            join cdw_component_link_classes on (
              cgl_lcl_id = lcl_id and
              lcl_transitive = 1)
          where
            cgl_level = i
        ) t1
        join cdw_component_graph_links t2 on (
          t1.cgl_child_cmp_id = t2.cgl_parent_cmp_id and
          t1.cgl_lcl_id = t2.cgl_lcl_id and
          t2.cgl_level = 1)
      where
        (t1.cgl_parent_cmp_id, t2.cgl_child_cmp_id, t2.cgl_lcl_id) not in (select t3.cgl_parent_cmp_id, t3.cgl_child_cmp_id, t3.cgl_lcl_id from cdw_component_graph_links t3);

      exit when SQL%ROWCOUNT = 0;

    end loop;

  END update_graph_links;

END CDW_ONTOLOGY;
/

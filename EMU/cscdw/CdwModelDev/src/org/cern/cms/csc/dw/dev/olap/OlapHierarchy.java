package org.cern.cms.csc.dw.dev.olap;

import java.io.PrintWriter;
import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;
import org.cern.cms.csc.dw.dev.OlapGenerator;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GServices;
import org.neo4j.graphdb.Direction;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class OlapHierarchy {

    private final String name;
    private final String tableName;
    private final String dbSchema;
    private final String sql;
    private final GServices gdb;
    private Collection<List<GComponent>> components = new LinkedList<List<GComponent>>();

    private OlapHierarchyLevel top;

    public OlapHierarchy(GServices gdb, Element el, String dbSchema) {

        this.gdb = gdb;
        this.name = el.getAttribute("name");
        this.tableName = el.getAttribute("table");
        this.dbSchema = dbSchema;

        Collection<Element> sqls = getElementsByTagName(el, "sql");
        if (sqls.isEmpty()) {
            this.sql = null;
        } else {
            this.sql = sqls.iterator().next().getTextContent();
        }

        Collection<Element> levels = getElementsByTagName(el, "Level");
        if (!levels.isEmpty()) {
            this.top = new OlapHierarchyLevel(levels.iterator().next(), (sql != null));
        }

    }

    public String getName() {
        return name;
    }

    public String getTableName() {
        return tableName;
    }

    public OlapHierarchyLevel getTop() {
        return top;
    }

    public void generateDDL(PrintWriter out) {

        out.println("DROP TABLE " + tableName);
        out.print(OlapGenerator.SQL_ENDL);

        if (sql == null) {

            out.println("CREATE TABLE " + tableName + " (");
            out.println("\tCMP_ID NUMBER,");

            OlapHierarchyLevel h = top;
            do {
                out.print("\t" + h.getColumnName() + " VARCHAR2(1000)");
                if (!h.isSetNextLevel()) {
                    break;
                }
                out.println(",");
                h = h.getNextLevel();
            } while (true);

            out.print("\n)");
            out.print(OlapGenerator.SQL_ENDL);

            for (List<GComponent> cl: components) {
                out.print("INSERT INTO " + tableName + " VALUES (");
                out.print(cl.get(cl.size() - 1).getId());
                out.print(",");
                boolean first = true;
                for (GComponent c: cl) {
                    if (!first) out.print(",");
                    out.print("'" + c.getName() + "'");
                    first = false;
                }
                out.print(")");
                out.print(OlapGenerator.SQL_ENDL);
            }
            
        } else {

            out.println("CREATE TABLE " + tableName + " AS ");
            out.println(sql);
            out.print(OlapGenerator.SQL_ENDL);

        }
    }

    /*
    <Dimension name="Component" type="StandardDimension">
        <Hierarchy hasAll="true" name="Component" primaryKey="CMP_ID">
            <Table name="CDW_COMPONENTS" schema="CMS_VALDAS"/>
            <Level column="CMP_NAME" hideMemberIf="Never" levelType="Regular" name="Component" type="String" uniqueMembers="true"/>
        </Hierarchy>
    </Dimension>
    */
    public Element getElement(Document doc) {

        Element el = doc.createElement("Dimension");
        el.setAttribute("name", this.name);
        el.setAttribute("type", "StandardDimension");

        Element hel = doc.createElement("Hierarchy");
        hel.setAttribute("name", this.name);
        hel.setAttribute("hasAll", "true");
        hel.setAttribute("primaryKey", "CMP_ID");
        el.appendChild(hel);

        Element tel = doc.createElement("Table");
        tel.setAttribute("name", this.tableName);
        tel.setAttribute("schema", dbSchema);
        hel.appendChild(tel);

        OlapHierarchyLevel h = this.getTop();
        while (h != null) {
            Element lel = doc.createElement("Level");
            lel.setAttribute("column", h.getColumnName());
            lel.setAttribute("hideMemberIf", "Never");
            lel.setAttribute("levelType", "Regular");
            lel.setAttribute("name", h.getName());
            lel.setAttribute("type", "String");
            lel.setAttribute("uniqueMembers", "true");
            hel.appendChild(lel);
            h = h.getNextLevel();
        }

        return el;
    }

    public class OlapHierarchyLevel {

        private final String name;
        private final String columnName;

        private final Collection<GComponentLinkClass> linkTypes;
        private final Set<GComponentClass> componentTypes;
        private OlapHierarchyLevel nextLevel = null;

        public OlapHierarchyLevel(Element el, boolean sqlProvided) {
            this.name = el.getAttribute("name");
            if (el.hasAttribute("column")) {
                this.columnName = el.getAttribute("column");
            } else {
                this.columnName = name.toUpperCase();
            }

            this.componentTypes = new TreeSet<GComponentClass>();
            for (Element ccel: getElementsByTagName(el, "ComponentClass")) {
                this.componentTypes.add(
                    gdb.getGComponentClass(
                        ComponentClassType.fromValue(ccel.getAttribute("name"))
                    )
                );
            }

            this.linkTypes = new LinkedList<GComponentLinkClass>();
            for (Element ccel: getElementsByTagName(el, "ComponentLinkClass")) {
                this.linkTypes.add(
                    gdb.getGComponentLinkClass(
                        ComponentLinkClassType.fromValue(ccel.getAttribute("name"))
                    )
                );
            }

            if (!sqlProvided) {
                if (components.isEmpty()) {

                    for (GComponentClass cc: componentTypes) {
                        for (GComponent c: cc.getComponents()) {
                            List<GComponent> list = new LinkedList<GComponent>();
                            list.add(c);
                            components.add(list);
                        }
                    }

                } else {

                    Collection<List<GComponent>> backup = new LinkedList<List<GComponent>>(components);
                    components.clear();
                    for (List<GComponent> cl: backup) {
                        GComponent cp = cl.get(cl.size() - 1);
                        for (GComponentLinkClass clc: linkTypes) {
                            for (GComponent c: cp.getRelatedGComponents(clc.getType(), Direction.INCOMING)) {
                                if (componentTypes.contains(c.getType()) || componentTypes.isEmpty()) {
                                    List<GComponent> list = new LinkedList<GComponent>(cl);
                                    list.add(c);
                                    components.add(list);
                                }
                            }
                        }
                    }

                }
            }

            Collection<Element> levels = getElementsByTagName(el, "Level");
            if (!levels.isEmpty()) {
                this.nextLevel = new OlapHierarchyLevel(levels.iterator().next(), sqlProvided);
            }

        }


        public String getColumnName() {
            return columnName;
        }

        public String getName() {
            return name;
        }

        public OlapHierarchyLevel getNextLevel() {
            return nextLevel;
        }

        public boolean isSetNextLevel() {
            return nextLevel != null;
        }

    }

    public static Collection<Element> getElementsByTagName(Node node, String tagName) {
        Collection<Element> ret = new LinkedList<Element>();
        NodeList list = node.getChildNodes();
        for (int i = 0; i < list.getLength(); i++) {
            if (list.item(i).getNodeType() == Node.ELEMENT_NODE) {
                if (((Element) list.item(i)).getTagName().equals(tagName)) {
                    ret.add((Element) list.item(i));
                }
            }
        }
        return ret;
    }

}

package org.cern.cms.csc.dw.dev.olap;

import java.beans.PropertyDescriptor;
import java.io.PrintWriter;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import javax.persistence.Column;
import javax.persistence.JoinColumn;
import org.apache.commons.beanutils.BeanUtilsBean;
import org.cern.cms.csc.dw.metadata.FactMd;
import org.cern.cms.csc.dw.model.annotation.OlapDimension;
import org.cern.cms.csc.dw.model.annotation.OlapMeasure;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class CubeDef {

    private FactMd fact;
    private List<ColumnDef> columns = new ArrayList<ColumnDef>();
    private BeanUtilsBean utils = BeanUtilsBean.getInstance();
    private boolean hasMeasure = false;
    private boolean hasDimension = false;
    private final String dbSchema;

    private int tableSuffixNumber = 1;

    public CubeDef(FactMd fact, String dbSchema) {
        this.fact = fact;
        this.dbSchema = dbSchema;

        for (PropertyDescriptor prop : utils.getPropertyUtils().getPropertyDescriptors(fact.getFactClass())) {
            Method m = prop.getReadMethod();

            if (m.isAnnotationPresent(Column.class) || m.isAnnotationPresent(JoinColumn.class)) {

                if (m.isAnnotationPresent(OlapMeasure.class)) {
                    columns.add(new MeasureDef(this, m));
                    hasMeasure = true;
                }

                if (m.isAnnotationPresent(OlapDimension.class)) {
                    columns.add(new DimensionDef(this, m));
                    hasDimension = true;
                }
                
            }

        }
    }

    private String getCubeTableName() {
        return fact.getTableName();
    }

    public FactMd getFact() {
        return fact;
    }

    public String getDbSchema() {
        return dbSchema;
    }

    public boolean hasMeasureAndDimension() {
        return (hasMeasure && hasDimension);
    }

    public String getTableSuffix() {
        return "D" + String.valueOf(tableSuffixNumber++);
    }

    public void generateSchema(Document doc, Node schemaNd) {

        Element cubeEl = doc.createElement("Cube");
        cubeEl.setAttribute("name", fact.getTableName());
        cubeEl.setAttribute("cache", "true");
        cubeEl.setAttribute("enabled", "true");
        Node cubeNd = schemaNd.appendChild(cubeEl);

        Element tableEl = doc.createElement("Table");
        tableEl.setAttribute("name", getCubeTableName());
        tableEl.setAttribute("schema", dbSchema);
        cubeNd.appendChild(tableEl);

        // Get the first cube node. We gonna put shared dimensions right before this node.
        Node firstCubeNd = cubeNd;
        for (int i = 0; i < schemaNd.getChildNodes().getLength(); i++) {
            Node itemNd = schemaNd.getChildNodes().item(i);
            if (itemNd.getNodeName().equals("Cube")) {
                firstCubeNd = itemNd;
                break;
            }
        }

        /**
         * Generating dimensions
         */

        for (ColumnDef col : columns) {
            if (col instanceof DimensionDef) {
                DimensionDef dim = (DimensionDef) col;
                if (dim.isShared()) {
                    if (dim.isBaseField()) {
                        schemaNd.insertBefore(dim.getElement(doc), firstCubeNd);
                    }
                    cubeNd.appendChild(dim.getUsageElement(doc));
                } else {
                    cubeNd.appendChild(dim.getElement(doc));
                }
            }
        }

        /**
         * Adding special measures
         */

        {
            Element el = doc.createElement("Measure");
            el.setAttribute("name", "No of Facts");
            el.setAttribute("column", "FCT_ID");
            el.setAttribute("datatype", "Integer");
            el.setAttribute("aggregator", "count");
            el.setAttribute("visible", "true");
            cubeNd.appendChild(el);
        }

        /**
         * Generating measures like this:
         *
         * <Measure name="Dataset Id" column="RDA_ID" datatype="Integer" formatString="###" aggregator="distinct-count" visible="true"></Measure>
         *
         */
        for (ColumnDef col : columns) {
            if (col instanceof MeasureDef) {
                MeasureDef meas = (MeasureDef) col;
                Element el = meas.getElement(doc);
                cubeNd.appendChild(el);
            }
        }
        
    }

    @SuppressWarnings(value = "unchecked")
    public void generateDDL(PrintWriter out) {
        
        for (ColumnDef col : columns) {
            col.getDDL(out);
        }
        
    }
}

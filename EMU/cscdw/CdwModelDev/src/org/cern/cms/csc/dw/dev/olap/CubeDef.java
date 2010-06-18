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
    private List<MeasureDef> measures = new ArrayList<MeasureDef>();
    private List<DimensionDef> dimensions = new ArrayList<DimensionDef>();
    private BeanUtilsBean utils = BeanUtilsBean.getInstance();

    public CubeDef(FactMd fact) {
        this.fact = fact;
        
        for (PropertyDescriptor prop : utils.getPropertyUtils().getPropertyDescriptors(fact.getFactClass())) {
            Method m = prop.getReadMethod();

            if (m.isAnnotationPresent(Column.class) || m.isAnnotationPresent(JoinColumn.class)) {

                if (m.isAnnotationPresent(OlapMeasure.class)) {
                    measures.add(new MeasureDef(m));
                }

                if (m.isAnnotationPresent(OlapDimension.class)) {
                    dimensions.add(new DimensionDef(m, this));
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

    public boolean hasColumns() {
        return (measures.size() > 0 || dimensions.size() > 0);
    }

    public void generateSchema(Document doc, Node schemaNd, String dbSchema) {

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

        for (DimensionDef dim : dimensions) {
            if (dim.isShared()) {
                if (dim.isBaseField()) {
                    schemaNd.insertBefore(dim.getElement(doc, dbSchema), firstCubeNd);
                }
                cubeNd.appendChild(dim.getUsageElement(doc));
            } else {
                cubeNd.appendChild(dim.getElement(doc, dbSchema));
            }
        }

        /**
         * Generating measures like this:
         *
         * <Measure name="Dataset Id" column="RDA_ID" datatype="Integer" formatString="###" aggregator="distinct-count" visible="true"></Measure>
         *
         */
        for (MeasureDef meas : measures) {
            Element el = meas.getElement(doc);
            cubeNd.appendChild(el);
        }
    }

    @SuppressWarnings(value = "unchecked")
    public void generateDDL(PrintWriter out) {
        
        for (DimensionDef dim : dimensions) {
            dim.getDDL(out, this);
        }
        
    }
}

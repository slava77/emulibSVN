package org.cern.cms.csc.dw.dev.olap;

import java.lang.reflect.Method;
import org.cern.cms.csc.dw.model.annotation.OlapMeasure;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class MeasureDef extends ColumnDef {

    private OlapMeasure olapMeasure;

    public MeasureDef(Method method) {
        super(method);
        this.olapMeasure = method.getAnnotation(OlapMeasure.class);
    }

    /**
     * Generating measures like this:
     * <Measure name="Dataset Id" column="RDA_ID" datatype="Integer" formatString="###" aggregator="distinct-count" visible="true"></Measure>
     * @param doc
     * @return XML element
     */
    public Element getElement(Document doc) {
        Element el = doc.createElement("Measure");
        el.setAttribute("name", olapMeasure.name());
        el.setAttribute("column", columnName);
        el.setAttribute("datatype", type.getTypeName());
        if (!olapMeasure.formatString().equals("")) {
            el.setAttribute("formatString", olapMeasure.formatString());
        }
        el.setAttribute("aggregator", olapMeasure.aggregator().getValue());
        el.setAttribute("visible", "true");
        return el;
    }

}

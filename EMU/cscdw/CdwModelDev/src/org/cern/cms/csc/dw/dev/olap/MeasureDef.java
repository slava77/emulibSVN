package org.cern.cms.csc.dw.dev.olap;

import java.lang.reflect.Method;
import org.cern.cms.csc.dw.model.annotation.OlapMeasure;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class MeasureDef extends ColumnDef {

    private final OlapMeasure.AggregatorType aggregator;
    private final String formatString;

    public MeasureDef(CubeDef cube, Method method, String name, OlapMeasure.AggregatorType aggregator, String formatString) {
        super(cube, method, name);
        this.aggregator = aggregator;
        this.formatString = formatString;
    }

    /**
     * Generating measures like this:
     * <Measure name="Dataset Id" column="RDA_ID" datatype="Integer" formatString="###" aggregator="distinct-count" visible="true"></Measure>
     * @param doc
     * @return XML element
     */
    @Override
    public Element getElement(Document doc) {
        Element el = doc.createElement("Measure");
        el.setAttribute("name", name);
        el.setAttribute("column", columnName);
        el.setAttribute("datatype", type.getTypeName());
        if (!formatString.equals("")) {
            el.setAttribute("formatString", formatString);
        }
        el.setAttribute("aggregator", aggregator.getValue());
        el.setAttribute("visible", "true");
        return el;
    }

}

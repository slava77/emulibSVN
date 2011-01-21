package org.cern.cms.csc.exsys.gui.component.table.converter;

import java.util.Iterator;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterIf;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.exsys.gui.component.table.BeanTableFilter;

public class FilterBooleanConverter extends FilterConverter {

    @Override
    public String valueToString(Object value) {
        Boolean o = (Boolean) value;
        return o.toString();
    }

    @Override
    public Object valueToObject(String value) {
        return (Object) Boolean.parseBoolean(value);
    }

    @Override
    public void checkFilter(BeanTableFilter filter) {
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf item = itemItr.next();
            BeanTableFilterIf.Operation op = item.getOperation();
            if (op != BeanTableFilterIf.Operation.EQUAL) {
                ConversionError("filterConverter.BooleanNotEqual");
            }
        }
    }
}


package org.cern.cms.csc.dw.gui.table.column.filter.converter;

import java.util.Iterator;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.dw.gui.table.column.filter.TableFilter;

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
    public void checkFilter(TableFilter filter) {
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf item = itemItr.next();
            TableFilter.Operation op = item.getOperation();
            if (op != TableFilter.Operation.EQUAL) {
                ConversionError("filterConverter.BooleanNotEqual");
            }
        }
    }
}


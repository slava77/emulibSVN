package org.cern.cms.csc.dw.gui.table.column.filter.converter;

import java.util.Iterator;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.dw.gui.table.column.filter.TableFilter;

public class FilterPeriodConverter extends FilterConverter {

    private PeriodConverter converter = new PeriodConverter();

    @Override
    public String valueToString(Object value) {
        return converter.getAsString(null, null, value);
    }

    @Override
    public Object valueToObject(String value) {
        return converter.getAsObject(null, null, value);
    }

    @Override
    public void checkFilter(TableFilter filter) {
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf item = itemItr.next();
            TableFilter.Operation op = item.getOperation();
            if (op == TableFilter.Operation.LIKE) {
                ConversionError("filterConverter.PeriodLikeOperation");
            }
        }
    }
}


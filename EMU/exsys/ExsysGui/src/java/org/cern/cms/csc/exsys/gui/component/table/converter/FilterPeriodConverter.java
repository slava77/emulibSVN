package org.cern.cms.csc.exsys.gui.component.table.converter;

import java.util.Iterator;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.exsys.gui.component.table.BeanTableFilter;

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
    public void checkFilter(BeanTableFilter filter) {
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf item = itemItr.next();
            BeanTableFilter.Operation op = item.getOperation();
            if (op == BeanTableFilter.Operation.LIKE) {
                ConversionError("filterConverter.PeriodLikeOperation");
            }
        }
    }
}


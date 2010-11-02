package org.cern.cms.csc.dw.gui.table.column.filter.converter;

import java.util.Iterator;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.dw.gui.table.column.filter.TableFilter;

public class FilterClobConverter extends FilterStringConverter {

    @Override
    public void checkFilter(TableFilter filter) {
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf item = itemItr.next();
            TableFilter.Operation op = item.getOperation();
            if (op == TableFilter.Operation.MORE
                    || op == TableFilter.Operation.LESS
                    || op == TableFilter.Operation.MORE_EQUAL
                    || op == TableFilter.Operation.LESS_EQUAL
                    || op == TableFilter.Operation.EQUAL
                    || op == TableFilter.Operation.NOT_EQUAL) {
                ConversionError("filterConverter.ClobIllegalOperation");
            }
        }
    }
}


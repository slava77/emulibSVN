package org.cern.cms.csc.dw.gui.table.column.filter.converter;

import java.util.Iterator;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.dw.gui.table.column.filter.TableFilter;

public abstract class FilterNumericConverter extends FilterConverter {

    @Override
    public final void checkFilter(TableFilter filter) {
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf item = itemItr.next();
            TableFilter.Operation op = item.getOperation();
            if (op == TableFilter.Operation.LIKE) {
                ConversionError("filterConverter.NumberLikeOperation");
            }
        }
    }
}


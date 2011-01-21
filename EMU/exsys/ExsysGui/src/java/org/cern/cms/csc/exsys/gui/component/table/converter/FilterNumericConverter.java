package org.cern.cms.csc.exsys.gui.component.table.converter;

import java.util.Iterator;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.exsys.gui.component.table.BeanTableFilter;

public abstract class FilterNumericConverter extends FilterConverter {

    @Override
    public final void checkFilter(BeanTableFilter filter) {
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf item = itemItr.next();
            BeanTableFilter.Operation op = item.getOperation();
            if (op == BeanTableFilter.Operation.LIKE) {
                ConversionError("filterConverter.NumberLikeOperation");
            }
        }
    }
}


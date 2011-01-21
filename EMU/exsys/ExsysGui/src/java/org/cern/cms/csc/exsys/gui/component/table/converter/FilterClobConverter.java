package org.cern.cms.csc.exsys.gui.component.table.converter;

import java.util.Iterator;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterIf;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.exsys.gui.component.table.BeanTableFilter;

public class FilterClobConverter extends FilterStringConverter {

    @Override
    public void checkFilter(BeanTableFilter filter) {
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf item = itemItr.next();
            BeanTableFilterIf.Operation op = item.getOperation();
            if (op == BeanTableFilterIf.Operation.MORE
                    || op == BeanTableFilterIf.Operation.LESS
                    || op == BeanTableFilterIf.Operation.MORE_EQUAL
                    || op == BeanTableFilterIf.Operation.LESS_EQUAL
                    || op == BeanTableFilterIf.Operation.EQUAL
                    || op == BeanTableFilterIf.Operation.NOT_EQUAL) {
                ConversionError("filterConverter.ClobIllegalOperation");
            }
        }
    }
}


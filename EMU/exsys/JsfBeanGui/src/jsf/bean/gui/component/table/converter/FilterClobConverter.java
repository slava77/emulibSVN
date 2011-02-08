package jsf.bean.gui.component.table.converter;

import java.util.Iterator;
import jsf.bean.gui.component.table.BeanTableFilterItem;
import jsf.bean.gui.component.table.BeanTableFilter;

public class FilterClobConverter extends FilterStringConverter {

    @Override
    public void checkFilter(BeanTableFilter filter) {
        for (Iterator<BeanTableFilterItem> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItem item = itemItr.next();
            BeanTableFilter.Operation op = item.getOperation();
            if (op == BeanTableFilter.Operation.MORE
                    || op == BeanTableFilter.Operation.LESS
                    || op == BeanTableFilter.Operation.MORE_EQUAL
                    || op == BeanTableFilter.Operation.LESS_EQUAL
                    || op == BeanTableFilter.Operation.EQUAL
                    || op == BeanTableFilter.Operation.NOT_EQUAL) {
                ConversionError("filterConverter.ClobIllegalOperation");
            }
        }
    }
}


package jsf.bean.gui.component.table.converter;

import java.util.Iterator;
import jsf.bean.gui.component.table.BeanTableFilterItem;
import jsf.bean.gui.component.table.BeanTableFilter;

public abstract class FilterNumericConverter extends FilterConverter {

    @Override
    public final void checkFilter(BeanTableFilter filter) {
        for (Iterator<BeanTableFilterItem> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItem item = itemItr.next();
            BeanTableFilter.Operation op = item.getOperation();
            if (op == BeanTableFilter.Operation.LIKE ||
                op == BeanTableFilter.Operation.NOTLIKE) {
                ConversionError("filterConverter.NumberLikeOperation");
            }
        }
    }
}


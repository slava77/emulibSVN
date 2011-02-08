package jsf.bean.gui.component.table.converter;

import java.util.Iterator;
import jsf.bean.gui.component.table.BeanTableFilter;
import jsf.bean.gui.component.table.BeanTableFilterItem;

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
        for (Iterator<BeanTableFilterItem> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItem item = itemItr.next();
            BeanTableFilter.Operation op = item.getOperation();
            if (op != BeanTableFilter.Operation.EQUAL) {
                ConversionError("filterConverter.BooleanNotEqual");
            }
        }
    }
}


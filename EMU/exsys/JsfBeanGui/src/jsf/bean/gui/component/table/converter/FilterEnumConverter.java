package jsf.bean.gui.component.table.converter;

import java.util.Iterator;
import jsf.bean.gui.component.table.BeanTableFilterItem;
import jsf.bean.gui.component.table.BeanTableFilter;

public class FilterEnumConverter extends FilterConverter {

    private Class enumClass;

    public FilterEnumConverter(Class enumClass) {
        this.enumClass = enumClass;
    }

    @Override
    public String valueToString(Object value) {
        if (value == null) {
            return null;
        }
        Enum e = (Enum) value;
        return e.name();
    }

    @Override
    @SuppressWarnings("unchecked")
    public Object valueToObject(String value) {
        try {
            return (Enum) Enum.valueOf(enumClass, value);
        } catch (Exception ex) {
            ConversionError(String.format("Value %s not found in restricted list of values!", value));
        }
        return null;
    }

    @Override
    public void checkFilter(BeanTableFilter filter) {
        for (Iterator<BeanTableFilterItem> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItem item = itemItr.next();
            BeanTableFilter.Operation op = item.getOperation();
            if (op != BeanTableFilter.Operation.EQUAL &&
                op != BeanTableFilter.Operation.NOT_EQUAL) {
                ConversionError("Enum filter supports only Equals and Not Equals operations. Please fix the filter!");
            }
        }
    }

}


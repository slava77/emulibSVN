package jsf.bean.gui.component.table.converter;

import java.util.Iterator;
import java.util.regex.Pattern;
import jsf.bean.gui.component.table.BeanTableFilterItem;
import jsf.bean.gui.component.table.BeanTableFilter;

public class FilterStringConverter extends FilterConverter {

    private static Pattern q01Pattern = Pattern.compile("\"");
    private static Pattern q02Pattern = Pattern.compile("'");

    @Override
    public String valueToString(Object value) {
        String o = (String) value;
        boolean b01Found = q01Pattern.matcher(o).find();
        boolean b02Found = q02Pattern.matcher(o).find();
        if (!b01Found) {
            o = "\"" + o + "\"";
        } else if (!b02Found) {
            o = "\'" + o + "\'";
        } else {
            o = q01Pattern.matcher(o).replaceAll("");
            o = "\"" + o + "\"";
        }
        return o;
    }

    @Override
    public Object valueToObject(String value) {
        return (Object) value;
    }

    @Override
    public void checkFilter(BeanTableFilter filter) {
        for (Iterator<BeanTableFilterItem> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItem item = itemItr.next();
            BeanTableFilter.Operation op = item.getOperation();
            if (op == BeanTableFilter.Operation.MORE
                    || op == BeanTableFilter.Operation.LESS
                    || op == BeanTableFilter.Operation.MORE_EQUAL
                    || op == BeanTableFilter.Operation.LESS_EQUAL) {
                ConversionError("String filter does not support MORE, LESS operations. Please fix the filter!");
            }
        }
    }
}


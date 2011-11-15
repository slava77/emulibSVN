package jsf.bean.gui.component.table.converter;

public class FilterLongConverter extends FilterNumericConverter {

    @Override
    public String valueToString(Object value) {
        Long o = (Long) value;
        return o.toString();
    }

    @Override
    public Object valueToObject(String value) {
        try {
            return (Object) Long.parseLong(value);
        } catch (NumberFormatException e) {
            ConversionError("Error converting filter string to Long filter. Please fix the filter!");
        }
        return null;
    }

}


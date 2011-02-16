package jsf.bean.gui.component.table.converter;

public class FilterFloatConverter extends FilterNumericConverter {

    @Override
    public String valueToString(Object value) {
        Float o = (Float) value;
        return o.toString();
    }

    @Override
    public Object valueToObject(String value) {
        try {
            return (Object) Float.parseFloat(value);
        } catch (NumberFormatException e) {
            ConversionError("filterConverter.NumberFormatException");
        }
        return null;
    }


}

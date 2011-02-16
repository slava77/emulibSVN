package jsf.bean.gui.component.table.converter;

public class FilterDoubleConverter extends FilterNumericConverter {

    @Override
    public String valueToString(Object value) {
        Double o = (Double) value;
        return o.toString();
    }

    @Override
    public Object valueToObject(String value) {
        try {
            return (Object) Double.parseDouble(value);
        } catch (NumberFormatException e) {
            ConversionError("filterConverter.NumberFormatException");
        }
        return null;
    }


}
package jsf.bean.gui.component.table.converter;

import java.math.BigInteger;

public class FilterBigIntegerConverter extends FilterNumericConverter {

    @Override
    public String valueToString(Object value) {
        if (value instanceof BigInteger) {
            BigInteger o = (BigInteger) value;
            return o.toString();
        } else {
            return null;
        }
    }

    @Override
    public Object valueToObject(String value) {
        try {
            return (Object) new BigInteger(value);
        } catch (NumberFormatException e) {
            ConversionError("Error converting filter string to Integer filter. Please fix the filter!");
        }
        return null;
    }

}


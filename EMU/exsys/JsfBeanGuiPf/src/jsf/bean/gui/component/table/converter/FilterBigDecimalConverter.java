package jsf.bean.gui.component.table.converter;

import java.math.BigDecimal;

public class FilterBigDecimalConverter extends FilterNumericConverter {

    @Override
    public String valueToString(Object value) {
        BigDecimal o = (BigDecimal) value;
        return o.toPlainString();
    }

    @Override
    public Object valueToObject(String value) {
        try {
            return (Object) new BigDecimal(value);
        } catch (NumberFormatException e) {
            ConversionError("Error converting filter string to Decimal filter. Please fix the filter!");
        }
        return null;
    }

}


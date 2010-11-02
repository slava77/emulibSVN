package org.cern.cms.csc.dw.gui.table.column.filter.converter;

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
            ConversionError("filterConverter.NumberFormatException");
        }
        return null;
    }

}


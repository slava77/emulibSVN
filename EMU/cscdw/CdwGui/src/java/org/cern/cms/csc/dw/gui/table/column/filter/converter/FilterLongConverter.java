package org.cern.cms.csc.dw.gui.table.column.filter.converter;

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
            ConversionError("filterConverter.NumberFormatException");
        }
        return null;
    }

}


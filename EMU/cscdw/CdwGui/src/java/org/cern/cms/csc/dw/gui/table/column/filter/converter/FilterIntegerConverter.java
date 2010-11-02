package org.cern.cms.csc.dw.gui.table.column.filter.converter;

public class FilterIntegerConverter extends FilterNumericConverter {

    @Override
    public String valueToString(Object value) {
        Integer o = (Integer) value;
        return o.toString();
    }

    @Override
    public Object valueToObject(String value) {
        try {
            return (Object) Integer.parseInt(value);
        } catch (NumberFormatException e) {
            ConversionError("filterConverter.NumberFormatException");
        }
        return null;
    }

}

package org.cern.cms.csc.exsys.gui.component.table.converter;

import java.util.Calendar;
import java.util.Date;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.exsys.gui.component.table.BeanTableFilter;

public class FilterDateConverter extends FilterConverter {

    private static Pattern datePattern = Pattern.compile("^([0-9]{4})-([0-9]{1,2})-([0-9]{1,2})$");

    private static String dateNumber(String s) {
        if (s.length() == 1) {
            s = "0" + s;
        }
        return s;
    }

    @Override
    public String valueToString(Object value) {
        Calendar cal = Calendar.getInstance();
        cal.setTime((Date) value);
        String s = String.valueOf(cal.get(Calendar.YEAR)).trim();
        s += "-" + dateNumber(String.valueOf(cal.get(Calendar.MONTH) + 1).trim());
        s += "-" + dateNumber(String.valueOf(cal.get(Calendar.DATE)).trim());
        return s;
    }

    @Override
    public Object valueToObject(String value) {
        Calendar cal = Calendar.getInstance();
        Matcher m = datePattern.matcher(value);
        if (!m.matches()) {
            ConversionError("filterConverter.DateFormatException");
        }
        cal.set(Integer.parseInt(m.group(1)),
                Integer.parseInt(m.group(2)) - 1,
                Integer.parseInt(m.group(3)));
        return (Object) cal.getTime();
    }

    @Override
    public void checkFilter(BeanTableFilter filter) {
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf item = itemItr.next();
            BeanTableFilter.Operation op = item.getOperation();
            if (op == BeanTableFilter.Operation.LIKE) {
                ConversionError("filterConverter.DateLikeOperation");
            }
        }
    }
}


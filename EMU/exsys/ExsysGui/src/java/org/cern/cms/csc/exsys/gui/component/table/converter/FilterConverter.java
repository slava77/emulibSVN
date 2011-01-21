package org.cern.cms.csc.exsys.gui.component.table.converter;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Date;
import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import javax.faces.application.FacesMessage;
import javax.faces.convert.ConverterException;
import java.util.Iterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.faces.convert.Converter;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterIf;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.exsys.gui.component.table.BeanTableFilter;
import org.cern.cms.csc.exsys.gui.component.table.BeanTableFilterItem;

public abstract class FilterConverter implements Converter {

    private static Pattern tokenPattern = Pattern.compile("('[^']+'|\"[^\"]+\"|[^\" ]+)");
    private static Pattern quotePattern = Pattern.compile("^['\"]+(.*)['\"]+$");

    abstract String valueToString(Object value);

    abstract Object valueToObject(String value);

    abstract void checkFilter(BeanTableFilter filter);

    public void ConversionError(String title) {
        FacesMessage message = new FacesMessage(FacesMessage.SEVERITY_ERROR, title, title);
        throw new ConverterException(message);
    }

    private String removeQuotes(String value) {
        Matcher m = quotePattern.matcher(value.trim());
        return m.replaceAll("$1");
    }

    @Override
    public Object getAsObject(FacesContext context, UIComponent component, String value) {
        BeanTableFilter filter = new BeanTableFilter();
        BeanTableFilterItem item = new BeanTableFilterItem();
        if (value != null && value.trim().length() > 0) {
            Matcher m = tokenPattern.matcher(value);
            while (m.find()) {
                String token = m.group();

                BeanTableFilterIf.Operator opr = BeanTableFilterIf.Operator.fromValue(token);
                if (opr != null) {
                    item.setOperator(opr);
                } else {
                    BeanTableFilterIf.Operation opn = BeanTableFilterIf.Operation.fromValue(token);
                    if (opn != null) {
                        item.setOperation(opn);
                        if (opn.equals(BeanTableFilterIf.Operation.ISNULL) || opn.equals(BeanTableFilterIf.Operation.ISNOTNULL)) {
                            filter.getItems().add(item);
                            item = new BeanTableFilterItem();
                        }
                    } else {
                        item.setValue(valueToObject(removeQuotes(token)));
                        filter.getItems().add(item);
                        item = new BeanTableFilterItem();
                    }
                }
            }
        }
        checkFilter(filter);
        filter.setString(getAsString(context, component, filter));
        return filter;
    }

    @Override
    public String getAsString(FacesContext context, UIComponent component, Object value) {
        if (value == null) {
            return "";
        }
        BeanTableFilter filter = (BeanTableFilter) value;
        String s = "";
        for (Iterator<BeanTableFilterItemIf> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItemIf i = itemItr.next();
            if (!s.equals("")) {
                switch (i.getOperator()) {
                    case AND:
                        s += " AND";
                        break;
                    case OR:
                        s += " OR";
                        break;
                }
            }
            switch (i.getOperation()) {
                case EQUAL:
                    s += " =";
                    break;
                case MORE:
                    s += " >";
                    break;
                case LESS:
                    s += " <";
                    break;
                case MORE_EQUAL:
                    s += " >=";
                    break;
                case LESS_EQUAL:
                    s += " <=";
                    break;
                case NOT_EQUAL:
                    s += " <>";
                    break;
                case LIKE:
                    s += " LIKE";
                    break;
                case NOTLIKE:
                    s += " NOTLIKE";
                    break;
                case ISNULL:
                    s += " isNull";
                    break;
                case ISNOTNULL:
                    s += " isNotNull";
                    break;
            }
            if (i.getValue() != null) {
                s += " " + valueToString(i.getValue());
            }
        }
        return s.trim();
    }

    public static FilterConverter getFilterConverter(Class type) {
        
        if (type.equals(BigDecimal.class)) {
            
            return new FilterBigDecimalConverter();

        } else
        if (type.equals(BigInteger.class)) {

            return new FilterBigIntegerConverter();

        } else
        if (type.equals(Long.class)) {

            return new FilterLongConverter();

        } else
        if (type.equals(Integer.class)) {

            return new FilterIntegerConverter();

        } else
        if (type.equals(Boolean.class)) {

            return new FilterBooleanConverter();

        } else
        if (type.equals(Date.class)) {

            return new FilterDateConverter();

        }
        
        return new FilterStringConverter();
        
    }

}


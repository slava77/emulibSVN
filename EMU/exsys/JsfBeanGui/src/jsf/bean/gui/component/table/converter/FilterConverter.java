package jsf.bean.gui.component.table.converter;

import java.io.Serializable;
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
import javax.persistence.Lob;
import jsf.bean.gui.annotation.PeriodType;
import jsf.bean.gui.component.table.BeanTableFilter;
import jsf.bean.gui.component.table.BeanTableFilterItem;
import jsf.bean.gui.metadata.PropertyMd;

public abstract class FilterConverter implements Converter, Serializable {

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

    public BeanTableFilter.Operation getDefaultOperation() {
        return BeanTableFilter.Operation.EQUAL;
    }
    
    @Override
    public Object getAsObject(FacesContext context, UIComponent component, String value) {
        BeanTableFilter filter = new BeanTableFilter();
        BeanTableFilterItem item = new BeanTableFilterItem();
        item.setOperation(getDefaultOperation());
        if (value != null && value.trim().length() > 0) {
            Matcher m = tokenPattern.matcher(value);
            while (m.find()) {
                String token = m.group();

                BeanTableFilter.Operator opr = BeanTableFilter.Operator.fromValue(token);
                if (opr != null) {
                    item.setOperator(opr);
                } else {
                    BeanTableFilter.Operation opn = BeanTableFilter.Operation.fromValue(token);
                    if (opn != null) {
                        item.setOperation(opn);
                        if (opn.equals(BeanTableFilter.Operation.ISNULL) || opn.equals(BeanTableFilter.Operation.ISNOTNULL)) {
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
        for (Iterator<BeanTableFilterItem> itemItr = filter.getItems().iterator(); itemItr.hasNext();) {
            BeanTableFilterItem i = itemItr.next();
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

    public static FilterConverter getFilterConverter(PropertyMd propertyMd) {
        
        Class type = propertyMd.getType();
        
        if (type.equals(BigDecimal.class)) {
            
            return new FilterBigDecimalConverter();

        } else
        if (type.equals(BigInteger.class)) {

            if (propertyMd.getField().isAnnotationPresent(PeriodType.class)) {
                return new FilterPeriodConverter();
            }

            return new FilterBigIntegerConverter();

        } else
        if (type.equals(Long.class) || (type.isPrimitive() && type.getSimpleName().equals("long"))) {

            return new FilterLongConverter();

        } else
        if (type.equals(Integer.class) || (type.isPrimitive() && type.getSimpleName().equals("int"))) {

            return new FilterIntegerConverter();

        } else
        if (type.equals(Boolean.class) || (type.isPrimitive() && type.getSimpleName().equals("boolean"))) {

            return new FilterBooleanConverter();

        } else
        if (type.equals(Date.class)) {

            return new FilterDateConverter();

        } else
        if (type.equals(Float.class) || (type.isPrimitive() && type.getSimpleName().equals("float"))) {

            return new FilterFloatConverter();
        } else
        if (type.equals(Double.class) || (type.isPrimitive() && type.getSimpleName().equals("double"))) {

            return new FilterDoubleConverter();
        } else
        if (type.isEnum()) {
            return new FilterEnumConverter(type);
        }
        
        if (propertyMd.getGetterMethod().isAnnotationPresent(Lob.class)) {
            return new FilterClobConverter();
        }
        
        return new FilterStringConverter();
        
    }

}


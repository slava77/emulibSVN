package jsf.bean.gui.component.table.column;

import java.beans.PropertyDescriptor;
import javax.faces.convert.DateTimeConverter;
import javax.faces.convert.NumberConverter;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.converter.PeriodConverter;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;
import org.apache.commons.beanutils.PropertyUtils;

public class BeanTableColumnSimple extends BeanTableColumnSortable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnSimple.class);
    private String enumNameItem = null;

    public BeanTableColumnSimple(BeanTable table, PropertyMd propertyMd) {
        super(table, propertyMd);

        // Exception for the enumerations
        this.enumNameItem = null;
        if (getType().isEnum()) {
            String nameItem = this.name.concat("Item");
            // Check if nameItem exists
            for (PropertyDescriptor pd: PropertyUtils.getPropertyDescriptors(table.getRowClass())) {
                if (pd.getName().equals(nameItem)) {
                    this.enumNameItem = nameItem;
                    break;
                }
            }
        }

        if (isPeriod()) {
            this.converter = new PeriodConverter();
        } else {
            if (isNumeric()) {

                NumberConverter numberConverter = new NumberConverter();
                numberConverter.setGroupingUsed(table.getProperties().getColumnNumberGrouping(name));
                {
                    Integer v = table.getProperties().getColumnNumberMinFractionDigits(name);
                    if (v != null) {
                        numberConverter.setMinFractionDigits(v);
                    }
                }
                {
                    Integer v = table.getProperties().getColumnNumberMaxFractionDigits(name);
                    if (v != null) {
                        numberConverter.setMaxFractionDigits(v);
                    }
                }
                numberConverter.setPattern(table.getProperties().getColumnNumberPattern(name));
                this.converter = numberConverter;
            } else {
                if (isDate()) {
                    this.converter = new DateTimeConverter();
                    ((DateTimeConverter) converter).setTimeZone(table.getProperties().getColumnDateTimeZone(name));
                    ((DateTimeConverter) converter).setPattern(table.getProperties().getColumnDateFormat(name));
                }
            }
        }

    }

    public final String getOutputFormat() {
        if (isDate()) {
            return table.getProperties().getColumnDateFormat(name);
        }
        if (isNumeric()) {
            return table.getProperties().getColumnNumberPattern(name);
        }
        return null;
    }

    @Override
    public String getFilterName() {
        String fn = super.getFilterName();
        if (enumNameItem != null) {
            fn = fn.replaceFirst(name.concat("$"), enumNameItem);
        }
        return fn;
    }

}

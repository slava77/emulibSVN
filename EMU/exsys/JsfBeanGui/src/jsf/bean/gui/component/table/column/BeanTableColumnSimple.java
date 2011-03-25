package jsf.bean.gui.component.table.column;

import java.beans.PropertyDescriptor;
import javax.faces.convert.DateTimeConverter;
import javax.faces.convert.NumberConverter;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;
import org.apache.commons.beanutils.PropertyUtils;

public class BeanTableColumnSimple extends BeanTableColumnSortable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnSimple.class);

    public BeanTableColumnSimple(BeanTable table, PropertyMd propertyMd) {
        super(table, propertyMd);

        // Exception for the enumerations
        if (propertyMd.getType().isEnum()) {
            // Check if nameItem exists
            String nameItem = this.name.concat("Item");
            for (PropertyDescriptor pd: PropertyUtils.getPropertyDescriptors(table.getRowClass())) {
                if (pd.getName().equals(nameItem)) {
                    this.name = nameItem;
                    break;
                }
            }
        }

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
        } else
        if (isDate()) {
            this.converter = new DateTimeConverter();
            ((DateTimeConverter) converter).setTimeZone(table.getProperties().getColumnDateTimeZone(name));
            ((DateTimeConverter) converter).setPattern(table.getProperties().getColumnDateFormat(name));
        }

    }

}

package jsf.bean.gui.component.table.column;

import javax.faces.convert.DateTimeConverter;
import javax.faces.convert.NumberConverter;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.converter.PeriodConverter;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;

public class BeanTableColumnSimple extends BeanTableColumnSortable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnSimple.class);

    public BeanTableColumnSimple(BeanTable table, PropertyMd propertyMd) {
        super(table, propertyMd);

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

}

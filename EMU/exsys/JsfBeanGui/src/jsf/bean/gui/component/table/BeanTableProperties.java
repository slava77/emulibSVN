package jsf.bean.gui.component.table;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Properties;
import java.util.TimeZone;
import javax.faces.model.SelectItem;

public class BeanTableProperties extends GenericProperties {

    public static final String KEY_TABLE_FILTER = "tableFilter";

    public static final String KEY_PAGE_SIZE = "pageSize";
    public static final String KEY_PAGE_FAST_STEP = "pageFastStep";
    public static final String KEY_PAGE_SIZES = "pageSizes";
    public static final String KEY_DISPLAY_FILTER = "displayFilter";
    public static final String KEY_DISPLAY_COLUMNS = "displayColumns";
    public static final String KEY_COLUMNS = "columns";
    public static final String KEY_SORTING = "sorting";
    public static final String KEY_COL_WIDTH_STEP = "columnWidthStep";
    public static final String KEYP_COL_SORT_ASC = "%s.asc";
    public static final String KEYP_COL_WIDTH = "%s.width";
    public static final String KEYP_COL_NUMBER_GROUPING = "%s.numberGrouping";
    public static final String KEYP_COL_NUMBER_MIN_FRAC_DIGITS = "%s.numberMinFractionDigits";
    public static final String KEYP_COL_NUMBER_MAX_FRAC_DIGITS = "%s.numberMaxFractionDigits";
    public static final String KEYP_COL_NUMBER_PATTERN = "%s.numberPattern";
    public static final String KEYP_COL_DATE_TIME_ZONE = "%s.dateTimeZone";
    public static final String KEYP_COL_DATE_FORMAT = "%s.dateTimeFormat";

    public static final String DEFAULT_KEY_TABLE_FILTER = null;

    public static final Integer DEFAULT_MAX_PAGE_SIZE = 100;
    public static final Integer DEFAULT_PAGE_SIZE = 20;
    public static final Integer DEFAULT_PAGE_FAST_STEP = 5;
    public static final List<SelectItem> DEFAULT_PAGE_SIZES;
    public static final Boolean DEFAULT_DISPLAY_FILTER = false;
    public static final Boolean DEFAULT_DISPLAY_COLUMNS = false;
    public static final Integer DEFAULT_COL_WIDTH_STEP = 5;
    public static final Boolean DEFAULT_COL_SORT_ASC = true;
    public static final Integer DEFAULT_COL_WIDTH = null;
    public static final Integer DEFAULT_COL_NUMBER_MIN_FRAC_DIGITS = null;
    public static final Integer DEFAULT_COL_NUMBER_MAX_FRAC_DIGITS = null;
    public static final String DEFAULT_COL_NUMBER_PATTERN = "";
    public static final String DEFAULT_COL_DATE_FORMAT = "EEE dd-MM-yy HH:mm:ss";
    public static final String DEFAULT_COL_DATE_TIME_ZONE = "Europe/Zurich";

    static {
        DEFAULT_PAGE_SIZES = new ArrayList<SelectItem>();
        for (int i = DEFAULT_PAGE_FAST_STEP; i <= DEFAULT_MAX_PAGE_SIZE; i+= DEFAULT_PAGE_FAST_STEP) {
            DEFAULT_PAGE_SIZES.add(new SelectItem(String.valueOf(i), String.valueOf(i)));
        }
    }

    private String tablePrefix = null;

    public BeanTableProperties() {
        super(new Properties());
    }

    public BeanTableProperties(Properties properties) {
        super(properties);
    }

    public String getTableFilter() {
        return get(tableKey(KEY_TABLE_FILTER), DEFAULT_KEY_TABLE_FILTER);
    }

    public void setTableFilter(String tableFilter) {
        set(tableKey(KEY_TABLE_FILTER), tableFilter);
    }

    public void setTablePrefix(String tablePrefix) {
        this.tablePrefix = tablePrefix;
    }

    public String getTablePrefix() {
        return tablePrefix;
    }

    private String tableKey(String key) {
        if (tablePrefix != null) {
            return tablePrefix.concat(key);
        }
        return key;
    }

    private String columnKey(String keyPattern, String name) {
        return tableKey(String.format(keyPattern, name));
    }

    public int getPageSize() {
        return get(tableKey(KEY_PAGE_SIZE), DEFAULT_PAGE_SIZE);
    }

    public void setPageSize(int pageSize) {
        set(tableKey(KEY_PAGE_SIZE), pageSize);
    }

    public int getPageFastStep() {
        return get(tableKey(KEY_PAGE_FAST_STEP), DEFAULT_PAGE_FAST_STEP);
    }

    public void setPageFastStep(int pageFastStep) {
        set(tableKey(KEY_PAGE_FAST_STEP), pageFastStep);
    }

    public Collection<SelectItem> getPageSizes() {
        return getSelectItemList(tableKey(KEY_PAGE_SIZES), DEFAULT_PAGE_SIZES);
    }

    public String getColumnDateFormat(String name) {
        return get(columnKey(KEYP_COL_DATE_FORMAT, name), DEFAULT_COL_DATE_FORMAT);
    }

    public TimeZone getColumnDateTimeZone(String name) {
        return TimeZone.getTimeZone(get(columnKey(KEYP_COL_DATE_TIME_ZONE, name), DEFAULT_COL_DATE_TIME_ZONE));
    }

    public List<String> getColumns() {
        return getStrList(tableKey(KEY_COLUMNS));
    }

    public void setColumns(List<String> columns) {
        setStrList(tableKey(KEY_COLUMNS), columns);
    }

    public List<String> getSorting() {
        return getStrList(tableKey(KEY_SORTING));
    }

    public void setSorting(List<String> columns) {
        setStrList(tableKey(KEY_SORTING), columns);
    }

    public Integer getColumnWidthStep() {
        return get(tableKey(KEY_COL_WIDTH_STEP), DEFAULT_COL_WIDTH_STEP);
    }

    public void setColumnWidthStep(Integer columnWidthStep) {
        set(tableKey(KEY_COL_WIDTH_STEP), columnWidthStep);
    }

    public Integer getColumnWidth(String name) {
        return get(columnKey(KEYP_COL_WIDTH, name), DEFAULT_COL_WIDTH);
    }

    public void setColumnWidth(String name, Integer columnWidth) {
        set(columnKey(KEYP_COL_WIDTH, name), columnWidth);
    }

    public boolean getColumnSortAsc(String name) {
        return get(columnKey(KEYP_COL_SORT_ASC, name), DEFAULT_COL_SORT_ASC);
    }

    public void setColumnSortAsc(String name, boolean asc) {
        set(columnKey(KEYP_COL_SORT_ASC, name), asc);
    }

    public Boolean getColumnNumberGrouping(String name) {
        return get(columnKey(KEYP_COL_NUMBER_GROUPING, name), true);
    }

    public Integer getColumnNumberMinFractionDigits(String name) {
        return get(columnKey(KEYP_COL_NUMBER_MIN_FRAC_DIGITS, name), DEFAULT_COL_NUMBER_MIN_FRAC_DIGITS);
    }

    public Integer getColumnNumberMaxFractionDigits(String name) {
        return get(columnKey(KEYP_COL_NUMBER_MAX_FRAC_DIGITS, name), DEFAULT_COL_NUMBER_MAX_FRAC_DIGITS);
    }

    public String getColumnNumberPattern(String name) {
        return get(columnKey(KEYP_COL_NUMBER_PATTERN, name), DEFAULT_COL_NUMBER_PATTERN);
    }
}

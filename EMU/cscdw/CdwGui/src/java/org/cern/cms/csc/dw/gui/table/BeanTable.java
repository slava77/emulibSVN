package org.cern.cms.csc.dw.gui.table;

import com.icesoft.faces.component.panelpositioned.PanelPositionedEvent;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.UUID;
import javax.ejb.EJB;
import javax.faces.event.ActionEvent;
import javax.faces.model.DataModel;
import javax.faces.model.ListDataModel;
import javax.faces.model.SelectItem;
import org.apache.commons.beanutils.PropertyUtils;
import org.cern.cms.csc.dw.dao.BeanTableDaoLocal;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterIf;
import org.cern.cms.csc.dw.dao.table.BeanTableIf;
import org.cern.cms.csc.dw.dao.table.BeanTableSortColumnIf;
import org.cern.cms.csc.dw.gui.table.column.filter.TableFilter;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.util.ServiceLocator;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

@EJB(name="BeanTableDaoRef", beanInterface=BeanTableDaoLocal.class)
public class BeanTable implements BeanTableIf {

    private static final int DEFAULT_PAGE_SIZE = 20;

    private final String uid = ("uid").concat(UUID.randomUUID().toString());

    private final Class<? extends EntityBase> rowClass;
    private final EntityBase rowBase;
    private final String title;

    private DataModel data;
    private DataModel columns;

    private List<BeanTableSelectColumn> selectedColumns = new ArrayList<BeanTableSelectColumn>();
    private List<BeanTableSelectColumn> availableColumns = new ArrayList<BeanTableSelectColumn>();

    private List<BeanTableSortColumnIf> sortByColumns = new ArrayList<BeanTableSortColumnIf>();
    private List<BeanTableSortColumnIf> availableSortByColumns = new ArrayList<BeanTableSortColumnIf>();

    private Map<String, BeanTableFilterIf> filter = new HashMap<String, BeanTableFilterIf>();
    private BeanTableDaoLocal tableDao;

    private Long dataCount;
    private boolean interactiveMode = true;
    private int pageIndex = 1;
    private int sortPhase = 0;
    private String sortBy = "";
    private int pageSize = DEFAULT_PAGE_SIZE;

    public BeanTable(String title, Class<? extends EntityBase> rowClass, boolean interactiveMode) throws Exception {
        this.title = title;
        this.interactiveMode = interactiveMode;
        this.rowClass = rowClass;
        this.rowBase = rowClass.newInstance();
        this.tableDao = (BeanTableDaoLocal) ServiceLocator.getInstance().getEnvService("BeanTableDaoRef");

        for (PropertyMd pmd: rowBase.getPropertyMetadata()) {
            selectedColumns.add(new BeanTableSelectColumn(pmd));
        }

        columns = new ListDataModel(selectedColumns);

        resetSortByColumns();

        refresh();
    }

    public BeanTable(String title, Class<? extends EntityBase> rowClass) throws Exception {
        this(title, rowClass, true);
    }

    public String getUid() {
        return uid;
    }

    public void refreshListener(ActionEvent e) {
        refresh();
    }

    public List getDataList(boolean pageOnly) {

        List list = tableDao.getData(this, pageOnly);
        if (pageOnly) {
            dataCount = tableDao.getDataCount(this);
        }

        return list;

    }

    @SuppressWarnings("unchecked")
    public void refresh() {
        data = new ListDataModel(getDataList(true));
    }

    public List<PropertyMd> getPropertyMds() throws InvalidEntityBeanPropertyException {
        return this.rowBase.getPropertyMetadata();
    }

    public String getTitle() {
        return title;
    }

    public DataModel getData() {
        return data;
    }

    public DataModel getColumns() {
        return columns;
    }

    public Long getDataCount() {
        return dataCount;
    }

    public int getPageSize() {
        return this.pageSize;
    }

    public void setPageSize(int pageSize) {
        this.pageSize = pageSize;
        setPageIndex(1);
    }

    public int getDataSize() {
        return getData().getRowCount();
    }

    public int getDataFirstIndex() {
        return ((getPageIndex() - 1) * getPageSize()) + 1;
    }

    public int getDataLastIndex() {
        return getDataFirstIndex() + getDataSize() - 1;
    }

    public int getPageCount() {
        return (int) Math.ceil((1.0 * getDataCount()) / (1.0 * getPageSize()));
    }

    public int getPageIndex() {
        return pageIndex;
    }

    public void setPageIndex(int pageIndex) {
        if (pageIndex > 0 && pageIndex <= getPageCount()) {
            this.pageIndex = pageIndex;
            refresh();
        }
    }

    public SelectItem[] getPageSizeItems() {
        SelectItem[] sizes =
                new SelectItem[
                    (BeanTableController.getPageSizeMax() -
                     BeanTableController.getPageSizeMin() +
                     BeanTableController.getPageSizeStep()) /
                     BeanTableController.getPageSizeStep()];
        int r = 0;
        for (int i = BeanTableController.getPageSizeMin(); 
                 i <= BeanTableController.getPageSizeMax();
                 i += BeanTableController.getPageSizeStep()) {
            sizes[r++] = new SelectItem(i, String.valueOf(i));
        }
        return sizes;
    }

    public void firstPageListener(ActionEvent e) {
        if (getPageIndex() > 1) {
            setPageIndex(1);
        }
    }

    public void nextPageListener(ActionEvent e) {
        if (getPageIndex() < getPageCount()) {
            setPageIndex(getPageIndex() + 1);
        } else {
            setPageIndex(getPageCount());
        }
    }

    public void fastforwardPageListener(ActionEvent e) {
        if (getPageIndex() < getPageCount()) {
            if (getPageIndex() + BeanTableController.getFastStep() < getPageCount()) {
                setPageIndex(getPageIndex() + BeanTableController.getFastStep());
            } else {
                setPageIndex(getPageCount());
            }
        } else {
            setPageIndex(getPageCount());
        }
    }

    public void fastbackwardPageListener(ActionEvent e) {
        if (getPageIndex() > 1) {
            if (getPageIndex() - BeanTableController.getFastStep() > 1) {
                setPageIndex(getPageIndex() - BeanTableController.getFastStep());
            } else {
                setPageIndex(1);
            }
        }
        if (getPageIndex() > getPageCount()) {
            setPageIndex(getPageCount());
        }

    }

    public void prevPageListener(ActionEvent e) {
        if (getPageIndex() > 1) {
            setPageIndex(getPageIndex() - 1);
        }
        if (getPageIndex() > getPageCount()) {
            setPageIndex(getPageCount());
        }
    }

    public void lastPageListener(ActionEvent e) {
        if (getPageIndex() != getPageCount()) {
            setPageIndex(getPageCount());
        }
    }

    public Map<String, BeanTableFilterIf> getFilter() {
        return filter;
    }

    public void filterListener(ActionEvent e) {
        refresh();
        firstPageListener(null);
    }

    public boolean isFilterOn() {
        for (Iterator<BeanTableFilterIf> filterItr = filter.values().iterator(); filterItr.hasNext();) {
            TableFilter f = (TableFilter) filterItr.next();
            if (f.getItems().size() > 0 && !f.isDefault()) {
                return true;
            }
        }
        return false;
    }

    public void removeFilterListener(ActionEvent e) {
        removeFilter();
    }

    public void removeFilter() {
        filter.clear();
        refresh();
    }

    public Object getCurrentRow() {
        if (data.isRowAvailable()) {
            return data.getRowData();
        }
        return null;
    }

    public BeanTableSelectColumn getCurrentColumn() {
        if (columns.isRowAvailable()) {
            return (BeanTableSelectColumn) columns.getRowData();
        }
        return null;
    }

    public Object getCellValue() {
        if (data.isRowAvailable() && columns.isRowAvailable()) {

            BeanTableSelectColumn col = getCurrentColumn();
            try {
                return PropertyUtils.getSimpleProperty(getCurrentRow(), col.getPropertyMd().getName());
            } catch (Exception ex) {
                ex.printStackTrace(System.err);
            }
        }
        return null;
    }

    public boolean isInteractiveMode() {
        return interactiveMode;
    }

    public String getAdvancedQuery() {
        return null;
    }

    public boolean isAdvancedQuerySet() {
        return false;
    }

    /**
     * Column widths
     */

    public String getColumnWidths() {
        StringBuilder sb = new StringBuilder("");
        for (BeanTableSelectColumn sc: selectedColumns) {
            Integer w = sc.getWidth();
            if (w == null) {
                return null;
            }
            if (sb.length() != 0) {
                sb.append(",");
            }
            sb.append(sc.getWidth());
        }
        return sb.toString();
    }

    public void setColumnWidths(String colWidths) {
        StringTokenizer tok = new StringTokenizer(colWidths, ",");
        for (BeanTableSelectColumn sc: selectedColumns) {
            if (tok.hasMoreTokens()) {
                sc.setWidth(Integer.parseInt(tok.nextToken()));
            } else {
                return;
            }
        }
    }

    public boolean isSetColumnWidth() {
        for (BeanTableSelectColumn sc: selectedColumns) {
            if (sc.getWidth() == null) {
                return true;
            }
        }
        return false;
    }

    public void columnWidthListener(ActionEvent ev) {
        String colName = (String) ev.getComponent().getAttributes().get("colName");
        Integer step = ((Long) ev.getComponent().getAttributes().get("step")).intValue();
        if (colName != null) {
            BeanTableSelectColumn prev = null;
            boolean found = false, processed = false;
            for (BeanTableSelectColumn sc: selectedColumns) {
                if (found) {
                    sc.setWidth(sc.getWidth() + ((-1) * step));
                    if (sc.getWidth() <= 0) {
                        sc.setWidth(0);
                    } else {
                        processed = true;
                        break;
                    }
                } else {
                    if (sc.getPropertyMd().getName().equals(colName)) {
                        sc.setWidth(sc.getWidth() + step);
                        if (sc.getWidth() <= 0) {
                            sc.setWidth(0);
                            processed = true;
                            break;
                        }
                        found = true;
                    } else {
                        prev = sc;
                    }
                }
            }
            if (!processed && prev != null) {
                prev.setWidth(prev.getWidth() + ((-1) * step));
                if (prev.getWidth() < 0) {
                    prev.setWidth(1);
                }
            }
        }
    }

    public void resetColumnWidthListener(ActionEvent ev) {
        for (BeanTableSelectColumn sc: selectedColumns) {
            sc.setWidth(null);
        }
        for (BeanTableSelectColumn sc: availableColumns) {
            sc.setWidth(null);
        }
    }

    /**
     * Columns
     */

    public List getSelectedColumns() {
        return selectedColumns;
    }

    public void applySelectedColumnsListener(PanelPositionedEvent event) {
        StringBuilder sb = new StringBuilder();
        for (BeanTableSelectColumn sc: this.selectedColumns) {
            if (sb.length() > 0) {
                sb.append(",");
            }
            sb.append(sc.getPropertyMd().getName());
        }
        //getAppSession().set(metadata.getSelectedColumnsKey(), sb.toString());
        //System.out.println(getAppSession().get(metadata.getSelectedColumnsKey()));
    }

    public List getAvailableColumns() {
        return availableColumns;
    }

    public void setAvailableColumns(List availableColumns) {
        this.availableColumns = availableColumns;
    }

    /**
     * Sorting
     */

    public List<BeanTableSortColumnIf> getAvailableSortByColumns() {
        return availableSortByColumns;
    }

    public void setAvailableSortByColumns(List<BeanTableSortColumnIf> availableSortByColumns) {
        this.availableSortByColumns = availableSortByColumns;
    }

    public List<BeanTableSortColumnIf> getSortByColumns() {
        return sortByColumns;
    }

    public void setSortByColumns(List<BeanTableSortColumnIf> sortByColumns) {
        this.sortByColumns = sortByColumns;
    }

    private void setSortBy(String sortBy, boolean skipPhase) {
        this.sortBy = sortBy;
        sortPhase++;
        if (sortPhase > 1 || skipPhase) {
            if (!skipPhase) {
                resetSortByColumns();
            }
            refresh();
            sortPhase = 0;
        }
    }

    private String getSortBy() {
        return this.sortBy;
    }

    public void setAscending(boolean ascending_) throws InvalidEntityBeanPropertyException {
        setSortBy(getSortColumn()
                + " "
                + (ascending_ ? BeanTableSortColumn.SORT_DIR.ASCENDING.getValue() :BeanTableSortColumn.SORT_DIR.DESCENDING.getValue()),
                false);
    }

    public boolean isAscending() {
        String ascending = "";
        if (getSortBy() == null) {
            return true;
        }
        StringTokenizer tokenizer = new StringTokenizer(getSortBy(), " ,");
        if (tokenizer.countTokens() == 2) {
            tokenizer.nextToken();
            ascending = tokenizer.nextToken();
        }
        return (ascending.equals(BeanTableSortColumn.SORT_DIR.ASCENDING.getValue()));
    }

    public void setSortColumn(String sortColumn) {
        setSortBy(sortColumn
                + " "
                + (isAscending() ? BeanTableSortColumn.SORT_DIR.ASCENDING.getValue() : BeanTableSortColumn.SORT_DIR.DESCENDING.getValue()),
                false);
    }

    public String getSortColumn() {
        String sortColumn = "";
        StringTokenizer tokenizer = new StringTokenizer(getSortBy(), " ,");
        if (tokenizer.countTokens() == 2) {
            sortColumn = tokenizer.nextToken();
        }
        return sortColumn;
    }

    public void applySortListener(ActionEvent ev) {
        StringBuilder sb = new StringBuilder("");
        for (BeanTableSortColumnIf sci: sortByColumns) {
            BeanTableSortColumn sc = (BeanTableSortColumn) sci;
            if (sb.length() > 0) {
                sb.append(",");
            }
            sb.append(sc.getPropertyName());
            sb.append(" ");
            sb.append(sc.getDir().getValue());
        }
        setSortBy(sb.toString(), true);
    }

    public void cancelSortListener(ActionEvent ev) throws InvalidEntityBeanPropertyException {
        resetSortByColumns();
    }

    private void resetSortByColumns() {

        sortByColumns.clear();
        availableSortByColumns.clear();

        if (getSortBy() != null) {
            StringTokenizer colTok = new StringTokenizer(getSortBy(), ",");
            while(colTok.hasMoreTokens()) {
                try {
                    sortByColumns.add(new BeanTableSortColumn(this, colTok.nextToken()));
                } catch (InvalidEntityBeanPropertyException ex) {
                    Logger.getLogger(BeanTable.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
        try {
            for (PropertyMd pmd : getPropertyMds()) {
                boolean found = false;
                for (BeanTableSortColumnIf sci : sortByColumns) {
                    BeanTableSortColumn sc = (BeanTableSortColumn) sci;
                    if (sc.getPropertyMd().equals(pmd)) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    availableSortByColumns.add(new BeanTableSortColumn(pmd));
                }
            }
        } catch (InvalidEntityBeanPropertyException ex) {
            Logger.getLogger(BeanTable.class.getName()).log(Level.SEVERE, null, ex);
        }

    }

    public Class<? extends EntityBase> getRowClass() {
        return rowClass;
    }

}

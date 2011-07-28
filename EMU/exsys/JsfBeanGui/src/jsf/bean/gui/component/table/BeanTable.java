package jsf.bean.gui.component.table;

import java.util.logging.Level;
import jsf.bean.gui.component.table.column.BeanTableColumn;
import jsf.bean.gui.component.table.column.BeanTableColumnFactory;
import com.icesoft.faces.component.panelpositioned.PanelPositionedEvent;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.SortedSet;
import java.util.StringTokenizer;
import java.util.TreeSet;
import java.util.logging.Logger;
import javax.faces.event.ActionEvent;
import javax.faces.event.ValueChangeEvent;
import javax.faces.model.DataModel;
import javax.faces.model.ListDataModel;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.column.BeanTableColumnSortable;
import jsf.bean.gui.component.table.column.BeanTableQueryColumn;
import jsf.bean.gui.component.table.export.BeanTableExportTemplateList;
import jsf.bean.gui.metadata.PropertyMd;

public class BeanTable extends BeanTableControls {

    private static final Logger logger = Logger.getLogger(BeanTable.class.getName());
    private static final Integer DEFAULT_NEW_COL_WIDTH = 50;

    private final BeanTablePack pack;
    private final Class<? extends EntityBeanBase> rowClass;
    private final BeanTableExportTemplateList exportTemplateList;

    private DataModel<EntityBeanBase> data;
    private Long dataCount;

    private List<BeanTableColumn> columns = new LinkedList<BeanTableColumn>();
    private DualList<BeanTableColumn> selectedColumns = new DualList<BeanTableColumn>();
    private DualList<BeanTableColumnSortable> sortingColumns = new DualList<BeanTableColumnSortable>();

    @SuppressWarnings("unchecked")
    public BeanTable(BeanTableProperties properties, BeanTablePack pack, Class<? extends EntityBeanBase> rowClass) throws Exception {
        super(properties);

        this.pack = pack;
        this.rowClass = rowClass;
        this.exportTemplateList = new BeanTableExportTemplateList(this);

        for (PropertyMd pmd: this.rowClass.newInstance().getPropertyMetadata()) {
            BeanTableColumn col = BeanTableColumnFactory.getBeanTableColumn(this, pmd);
            this.columns.add(col);
        }

        /*
         * Set selected columns
         */

        for (String cname: getProperties().getColumns()) {
            for (BeanTableColumn col: this.columns) {
                if (col.getName().equals(cname)) {
                    this.selectedColumns.getTarget().add(col);
                    break;
                }
            }
        }

        if (this.selectedColumns.getTarget().isEmpty()) {
            this.selectedColumns.getTarget().addAll(columns);
            this.selectedColumns.getSource().clear();
        } else {
            this.selectedColumns.setSourceExceptTarget(columns);
        }

        /*
         * Set sorting columns
         */

        for (String cname: getProperties().getSorting()) {
            for (BeanTableColumn col: this.columns) {
                if (col.getName().equals(cname)) {
                    if (col instanceof BeanTableColumnSortable && col.isSortable()) {
                        BeanTableColumnSortable scol = (BeanTableColumnSortable) col;
                        this.sortingColumns.getTarget().add(scol);
                    }
                    break;
                }
            }
        }

        for (BeanTableColumn col: this.columns) {
            if (col instanceof BeanTableColumnSortable && col.isSortable()) {
                BeanTableColumnSortable scol = (BeanTableColumnSortable) col;
                if (!this.sortingColumns.getTarget().contains(scol)) {
                    this.sortingColumns.getSource().add(scol);
                }
            }
        }

    }

    /*********************************************
     * 
     * General getters
     *
     *********************************************/

    public BeanTablePack getPack() {
        return pack;
    }

    public Class<? extends EntityBeanBase> getRowClass() {
        return rowClass;
    }

    public DualList<BeanTableColumnSortable> getSortingColumns() {
        return sortingColumns;
    }

    public DualList<BeanTableColumn> getSelectedColumns() {
        return selectedColumns;
    }

    public List<BeanTableColumn> getColumns() {
        return columns;
    }

    public BeanTableColumn getColumn(String name) {
        for (BeanTableColumn c: columns) {
            if (c.getName().equals(name)) {
                return c;
            }
        }
        return null;
    }

    public void refreshListener(ActionEvent e) {
        refresh();
    }

    public void filterOperationChangeListener(ValueChangeEvent e) {
        String[] split = ((String) e.getNewValue()).split(":");
        String colName = split[0];
        String opName = split[1];

        if (opName != null && colName != null) {
            for (BeanTableColumn c: getColumns()) {
                if (c.getName().equals(colName)) {
                    ((BeanTableProjectionFilter) c.getFilter()).setOperation(opName);
                    refresh();
                    return;
                }
            }
        }
    }

    public void setRefresh(boolean refresh) {
        refresh();
    }

    @SuppressWarnings("unchecked")
    public void refresh() {
        BeanTableDaoIf dao = pack.getManager().getBeanTableDao();
        this.data = new ListDataModel<EntityBeanBase>(dao.getData(this));
        this.dataCount = dao.getDataCount((BeanTable) this);
        this.pack.getManager().clearSelected();
    }

    public DataModel<EntityBeanBase> getData() {
        if (data == null) {
            refresh();
        }
        return data;
    }

    @Override
    public Long getDataCount() {
        if (data == null) {
            refresh();
        }
        return dataCount;
    }

    public EntityBeanBase getCurrentRow() {
        if (data != null) {
            if (data.isRowAvailable()) {
                return data.getRowData();
            }
        }
        return null;
    }

    @Override
    public int getDataSize() {
        return getData().getRowCount();
    }

    public int getLiveRefreshInterval() {
        return getProperties().getLiveRefreshInterval();
    }

    public void setLiveRefreshInterval(int liveRefreshInterval) {
        getProperties().setLiveRefreshInterval(liveRefreshInterval);
    }

    public BeanTableExportTemplateList getExportTemplateList() {
        return exportTemplateList;
    }

    /*********************************************
     * 
     * Filter
     *
     *********************************************/

    public void filterListener(ActionEvent e) {
        if (getPageIndex() == 1) {
            refresh();
        } else {
            setPageIndex(1);
        }
    }

    public boolean isFilterOn() {
        for (BeanTableColumn c: columns) {
            if (c.isFilterSet()) {
                return true;
            }
        }
        return false;
    }

    public void removeFilterListener(ActionEvent e) {
        removeFilter();
        filterListener(e);
    }

    public void removeFilter() {
        for (BeanTableColumn c: columns) {
            c.clearFilter();
        }
        refresh();
    }

    /*********************************************
     * 
     * Column widths
     * 
     *********************************************/

    public String getColumnWidths() {
        StringBuilder sb = new StringBuilder("");
        for (BeanTableColumn sc: selectedColumns.getTarget()) {
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
        for (BeanTableColumn sc: selectedColumns.getTarget()) {
            if (tok.hasMoreTokens()) {
                sc.setWidth(Integer.parseInt(tok.nextToken()));
            } else {
                return;
            }
        }
    }

    public boolean isSetColumnWidth() {
        for (BeanTableColumn c: selectedColumns.getTarget()) {
            if (c.getWidth() == null) {
                return true;
            }
        }
        return false;
    }

    public void resetColumnWidthListener(ActionEvent ev) {
        for (BeanTableColumn sc: columns) {
            sc.setWidth(null);
        }
    }

    /*********************************************
     *
     * Column listeners
     *
     *********************************************/

    public void columnsChangeListener(PanelPositionedEvent ev) {
        List<String> cols = new ArrayList<String>();
        for (BeanTableColumn col: selectedColumns.getTarget()) {
            cols.add(col.getName());
            if (col.getWidth() == null) {
                col.setWidth(DEFAULT_NEW_COL_WIDTH);
            }
        }
        getProperties().setColumns(cols);
    }

    public void sortingChangeListener(PanelPositionedEvent ev) {
        List<String> cols = new ArrayList<String>();
        for (BeanTableColumn col: sortingColumns.getTarget()) {
            cols.add(col.getName());
        }
        getProperties().setSorting(cols);
    }


    @Override
    protected void createSerializedTableFilter() {
         try {
            getProperties().setTableFilter(this.getPack().getSerializedFilter().toString());
        } catch (Exception ex) {
            logger.log(Level.SEVERE, ex.toString());
        }
    }

    /**
     * 
     * Columns metadata
     * 
     */

    private static Map<Class, Collection<BeanTableQueryColumn>> queryColumnCache 
                        = new HashMap<Class, Collection<BeanTableQueryColumn>>();
    
    public Collection<BeanTableQueryColumn> getQueryColumns() {
        if (!queryColumnCache.containsKey(this.rowClass)) {
            SortedSet<BeanTableQueryColumn> cols = new TreeSet<BeanTableQueryColumn>();
            for (BeanTableColumn col : getColumns()) {
                cols.addAll(col.getQueryColumns());
            }
            queryColumnCache.put(this.rowClass, cols);
        }
        return queryColumnCache.get(this.rowClass);
    }

}

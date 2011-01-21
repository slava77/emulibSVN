package org.cern.cms.csc.exsys.gui.component;

import org.cern.cms.csc.dw.dao.table.DualList;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;
import java.util.logging.Logger;
import javax.faces.event.ActionEvent;
import javax.faces.model.DataModel;
import javax.faces.model.ListDataModel;
import org.cern.cms.csc.dw.dao.table.BeanTableColumnIf;
import org.cern.cms.csc.dw.dao.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.component.table.BeanTableColumn;
import org.cern.cms.csc.exsys.gui.component.table.BeanTableControls;

public abstract class BeanTable extends BeanTableControls {

    private static final Logger logger = Logger.getLogger(BeanTable.class.getName());

    private final Class<? extends EntityBase> rowClass;
    private final String title;

    private DataModel<EntityBase> data;
    private Long dataCount;
    private Integer widthChangeStep;
    private BeanTable filterTable = null;

    private List<BeanTableColumnIf> columns = new LinkedList<BeanTableColumnIf>();
    private DualList<BeanTableColumnIf> selectedColumns = new DualList<BeanTableColumnIf>();
    private DualList<BeanTableColumnIf> sortingColumns = new DualList<BeanTableColumnIf>();

    abstract public BeanTableDaoIf getBeanTableDao();

    public BeanTable(String title, Class<? extends EntityBase> rowClass) throws Exception {
        this(title, rowClass, false, 0, 0, new int[0]);
    }

    public BeanTable(String title, Class<? extends EntityBase> rowClass, boolean filtering) throws Exception {
        this(title, rowClass, filtering, 0, 0, new int[0]);
    }

    public BeanTable(String title, Class<? extends EntityBase> rowClass, int pageSize, int pageFastStep, int[] pageSizes) throws Exception {
        this(title, rowClass, false, pageSize, pageFastStep, pageSizes);
    }

    @SuppressWarnings("unchecked")
    public BeanTable(String title, Class<? extends EntityBase> rowClass, boolean filtering, int pageSize, int pageFastStep, int[] pageSizes) throws Exception {
        super(pageSize, pageFastStep, pageSizes);

        this.title = title;
        this.rowClass = rowClass;

        for (PropertyMd pmd: this.rowClass.newInstance().getPropertyMetadata()) {
            BeanTableColumn c = new BeanTableColumn(this, pmd);

            if (!filtering || (!c.isEntityType() && !c.isListType())) {
                this.columns.add(c);
            }

            if (!c.isEntityType() && !c.isListType()) {
                this.sortingColumns.getSource().add(c);
            }

        }

        this.selectedColumns.setTarget(columns);
        this.widthChangeStep = 5;
    }

    /*********************************************
     * 
     * General getters
     *
     *********************************************/

    public String getTitle() {
        return title;
    }

    @Override
    public Class<? extends EntityBase> getRowClass() {
        return rowClass;
    }

    @Override
    public DualList<BeanTableColumnIf> getSortingColumns() {
        return sortingColumns;
    }

    public DualList<BeanTableColumnIf> getSelectedColumns() {
        return selectedColumns;
    }

    @Override
    public List<BeanTableColumnIf> getColumns() {
        return columns;
    }

    public void refreshListener(ActionEvent e) {
        refresh();
    }

    @Override
    @SuppressWarnings("unchecked")
    protected void refresh() {
        BeanTableDaoIf dao = getBeanTableDao();
        this.data = new ListDataModel<EntityBase>(dao.getData(this));
        this.dataCount = dao.getDataCount((BeanTable) this);
    }

    @Override
    public DataModel<EntityBase> getData() {
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

    public EntityBase getCurrentRow() {
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

    /*********************************************
     * 
     * Filter
     *
     *********************************************/

    public void filterListener(ActionEvent e) {
        refresh();
    }

    @Override
    public boolean isFilterOn() {
        for (BeanTableColumnIf c: columns) {
            if (c.isFilterSet()) {
                return true;
            }
        }
        return false;
    }

    public void removeFilterListener(ActionEvent e) {
        removeFilter();
    }

    public void removeFilter() {
        for (BeanTableColumnIf c: columns) {
            ((BeanTableColumn) c).clearFilterListener(null);
        }
    }

    public String getAdvancedQuery() {
        return null;
    }

    @Override
    public boolean isAdvancedQuerySet() {
        return false;
    }

    /*********************************************
     * 
     * Column widths
     * 
     *********************************************/

    public String getColumnWidths() {
        StringBuilder sb = new StringBuilder("");
        for (BeanTableColumnIf sci: selectedColumns.getTarget()) {
            BeanTableColumn sc = (BeanTableColumn) sci;
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
        for (BeanTableColumnIf sc: selectedColumns.getTarget()) {
            if (tok.hasMoreTokens()) {
                ((BeanTableColumn) sc).setWidth(Integer.parseInt(tok.nextToken()));
            } else {
                return;
            }
        }
    }

    public boolean isSetColumnWidth() {
        for (BeanTableColumnIf c: selectedColumns.getTarget()) {
            if (((BeanTableColumn) c).getWidth() == null) {
                return true;
            }
        }
        return false;
    }

    public void resetColumnWidthListener(ActionEvent ev) {
        for (BeanTableColumnIf sc: columns) {
            ((BeanTableColumn) sc).setWidth(null);
        }
    }

    public Integer getWidthChangeStep() {
        return widthChangeStep;
    }

    public void setWidthChangeStep(Integer widthChangeStep) {
        this.widthChangeStep = widthChangeStep;
    }

    public BeanTable getFilterTable() {
        return filterTable;
    }

    public void setFilterTable(BeanTable filterTable) {
        this.filterTable = filterTable;
    }

}

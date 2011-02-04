package org.cern.cms.csc.exsys.gui.component.table;

import com.icesoft.faces.component.panelpositioned.PanelPositionedEvent;
import java.util.ArrayList;
import org.cern.cms.csc.dw.dao.table.DualList;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;
import java.util.logging.Logger;
import javax.faces.event.ActionEvent;
import javax.faces.event.ValueChangeEvent;
import javax.faces.model.DataModel;
import javax.faces.model.ListDataModel;
import org.cern.cms.csc.dw.dao.table.BeanTableColumnIf;
import org.cern.cms.csc.dw.dao.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;

public class BeanTable extends BeanTableControls {

    private static final Logger logger = Logger.getLogger(BeanTable.class.getName());

    private final BeanTablePack pack;
    private final Class<? extends EntityBase> rowClass;

    private DataModel<EntityBase> data;
    private Long dataCount;

    private List<BeanTableColumnIf> columns = new LinkedList<BeanTableColumnIf>();
    private DualList<BeanTableColumnIf> selectedColumns = new DualList<BeanTableColumnIf>();
    private DualList<BeanTableColumnIf> sortingColumns = new DualList<BeanTableColumnIf>();

    @SuppressWarnings("unchecked")
    public BeanTable(BeanTablePack pack, Class<? extends EntityBase> rowClass) throws Exception {
        super(new BeanTableProperties(pack.getManager().getProperties()));

        this.pack = pack;
        this.rowClass = rowClass;

        for (PropertyMd pmd: this.rowClass.newInstance().getPropertyMetadata()) {
            BeanTableColumn c = new BeanTableColumn(this, pmd);
            this.columns.add(c);
        }

        /*
         * Set selected columns
         */

        for (String cname: getProperties().getColumns()) {
            for (BeanTableColumnIf col: this.columns) {
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
            for (BeanTableColumnIf col: this.columns) {
                if (col.getName().equals(cname)) {
                    if (!((BeanTableColumn) col).isEntityType() && !((BeanTableColumn) col).isListType()) {
                        this.sortingColumns.getTarget().add(col);
                    }
                    break;
                }
            }
        }

        for (BeanTableColumnIf col: this.columns) {
            if (!this.sortingColumns.getTarget().contains(col)) {
                if (!((BeanTableColumn) col).isEntityType() && !((BeanTableColumn) col).isListType()) {
                    this.sortingColumns.getSource().add(col);
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

    public void filterOperationChangeListener(ValueChangeEvent e) {
        String[] split = ((String) e.getNewValue()).split(":");
        String colName = split[0];
        String opName = split[1];

        if (opName != null && colName != null) {
            for (BeanTableColumnIf c: getColumns()) {
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

    @Override
    @SuppressWarnings("unchecked")
    public void refresh() {
        BeanTableDaoIf dao = pack.getManager().getBeanTableDao();
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
        refresh();
    }

    public void removeFilter() {
        for (BeanTableColumnIf c: columns) {
            ((BeanTableColumn) c).clearFilterListener(null);
        }
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

    /*********************************************
     *
     * Column listeners
     *
     *********************************************/

    public void columnsChangeListener(PanelPositionedEvent ev) {
        List<String> cols = new ArrayList<String>();
        for (BeanTableColumnIf col: selectedColumns.getTarget()) {
            cols.add(col.getName());
        }
        getProperties().setColumns(cols);
    }

    public void sortingChangeListener(PanelPositionedEvent ev) {
        List<String> cols = new ArrayList<String>();
        for (BeanTableColumnIf col: sortingColumns.getTarget()) {
            cols.add(col.getName());
        }
        getProperties().setSorting(cols);
    }

}

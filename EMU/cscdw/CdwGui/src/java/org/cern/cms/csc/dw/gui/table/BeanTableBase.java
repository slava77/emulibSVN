package org.cern.cms.csc.dw.gui.table;

import com.icesoft.faces.component.panelpositioned.PanelPositionedEvent;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;
import javax.faces.event.ActionEvent;
import javax.faces.model.DataModel;
import javax.faces.model.ListDataModel;
import org.cern.cms.csc.dw.dao.table.BeanTableColumnIf;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

public abstract class BeanTableBase extends BeanTableBaseBase {

    private DataModel columns;
    private List<BeanTableColumnIf> allCols = new LinkedList<BeanTableColumnIf>();
    private List<BeanTableColumnIf> selectedCols = new LinkedList<BeanTableColumnIf>();
    private List<BeanTableColumnIf> availableCols = new LinkedList<BeanTableColumnIf>();
    private List<BeanTableColumnIf> sortCols = new LinkedList<BeanTableColumnIf>();
    private List<BeanTableColumnIf> availableSortCols = new LinkedList<BeanTableColumnIf>();

    private int sortPhase = 0;
    private String sortBy = "";

    public BeanTableBase(String title, Class<? extends EntityBase> rowClass, boolean interactiveMode) throws Exception {
        super(title, rowClass, interactiveMode);
        
        for (PropertyMd pmd: rowBase.getPropertyMetadata()) {
            allCols.add(new BeanTableColumn(this, pmd));
        }

        // FIXME: take cols from default or cookie
        selectedCols.addAll(allCols);

        this.columns = new ListDataModel(selectedCols);

        resetSortByColumns();

    }

    public DataModel getColumns() {
        return columns;
    }

    public void filterListener(ActionEvent e) {
        refresh();
        firstPageListener(null);
    }

    public boolean isFilterOn() {
        for (BeanTableColumnIf c: allCols) {
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
        for (BeanTableColumnIf c: allCols) {
            ((BeanTableColumn) c).clearFilter();
        }
    }

    protected BeanTableColumn getCurrentColumn() {
        if (columns.isRowAvailable()) {
            return (BeanTableColumn) columns.getRowData();
        }
        return null;
    }

    protected boolean isColumnAvailable() {
        return columns.isRowAvailable();
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
        for (BeanTableColumnIf sci: selectedCols) {
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
        for (BeanTableColumnIf sc: selectedCols) {
            if (tok.hasMoreTokens()) {
                ((BeanTableColumn) sc).setWidth(Integer.parseInt(tok.nextToken()));
            } else {
                return;
            }
        }
    }

    public boolean isSetColumnWidth() {
        for (BeanTableColumnIf sc: selectedCols) {
            if (((BeanTableColumn) sc).getWidth() == null) {
                return true;
            }
        }
        return false;
    }

    public void columnWidthListener(ActionEvent ev) {
        String colName = (String) ev.getComponent().getAttributes().get("colName");
        Integer step = ((Long) ev.getComponent().getAttributes().get("step")).intValue();
        if (colName != null) {
            BeanTableColumn prev = null;
            boolean found = false, processed = false;
            for (BeanTableColumnIf sci: selectedCols) {
                BeanTableColumn sc = (BeanTableColumn) sci;
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
        for (BeanTableColumnIf sc: allCols) {
            ((BeanTableColumn) sc).setWidth(null);
        }
    }

    /**
     * Columns
     */

    public List getSelectedColumns() {
        return selectedCols;
    }

    public void applySelectedColumnsListener(PanelPositionedEvent event) {
        StringBuilder sb = new StringBuilder();
        for (BeanTableColumnIf sc: this.selectedCols) {
            if (sb.length() > 0) {
                sb.append(",");
            }
            sb.append(sc.getName());
        }
        //getAppSession().set(metadata.getSelectedColumnsKey(), sb.toString());
        //System.out.println(getAppSession().get(metadata.getSelectedColumnsKey()));
    }

    public List getAvailableColumns() {
        return availableCols;
    }

    public void setAvailableColumns(List availableCols) {
        this.availableCols = availableCols;
    }

    public BeanTableColumnIf getColumnByName(String name) {
        for (BeanTableColumnIf c: allCols) {
            if (c.getName().equals(name)) {
                return c;
            }
        }
        return null;
    }

    public List<BeanTableColumnIf> getAllColumns() {
        return allCols;
    }

    /**
     * Sorting
     */

    public List<BeanTableColumnIf> getAvailableSortColumns() {
        return availableSortCols;
    }

    public void setAvailableSortColumns(List<BeanTableColumnIf> availableSortByCols) {
        this.availableSortCols = availableSortByCols;
    }

    public List<BeanTableColumnIf> getSortColumns() {
        return sortCols;
    }

    public void setSortColumns(List<BeanTableColumnIf> sortCols) {
        this.sortCols = sortCols;
    }

    private void setSortBy(String sortBy, boolean skipPhase) {
        this.sortBy = sortBy;
        this.sortPhase++;
        if (this.sortPhase > 1 || skipPhase) {
            if (!skipPhase) {
                resetSortByColumns();
            }
            refresh();
            this.sortPhase = 0;
        }
    }

    private String getSortBy() {
        return this.sortBy;
    }

    public void setAscending(boolean ascending) throws InvalidEntityBeanPropertyException {
        setSortBy(getSortColumn()
                + " "
                + (ascending ? BeanTableColumn.SORT_DIR.ASCENDING.getValue() : BeanTableColumn.SORT_DIR.DESCENDING.getValue()),
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
        return (ascending.equals(BeanTableColumn.SORT_DIR.ASCENDING.getValue()));
    }

    public void setSortColumn(String sortColumn) {
        setSortBy(sortColumn
                + " "
                + (isAscending() ? BeanTableColumn.SORT_DIR.ASCENDING.getValue() : BeanTableColumn.SORT_DIR.DESCENDING.getValue()),
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
        for (BeanTableColumnIf sci: sortCols) {
            BeanTableColumn sc = (BeanTableColumn) sci;
            if (sb.length() > 0) {
                sb.append(",");
            }
            sb.append(sc.getName());
            sb.append(" ");
            sb.append(sc.getDir().getValue());
        }
        setSortBy(sb.toString(), true);
    }

    public void cancelSortListener(ActionEvent ev) throws InvalidEntityBeanPropertyException {
        resetSortByColumns();
    }

    private void resetSortByColumns() {

        sortCols.clear();
        availableSortCols.clear();

        if (getSortBy() != null) {
            StringTokenizer colTok = new StringTokenizer(getSortBy(), " ,");
            while(colTok.hasMoreTokens()) {
                BeanTableColumnIf c = getColumnByName(colTok.nextToken());
                if (c != null) {
                    if (c.getType() == BeanTableColumnIf.TYPE.SIMPLE) {
                        ((BeanTableColumn) c).setAscending(BeanTableColumnIf.SORT_DIR.ASCENDING.getValue().equals(colTok.nextToken()));
                        sortCols.add(c);
                    }
                }
            }
        }

        for (BeanTableColumnIf c: allCols) {
            if (!sortCols.contains(c) && c.getType() == BeanTableColumnIf.TYPE.SIMPLE) {
                availableSortCols.add(c);
            }
        }

    }

}

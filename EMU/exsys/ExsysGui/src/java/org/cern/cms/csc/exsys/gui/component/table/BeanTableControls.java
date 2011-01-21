package org.cern.cms.csc.exsys.gui.component.table;

import javax.faces.event.ActionEvent;
import javax.faces.model.DataModel;
import javax.faces.model.SelectItem;
import org.cern.cms.csc.dw.dao.table.BeanTableIf;
import org.cern.cms.csc.dw.model.base.EntityBase;

public abstract class BeanTableControls implements BeanTableIf {

    private int pageSize;
    private int pageIndex = 1;
    private final int pageFastStep;
    private final SelectItem[] pageSizeItems;
    private boolean displayFilter = false;
    private boolean showSelectColumns = false;

    public abstract DataModel<EntityBase> getData();
    public abstract Long getDataCount();
    public abstract int getDataSize();
    protected abstract void refresh();

    public BeanTableControls(int pageSize, int pageFastStep, int[] pageSizes) {
        this.pageSize = pageSize;
        this.pageFastStep = pageFastStep;
        this.pageSizeItems = new SelectItem[pageSizes.length];
        for (int i = 0; i < pageSizes.length; i++) {
            this.pageSizeItems[i] = new SelectItem(pageSizes[i], String.valueOf(pageSizes[i]));
        }
    }

    /**
     * Paging
     */

    public SelectItem[] getPageSizeItems() {
        return pageSizeItems;
    }

    @Override
    public int getPageSize() {
        return pageSize;
    }

    public void setPageSize(int pageSize) {
        if (this.pageSize != pageSize) {
            this.pageSize = pageSize;
            setPageIndex(1);
        }
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

    @Override
    public int getPageIndex() {
        return pageIndex;
    }

    public void setPageIndex(int pageIndex) {
        if (pageIndex > 0 && pageIndex <= getPageCount()) {
            this.pageIndex = pageIndex;
            refresh();
        }
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
            if (getPageIndex() + pageFastStep < getPageCount()) {
                setPageIndex(getPageIndex() + pageFastStep);
            } else {
                setPageIndex(getPageCount());
            }
        } else {
            setPageIndex(getPageCount());
        }
    }

    public void fastbackwardPageListener(ActionEvent e) {
        if (getPageIndex() > 1) {
            if (getPageIndex() - pageFastStep > 1) {
                setPageIndex(getPageIndex() - pageFastStep);
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

    /**
     * Flags
     */

    public boolean isDisplayFilter() {
        return displayFilter;
    }

    public void setDisplayFilter(boolean displayFilter) {
        this.displayFilter = displayFilter;
    }

    public boolean isShowSelectColumns() {
        return showSelectColumns;
    }

    public void setShowSelectColumns(boolean showSelectColumns) {
        this.showSelectColumns = showSelectColumns;
    }

}

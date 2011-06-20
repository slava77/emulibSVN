package jsf.bean.gui.component.table;

import java.io.Serializable;
import javax.faces.event.ActionEvent;
import javax.faces.model.DataModel;
import jsf.bean.gui.EntityBeanBase;

public abstract class BeanTableControls implements Serializable {

    private BeanTableProperties prop;

    private int pageIndex = 1;

    public abstract DataModel<EntityBeanBase> getData();
    public abstract Long getDataCount();
    public abstract int getDataSize();
    protected abstract void refresh();
    protected abstract void createSerializedTableFilter();

    public BeanTableControls(BeanTableProperties prop) {
        this.prop = prop;
    }

    public BeanTableProperties getProperties() {
        return prop;
    }

    /**
     * Paging
     */

    public int getPageSize() {
        return prop.getPageSize();
    }

    public void setPageSize(int pageSize) {
        if (getPageSize() != pageSize) {
            prop.setPageSize(pageSize);
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
        if (getDataCount() != null) {
            return (int) Math.ceil((1.0 * getDataCount()) / (1.0 * getPageSize()));
        }
        return 0;
    }

    public int getPageIndex() {
        return pageIndex;
    }

    public void setPageIndex(int pageIndex) {
        if (pageIndex > 0 && pageIndex <= getPageCount() && pageIndex != this.pageIndex) {
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
            if (getPageIndex() + prop.getPageFastStep() < getPageCount()) {
                setPageIndex(getPageIndex() + prop.getPageFastStep());
            } else {
                setPageIndex(getPageCount());
            }
        } else {
            setPageIndex(getPageCount());
        }
    }

    public void fastbackwardPageListener(ActionEvent e) {
        if (getPageIndex() > 1) {
            if (getPageIndex() - prop.getPageFastStep() > 1) {
                setPageIndex(getPageIndex() - prop.getPageFastStep());
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

    private boolean displayColumns = false;

    public boolean isDisplayColumns() {
        return displayColumns;
    }

    public void setDisplayColumns(boolean displayColumns) {
        createSerializedTableFilter();
        this.displayColumns = displayColumns;
    }

}

package org.cern.cms.csc.dw.gui.table;

import java.util.List;
import java.util.UUID;
import javax.faces.event.ActionEvent;
import javax.faces.model.DataModel;
import javax.faces.model.ListDataModel;
import javax.faces.model.SelectItem;
import org.apache.commons.beanutils.PropertyUtils;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.dao.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.dao.table.BeanTableIf;
import org.cern.cms.csc.dw.model.base.EntityBase;


public abstract class BeanTableBaseBase implements BeanTableIf {

    private static final Logger logger = Logger.getLogger(BeanTableBaseBase.class);

    private static final int DEFAULT_PAGE_SIZE = 20;
    private static final int PAGE_FAST_STEP = 5;
    private static final int PAGE_SIZE_MIN = 5;
    private static final int PAGE_SIZE_MAX = 100;
    private static final int PAGE_SIZE_STEP = 5;

    private final String uid = ("uid").concat(UUID.randomUUID().toString());

    private final Class<? extends EntityBase> rowClass;
    protected final EntityBase rowBase;
    private final String title;

    private DataModel data;

    private Long dataCount;
    private boolean interactiveMode = true;
    private int pageIndex = 1;
    private int pageSize = DEFAULT_PAGE_SIZE;

    abstract protected BeanTableDaoIf getBeanTableDao();

    public BeanTableBaseBase(String title, Class<? extends EntityBase> rowClass, boolean interactiveMode) throws Exception {
        this.title = title;
        this.interactiveMode = interactiveMode;
        this.rowClass = rowClass;
        this.rowBase = rowClass.newInstance();
    }

    public String getUid() {
        return uid;
    }

    public void refreshListener(ActionEvent e) {
        refresh();
    }

    public List getDataList(boolean pageOnly) {
        List list = getBeanTableDao().getData(this, pageOnly);
        if (pageOnly) {
            dataCount = getBeanTableDao().getDataCount(this);
        }
        return list;
    }

    public void refresh() {
        data = new ListDataModel(getDataList(true));
    }

    public String getTitle() {
        return title;
    }

    public DataModel getData() {
        if (data == null) {
            refresh();
        }
        return data;
    }

    public Long getDataCount() {
        return dataCount;
    }

    public int getPageSize() {
        return this.pageSize;
    }

    public void setPageSize(int pageSize) {
        if (this.pageSize != pageSize) {
            this.pageSize = pageSize;
            setPageIndex(1);
        }
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
                new SelectItem[(PAGE_SIZE_MAX - PAGE_SIZE_MIN + PAGE_SIZE_STEP) / PAGE_SIZE_STEP];
        int r = 0;
        for (int i = PAGE_SIZE_MIN; i <= PAGE_SIZE_MAX; i += PAGE_SIZE_STEP) {
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
            if (getPageIndex() + PAGE_FAST_STEP < getPageCount()) {
                setPageIndex(getPageIndex() + PAGE_FAST_STEP);
            } else {
                setPageIndex(getPageCount());
            }
        } else {
            setPageIndex(getPageCount());
        }
    }

    public void fastbackwardPageListener(ActionEvent e) {
        if (getPageIndex() > 1) {
            if (getPageIndex() - PAGE_FAST_STEP > 1) {
                setPageIndex(getPageIndex() - PAGE_FAST_STEP);
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

    public Object getCurrentRow() {
        if (data.isRowAvailable()) {
            return data.getRowData();
        }
        return null;
    }

    protected abstract BeanTableColumn getCurrentColumn();

    protected abstract boolean isColumnAvailable();

    public Object getCellValue() {
        if (data.isRowAvailable() && isColumnAvailable()) {

            BeanTableColumn col = getCurrentColumn();
            try {
                return PropertyUtils.getSimpleProperty(getCurrentRow(), col.getName());
            } catch (Exception ex) {
                logger.error("Error while retrieving bean value", ex);
            }
        }
        return null;
    }

    public boolean isInteractiveMode() {
        return interactiveMode;
    }

    public Class<? extends EntityBase> getRowClass() {
        return rowClass;
    }

}

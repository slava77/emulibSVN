package jsf.bean.gui.component.table;

import java.io.IOException;
import java.io.Serializable;
import java.util.Enumeration;
import java.util.Properties;
import java.util.regex.Pattern;
import javax.faces.context.FacesContext;
import javax.faces.event.ActionEvent;
import javax.faces.model.DataModel;
import javax.faces.model.SelectItem;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.JsfBeanBase;

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

    public void setFirstPageOrRefresh() {
        if (getPageIndex() > 1) {
            setPageIndex(1);
        } else {
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
    private boolean displayQuery = false;

    public boolean isDisplayColumns() {
        return displayColumns;
    }

    public void setDisplayColumns(boolean displayColumns) {
        createSerializedTableFilter();
        this.displayColumns = displayColumns;
    }

    public boolean isDisplayQuery() {
        return displayQuery;
    }

    public void setDisplayQuery(boolean displayQuery) {
        this.displayQuery = displayQuery;
    }
    
    public boolean isInteractiveMode() {
        return FacesContext.getCurrentInstance() != null;
    }
    
    /** 
     * Advanced Query
     */
    
    private static final Pattern SUBQUERY_PATTERN = Pattern.compile(
                ".*[^a-zA-Z0-9_]select .* from .*", 
                Pattern.CASE_INSENSITIVE | Pattern.MULTILINE);
    
    private String query = null;
    private String appliedQuery = null;

    public String getQuery() {
        return query;
    }
    
    public String getAppliedQuery() {
        return appliedQuery;
    }
    
    public boolean isQueryApplied() {
        return appliedQuery != null;
    }

    public void setQuery(String nquery) {

        // Initial query cleanup
        
        if (nquery != null) {
            nquery = nquery.trim();
            if (nquery.equals("")) {
                nquery = null;
            } else {
                nquery = nquery.replace('"', '\'');
            }
        }

        // Changing stored value
        this.query = nquery;

        if ((query != null && appliedQuery == null) ||
            (query == null && appliedQuery != null) ||
            (query != null && appliedQuery != null && !query.equals(appliedQuery))) {
        
            // Secondary query checkup
            if (query != null && SUBQUERY_PATTERN.matcher(query).matches()) {
                if (isInteractiveMode()) {
                    JsfBeanBase.addErrorMessage("Subqueries are not allowed!");
                    return;
                } else {
                    throw new RuntimeException("Query error: subqueries are not allowed!");
                }
            }

            String prevAppliedQuery = appliedQuery;
            this.appliedQuery = query;

            Exception sqlException = null;

            try {

                refresh();

            } catch (org.hibernate.exception.SQLGrammarException e) {
                sqlException = e;
            } catch (org.hibernate.QueryException e) {
                sqlException = e;
            }

            if (sqlException != null) {

                this.appliedQuery = prevAppliedQuery;
                refresh();

                if (isInteractiveMode()) {
                    JsfBeanBase.addErrorMessage(sqlException);
                } else {
                    throw new RuntimeException(sqlException);
                }

            } else {

                if (isInteractiveMode()) {
                    //addAdvancedQueryHistoryItem(advancedQueryTemp);
                    firstPageListener(null);
                }

            }
        }
        
    }

    private static final SelectItem[] QUERY_FUNCTIONS;
    private static final String QUERY_FUNCTIONS_FILE = "/jsf/bean/gui/component/table/QueryFunctions.properties";
    
    static {
        try {
            Properties funcs = new Properties();
            funcs.load(BeanTableControls.class.getResourceAsStream(QUERY_FUNCTIONS_FILE));
            QUERY_FUNCTIONS = new SelectItem[funcs.size()];
            int i = 0;
            for (Enumeration<Object> keys = funcs.keys(); keys.hasMoreElements(); ) {
                String k = (String) keys.nextElement();
                QUERY_FUNCTIONS[i++] = new SelectItem(funcs.getProperty(k), k.replace('-', ' '));
            }
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }

    public SelectItem[] getQueryFunctions() {
        return QUERY_FUNCTIONS;
    }
    
    /*
    private SortedMap<Date, String> advHistory = new TreeMap<Date, String>();

    public List<String> getAdvancedQueryHistory() {
        List<String> list = new ArrayList<String>();
        for (Date d : advHistory.keySet()) {
            list.add(advHistory.get(d));
        }
        return list;
    }

    private void addAdvancedQueryHistoryItem(String query) {
        if (!advHistory.containsValue(query)) {
            if (advHistory.size() >= 10) {
                advHistory.remove(advHistory.firstKey());
            }
            advHistory.put(GeneralUtil.getCETDate(), query);
        }
    }

    public boolean isSetAdvancedQueryHistory() {
        return (advHistory.size() > 0);
    }
    */

}

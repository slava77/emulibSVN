package jsf.bean.gui.component;

import java.awt.event.ActionEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import javax.faces.application.Application;
import javax.faces.application.ViewHandler;
import javax.faces.component.UIViewRoot;
import javax.faces.context.FacesContext;
import javax.faces.event.FacesEvent;
import javax.persistence.Transient;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.component.table.BeanTableFilter;
import jsf.bean.gui.component.table.BeanTableFilterItem;
import jsf.bean.gui.component.table.BeanTablePack;
import jsf.bean.gui.converter.ClassConverter;
import jsf.bean.gui.converter.NewLineConverter;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

public abstract class BeanTableManager implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableManager.class);

    private BeanTablePack tablePack;
    private List<BeanTablePack> tables = new ArrayList<BeanTablePack>();
    private final String id;

    public abstract BeanTableDaoIf getBeanTableDao();

    public abstract ClassFinderIf getClassFinder();

    public BeanTableManager(String id, Class<? extends EntityBeanBase> rowClass) throws Exception {
        this.id = id;
        this.tablePack = new BeanTablePack(this, rowClass);
    }

    public String getId() {
        return id;
    }

    public List<BeanTablePack> getTables() {
        return tables;
    }

    public BeanTablePack getTablePack() {
        return this.tablePack;
    }

    public BeanTable getTable() {
        return this.tablePack.getTable();
    }

    public BeanTablePack getTopPack() {
        if (!isTop()) {
            return getTables().get(0);
        } else {
            return getTablePack();
        }
    }

    public void pushTable(BeanTablePack nextTable) {
        this.tables.add(this.tablePack);
        this.tablePack = nextTable;
    }

    public void popTable() {
        if (!this.tables.isEmpty()) {
            this.tablePack = this.tables.remove(this.tables.size() - 1);
        }
    }

    public boolean isTop() {
        return this.tables.isEmpty();
    }

    public int getDepth() {
        return this.tables.size();
    }

    public void backActionListener() {
        popTable();
        this.getTable().refresh();
    }

    public void topActionListener() {
        while (!isTop()) {
            popTable();
        }
        this.getTable().refresh();
    }

    public void toTableActionListener() {
        FacesContext context = FacesContext.getCurrentInstance();
        Map map = context.getExternalContext().getRequestParameterMap();
        int tableIndex = Integer.valueOf((String) map.get("tableIndex"));
        while (this.tables.size() != tableIndex) {
            tablePack = tables.get(tables.size()-1);
            this.popTable();
        }
    }

    public String buildTablePrefix(String nextPrefix) {
        String tablePrefix = null;
        if (nextPrefix != null) {
            if (getTable() != null) {
                tablePrefix = getTable().getProperties().getTablePrefix();
            }
            tablePrefix = (tablePrefix != null ? tablePrefix.concat(nextPrefix) : nextPrefix).concat(".");
        }
        return tablePrefix;
    }

    /*********************************************
     *
     * Row selection manager
     *
     *********************************************/

    private EntityBeanBase selected;

    public void rowSelectionListener(FacesEvent event) {

        if (event.isSelected()) {
            Iterator<EntityBeanBase> it = this.getTable().getData().iterator();
            while (it.hasNext()) {
                EntityBeanBase row = it.next();
                if (row.getSelected()) {
                    this.selected = row;
                    break;
                }
            }
        } else {
            this.selected = null;
        }
    }

    public final EntityBeanBase getSelected() {
        return selected;
    }

    public final void setSelected(EntityBeanBase selected) {
        this.selected = selected;
    }

    public final void clearSelected() {
        if (this.selected != null) {
            this.selected = null;
        }
    }


    /*********************************************
     *
     * Bean Table Properties
     *
     *********************************************/

    private static final String PROPERTIES_BASE_PATH = "resources/tables/";
    private static final String PROPERTIES_EXTENSION = ".properties";
    private static final String COOKIE_NAME_PATTERN = "table.%s.properties";

    // Caching default properties
    private static Map<String, Properties> beanProperties = new HashMap<String, Properties>();

    public Properties getProperties() {
        Properties p = new Properties();

        // Loading personal properties
        {
            // Get cookie for the table
            String cookieName = String.format(COOKIE_NAME_PATTERN, id);

            String myId = (String) getCookie(cookieName);

            if (myId != null) {
                File f = getRealFile(PROPERTIES_BASE_PATH.concat(File.separator).concat(myId).concat(PROPERTIES_EXTENSION));
                try {
                    p.load(new FileInputStream(f));
                    return p;
                } catch (Exception ex) {
                    logger.warn("Table [id = {0}] personal properties not found at [{1}]. Reseting and loading defaults...", id, f.getAbsolutePath());
                    setCookie(cookieName, "", 0);
                }
            }
        }

        // Loading default properties
        {
            if (!beanProperties.containsKey(id)) {
                File f = getRealFile(PROPERTIES_BASE_PATH.concat(File.separator).concat(id).concat(PROPERTIES_EXTENSION));
                try {
                    p.load(new FileInputStream(f));
                } catch (Exception ex) {
                    logger.warn("Table [id = {0}] properties not found at [{1}]. Loading defaults...", id, f.getAbsolutePath());
                }
                beanProperties.put(id, p);
            }
        }

        return beanProperties.get(id);

    }

    public void savePersonalPropertiesListener(ActionEvent ev) throws IOException {
        saveProperties(false);
    }

    public void saveGlobalPropertiesListener(ActionEvent ev) throws IOException {
        saveProperties(true);
    }

    private void saveProperties(boolean global) throws IOException {
        if (getTable() != null) {

            String cookieName = String.format(COOKIE_NAME_PATTERN, id);

            // Writting a file
            Properties p = getTable().getProperties().getProperties();
            String filename = id;
            if (!global) {
                filename = (String) getCookie(cookieName);
                if (filename == null) {
                    File f = File.createTempFile(id.concat("."), "", getRealFile(PROPERTIES_BASE_PATH));
                    filename = f.getName();
                    f.delete();
                }
            }
            p.store(new FileOutputStream(
                    getRealFile(PROPERTIES_BASE_PATH.concat(File.separator).concat(filename).concat(PROPERTIES_EXTENSION))), filename);

            // Saving filename in cookie
            if (!global) {
                setCookie(cookieName, filename, 356 * 24 * 60 * 60);
            }

        }
    }

    @Transient
    private void setCookie(String name, String value, int age) {
        Cookie cookie = new Cookie(name, value);
        cookie.setMaxAge(age);
        ((HttpServletResponse) FacesContext.getCurrentInstance()
                .getExternalContext().getResponse()).addCookie(cookie);
    }

    @Transient
    private String getCookie(String name) {
        Cookie cookie[] = ((HttpServletRequest) FacesContext.getCurrentInstance()
                .getExternalContext().getRequest()).getCookies();
        if (cookie != null && cookie.length > 0) {
            for (int i = 0; i < cookie.length; i++) {
                if (cookie[i].getName().equals(name)) {
                    return cookie[i].getValue();
                }
            }
        }
        return null;
    }

    private File getRealFile(String file) {
        String path = FacesContext.getCurrentInstance().getExternalContext().getRealPath(file);
        return new File(path);
    }

    public ClassConverter getClassConverter() {
        return new ClassConverter();
    }

    public NewLineConverter getNewLineConverter() {
        return new NewLineConverter();
    }

    public void addPropertyFilter(String property, BeanTableFilter filter) {
        getTopPack().getPropertyFilters().put(property, filter);
    }

    public void addPropertyFilter(String property, BeanTableFilter.Operation operation, Object value) {
        BeanTableFilter filter = new BeanTableFilter();
        BeanTableFilterItem filterItem = new BeanTableFilterItem();
        filterItem.setOperation(operation);
        filterItem.setValue(value);
        filter.getItems().add(filterItem);
        addPropertyFilter(property, filter);
    }

    public void removePropertyFilter(String property) {
        if (getTopPack().getPropertyFilters().containsKey(property)) {
            getTopPack().getPropertyFilters().remove(property);
        }
    }

    public void removeAllPropertyFilters() {
        getTopPack().getPropertyFilters().clear();
    }

    // TODO
    public void refreshTable() {
        FacesContext context = FacesContext.getCurrentInstance();
        Application application = context.getApplication();
        ViewHandler viewHandler = application.getViewHandler();
        UIViewRoot viewRoot = viewHandler.createView(context, context.getViewRoot().getViewId());
        context.setViewRoot(viewRoot);
        context.renderResponse();
    }

}

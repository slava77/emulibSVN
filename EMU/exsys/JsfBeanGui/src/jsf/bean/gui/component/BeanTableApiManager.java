/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.component.table.api.BeanTableApi;
import jsf.bean.gui.component.table.api.BeanTableApiConfig;
import jsf.bean.gui.component.table.api.BeanTableApiConfigProviderIf;
import jsf.bean.gui.component.table.api.BeanTableApiTemplate;
import jsf.bean.gui.component.table.column.BeanTableQueryColumn;
import jsf.bean.gui.component.table.export.BeanTableExportTemplateProvider;

/**
 *
 * @author valdo
 */
public abstract class BeanTableApiManager {
    
    private final BeanTableApiConfigProviderIf configProvider;
    private Map<String, Collection<BeanTableQueryColumn>> cacheColumns;
    private Map<String, Collection<BeanTableApiTemplate>> cacheTemplates;

    public abstract BeanTableDaoIf getBeanTableDao();
    public abstract ClassFinderIf getClassFinder();
    public abstract BeanTableExportTemplateProvider getTemplateProvider(BeanTable table);
    
    public BeanTableApiManager(BeanTableApiConfigProviderIf configProvider) {
        this.configProvider = configProvider;
        this.cacheColumns = new HashMap<String, Collection<BeanTableQueryColumn>>();
        this.cacheTemplates = new HashMap<String, Collection<BeanTableApiTemplate>>();
    }
    
    public Collection<BeanTableQueryColumn> getColumns(String id) throws Exception {
        if (!cacheColumns.containsKey(id)) {
            cacheColumns.put(id, getApi(id).getColumns());
        }
        return cacheColumns.get(id);
    }
    
    public boolean isColumnExists(String id, String name) throws Exception {
        for (BeanTableQueryColumn col: getColumns(id)) {
            if (col.getName().equals(name)) {
                return true;
            }
        }
        return false;
    }
    
    public boolean isOrderColumnExists(String id, String name) throws Exception {
        for (BeanTableQueryColumn col: getColumns(id)) {
            if (col.getName().equals(name)) {
                return col.isSortable();
            }
        }
        return false;
    }
    
    public Collection<BeanTableApiTemplate> getTemplates(String id) throws Exception {
        if (!cacheTemplates.containsKey(id)) {
            cacheTemplates.put(id, getApi(id).getTemplates());
        }
        return cacheTemplates.get(id);
    }

    public boolean isTemplateExists(String id, String name) throws Exception {
        for (BeanTableApiTemplate t: getTemplates(id)) {
            if (t.getId().equals(name)) {
                return true;
            }
        }
        return false;
    }
    
    public BeanTableApi getApi(String id) throws Exception {
        return getApi(id, getBeanTableDao());
    }
    
    public BeanTableApi getApi(String id, BeanTableDaoIf tableDao) throws Exception {
        BeanTableApiConfig config = configProvider.getConfig(id);
        if (config != null) {
            return new BeanTableApi(config, this, tableDao);
        }
        throw new Exception(String.format("Bean table API with ID = [%s] not found?!", id));
    }

    public BeanTableApiConfigProviderIf getConfigProvider() {
        return configProvider;
    }
    
}

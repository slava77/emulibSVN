/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component;

import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.component.table.api.BeanTableApi;
import jsf.bean.gui.component.table.api.BeanTableApiConfig;
import jsf.bean.gui.component.table.api.BeanTableApiConfigProviderIf;
import jsf.bean.gui.component.table.export.BeanTableExportTemplateProvider;

/**
 *
 * @author valdo
 */
public abstract class BeanTableApiManager {
    
    private BeanTableApiConfigProviderIf configProvider;

    public abstract BeanTableDaoIf getBeanTableDao();
    public abstract ClassFinderIf getClassFinder();
    public abstract BeanTableExportTemplateProvider getTemplateProvider(BeanTable table);
    
    public BeanTableApiManager(BeanTableApiConfigProviderIf configProvider) {
        this.configProvider = configProvider;
    }
    
    public BeanTableApi getApi(String id) throws Exception {
        BeanTableApiConfig config = configProvider.getConfig(id);
        if (config != null) {
            return new BeanTableApi(config, this);
        }
        throw new Exception(String.format("Bean table API with ID = [%s] not found?!", id));
    }

    public BeanTableApiConfigProviderIf getConfigProvider() {
        return configProvider;
    }
    
}

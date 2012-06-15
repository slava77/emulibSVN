/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.api;

import java.util.Collection;
import jsf.bean.gui.EntityBeanBase;

/**
 *
 * @author valdo
 */
public interface BeanTableApiConfigProviderIf {
    
    BeanTableApiConfig getConfig(String id);
    BeanTableApiConfig getConfig(Class<? extends EntityBeanBase> rowClass);
    Collection<BeanTableApiConfig> getAllConfigs();
    
}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.api;

import java.util.ArrayList;
import java.util.List;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTableFilter.Operation;

/**
 * Single table API configuration
 * @author valdo
 */
public abstract class BeanTableApiConfig {

    /**
     * Table API name (without spaces and special characters)
     */
    private final String id;
    
    private final Class<? extends EntityBeanBase> rowClass;
    private List<BeanTableApiConfigPropertyFilter> propertyFilters 
                = new ArrayList<BeanTableApiConfigPropertyFilter>();

    public abstract String getTemplateId(String name);
    
    public BeanTableApiConfig(String id, Class<? extends EntityBeanBase> rowClass) {
        this.id = id;
        this.rowClass = rowClass;
    }
    
    public String getId() {
        return id;
    }

    public void addPropertyFilter(String name, Operation operation, Object value) {
        propertyFilters.add(new BeanTableApiConfigPropertyFilter(name, operation, value));
    }
    
    public List<BeanTableApiConfigPropertyFilter> getPropertyFilters() {
        return propertyFilters;
    }

    public Class<? extends EntityBeanBase> getRowClass() {
        return rowClass;
    }

    public class BeanTableApiConfigPropertyFilter {

        private final String name;
        private final Operation operation;
        private final Object value;

        public BeanTableApiConfigPropertyFilter(String name, Operation operation, Object value) {
            this.name = name;
            this.operation = operation;
            this.value = value;
        }
        
        public String getName() {
            return name;
        }

        public Operation getOperation() {
            return operation;
        }

        public Object getValue() {
            return value;
        }
        
    }
    
}


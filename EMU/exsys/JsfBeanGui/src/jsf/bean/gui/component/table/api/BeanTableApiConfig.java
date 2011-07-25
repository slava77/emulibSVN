/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.api;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.regex.PatternSyntaxException;
import jsf.bean.gui.EntityBeanBase;

/**
 * Single table API configuration
 * @author valdo
 */
public class BeanTableApiConfig {
    
    private static final List<Pattern> NAME_TO_IDS;
    static {
        NAME_TO_IDS = new ArrayList<Pattern>();
        NAME_TO_IDS.add(Pattern.compile("([^ ]+) \\(.*\\)"));
    }

    /**
     * Table API name (without spaces and special characters)
     */
    private final String id;
    
    private final Class<? extends EntityBeanBase> rowClass;
    private List<BeanTableApiConfigPropertyFilter> propertyFilters 
                = new ArrayList<BeanTableApiConfigPropertyFilter>();

    public BeanTableApiConfig(String id, Class<? extends EntityBeanBase> rowClass) {
        this.id = id;
        this.rowClass = rowClass;
    }
    
    public String getId() {
        return id;
    }

    public void add(String name, String operation, Object value) {
        propertyFilters.add(new BeanTableApiConfigPropertyFilter(name, operation, value));
    }
    
    public List<BeanTableApiConfigPropertyFilter> getPropertyFilters() {
        return propertyFilters;
    }

    public Class<? extends EntityBeanBase> getRowClass() {
        return rowClass;
    }

    public String getTemplateId(String name) {
        String id = name;
        for (Pattern p: NAME_TO_IDS) {
            Matcher m = p.matcher(name);
            if (m.matches()) {
                id = m.group(1);
                break;
            }
        }
        return id.replaceAll(" ", "").toLowerCase();
    }
    
    public class BeanTableApiConfigPropertyFilter {

        private final String name;
        private final String operation;
        private final Object value;

        public BeanTableApiConfigPropertyFilter(String name, String operation, Object value) {
            this.name = name;
            this.operation = operation;
            this.value = value;
        }
        
        public String getName() {
            return name;
        }

        public String getOperation() {
            return operation;
        }

        public Object getValue() {
            return value;
        }
        
    }
    
}


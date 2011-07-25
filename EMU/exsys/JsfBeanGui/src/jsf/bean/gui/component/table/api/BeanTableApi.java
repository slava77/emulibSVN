/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.api;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.BeanTableApiManager;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.component.table.BeanTableFilter.Operation;
import jsf.bean.gui.component.table.column.BeanTableColumn;
import jsf.bean.gui.component.table.export.BeanTableExportResource;
import jsf.bean.gui.component.table.export.BeanTableExportTemplateProvider;

/**
 *
 * @author valdo
 */
public class BeanTableApi {
    
    private final BeanTableApiConfig config;
    private final BeanTableManager manager;
    private final BeanTableApiManager apimanager;
    private SortedSet<BeanTableApiColumn> columns = new TreeSet<BeanTableApiColumn>();
    private SortedSet<BeanTableApiTemplate> templates = new TreeSet<BeanTableApiTemplate>();
    private Map<String, BeanTableExportResource> exportResources = new HashMap<String, BeanTableExportResource>();
    
    public BeanTableApi(BeanTableApiConfig config, 
                        BeanTableApiManager apimanager_) throws Exception {
        this.config = config;
        this.apimanager = apimanager_;
        this.manager = new BeanTableManager(config.getId(), this.config.getRowClass(), false) {

            @Override
            public BeanTableDaoIf getBeanTableDao() {
                return apimanager.getBeanTableDao();
            }

            @Override
            public ClassFinderIf getClassFinder() {
                return apimanager.getClassFinder();
            }

            @Override
            public BeanTableExportTemplateProvider getTemplateProvider() {
                return apimanager.getTemplateProvider(this.getTable());
            }
            
        };
        
        for (BeanTableApiConfig.BeanTableApiConfigPropertyFilter pf: this.config.getPropertyFilters()) {
            this.manager.addPropertyFilter(
                    pf.getName(), 
                    Operation.fromValue(pf.getOperation()), 
                    pf.getValue());
        }
        
        for (BeanTableColumn col: this.manager.getTable().getColumns()) {
            columns.add(new BeanTableApiColumn(col.getName(), col.getType().getSimpleName()));
        }
        
        List<BeanTableExportResource> erl = new LinkedList<BeanTableExportResource>();
        erl.addAll(this.manager.getTable().getExportTemplateList().getDefaultExportResources());
        erl.addAll(this.manager.getTable().getExportTemplateList().getPublicExportResources());
        
        for (BeanTableExportResource r: erl) {
            String oid = this.config.getTemplateId(r.getTemplate().getTitle());
            if (oid != null) {
                int i = 1;
                String id = oid;
                while (exportResources.containsKey(oid)) {
                    oid = id.concat(String.valueOf(i++));
                }
                templates.add(
                   new BeanTableApiTemplate(oid, 
                        r.getTemplate().getTitle(), 
                        r.getTemplate().getMimeType()));
                exportResources.put(oid, r);
            }
        }
        
    }

    public SortedSet<BeanTableApiColumn> getColumns() {
        return columns;
    }

    public SortedSet<BeanTableApiTemplate> getTemplates() {
        return templates;
    }
    
    public InputStream export(String id) throws BeanTableApiTemplateNotExists, IOException {
        
        if (!exportResources.containsKey(id)) {
            throw new BeanTableApiTemplateNotExists(id);
        }
        
        return exportResources.get(id).open();
        
    }
    
}

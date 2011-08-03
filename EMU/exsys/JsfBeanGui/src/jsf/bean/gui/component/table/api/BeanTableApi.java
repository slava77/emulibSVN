/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.api;

import java.io.InputStream;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.SortedSet;
import java.util.TreeSet;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.BeanTableApiManager;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.component.table.column.BeanTableQueryColumn;
import jsf.bean.gui.component.table.export.BeanTableExportResource;
import jsf.bean.gui.component.table.export.BeanTableExportTemplateProvider;
import org.json.JSONObject;

/**
 *
 * @author valdo
 */
public class BeanTableApi {
    
    private final BeanTableApiConfig config;
    private final BeanTableManager manager;
    private final BeanTableApiManager apimanager;
    private final BeanTableDaoIf tableDao;
    private Map<String, BeanTableExportResource> exportResources = new HashMap<String, BeanTableExportResource>();
    
    public BeanTableApi(BeanTableApiConfig config, 
                        BeanTableApiManager apimanager_,
                        BeanTableDaoIf tableDao_) throws Exception {
        this.config = config;
        this.apimanager = apimanager_;
        this.tableDao = tableDao_;
        this.manager = new BeanTableManager(config.getId(), this.config.getRowClass(), false) {

            @Override
            public BeanTableDaoIf getBeanTableDao() {
                return tableDao;
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
                    pf.getOperation(), 
                    pf.getValue());
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
                exportResources.put(oid, r);
            }
        }
        
    }

    public Collection<BeanTableQueryColumn> getColumns() {
        return manager.getTable().getQueryColumns();
    }

    public SortedSet<BeanTableApiTemplate> getTemplates() {
        SortedSet<BeanTableApiTemplate> templates = new TreeSet<BeanTableApiTemplate>();
        for (String rid: exportResources.keySet()) {
            templates.add(
               new BeanTableApiTemplate(rid, 
                    exportResources.get(rid).getTemplate().getTitle(), 
                    exportResources.get(rid).getTemplate().getMimeType()));
        }
        return templates;
    }
    
    public BeanTable getTable() {
        return this.manager.getTable();
    }

    public InputStream export(String templateId, JSONObject apiFilter) throws Exception {
        
        if (!exportResources.containsKey(templateId)) {
            throw new Exception(String.format("Template [%s] not found?!", templateId));
        }
        
        if (apiFilter != null) {
            this.manager.getTablePack().setSerializedFilter(apiFilter);
        }
        
        return exportResources.get(templateId).open();
        
    }
    
    public Long count(JSONObject apiFilter) throws Exception {
        
        if (apiFilter != null) {
            this.manager.getTablePack().setSerializedFilter(apiFilter);
        }
        
        return this.manager.getTable().getDataCount();
        
    }
    
}

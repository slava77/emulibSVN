/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import jsf.bean.gui.component.table.BeanTable;

/**
 *
 * @author valdo
 */
public class BeanTableExportTemplateList {
 
    private final BeanTable table;
    
    private List<BeanTableExportResource> defaultExportResources;
    private List<BeanTableExportResource> publicExportResources = new ArrayList<BeanTableExportResource>();
    private List<BeanTableExportResource> userExportResources = new ArrayList<BeanTableExportResource>();
    private Date templatesChecked = new Date(0);

    public BeanTableExportTemplateList(BeanTable table) throws IOException {
        this.table = table;

        List<BeanTableExportTemplate> templates = new ArrayList<BeanTableExportTemplate>();

        // Adding defaults
        templates.addAll(BeanTableDefaultExportTemplateFactory.getTemplates());
        Collections.sort(templates, new ExportTemplateComparator());
        defaultExportResources = new ArrayList<BeanTableExportResource>();
        for (BeanTableExportTemplate t : templates) {
            defaultExportResources.add(new BeanTableExportResource(table, t));
        }
        
    }

    public List<BeanTableExportResource> getDefaultExportResources() {
        return defaultExportResources;
    }

    @SuppressWarnings(value="unchecked")
    public List<BeanTableExportResource> getPublicExportResources() {
        BeanTableExportTemplateProvider provider = table.getPack().getManager().getTemplateProvider();
        if (provider.getModified() == null) {
            return Collections.EMPTY_LIST;
        }
        
        if (this.templatesChecked.before(provider.getModified())) {
            List<BeanTableExportTemplate> templates = new ArrayList<BeanTableExportTemplate>();
            templates.addAll(provider.getPublicTemplates());
            Collections.sort(templates, new ExportTemplateComparator());
            publicExportResources.clear();
            for (BeanTableExportTemplate t : templates) {
                publicExportResources.add(new BeanTableExportResource(table, t));
            }
            return publicExportResources;
        }
        return publicExportResources;
    }

    @SuppressWarnings(value="unchecked")
    public List<BeanTableExportResource> getUserExportResources() {
        BeanTableExportTemplateProvider provider = table.getPack().getManager().getTemplateProvider();
        if (provider.getModified() == null) {
            this.templatesChecked = Calendar.getInstance().getTime();
            return Collections.EMPTY_LIST;
        }
        
        if (this.templatesChecked.before(provider.getModified())) {
            List<BeanTableExportTemplate> templates = new ArrayList<BeanTableExportTemplate>();
            templates.addAll(provider.getUserTemplates());
            Collections.sort(templates, new ExportTemplateComparator());
            userExportResources.clear();
            for (BeanTableExportTemplate t : templates) {
                userExportResources.add(new BeanTableExportResource(table, t));
            }
            this.templatesChecked = Calendar.getInstance().getTime();
            return userExportResources;
        }

        this.templatesChecked = Calendar.getInstance().getTime();
        return userExportResources;
    }
    
}

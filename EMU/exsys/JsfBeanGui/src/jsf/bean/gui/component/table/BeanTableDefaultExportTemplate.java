/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.ResourceBundle;

/**
 *
 * @author valdor
 */
public class BeanTableDefaultExportTemplate implements BeanTableExportTemplate {

    private static final String TEMPLATES_LOCATION = "jsf.bean.gui.component.table.template";
    private static final String TEMPLATES_BUNDLE = TEMPLATES_LOCATION + ".templates";
    private static final String HEADER_TEMPLATE = TEMPLATES_LOCATION + ".%s_header.ftl";
    private static final String ITEM_TEMPLATE = TEMPLATES_LOCATION + ".%s_item.ftl";
    private static final String FOOTER_TEMPLATE = TEMPLATES_LOCATION + ".%s_footer.ftl";

    private String name;
    private String headerTemplate;

    public BeanTableDefaultExportTemplate(String name) {
        //TODO
    }

    public String getFooterTemplate() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public String getHeaderTemplate() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public String getItemTemplate() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public String getMimeType() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public String getName() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public static List<BeanTableExportTemplate> getTemplates() {
        List<BeanTableExportTemplate> templates = new ArrayList<BeanTableExportTemplate>();
        ResourceBundle bundle = ResourceBundle.getBundle(TEMPLATES_BUNDLE);
        Enumeration<String> keys = bundle.getKeys();
        while (keys.hasMoreElements()) {
            templates.add(new BeanTableDefaultExportTemplate(keys.nextElement()));
        }
        return templates;
    }

}

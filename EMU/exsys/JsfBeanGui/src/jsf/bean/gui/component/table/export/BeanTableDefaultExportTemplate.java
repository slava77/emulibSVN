/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.EnumMap;
import java.util.Enumeration;
import java.util.List;
import java.util.Map;
import java.util.ResourceBundle;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdor
 */
public class BeanTableDefaultExportTemplate implements BeanTableExportTemplate {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableDefaultExportTemplate.class);
    private static final String TEMPLATES_BUNDLE = "jsf.bean.gui.component.table.template.templates";
    private static final String TEMPLATE = "/jsf/bean/gui/component/table/template/%s.ftl";
    private static final String MIME_TYPE_SUFFIX = "_mimeType";
    private static final String EXT_SUFFIX = "_ext";
    private static final String SECTION_SEP = "${}SEPARATOR${}";

    public static List<BeanTableExportTemplate> getTemplates() throws IOException {
        List<BeanTableExportTemplate> templates = new ArrayList<BeanTableExportTemplate>();
        ResourceBundle bundle = ResourceBundle.getBundle(TEMPLATES_BUNDLE);
        Enumeration<String> keys = bundle.getKeys();
        while (keys.hasMoreElements()) {
            String key = keys.nextElement();
            if (!key.endsWith(MIME_TYPE_SUFFIX) && !key.endsWith(EXT_SUFFIX)) {
                templates.add(new BeanTableDefaultExportTemplate(key, bundle));
            }
        }
        return templates;
    }

    private final String key;
    private final String title;
    private final String ext;
    private final String mimeType;

    public BeanTableDefaultExportTemplate(String key, ResourceBundle bundle) throws IOException {
        this.key = key;
        title = bundle.getString(key);
        mimeType = bundle.getString(key.concat(MIME_TYPE_SUFFIX));
        ext = bundle.getString(key.concat(EXT_SUFFIX));
    }

    public String getMimeType() {
        return mimeType;
    }

    public String getTitle() {
        return title;
    }

    public String getExt() {
        return ext;
    }

    public  Map<TemplateTypeKey, String> getTemplate() {
        try {
            StringBuilder sb = new StringBuilder();
            InputStream in = this.getClass().getResourceAsStream(String.format(TEMPLATE, key));
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            String line;
            Map<TemplateTypeKey, String> templates = new EnumMap<TemplateTypeKey, String>(TemplateTypeKey.class);
            int separatorNumber = 0;
            while ((line = br.readLine()) != null) {
                if (line.equals(SECTION_SEP)) {
                    templates.put(TemplateTypeKey.values()[separatorNumber], sb.toString());
                    separatorNumber++;
                    sb.setLength(0);
                } else {
                    sb.append(line);
                    sb.append(System.getProperty("line.separator"));
                }
            }
            templates.put(TemplateTypeKey.values()[separatorNumber], sb.toString());
            return templates;
        } catch (IOException ex) {
            logger.error(ex);
            return null;
        }
    }
}

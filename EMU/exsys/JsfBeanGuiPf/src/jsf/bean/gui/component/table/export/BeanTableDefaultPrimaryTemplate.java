/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.EnumMap;
import java.util.Map;
import java.util.ResourceBundle;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdor
 */
public class BeanTableDefaultPrimaryTemplate extends BeanTableDefaultExportTemplate implements BeanTableExportTemplatePrimary {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableDefaultPrimaryTemplate.class);
    private static final String SECTION_SEP = "${}SEPARATOR${}";

    public BeanTableDefaultPrimaryTemplate(String key, ResourceBundle bundle) throws IOException {
        super(key, bundle);
    }

    public  Map<TemplatePosition, String> getTemplates() {
        try {
            StringBuilder sb = new StringBuilder();
            InputStream in = this.getClass().getResourceAsStream(String.format(TEMPLATE, key));
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            Map<TemplatePosition, String> templates = new EnumMap<TemplatePosition, String>(TemplatePosition.class);
            int separatorNumber = 0;
            String line = null;
            while ((line = br.readLine()) != null) {
                if (line.equals(SECTION_SEP)) {
                    templates.put(TemplatePosition.values()[separatorNumber], sb.toString());
                    separatorNumber++;
                    sb.setLength(0);
                } else {
                    sb.append(line);
                    sb.append(System.getProperty("line.separator"));
                }
            }
            templates.put(TemplatePosition.values()[separatorNumber], sb.toString());
            return templates;
        } catch (IOException ex) {
            logger.error(ex);
            return null;
        }
    }

    public boolean isPrimary() {
        return true;
    }

}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdo
 */
public abstract class BeanTableExportProcessorSecondary extends BeanTableExportProcessor {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportProcessorSecondary.class);
    
    protected final BeanTableExportTemplateSecondary template;

    public BeanTableExportProcessorSecondary(BeanTable table, BeanTableExportTemplateSecondary template) {
        super(table);
        this.template = template;
    }

    public abstract boolean export(InputStream in, OutputStream out) throws IOException;

    protected List inputStreamToList(InputStream in) throws IOException {
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        List<String> list = new ArrayList<String>();
        String line = null;

        while ((line = br.readLine()) != null) {
            list.add(line);
        }

        br.close();
        return list;
    }
    
    public static BeanTableExportProcessorSecondary getProcessor(BeanTable table, BeanTableExportTemplateSecondary template) {
        if (template.getTemplateType().equals(BeanTableExportTemplateSecondary.TemplateType.FREEMARKER)) {
            return new BeanTableExportProcessorSecFreemarker(table, template);
        } else {
            return new BeanTableExportProcessorSecXslt(table, template);
        }
    }
    
}

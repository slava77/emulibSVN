/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import freemarker.template.TemplateException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Map;
import jsf.bean.gui.component.fm.TemplateManager;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.component.table.export.BeanTableExportTemplatePrimary.TemplatePosition;

/**
 *
 * @author valdo
 */
public class BeanTableExportProcessorSecFreemarker extends BeanTableExportProcessorSecondary {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportProcessorSecFreemarker.class);
    
    public BeanTableExportProcessorSecFreemarker(BeanTable table, BeanTableExportTemplateSecondary template) {
        super(table, template);
    }

    public boolean export(InputStream in, OutputStream out) throws IOException {

        TemplateManager manager = new TemplateManager();
        manager.addTemplate(TemplatePosition.ITEM.name(), template.getTemplate());
        
        Map root = getRoot();
        root.put(LINES, inputStreamToList(in));

        try {
            write(out, manager.execute(TemplatePosition.ITEM.name(), root));
        } catch (TemplateException ex) {
            write(out, ex.getMessage().concat(ex.getFTLInstructionStack()));
            return false;
        }
        
        return true;
        
    }

}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Stack;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdo
 */
public class BeanTableExportManager {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportManager.class);

    private static final File tmpDir = new File("/tmp");
    
    private final BeanTable table;
    private final BeanTableExportTemplate topTemplate;
    private final boolean isPreview;

    public BeanTableExportManager(BeanTable table, BeanTableExportTemplate template) throws IOException {
        this(table, template, false);
    }

    public BeanTableExportManager(BeanTable table, BeanTableExportTemplate template, boolean isPreview) throws IOException {
        this.table = table;
        this.topTemplate = template;
        this.isPreview = isPreview;
    }

    public InputStream export() throws IOException {

        Stack<BeanTableExportTemplate> stack = new Stack<BeanTableExportTemplate>();
        BeanTableExportTemplate ct = topTemplate;
        
        while (ct instanceof BeanTableExportTemplateSecondary && ct.isPrimary() == false) {
            stack.push(ct);
            ct = ((BeanTableExportTemplateSecondary) ct).getPreviousTemplate();
        }
        
        boolean success;
        
        StreamWorker output = new StreamWorker();
        BeanTableExportProcessorPrimary pm = new BeanTableExportProcessorPrimary(table, (BeanTableExportTemplatePrimary) ct, isPreview);
        success = pm.export(output.getOutputStream(ct.getExt()));
        output.close();
        
        while (success && !stack.empty()) {
            ct = stack.pop();
            BeanTableExportProcessorSecondary sm = BeanTableExportProcessorSecondary.getProcessor(table, (BeanTableExportTemplateSecondary) ct);
            success = sm.export(output.getInputStream(), output.getOutputStream(ct.getExt()));
            output.close();
        }
        
        return output.getInputStream();
    }

    private class StreamWorker {
        
        private File fout = null;
        private File fin = null;
        
        private FileOutputStream sout = null;
        private FileInputStream sin = null;

        public OutputStream getOutputStream(String ext) throws FileNotFoundException, IOException {
            this.fout = File.createTempFile("rr3", ext, tmpDir);
            this.sout = new FileOutputStream(this.fout);
            return this.sout;
        }
        
        public InputStream getInputStream() {
            return this.sin;
        }
        
        public void close() throws IOException {
            this.sout.flush();
            this.sout.close();
            this.fin = this.fout;
            this.sin = new FileInputStream(fin);
            this.fin.deleteOnExit();
            this.fout = null;
        }
        
    }
        
}

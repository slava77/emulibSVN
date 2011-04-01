/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import com.icesoft.faces.context.Resource;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.Serializable;
import java.util.Date;
import java.util.UUID;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.icefaces.apache.commons.io.IOUtils;

public class BeanTableExportResource implements Resource, Serializable {


    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportManager.class);

    private final BeanTableExportTemplate template;
    private final BeanTable table;

    public BeanTableExportResource(BeanTable table, BeanTableExportTemplate template) {
        this.template = template;
        this.table = table;
    }

    public String calculateDigest() {
        return UUID.randomUUID().toString();
    }

    public InputStream open() throws IOException {
        File f = null;
        byte[] bytes;
        try {
            BeanTableExportManager manager = new BeanTableExportManager(table, template);
            f = manager.export();
            InputStream input = new FileInputStream(f);
            bytes = IOUtils.toByteArray(input);
        } catch (Exception ex) {
            logger.error(ex);
            bytes = "".getBytes();
        }
        InputStream inputStream = new ByteArrayInputStream(bytes);
        return inputStream;
    }

    public Date lastModified() {
        return new Date();
    }

    public void withOptions(Options optns) throws IOException {
        optns.setFileName("rr3_export_file".concat(template.getExt()));
    }

    public BeanTableExportTemplate getTemplate() {
        return template;
    }

    
}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import com.icesoft.faces.context.Resource;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.Serializable;
import java.util.Calendar;
import java.util.Date;
import java.util.UUID;
import jsf.bean.gui.component.table.BeanTable;
import org.icefaces.apache.commons.io.IOUtils;

@SuppressWarnings("deprecation")
public class BeanTableExportResource implements Resource, Serializable {

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
        BeanTableExportManager manager = new BeanTableExportManager(table, template);
        InputStream input = manager.export();
        byte[] bytes = IOUtils.toByteArray(input);
        InputStream inputStream = new ByteArrayInputStream(bytes);
        return inputStream;
    }

    public Date lastModified() {
        return Calendar.getInstance().getTime();
    }

    public void withOptions(Options optns) throws IOException {
        optns.setFileName("rr3_export_file".concat(template.getExt()));
    }

    public BeanTableExportTemplate getTemplate() {
        return template;
    }
}

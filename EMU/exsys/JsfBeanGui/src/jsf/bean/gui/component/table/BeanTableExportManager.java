/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table;

import java.io.File;
import java.io.IOException;

/**
 *
 * @author valdo
 */
public class BeanTableExportManager {

    private static BeanTableExportManager instance;

    public static BeanTableExportManager getInstance() {
        if (instance == null) {
            instance = new BeanTableExportManager();
        }
        return instance;
    }

    private File tmpDir = new File("/tmp");

    public File export(BeanTable table, BeanTableExportTemplate template) throws IOException {
        File f = File.createTempFile("rr3", "", tmpDir);
        return f;
    }

}

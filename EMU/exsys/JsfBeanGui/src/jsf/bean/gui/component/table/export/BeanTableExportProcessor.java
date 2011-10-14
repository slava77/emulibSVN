/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.IOException;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.column.BeanTableColumn;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdo
 */
public abstract class BeanTableExportProcessor {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportProcessor.class);
    
    public static final String COLUMNS = "columns";
    public static final String LINES = "lines";
    
    protected final BeanTable table;

    public BeanTableExportProcessor(BeanTable table) {
        this.table = table;
    }

    protected void write(OutputStream fileStream, String text) throws IOException {
        fileStream.write(text.getBytes());
        fileStream.flush();
    }
    
    protected Map getRoot() {
        
        Map root = new HashMap();
        
        // Adding visible columns to root
        List<BeanTableExportColumn> exportColumns = new LinkedList<BeanTableExportColumn>();
        for (BeanTableColumn col : table.getSelectedColumns().getTarget()) {
            exportColumns.add(new BeanTableExportColumn(col));
        }
        root.put(COLUMNS, exportColumns);
        
        return root;
    }
    
}

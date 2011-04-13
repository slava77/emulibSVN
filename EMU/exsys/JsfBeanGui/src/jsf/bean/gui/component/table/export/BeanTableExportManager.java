/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import jsf.bean.gui.component.fm.TemplateManager;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.column.BeanTableColumn;
import jsf.bean.gui.component.table.export.BeanTableExportTemplate.TemplateTypeKey;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdo
 */
public class BeanTableExportManager {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportManager.class);
    private static final String KEY_IS_LAST_PAGE = "isLastPage";
    private static final String KEY_IS_FIRST_PAGE = "isLastPage";
    public static final String KEY_IS_LAST = "isLast";
    public static final String KEY_IS_FIRST = "isFirst";
    private static final String KEY_PAGE = "page";
    public static final String ITEM = "item";
    public static final String COLUMNS = "columns";
    private static final String ITEM_TEMPLATE_PREFIX = "<#list " + KEY_PAGE + " as " + ITEM + ">"
            + "<#if " + KEY_IS_LAST_PAGE + " && !" + ITEM + "_has_next>"
            + "<#assign " + KEY_IS_LAST + "=true>"
            + "<#else>"
            + "<#assign " + KEY_IS_LAST + "=false>"
            + "</#if>"
            + "<#if " + KEY_IS_FIRST_PAGE + " && !(" + ITEM + "_index = 0)>"
            + "<#assign " + KEY_IS_FIRST + "=true>"
            + "<#else>"
            + "<#assign " + KEY_IS_FIRST + "=false>"
            + "</#if>";
    private static final String ITEM_TEMPLATE_SUFFIX = "</#list>";
    private static final int PAGE_SIZE = 50;
    private BeanTable table;
    private BeanTableExportTemplate template;
    private File file;
    private File tmpDir = new File("/tmp");
    private FileOutputStream fileStream;
    private boolean isPreview = false;

    public BeanTableExportManager(BeanTable table, BeanTableExportTemplate template) throws IOException {
        this.file = File.createTempFile("rr3tmp", template.getExt(), tmpDir);
        fileStream = new FileOutputStream(file);
        this.table = table;
        this.template = template;
        this.isPreview = false;
    }

    public BeanTableExportManager(BeanTable table, BeanTableExportTemplate template, boolean isPreview) throws IOException {
        this.file = File.createTempFile("rr3tmp", template.getExt(), tmpDir);
        fileStream = new FileOutputStream(file);
        this.table = table;
        this.template = template;
        this.isPreview = isPreview;
    }

    public File export() throws IOException {

        TemplateManager manager = new TemplateManager();
        Map root = new HashMap();

        Map<TemplateTypeKey, String> templates = template.getTemplate();
        for (TemplateTypeKey k : TemplateTypeKey.values()) {
            if (templates.containsKey(k)) {
                String s = templates.get(k);
                if (k.equals(TemplateTypeKey.ITEM) && s != null) {
                    s = ITEM_TEMPLATE_PREFIX.concat(s).concat(ITEM_TEMPLATE_SUFFIX);
                }
                manager.addTemplate(k.name(), s == null ? "" : s);
            } else {
                manager.addTemplate(k.name(), "");
            }
        }

        List<BeanTableExportColumn> exportColumns = new LinkedList<BeanTableExportColumn>();
        for (BeanTableColumn col : table.getSelectedColumns().getTarget()) {
            exportColumns.add(new BeanTableExportColumn(col));
        }
        root.put(COLUMNS, exportColumns);

        writeToFile(manager.execute(TemplateTypeKey.HEADER.name(), root));
        if (isPreview) {
            writeItemToFilePreview(manager, root);
        } else {
            writeItemToFile(manager, root);
        }
        writeToFile(manager.execute(TemplateTypeKey.FOOTER.name(), root));

        fileStream.close();
        file.deleteOnExit();
        
        return file;
    }

    private void writeItemToFile(TemplateManager manager, Map root) throws IOException {
        root.put(KEY_IS_LAST, false);
        long pages = ((table.getDataCount() % PAGE_SIZE) == 0 ? (table.getDataCount() / PAGE_SIZE) : (table.getDataCount() / PAGE_SIZE) + 1);
        for (Integer p = 1; p <= pages; p++) {
            root.put(KEY_PAGE, table.getPack().getManager().getBeanTableDao().getData(table, PAGE_SIZE, p));
            if (p == 1) {
                root.put(KEY_IS_FIRST_PAGE, true);
            } else {
                root.put(KEY_IS_FIRST_PAGE, false);
            }
            if (p == pages) {
                root.put(KEY_IS_LAST_PAGE, true);
            } else {
                root.put(KEY_IS_LAST_PAGE, false);
            }
            String out = manager.execute(TemplateTypeKey.ITEM.name(), root);
            writeToFile(out);
            root.remove(KEY_IS_LAST_PAGE);
            root.put(KEY_IS_FIRST_PAGE, true);
        }
        root.remove(KEY_IS_LAST);
        root.remove(KEY_IS_FIRST_PAGE);
        root.remove(KEY_PAGE);
    }

    private void writeItemToFilePreview(TemplateManager manager, Map root) {
        root.put(KEY_PAGE, table.getPack().getManager().getBeanTableDao().getData(table, PAGE_SIZE, 1));
        root.put(KEY_IS_FIRST_PAGE, true);
        root.put(KEY_IS_LAST_PAGE, true);
        try {
            String out = manager.execute(TemplateTypeKey.ITEM.name(), root);
            writeToFile(out);
        } catch (IOException ex) {
            logger.error(ex);
        }
        root.remove(KEY_IS_LAST_PAGE);
        root.remove(KEY_IS_FIRST_PAGE);
        root.remove(KEY_PAGE);
    }

    private void writeToFile(String text) {
        try {
            fileStream.write(text.getBytes());
            fileStream.flush();
        } catch (IOException ex) {
            logger.error("File stream did not write to file. " + ex);
        }
    }
}

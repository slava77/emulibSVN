/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import freemarker.template.TemplateException;
import java.io.IOException;
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
public class BeanTableExportProcessorPrimary extends BeanTableExportProcessor {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportProcessorPrimary.class);
    
    private static final String KEY_IS_LAST_PAGE = "isLastPage";
    private static final String KEY_IS_FIRST_PAGE = "isLastPage";
    public static final String KEY_IS_LAST = "isLast";
    public static final String KEY_IS_FIRST = "isFirst";
    private static final String KEY_PAGE = "page";
    public static final String ITEM = "item";
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
    
    private final BeanTableExportTemplatePrimary template;
    private final boolean isPreview;

    public BeanTableExportProcessorPrimary(BeanTable table, BeanTableExportTemplatePrimary template) {
        this(table, template, false);
    }

    public BeanTableExportProcessorPrimary(BeanTable table, BeanTableExportTemplatePrimary template, boolean isPreview) {
        super(table);
        this.template = template;
        this.isPreview = isPreview;
    }

    public boolean export(OutputStream out) throws IOException {

        TemplateManager manager = new TemplateManager();

        // Addding appropriate templates to manager
        Map<TemplatePosition, String> templates = template.getTemplates();
        for (TemplatePosition k : TemplatePosition.values()) {
            
            if (templates.containsKey(k)) {
                String s = templates.get(k);
                if (k.equals(TemplatePosition.ITEM) && s != null) {
                    s = ITEM_TEMPLATE_PREFIX.concat(s).concat(ITEM_TEMPLATE_SUFFIX);
                }
                manager.addTemplate(k.name(), s == null ? "" : s);
            } else {
                manager.addTemplate(k.name(), "");
            }
            
        }
        
        Map root = getRoot();
        
        // Executing header and writing output to file
        try {
            write(out, manager.execute(TemplatePosition.HEADER.name(), root));
        } catch (TemplateException ex) {
            write(out, ex.getMessage().concat(ex.getFTLInstructionStack()));
            return false;
        }
        
        // Executing items and writing to file
        try {
            if (isPreview) {
                writeItemsPreview(out, manager, root);
            } else {
                writeItems(out, manager, root);
            }
        } catch (TemplateException ex) {
            write(out, ex.getMessage().concat(ex.getFTLInstructionStack()));
            return false;
        }
        
        // Executing footer and writing to file
        try {
            write(out, manager.execute(TemplatePosition.FOOTER.name(), root));
        } catch (TemplateException ex) {
            write(out, ex.getMessage().concat(ex.getFTLInstructionStack()));
            return false;
        }

        return true;
        
    }

    private void writeItems(OutputStream out, TemplateManager manager, Map root) throws IOException, TemplateException {
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
            
            write(out, manager.execute(TemplatePosition.ITEM.name(), root));
            root.remove(KEY_IS_LAST_PAGE);
            root.put(KEY_IS_FIRST_PAGE, true);
        }
        root.remove(KEY_IS_LAST);
        root.remove(KEY_IS_FIRST_PAGE);
        root.remove(KEY_PAGE);
    }

    private void writeItemsPreview(OutputStream out, TemplateManager manager, Map root) throws IOException, TemplateException {
        root.put(KEY_PAGE, table.getPack().getManager().getBeanTableDao().getData(table, PAGE_SIZE, 1));
        root.put(KEY_IS_FIRST_PAGE, true);
        root.put(KEY_IS_LAST_PAGE, true);
        write(out, manager.execute(TemplatePosition.ITEM.name(), root));
        root.remove(KEY_IS_LAST_PAGE);
        root.remove(KEY_IS_FIRST_PAGE);
        root.remove(KEY_PAGE);
    }

}

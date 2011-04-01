/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.fm;

import freemarker.cache.StringTemplateLoader;
import freemarker.template.Configuration;
import freemarker.template.ObjectWrapper;
import freemarker.template.Template;
import freemarker.template.TemplateException;
import java.io.IOException;
import java.io.StringWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;

/**
 *
 * @author valdo
 */
public class TemplateManager {

    private Configuration cfg;
    private StringTemplateLoader loader;
    private List<String> templateNames = new ArrayList<String>();
    private Map<String, Object> root;

    public TemplateManager() {
        this.loader = new StringTemplateLoader();
        this.cfg = new Configuration();
        this.cfg.setObjectWrapper(ObjectWrapper.BEANS_WRAPPER);
        this.cfg.setTemplateLoader(loader);
        this.cfg.setDateTimeFormat("EEE dd-MM-yy HH:mm:ss");
    }

    public void addTemplate(String name, String templateStr) {
        loader.putTemplate(name, templateStr);
        this.templateNames.add(name);
    }

    public String execute(String name, Map<String, Object> root) throws IOException, TemplateException {
        this.root = root;
        Writer out = new StringWriter();
        try {
            Template t = cfg.getTemplate(name);
            t.process(root, out);
        } catch (RuntimeException ex) {
            throw new IOException(ex);
        }
        return out.toString();
    }

    public List<String> getTemplateNames() {
        return Collections.unmodifiableList(templateNames);
    }

    public Configuration getCfg() {
        return cfg;
    }

    public Map<String, Object> getRoot() {
        return root;
    }

}

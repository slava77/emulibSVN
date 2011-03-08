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
import java.util.Map;

/**
 *
 * @author valdo
 */
public class TemplateManager {

    private Configuration cfg;
    private StringTemplateLoader loader;

    public TemplateManager() {
        this.loader = new StringTemplateLoader();
        this.cfg = new Configuration();
        this.cfg.setObjectWrapper(ObjectWrapper.BEANS_WRAPPER);
        this.cfg.setTemplateLoader(loader);
    }

    public void putTemplate(String name, String templateStr) {
        loader.putTemplate(name, templateStr);
    }

    public String execute(String name, Map<String, Object> root) throws IOException, TemplateException {
        Writer out = new StringWriter();
        Template t = cfg.getTemplate(name);
        try {
            t.process(root, out);
        } catch (RuntimeException ex) {
            throw new IOException(ex);
        }
        return out.toString();
    }

}

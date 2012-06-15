/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.fm;

import freemarker.cache.StringTemplateLoader;
import freemarker.core.Environment;
import freemarker.template.Configuration;
import freemarker.template.ObjectWrapper;
import freemarker.template.Template;
import freemarker.template.TemplateException;
import freemarker.template.TemplateExceptionHandler;
import java.io.IOException;
import java.io.StringWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdo
 */
public class TemplateManager {

    private static final Logger logger = SimpleLogger.getLogger(TemplateManager.class);
    
    private Configuration cfg;
    private StringTemplateLoader loader;
    private List<String> templateNames = new ArrayList<String>();
    private Map<String, Object> root;
    private String DATE_FORMAT = "EEE dd-MM-yy HH:mm:ss";
    private String NUMBER_FORMAT = "#.###########";

    public TemplateManager() {
        this.loader = new StringTemplateLoader();
        this.cfg = new Configuration();
        this.cfg.setObjectWrapper(ObjectWrapper.BEANS_WRAPPER);
        this.cfg.setTemplateLoader(loader);
        this.cfg.setDateTimeFormat(DATE_FORMAT);
        this.cfg.setNumberFormat(NUMBER_FORMAT);
    }

    public void addTemplate(String name, String templateStr) {
        loader.putTemplate(name, templateStr);
        this.templateNames.add(name);
    }

    public String execute(String name, Map<String, Object> root) throws IOException, TemplateException {
        this.root = root;
        Writer out = new StringWriter();
        Template t = cfg.getTemplate(name);
        t.setTemplateExceptionHandler(TemplateExceptionHandler.RETHROW_HANDLER);
        t.process(root, out);
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
    
    public class TemplateManagerExceptionHandler implements TemplateExceptionHandler {

        public void handleTemplateException(TemplateException te, Environment env, Writer out) throws TemplateException {
            throw new UnsupportedOperationException("Not supported yet.");
        }
        
    }
    
}

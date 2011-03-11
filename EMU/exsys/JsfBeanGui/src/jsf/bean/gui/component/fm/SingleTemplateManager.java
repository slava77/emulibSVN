/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.fm;

import freemarker.template.TemplateException;
import java.io.IOException;
import java.util.Map;

/**
 *
 * @author valdo
 */
public class SingleTemplateManager {

    private static final String TEMPLATE_NAME = "template";
    private TemplateManager manager = new TemplateManager();

    public SingleTemplateManager(String template) {
        manager.addTemplate(TEMPLATE_NAME, template);
    }

    public String execute(Map root) throws IOException, TemplateException {
        return manager.execute(TEMPLATE_NAME, root);
    }

}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.fm;

import freemarker.template.TemplateException;
import java.io.IOException;
import java.util.List;
import java.util.Map;

/**
 *
 * @author valdo
 */
public class TestManager {

    private static final String TEST_TEMPLATE = "<#if (%s)>true<#else>false</#if>";

    private TemplateManager manager;

    public TestManager() {
        this.manager = new TemplateManager();
    }

    public void addTest(String name, String testStr) {
        this.manager.addTemplate(name, String.format(TEST_TEMPLATE, testStr));
    }

    public boolean test(String name, Map<String, Object> root) throws IOException, TemplateException {
        return Boolean.valueOf(this.manager.execute(name, root));
    }

    public List<String> getEnabledTests() {
        return this.manager.getTemplateNames();
    }

}

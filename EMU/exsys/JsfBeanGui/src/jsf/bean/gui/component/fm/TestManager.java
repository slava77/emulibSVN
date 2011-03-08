/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.fm;

import freemarker.template.TemplateException;
import java.io.IOException;
import java.util.Collections;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

/**
 *
 * @author valdo
 */
public class TestManager {

    private static final String TEST_TEMPLATE = "<#if (%s)>true<#else>false</#if>";

    private Set<String> enabledTests = new HashSet<String>();

    private TemplateManager manager;

    public TestManager() {
        this.manager = new TemplateManager();
    }

    public void addTest(String name, String testStr) {
        this.manager.putTemplate(name, String.format(TEST_TEMPLATE, testStr));
        this.enabledTests.add(name);
    }

    public boolean test(String name, Map<String, Object> root) throws IOException, TemplateException {
        return Boolean.valueOf(this.manager.execute(name, root));
    }

    public Set<String> getEnabledTests() {
        return Collections.unmodifiableSet(enabledTests);
    }

}

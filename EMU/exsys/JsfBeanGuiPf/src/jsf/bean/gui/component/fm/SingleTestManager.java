/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.fm;

import freemarker.template.TemplateException;
import java.io.IOException;
import java.util.Map;

/**
 * Test against the single test
 * @author valdo
 */
public class SingleTestManager {

    private final static String TEST_NAME = "test";
    private TestManager manager;

    /**
     * Constructor
     * @param testStr Test
     */
    public SingleTestManager(String testStr) {
        this.manager = new TestManager();
        this.manager.addTest(TEST_NAME, testStr);
    }

    /**
     * Perform a test
     * @param root Map of objects
     * @return true if test successful, false - otherwise
     * @throws IOException
     * @throws TemplateException
     */
    public boolean test(Map<String, Object> root) throws IOException, TemplateException {
        return this.manager.test(TEST_NAME, root);
    }

}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.fm;

import freemarker.template.TemplateException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 *
 * @author valdo
 */
public class MultipleTestManager<T extends Object> {

    private TestManager manager;
    private Map<String, T> results = new HashMap<String, T>();

    /**
     * Constructor
     */
    public MultipleTestManager() {
        this.manager = new TestManager();
    }

    /**
     * Add a test (order maintained!)
     * @param result Object that is considered as a result if test returns true
     * @param testStr Test string
     */
    public void add(T result, String testStr) {
        String id = "test_".concat(String.valueOf(results.size()));
        this.manager.addTest(id, testStr);
        this.results.put(id, result);
    }

    /**
     * Test if any of the tests matches
     * @param root Map of objects
     * @return true if any of the tests matches root
     * @throws IOException
     * @throws TemplateException
     */
    public boolean matchAny(Map<String, Object> root) throws IOException, TemplateException {
        for (String id: manager.getEnabledTests()) {
            if (manager.test(id, root)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Return a result object that matches the first test
     * @param root map of objects
     * @return first result object or null if none test matches
     * @throws IOException
     * @throws TemplateException
     */
    public T getFirstMatch(Map<String, Object> root) throws IOException, TemplateException {
        for (String id: manager.getEnabledTests()) {
            if (manager.test(id, root)) {
                return results.get(id);
            }
        }
        return (T) null;
    }

    /**
     * Return a list of result objects that are associated to tests that matches
     * root. Order of tests maintained!
     * @param root Map of objects
     * @return list of result objects of tests that matches root
     * @throws IOException
     * @throws TemplateException
     */
    public List<T> getAllMatches(Map<String, Object> root) throws IOException, TemplateException {
        List<T> res = new ArrayList<T>();
        for (String id: manager.getEnabledTests()) {
            if (manager.test(id, root)) {
                res.add(results.get(id));
            }
        }
        return res;
    }

}

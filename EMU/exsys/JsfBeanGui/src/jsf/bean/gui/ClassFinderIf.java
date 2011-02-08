/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui;

import java.io.IOException;
import java.util.Collection;

/**
 *
 * @author valdo
 */
public interface ClassFinderIf {

    Collection<Class> findSubclassesInSamePackage(Class clazz) throws ClassNotFoundException, IOException;

}

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

    /**
     * Finds all subclasses of the given class that reside in the same package (or subpackages)
     * @param clazz class whose subclasses you want to get
     * @return subclasses of the given class that reside in the same package (or subpackages)
     * @throws ClassNotFoundException
     * @throws IOException
     */
    Collection<Class> findSubclassesInSamePackage(Class clazz) throws ClassNotFoundException, IOException;
    /**
     * Finds all subclasses of the given class that reside in the given package (or subpackages)
     * @param clazz class whose subclasses you want to get
     * @param packageName name of the package to search for the subclasses
     * @return subclasses of the given class that reside in the given package (or subpackages)
     * @throws ClassNotFoundException
     * @throws IOException
     */
    Collection<Class> findSubclassesInPackage(Class clazz, String packageName) throws ClassNotFoundException, IOException;

    Class getClassForName(String className) throws ClassNotFoundException;

}

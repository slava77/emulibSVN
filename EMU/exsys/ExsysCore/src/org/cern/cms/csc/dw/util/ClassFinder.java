/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.util;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import jsf.bean.gui.ClassFinderIf;

/**
 *
 * @author evka
 */
public class ClassFinder implements ClassFinderIf {

    private static ClassFinder instance;

    private ClassFinder() { }

    public static ClassFinder getInstance() {
        if (instance == null) {
            instance = new ClassFinder();
        }
        return instance;
    }

    /**
     * Finds all subclasses of the given class that reside in the given package (or subpackages)
     * @param clazz class whose subclasses you want to get
     * @param packageName name of the package to search for the subclasses
     * @return subclasses of the given class that reside in the given package (or subpackages)
     * @throws ClassNotFoundException
     * @throws IOException
     */
    @Override
    public List<Class> findSubclassesInPackage(Class clazz, String packageName) throws ClassNotFoundException, IOException {
        List<Class> classesInPackage = getClassesInPackage(packageName);
        List<Class> ret = new ArrayList<Class>();
        for (Class c: classesInPackage) {
            if (clazz.isAssignableFrom(c)) {
                ret.add(c);
            }
        }
        return ret;
    }

    /**
     * Finds all subclasses of the given class that reside in the same package (or subpackages)
     * @param clazz class whose subclasses you want to get
     * @return subclasses of the given class that reside in the same package (or subpackages)
     * @throws ClassNotFoundException
     * @throws IOException
     */
    @Override
    public List<Class> findSubclassesInSamePackage(Class clazz) throws ClassNotFoundException, IOException {
        String packageName = clazz.getPackage().getName();
        List<Class> classesInPackage = getClassesInPackage(packageName);
        List<Class> ret = new ArrayList<Class>();
        for (Class c: classesInPackage) {
            if (clazz.isAssignableFrom(c)) {
                ret.add(c);
            }
        }
        return ret;
    }

    /**
     * Scans all classes accessible from the context class loader which belong to the given package and subpackages.
     *
     * @param packageName The base package
     * @return The classes
     * @throws ClassNotFoundException
     * @throws IOException
     */
    private static List<Class> getClassesInPackage(String packageName)
            throws ClassNotFoundException, IOException {
        ClassLoader classLoader = Thread.currentThread().getContextClassLoader();
        assert classLoader != null;
        String path = packageName.replace('.', '/');
        Enumeration<URL> resources = classLoader.getResources(path);
        ArrayList<Class> classes = new ArrayList<Class>();
        while (resources.hasMoreElements()) {
            URL resource = resources.nextElement();
            if (resource.getProtocol().equals("file")) {
                classes.addAll(findClassesInDir(new File(resource.getFile()), packageName));
            } else if (resource.getProtocol().equals("jar")) {
                classes.addAll(findClassesInJarDir(resource));
            }
        }

        return classes;
    }

    /**
     * Finds all classes in a given directory within a jar
     * @param jarDir - URL to the directory of interest within a jar file
     * @return all classes in a given directory within a jar
     * @throws ClassNotFoundException
     */
    private static List<Class> findClassesInJarDir(URL jarDir) throws IOException, ClassNotFoundException {
        List<Class> classes = new ArrayList<Class>();

        String[] split = jarDir.getFile().split("!/");
        String jarFilename = split[0];
        String dirName = split[1];

        JarFile jar = new JarFile(jarFilename.replaceAll("file:", ""));
        Enumeration<JarEntry> entries = jar.entries();
        while (entries.hasMoreElements()) {
            JarEntry entry = entries.nextElement();
            if (entry.getName().startsWith(dirName) && entry.getName().endsWith(".class")) {
                String classname = entry.getName().replaceAll("/", ".");
                classname = classname.substring(0, classname.length() - 6);
                classes.add(classForName(classname));
            }
        }

        return classes;
    }

    /**
     * Recursive method used to find all classes in a given directory and subdirs.
     *
     * @param directory   The base directory
     * @param packageName The package name for classes found inside the base directory
     * @return The classes
     * @throws ClassNotFoundException
     */
    private static List<Class> findClassesInDir(File directory, String packageName) throws ClassNotFoundException {
        List<Class> classes = new ArrayList<Class>();
        if (!directory.exists()) {
            return classes;
        }
        File[] files = directory.listFiles();
        for (File file : files) {
            if (file.isDirectory()) {
                assert !file.getName().contains(".");
                classes.addAll(findClassesInDir(file, packageName + "." + file.getName()));
            } else if (file.getName().endsWith(".class") && !file.getName().contains("<error>")) {
                classes.add(classForName(packageName + '.' + file.getName().substring(0, file.getName().length() - 6)));
            }
        }
        return classes;
    }

    @Override
    public Class getClassForName(String className) throws ClassNotFoundException {
        return classForName(className);
    }

    public static Class classForName(String className) throws ClassNotFoundException {
        return Class.forName(className, true, Thread.currentThread().getContextClassLoader());
    }

}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.metadata.EntityPropertyMdFactory;
import java.io.Serializable;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;
import org.apache.commons.beanutils.PropertyUtils;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.dw.exception.OnReceiveProcessingException;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.model.annotation.UseInTitle;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 * This is a base class of all entity beans used in this project.
 * @author Evka
 */
//@EJB(name="ejb/EntityDao", beanInterface=org.cern.cms.csc.dw.dao.EntityDaoLocal.class)
public class EntityBase implements Serializable {

    private static Logger logger = Logger.getLogger(EntityBase.class.getName());

    /** Class property metadata cache. */
    private static Map<Class, List<PropertyMd>> propertyMetadataCache = new HashMap<Class, List<PropertyMd>>();
    /** Cache of class -> names of the fields annotated by @UseInTitle. */
    private static Map<Class, List<String>> titleFieldNamesCache = new HashMap<Class, List<String>>();

    public void onSave(EntityDaoLocal eDao, boolean queued) throws OnSaveProcessingException { }
    public void onReceive(EntityDaoLocal eDao) throws OnReceiveProcessingException { }

    /**
     * Get entity class property metadata (it's cached in a static map called propertyMetadataCache).
     * @return entity class property metadata (it's cached in a static map called propertyMetadataCache).
     */
    public List<PropertyMd> getPropertyMetadata() throws InvalidEntityBeanPropertyException {
        Class myClass = this.getClass();
        if (!propertyMetadataCache.containsKey(myClass)) {
            try {
                propertyMetadataCache.put(myClass, EntityPropertyMdFactory.createMetadataForEntity(myClass));
            } catch(InvalidEntityClassException ex) {} // will never happen
        }

        return propertyMetadataCache.get(myClass);
    }

    /**
     * Get title of the entity which is constructed from all fields annotated with @UseInTitle.
     * If there are no such fields, toString() is returned.
     * @return title of the entity which is constructed from all fields annotated with @UseInTitle. If there are no such fields, toString() is returned.
     */
    public String getEntityTitle() throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
        List<String> titleFieldNames = getTitleFieldNames();
        if (titleFieldNames.isEmpty()) {
            return toString();
        }

        String title = "";
        for (String titleFieldName: titleFieldNames) {
            if (!title.isEmpty()) {
                title += "; ";
            }
            title += PropertyUtils.getProperty(this, titleFieldName);
        }
        return title;
    }

    protected List<String> getTitleFieldNames() {
        Class myClass = this.getClass();
        if (!titleFieldNamesCache.containsKey(myClass)) {
            Field[] fields = this.getClass().getDeclaredFields();
            List<String> titleFieldNames = new ArrayList<String>();
            int numberOfFieldsFound = 0;
            for (Field field: fields) {
                UseInTitle useInTitleA = field.getAnnotation(UseInTitle.class);
                if (useInTitleA != null) {
                    titleFieldNames.add(useInTitleA.order() - 1, field.getName());
                    numberOfFieldsFound++;
                }
            }
            if (numberOfFieldsFound != titleFieldNames.size()) {
                throw new RuntimeException("There is something wrong with @UseInTitle annotations in class " + myClass.getName() +
                                           ". Check the order - it should start with 1 and there should be no gaps between subsequent indices");
            }
            titleFieldNamesCache.put(myClass, titleFieldNames);
        }

        return titleFieldNamesCache.get(myClass);
    }

}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.beans.PropertyDescriptor;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.metadata.EntityPropertyMdFactory;
import java.io.Serializable;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.apache.commons.beanutils.PropertyUtils;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.dw.exception.OnReceiveProcessingException;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.model.annotation.gui.UseInTitle;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 * This is a base class of all entity beans used in this project.
 * @author Evka
 */
//@EJB(name="ejb/EntityDao", beanInterface=org.cern.cms.csc.dw.dao.EntityDaoLocal.class)
public class EntityBase implements Serializable {

    private static Logger logger = SimpleLogger.getLogger(EntityBase.class);

    /** Class property metadata cache. */
    private static Map<Class, List<PropertyMd>> propertyMetadataCache = new HashMap<Class, List<PropertyMd>>();
    /** Cache of class -> names of the fields annotated by @UseInTitle. */
    private static Map<Class, List<String>> titleFieldNamesCache = new HashMap<Class, List<String>>();
    /** Cach of class -> ID property - used by equals and hashCode methods */
    private static Map<Class, PropertyDescriptor> idPropertyCache = new HashMap<Class, PropertyDescriptor>();

    public EntityBase() {
    }

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
        String title = "";

        UseInTitle useClassInTitleA = this.getClass().getAnnotation(UseInTitle.class);
        if (useClassInTitleA != null) {
            title = this.getClass().getSimpleName();
        }

        List<String> titleFieldNames = getTitleFieldNames();
        if (title.isEmpty() && titleFieldNames.isEmpty()) {
            return toString();
        } else if (titleFieldNames.isEmpty()) {
            return title;
        }
        
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


    public boolean equals(Object object) {
        if (object == null) {
            return false;
        }
        if (!(this.getClass().equals(object.getClass()))) {
            return false;
        }
        if (this == object) {
            return true;
        }
        EntityBase that = (EntityBase) object;
        PropertyDescriptor myIdProp = getIdPropertyMd();
        PropertyDescriptor thatIdProp = that.getIdPropertyMd();
        if ((myIdProp == null) || (thatIdProp == null)) {
            return super.equals(object);
        }
        try {
            Object myId = myIdProp.getReadMethod().invoke(this);
            Object thatId = thatIdProp.getReadMethod().invoke(that);

            if ((myId == null) || (thatId == null)){
                return super.equals(object);
            }

            return myId.equals(thatId);
        } catch (Exception ex) {
            return super.equals(object);
        }
    }

    public int hashCode() {
        PropertyDescriptor myIdProp = getIdPropertyMd();
        if (myIdProp == null) {
            return super.hashCode();
        }
        try {
            Object myId = myIdProp.getReadMethod().invoke(this);
            if (myId == null) {
                return super.hashCode();
            }

            return myId.hashCode();
        } catch (Exception ex) {
            return super.hashCode();
        }
    }

    public Object getEntityId() {
        try {
            return getIdPropertyMd().getReadMethod().invoke(this);
        } catch (Exception ex) {
            logger.error("Exception while trying to get entity ID", ex);
            return null;
        }
    }

    public PropertyDescriptor getIdPropertyMd() {
        Class myClass = this.getClass();
        if (!idPropertyCache.containsKey(myClass)) {
            try {
                PropertyDescriptor[] allProps = PropertyUtils.getPropertyDescriptors(myClass);
                boolean found = false;
                for (PropertyDescriptor prop: allProps) {
                    if (prop.getName().equalsIgnoreCase("id")) {
                        idPropertyCache.put(myClass, prop);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    idPropertyCache.put(myClass, null);
                }
            } catch (Exception ex) {
                idPropertyCache.put(myClass, null);
            }
        }
        return idPropertyCache.get(myClass);
    }
}

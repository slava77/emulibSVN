/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui;

import java.beans.PropertyDescriptor;
import java.io.Serializable;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.persistence.Id;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlTransient;
import jsf.bean.gui.annotation.UseInTitle;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.exception.InvalidEntityClassException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.EntityPropertyMdFactory;
import jsf.bean.gui.metadata.PropertyMd;
import org.apache.commons.beanutils.PropertyUtils;

/**
 * This is a base class of all entity beans used in this project.
 * @author Evka
 */
//@EJB(name="ejb/EntityDao", beanInterface=org.cern.cms.csc.dw.dao.EntityDaoLocal.class)
public class EntityBeanBase implements Serializable {

    private static Logger logger = SimpleLogger.getLogger(EntityBeanBase.class);

    /** Class property metadata cache. */
    @Transient
    private static Map<Class, List<PropertyMd>> propertyMetadataCache = new HashMap<Class, List<PropertyMd>>();

    /** Cache of class -> names of the fields annotated by @UseInTitle. */
    @Transient
    private static Map<Class, List<String>> titleFieldNamesCache = new HashMap<Class, List<String>>();

    /** Cach of class -> ID property - used by equals and hashCode methods */
    @Transient
    private static Map<Class, PropertyDescriptor> idPropertyCache = new HashMap<Class, PropertyDescriptor>();

    public EntityBeanBase() { }

    /**
     * Get entity class property metadata (it's cached in a static map called propertyMetadataCache).
     * @return entity class property metadata (it's cached in a static map called propertyMetadataCache).
     */
    @Transient
    public List<PropertyMd> getPropertyMetadata() throws InvalidEntityBeanPropertyException {
        return getPropertyMetadata(this.getClass());
    }

    public static List<PropertyMd> getPropertyMetadata(Class myClass) throws InvalidEntityBeanPropertyException {
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
    @Transient
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


    @Override
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
        EntityBeanBase that = (EntityBeanBase) object;
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

    @Override
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

    @Transient
    public Object getEntityId() {
        try {
            return getIdPropertyMd().getReadMethod().invoke(this);
        } catch (Exception ex) {
            logger.error("Exception while trying to get entity ID", ex);
            return null;
        }
    }

    @Transient
    public PropertyDescriptor getIdPropertyMd() {
        return getIdPropertyMd(this.getClass());
    }

    public static PropertyDescriptor getIdPropertyMd(Class myClass) {
        if (!idPropertyCache.containsKey(myClass)) {
            try {
                PropertyDescriptor[] allProps = PropertyUtils.getPropertyDescriptors(myClass);
                boolean found = false;
                for (PropertyDescriptor prop: allProps) {
                    if (prop.getReadMethod().isAnnotationPresent(Id.class)) {
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

    /**
     * Row (item) selection indicator in bean table
     */
    private Boolean selected = false;

    /**
     * Get indication if this item was selected
     * @return
     */
    @Transient
    @XmlTransient
    public final Boolean getSelected() {
        return selected;
    }

    /**
     * Set item selection indicator
     * @param selected
     */
    public final void setSelected(Boolean selected) {
        this.selected = selected;
    }

}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.dw.exception.OnReceiveProcessingException;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 * This is a base class of all entity beans used in this project.
 * @author Evka
 */
@EJB(name="ejb/EntityDao", beanInterface=org.cern.cms.csc.dw.dao.EntityDaoLocal.class)
public class EntityBase implements Serializable {

    private static Logger logger = Logger.getLogger(EntityBase.class.getName());

    private static Map<Class, List<EntityPropertyMD>> propertyMetadataCache = new HashMap<Class, List<EntityPropertyMD>>();

    private List<EntityProperty> properties = null;

    public void onSave(EntityDaoLocal eDao, boolean queued) throws OnSaveProcessingException { }
    public void onReceive(EntityDaoLocal eDao) throws OnReceiveProcessingException { }

    /**
     * Get entity properties - a list of objects of class EntityProperty representing properties of this particular instance.
     * @return entity properties - a list of objects of class EntityProperty representing properties of this particular instance.
     * @see org.cern.cms.csc.dw.model.base.EntityProperty
     */
    public List<EntityProperty> getProperties() throws InvalidEntityBeanPropertyException {
        // lazy initialization
        if (properties == null) {
            List<EntityProperty> props = new ArrayList<EntityProperty>();
            List<EntityPropertyMD> propertyMetadata = getPropertyMetadata();
            for (EntityPropertyMD metadataRecord: propertyMetadata) {
                Class clazz = metadataRecord.getType();
                EntityProperty prop = new EntityProperty(this, metadataRecord);
                props.add(prop);
            }
            this.properties = props;
        }
        return properties;
    }

    /**
     * Get entity class property metadata (it's cached in a static map called propertyMetadataCache).
     * @return entity class property metadata (it's cached in a static map called propertyMetadataCache).
     */
    public List<EntityPropertyMD> getPropertyMetadata() throws InvalidEntityBeanPropertyException {
        Class myClass = this.getClass();
        if (!propertyMetadataCache.containsKey(myClass)) {
            try {
                propertyMetadataCache.put(myClass, EntityPropertyMDFactory.createMetadataForEntity(myClass));
            } catch(InvalidEntityClassException ex) {} // will never happen
        }

        return propertyMetadataCache.get(myClass);
    }
}

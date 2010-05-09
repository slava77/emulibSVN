/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.io.Serializable;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.OnReceiveProcessingException;
import org.cern.cms.csc.dw.exception.OnSaveProcessingException;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 * This is a base class of all entity beans used in this project.
 * @author Evka
 */
public class EntityBase implements Serializable {

    private static Logger logger = Logger.getLogger(EntityBase.class.getName());

    private static Map<Class, List<EntityPropertyMetadata>> propertyMetadataCache = new HashMap<Class, List<EntityPropertyMetadata>>();
    private static Pattern ignoredGettersPattern = Pattern.compile("(isSet|getClass|getPropertyMetadata)(.*)");
    private static Pattern itemGetterPattern = Pattern.compile("(get\\p{Upper}.+)Item");

    public void onSave(EntityDaoLocal eDao, boolean queued) throws OnSaveProcessingException { }
    public void onReceive(EntityDaoLocal eDao) throws OnReceiveProcessingException { }

    /**
     * Get entity class property metadata (it's cached in a static map called propertyMetadataCache).
     * @return entity class property metadata (it's cached in a static map called propertyMetadataCache).
     */
    public List<EntityPropertyMetadata> getPropertyMetadata() throws InvalidEntityBeanPropertyException {
        Class myClass = this.getClass();
        if (!propertyMetadataCache.containsKey(myClass)) {
            List<EntityPropertyMetadata> metadata = new ArrayList<EntityPropertyMetadata>();

            // === Find out and fill in metadata ===
            Method[] allMethods = myClass.getMethods();
            // Filter out all unwanted methods:
            // * Remove all get* getters which have equivalent get*Item getter
            // * all those which don't match getterPattern
            // * all those which match ignoredGettersPattern
            List<String> methodNamesToRemove = new ArrayList<String>();
            for (Method method: allMethods) {
                Matcher m = itemGetterPattern.matcher(method.getName());
                if (m.matches()) {
                    methodNamesToRemove.add(m.group(1));
                }
            }
            List<Method> getters = new ArrayList<Method>();
            for (Method method: allMethods) {
                if (!methodNamesToRemove.contains(method.getName()) &&
                     EntityPropertyMetadata.getterPattern.matcher(method.getName()).matches() &&
                    !ignoredGettersPattern.matcher(method.getName()).matches()) {
                    
                    getters.add(method);
                }
            }

            // Go through all methods and make property metadata out of the getters
            for (Method getter: getters) {
                try {
                    EntityPropertyMetadata propMeta = new EntityPropertyMetadata(getter);
                    metadata.add(propMeta);
                } catch (InvalidEntityBeanPropertyException ex) {
                    logger.log(Level.SEVERE, "Exception while constructing entity bean " + myClass.getName() + "properties metadata - skipping this property", ex);
                    throw ex;
                }
            }

            propertyMetadataCache.put(myClass, metadata);
            return metadata;
        }

        return propertyMetadataCache.get(myClass);
    }
}

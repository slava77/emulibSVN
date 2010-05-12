/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.naming.NamingException;
import javax.persistence.Basic;
import javax.persistence.ManyToOne;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class EntityPropertyMDFactory {

    private static Logger logger = Logger.getLogger(EntityPropertyMDFactory.class.getName());

    private static Pattern ignoredGettersPattern = Pattern.compile("(isSet|getClass|getPropertyMetadata|getProperties)(.*)");
    private static Pattern itemGetterPattern = Pattern.compile("(get\\p{Upper}.+)Item");

    public static List<EntityPropertyMD> createMetadataForEntity(Class entityClass) throws InvalidEntityClassException, InvalidEntityBeanPropertyException {
        List<EntityPropertyMD> metadata = new ArrayList<EntityPropertyMD>();

        // === Find out and fill in metadata ===
        Method[] allMethods = entityClass.getMethods();
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
                 EntityPropertyMD.getterPattern.matcher(method.getName()).matches() &&
                !ignoredGettersPattern.matcher(method.getName()).matches()) {

                getters.add(method);
            }
        }

        // Go through all methods and make property metadata out of the getters
        for (Method getter: getters) {
            try {
                EntityPropertyMD propMeta = createMetadataForProperty(getter);
                metadata.add(propMeta);
            } catch (InvalidEntityBeanPropertyException ex) {
                logger.log(Level.SEVERE, "Exception while constructing entity bean " + entityClass.getName() + "properties metadata - skipping this property", ex);
//                throw ex;
            }
        }

        return metadata;
    }

    public static EntityPropertyMD createMetadataForProperty(Method getter) throws InvalidEntityBeanPropertyException {
        // process the annotations and decide what type of EntityPropertyMD object to create
        Basic basicA = getter.getAnnotation(Basic.class);
        if (basicA != null) {
            return new EntityBasicPropertyMD(getter);
        }

        ManyToOne manyToOneA = getter.getAnnotation(ManyToOne.class);
        if (manyToOneA != null) {
            return new EntityManyToOnePropertyMD(getter);
        }

        throw new InvalidEntityBeanPropertyException("Don't know what type of property metadata to create for " + getter.toGenericString());
    }

}

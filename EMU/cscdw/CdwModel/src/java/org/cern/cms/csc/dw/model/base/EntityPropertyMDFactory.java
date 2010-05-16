/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.beans.PropertyDescriptor;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.persistence.Basic;
import javax.persistence.ManyToOne;
import org.apache.commons.beanutils.PropertyUtils;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class EntityPropertyMDFactory {

    private static Logger logger = Logger.getLogger(EntityPropertyMDFactory.class.getName());

    private static Pattern ignoredPropertiesPattern = Pattern.compile("(set|id|class|propertyMetadata|properties)(.*)");
    private static Pattern itemPropertyPattern = Pattern.compile("(\\p{Lower}.+)Item");

    public static List<EntityPropertyMD> createMetadataForEntity(Class entityClass) throws InvalidEntityClassException, InvalidEntityBeanPropertyException {
        // get all properties
        PropertyDescriptor[] allProps = PropertyUtils.getPropertyDescriptors(entityClass);
        // Filter out all unwanted properties:
        // * Remove all properties which have an equivalent "[propertyName]Item" property
        // * all those which match ignoredPropertyPattern
        List<String> propNamesToRemove = new ArrayList<String>();
        for (PropertyDescriptor prop: allProps) {
            Matcher m = itemPropertyPattern.matcher(prop.getName());
            if (m.matches()) {
                propNamesToRemove.add(m.group(1));
            }
        }

        List<PropertyDescriptor> props = new ArrayList<PropertyDescriptor>();
        for (PropertyDescriptor prop: allProps) {
            if (!propNamesToRemove.contains(prop.getName()) &&
                !ignoredPropertiesPattern.matcher(prop.getName()).matches()) {

                props.add(prop);
            }
        }


        for (PropertyDescriptor prop: props) {
            logger.info("property: " + prop.getName());
        }

        List<EntityPropertyMD> metadata = new ArrayList<EntityPropertyMD>();
        // Go through all properties and make property metadata objects out of them
        for (PropertyDescriptor prop: props) {
            try {
                EntityPropertyMD propMeta = createMetadataForProperty(prop);
                metadata.add(propMeta);
            } catch (InvalidEntityBeanPropertyException ex) {
                logger.log(Level.SEVERE, "Exception while constructing entity bean " + entityClass.getName() + "properties metadata - skipping this property", ex);
//                throw ex;
            }
        }
        return metadata;
    }

    public static EntityPropertyMD createMetadataForProperty(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        // process the annotations and decide what type of EntityPropertyMD object to create
        Method getter = prop.getReadMethod();
        Basic basicA = getter.getAnnotation(Basic.class);
        if (basicA != null) {
            return new EntityBasicPropertyMD(prop);
        }

        ManyToOne manyToOneA = getter.getAnnotation(ManyToOne.class);
        if (manyToOneA != null) {
            return new EntityManyToOnePropertyMD(prop);
        }

        throw new InvalidEntityBeanPropertyException("Don't know what type of property metadata to create for " + getter.toGenericString());
    }

}

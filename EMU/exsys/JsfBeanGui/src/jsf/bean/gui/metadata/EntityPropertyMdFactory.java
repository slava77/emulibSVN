/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.metadata;

import java.beans.PropertyDescriptor;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.Transient;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.exception.InvalidEntityClassException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.apache.commons.beanutils.PropertyUtils;

/**
 *
 * @author Evka
 */
public class EntityPropertyMdFactory {

    private static Logger logger = SimpleLogger.getLogger(EntityPropertyMdFactory.class);

    private static Pattern ignoredPropertiesPattern = Pattern.compile("(set|id|entityId|class|propertyMetadata|properties|entityTitle|metadata|componentId)(.*)");
    private static Pattern itemPropertyPattern = Pattern.compile("(\\p{Lower}.+)Item");

    /**
     * Creates metadata for all the properties of the given entity class (which must be a subclass of EntityBase)
     * @param entityClass entity class you want the metadata for
     * @return list of PropertyMd for all the properties of the given entity class
     * @throws InvalidEntityClassException thrown if the given entityClass is not a subclass of EntityBase
     * @throws InvalidEntityBeanPropertyException thrown if there's a property for which it's not possible to create an PropertyMd object (or at least it's not defined how to create it)
     */
    public static List<PropertyMd> createMetadataForEntity(Class entityClass) throws InvalidEntityClassException, InvalidEntityBeanPropertyException {
        // get all properties
        PropertyDescriptor[] allProps = PropertyUtils.getPropertyDescriptors(entityClass);
        Map<String, PropertyDescriptor>  allPropsMap = new HashMap<String, PropertyDescriptor>();
        for (PropertyDescriptor prop: allProps) {
            allPropsMap.put(prop.getName(), prop);
        }

        // Filter out all unwanted properties:
        // * Remove all properties which have an equivalent "[propertyName]Item" property
        // * all those which match ignoredPropertyPattern
        List<String> propNamesToRemove = new ArrayList<String>();
        // properties in this list should be included unconditionally (e.g. even if they're transient), this is used to e.g. include transient enums
        List<String> propNamesToIncludeAlways = new ArrayList<String>();
        for (PropertyDescriptor prop: allProps) {
            if (prop.getReadMethod() == null) {
                propNamesToRemove.add(prop.getName());
            }
            Matcher m = itemPropertyPattern.matcher(prop.getName());
            if (m.matches()) {
                String nonItemPropName = m.group(1);
                // for enums take the simple property, for others, take the *Item property
                if ((allPropsMap.containsKey(nonItemPropName)) && (allPropsMap.get(nonItemPropName).getPropertyType().isEnum())) {
                    propNamesToRemove.add(prop.getName());

                    if (!allPropsMap.get(prop.getName()).getReadMethod().isAnnotationPresent(Transient.class)) {
                        propNamesToIncludeAlways.add(nonItemPropName);
                    }
                    // determine if it's mandatory (I know - it's ugly, but I just can't think of a nicer way right now...
                    Column columnA = allPropsMap.get(prop.getName()).getReadMethod().getAnnotation(Column.class);
                    if ((columnA != null) && (!columnA.nullable())) {
                        allPropsMap.get(nonItemPropName).setShortDescription("mandatory");
                    }
                } else {
                    propNamesToRemove.add(m.group(1));
                }
            }
        }

        List<PropertyDescriptor> props = new ArrayList<PropertyDescriptor>();
        for (PropertyDescriptor prop: allProps) {
            if ((!propNamesToRemove.contains(prop.getName()) && // not in the to-remove list
                !ignoredPropertiesPattern.matcher(prop.getName()).matches() && // doesn't match with ignore pattern
                (prop.getReadMethod().getAnnotation(Transient.class) == null))  // isn't transient
                ||
                (propNamesToIncludeAlways.contains(prop.getName()))) // OR is in the exception list - to be included unconditionally
            {
                props.add(allPropsMap.get(prop.getName()));
            }
        }

        List<PropertyMd> metadata = new ArrayList<PropertyMd>();
        // Go through all properties and make property metadata objects out of them
        for (PropertyDescriptor prop: props) {
            try {
                PropertyMd propMeta = createMetadataForProperty(prop);
                if (propMeta != null) {
                    metadata.add(propMeta);
                }
            } catch (InvalidEntityBeanPropertyException ex) {
                logger.warn("Exception while constructing entity bean " + entityClass.getName() + " properties metadata (" + ex.getMessage() + ") - skipping this property");
            }
        }
        return metadata;
    }

    /**
     * Create metadata for a given property
     * @param prop property for which you want the metadata object to be created
     * @return PropertyMd for a given property
     * @throws InvalidEntityBeanPropertyException thrown if it's not possible to create an PropertyMd object for the given property (or at least it's not defined how to create it)
     */
    public static PropertyMd createMetadataForProperty(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        // process the annotations and decide what type of PropertyMd object to create
        Method getter = prop.getReadMethod();

        if (getter.isAnnotationPresent(Basic.class)) {
            return new BasicPropertyMd(prop);
        }

        if (getter.isAnnotationPresent(Embedded.class)) {
            return new EmbeddedPropertyMd(prop);
        }

        if (prop.getPropertyType().isEnum()) {
            boolean isMandatory = prop.getShortDescription().equals("mandatory");
            return new EnumPropertyMd(prop, isMandatory);
        }

        ManyToOne manyToOneA = getter.getAnnotation(ManyToOne.class);
        if (manyToOneA != null) {
            return new ManyToOnePropertyMd(prop);
        }

        OneToOne oneToOneA = getter.getAnnotation(OneToOne.class);
        if (oneToOneA != null) {
            return new OneToOnePropertyMd(prop);
        }

        ManyToMany manyToManyA = getter.getAnnotation(ManyToMany.class);
        if (manyToManyA != null) {
            return new ManyToManyPropertyMd(prop);
        }

        OneToMany oneToManyA = getter.getAnnotation(OneToMany.class);
        if (oneToManyA != null) {
            return new OneToManyPropertyMd(prop);
        }

        throw new InvalidEntityBeanPropertyException("Don't know what type of property metadata to create for " + getter.toGenericString());
    }

}

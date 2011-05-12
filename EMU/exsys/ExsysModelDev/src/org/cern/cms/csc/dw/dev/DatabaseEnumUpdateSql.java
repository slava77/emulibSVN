/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dev;

import java.beans.PropertyDescriptor;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Properties;
import java.util.Set;
import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.Table;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.apache.commons.beanutils.PropertyUtils;
import org.cern.cms.csc.dw.metadata.MetadataManager;

/**
 * Executable class that generates DML script for updating all Enumeration
 * column values from (Enum.getValue()) value to Enum.name() value.
 * Afterwards the script must be executed on the database.
 * @author valdo
 */
public class DatabaseEnumUpdateSql {

    private final static Logger logger = SimpleLogger.getLogger(DatabaseEnumUpdateSql.class);

    private final static String UPDATE_SQL = "update %1$s set %2$s = '%3$s' where %2$s = '%4$s';";

    private static final String ENTITIES_RESOURCE = MetadataManager.RESOURCE_BASE + MetadataManager.MAIN_ENTITIES_RESOURCE;
    private final Properties entities;

    public DatabaseEnumUpdateSql() throws IOException {
        entities = new Properties();
        entities.load(DatabaseEnumUpdateSql.class.getResourceAsStream(ENTITIES_RESOURCE));
    }

    public void execute() throws Exception {

        // To hold all final (unique!) queries
        Set<String> dmls = new HashSet<String>();

        // Looping over entity classes
        for (Enumeration<Object> keys = entities.keys(); keys.hasMoreElements();) {
            String name = (String) keys.nextElement();
            Class clazz = Class.forName(entities.getProperty(name));
            if (clazz.isAnnotationPresent(Table.class)) {

                // Looping over properties and filtering persistent enums
                for (PropertyDescriptor property: PropertyUtils.getPropertyDescriptors(clazz)) {
                    if (property.getPropertyType().isEnum()) {
                        Class propertyType = property.getPropertyType();
                        Method getter = property.getReadMethod();
                        if (getter.isAnnotationPresent(Column.class) &&
                            getter.isAnnotationPresent(Enumerated.class)) {

                            // Taking declaring class table! (could be in a super class wrt above)
                            Class declClass = getter.getDeclaringClass();
                            if (declClass.isAnnotationPresent(Table.class)) {
                                String declTableName = ((Table) declClass.getAnnotation(Table.class)).name();
                                String colName = ((Column) getter.getAnnotation(Column.class)).name();

                                // Looing over enum constants
                                for (Object constant: propertyType.getEnumConstants()) {
                                    Enum e = (Enum) constant;
                                    Method valueMethod = propertyType.getMethod("value");
                                    String eValue = (String) valueMethod.invoke(e);
                                    String eName = e.toString();

                                    // If enum value and name does not match, create update dml
                                    if (!eName.equals(eValue)) {
                                        dmls.add(String.format(UPDATE_SQL, declTableName, colName, eName, eValue));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Print all DML's to cout
        for (String dml: dmls) {
            System.out.println(dml);
        }

    }

    public static void main(String[] args) {

        try {

            DatabaseEnumUpdateSql generator = new DatabaseEnumUpdateSql();
            generator.execute();

        } catch (Exception ex) {
            logger.error(ex);
        }

    }

}

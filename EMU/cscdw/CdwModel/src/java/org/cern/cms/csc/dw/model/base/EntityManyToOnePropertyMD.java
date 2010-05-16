/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base;

import java.beans.PropertyDescriptor;
import java.lang.reflect.Method;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class EntityManyToOnePropertyMD extends EntityPropertyMD {

    private EntityDaoLocal entityDao;

    static private Class[] mandatoryAnnotations = {ManyToOne.class, JoinColumn.class};

    public EntityManyToOnePropertyMD(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);

        JoinColumn joinColumn = prop.getReadMethod().getAnnotation(JoinColumn.class);
        setIsMandatory(joinColumn.nullable());

        entityDao = lookupEntityDao();
    }

    @Override
    @SuppressWarnings("unchecked")
    public List<Object> getListOfValues() throws InvalidEntityClassException {
        return entityDao.getAllEntitiesByClass(getType());
    }

    // ================ implementation of the abstract methods (and overriding some) ================

    @Override
    public boolean getIsCollection() {
        return false;
    }

    @Override
    public boolean getIsTypeBasic() {
        return false;
    }

    @Override
    public PropertyRelationType getRelationType() {
        return PropertyRelationType.MANY_TO_ONE;
    }

    /**
     * Validates a given value and returns an error message if the value is not valid, otherwise returns null.
     * @param value value to be validated
     * @return an error message if the value is not valid, otherwise - null is returned.
     */
    @Override
    public String validate(Object value) {
        // super validation
        String msg = super.validate(value);
        if (msg != null) {
            return msg;
        }

        // validation successful
        return null;
    }

    private EntityDaoLocal lookupEntityDao() {
        try {
            Context c = new InitialContext();
            return (EntityDaoLocal) c.lookup("java:comp/env/ejb/EntityDao");
        } catch (NamingException ne) {
            Logger.getLogger(getClass().getName()).log(Level.SEVERE, "exception caught", ne);
            throw new RuntimeException(ne);
        }
    }
}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base.metadata;

import java.beans.PropertyDescriptor;
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
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class EntityManyToOnePropertyMD extends EntityPropertyMD {

    private static Logger logger = Logger.getLogger(EntityManyToOnePropertyMD.class.getName());
    private static Class[] mandatoryAnnotations = {ManyToOne.class, JoinColumn.class};

    private EntityDaoLocal entityDao;

    public EntityManyToOnePropertyMD(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);

        JoinColumn joinColumn = prop.getReadMethod().getAnnotation(JoinColumn.class);
        setIsMandatory(!joinColumn.nullable());

        entityDao = lookupEntityDao();
    }

    @SuppressWarnings("unchecked")
    public List<Object> getListOfValues() throws InvalidEntityClassException {
        return entityDao.getAllEntitiesByClass(getType());
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

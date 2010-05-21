/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.base.metadata;

import java.beans.PropertyDescriptor;
import java.util.logging.Logger;
import javax.persistence.JoinColumn;
import javax.persistence.OneToOne;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class EntityOneToOnePropertyMD extends EntityPropertyMD {

    private static Logger logger = Logger.getLogger(EntityManyToOnePropertyMD.class.getName());
    private static Class[] mandatoryAnnotations = {OneToOne.class, JoinColumn.class};

    public EntityOneToOnePropertyMD(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);

        JoinColumn joinColumn = prop.getReadMethod().getAnnotation(JoinColumn.class);
        setIsMandatory(!joinColumn.nullable());
    }

}
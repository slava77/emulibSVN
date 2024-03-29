/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.metadata;

import java.beans.PropertyDescriptor;
import javax.persistence.JoinColumn;
import javax.persistence.OneToOne;
import org.apache.log4j.Logger;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class OneToOnePropertyMd extends PropertyMd {

    private static Logger logger = Logger.getLogger(ManyToOnePropertyMd.class);
    private static Class[] mandatoryAnnotations = {OneToOne.class, JoinColumn.class};

    public OneToOnePropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);

        JoinColumn joinColumn = prop.getReadMethod().getAnnotation(JoinColumn.class);
        setIsMandatory(!joinColumn.nullable());
    }

}

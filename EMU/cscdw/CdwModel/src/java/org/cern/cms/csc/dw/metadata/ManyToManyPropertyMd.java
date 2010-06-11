/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.metadata;

import java.beans.PropertyDescriptor;
import javax.persistence.ManyToMany;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class ManyToManyPropertyMd extends RestrictedPropertyMd {

    private static Class[] mandatoryAnnotations = {ManyToMany.class};

    public ManyToManyPropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);
        setIsMandatory(false);
    }

}

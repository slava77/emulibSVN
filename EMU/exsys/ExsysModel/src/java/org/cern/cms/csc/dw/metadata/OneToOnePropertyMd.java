/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.metadata;

import java.beans.PropertyDescriptor;
import javax.persistence.JoinColumn;
import javax.persistence.OneToOne;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

/**
 *
 * @author Evka
 */
public class OneToOnePropertyMd extends PropertyMd {

    private static Logger logger = SimpleLogger.getLogger(ManyToOnePropertyMd.class);
    private static Class[] mandatoryAnnotations = { OneToOne.class }; //, JoinColumn.class};

    public OneToOnePropertyMd(PropertyDescriptor prop) throws InvalidEntityBeanPropertyException {
        super(prop, mandatoryAnnotations);

        JoinColumn joinColumn = prop.getReadMethod().getAnnotation(JoinColumn.class);

        // TODO: determine if mandatory!
        if (joinColumn != null) {
            setIsMandatory(!joinColumn.nullable());
        }
    }

}

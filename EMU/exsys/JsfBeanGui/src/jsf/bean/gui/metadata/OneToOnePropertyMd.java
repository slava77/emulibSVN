/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.metadata;

import java.beans.PropertyDescriptor;
import javax.persistence.JoinColumn;
import javax.persistence.OneToOne;
import jsf.bean.gui.exception.InvalidEntityBeanPropertyException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

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

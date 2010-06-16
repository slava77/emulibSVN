/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion.factory;

import com.espertech.esper.client.EPServiceProvider;
import com.espertech.esper.client.EPStatement;
import com.espertech.esper.client.EventBean;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.cern.cms.csc.exsys.re.model.ConclusionType;

/**
 *
 * @author Evka
 */
public class DefaultConclusionFactory extends ConclusionFactory {

    private static Logger logger = Logger.getLogger(ConclusionFactory.class.getName());
    
    /**
     * @param conclType conclusion type that this factory is supposed to use to create conclusions.
     */
    public DefaultConclusionFactory(ConclusionType conclType) {
        super(conclType);
    }

    /**
     * This method is called by Esper runtime whenever a rule is triggered
     */
    public void update(EventBean[] newEvents, EventBean[] oldEvents,
                       EPStatement statement, EPServiceProvider epService) {

        logger.fine("Default conclusion factory: Got a conclusion for statement " +
                     statement.getName() +
                     ": " + statement.getText());
        logger.fine("Default conclusion factory: Number of new events provided: " + newEvents.length);
        if (oldEvents != null) {
            logger.fine("Default conclusion factory: Number of old events provided: " + oldEvents.length);
        } else {
            logger.fine("Default conclusion factory: no old events provided");
        }

        try {
            int i=0;
            for (EventBean event: newEvents) {
                logger.fine("Default conclusion factory: conclusion title for new event #" + i++ + ":" +
                             substituteParams(getConclusionType().getTitle(), event));
            }
            if (oldEvents != null) {
                i=0;
                for (EventBean event: oldEvents) {
                    logger.fine("Default conclusion factory: conclusion title for old event #" + i++ + ":" +
                                 substituteParams(getConclusionType().getTitle(), event));
                }
            }
        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Exception in DefaultConclusionFactory: ", ex);
        }
    }
}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion.maker;

import com.espertech.esper.client.EPServiceProvider;
import com.espertech.esper.client.EPStatement;
import com.espertech.esper.client.EventBean;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author evka
 */
public class DefaultConclusionMaker extends ConclusionMaker {

    private static Logger logger = Logger.getLogger(DefaultConclusionMaker.class.getName());

    public DefaultConclusionMaker(String type, String title, String description) {
        super(type, title, description);
    }

    public void update(EventBean[] newEvents, EventBean[] oldEvents,
                       EPStatement statement, EPServiceProvider epService) {

        logger.fine("Default conclusion maker: Got a conclusion for statement " +
                     statement.getName() +
                     ": " + statement.getText());
        logger.fine("Default conclusion maker: Number of new events provided: " + newEvents.length);
        if (oldEvents != null) {
            logger.fine("Default conclusion maker: Number of old events provided: " + oldEvents.length);
        } else {
            logger.fine("Default conclusion maker: no old events supplied");
        }

        try {
            int i=0;
            for (EventBean event: newEvents) {
                logger.fine("Default conclusion maker: conclusion title for new event #" + i++ + ":" +
                             substituteParams(getTitle(), event));
            }
            if (oldEvents != null) {
                i=0;
                for (EventBean event: oldEvents) {
                    logger.fine("Default conclusion maker: conclusion title for old event #" + i++ + ":" +
                                 substituteParams(getTitle(), event));
                }
            }
        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Exception in DefaultConclusionMaker: ", ex);
        }
    }
}

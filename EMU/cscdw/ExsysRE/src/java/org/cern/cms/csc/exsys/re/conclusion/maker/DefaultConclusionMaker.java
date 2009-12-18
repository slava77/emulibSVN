/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion.maker;

import com.espertech.esper.client.EPServiceProvider;
import com.espertech.esper.client.EPStatement;
import com.espertech.esper.client.EventBean;

/**
 *
 * @author evka
 */
public class DefaultConclusionMaker extends ConclusionMaker {

    public DefaultConclusionMaker(String type, String title, String description) {
        super(type, title, description);
    }

    public void update(EventBean[] newEvents, EventBean[] oldEvents,
                       EPStatement statement, EPServiceProvider epService) {

        System.out.println("Default conclusion maker: Got a conclusion for statement " +
                            statement.getName() +
                            ": " + statement.getText());
        System.out.println("Default conclusion maker: Number of new events provided: " + newEvents.length);
        if (oldEvents != null) {
            System.out.println("Default conclusion maker: Number of old events provided: " + oldEvents.length);
        } else {
            System.out.println("Default conclusion maker: no old events supplied");
        }

        try {
            int i=0;
            for (EventBean event: newEvents) {
                System.out.println("Default conclusion maker: conclusion title for new event #" + i++ + ":" +
                                   substituteParams(getTitle(), event));
            }
            if (oldEvents != null) {
                i=0;
                for (EventBean event: oldEvents) {
                    System.out.println("Default conclusion maker: conclusion title for old event #" + i++ + ":" +
                                       substituteParams(getTitle(), event));
                }
            }
        } catch (Exception ex) {
            System.out.println("some exception happened :) ");
            ex.printStackTrace();
        }
    }
}

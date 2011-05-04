/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.util;

import java.util.Calendar;
import java.util.Date;
import javax.xml.bind.DatatypeConverter;

/**
 *
 * @author valdo
 */
public class JaxbDateConverter {

    public static Date parseDate(String value) {
        return DatatypeConverter.parseDateTime(value).getTime();
    }

    public static String printDate(Date value) {
        Calendar c = Calendar.getInstance();
        c.setTime(value);
        return DatatypeConverter.printDateTime(c);
    }

}

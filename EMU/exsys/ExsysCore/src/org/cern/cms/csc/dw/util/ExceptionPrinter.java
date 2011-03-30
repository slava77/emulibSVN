/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.util;

/**
 *
 * @author evka
 */
public class ExceptionPrinter {

    public static String printStackTraceToString(final Throwable throwable) {
        Throwable th = throwable;
        StringBuilder ths = new StringBuilder();
        while (th != null) {
            ths.append(th.getClass().getName());
            ths.append(": ");
            ths.append(th.getMessage());
            ths.append("\n");
            StackTraceElement[] stackTrace = th.getStackTrace();
            for (StackTraceElement stackTraceEl: stackTrace) {
                ths.append("    at ");
                ths.append(stackTraceEl.toString());
                ths.append("\n");
            }

            th = th.getCause();
        }
        return ths.toString();
    }

}

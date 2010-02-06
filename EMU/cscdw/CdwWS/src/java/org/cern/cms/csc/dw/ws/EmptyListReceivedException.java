package org.cern.cms.csc.dw.ws;

public class EmptyListReceivedException extends IllegalArgumentException {

    private final static String MSG = "No [%s] items received in [%s] (empty list)";

    public EmptyListReceivedException(String argument, String itemName) {
        super(String.format(MSG, itemName, argument));
    }
}

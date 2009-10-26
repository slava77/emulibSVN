/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.knowledgebase;

/**
 *
 * @author Evaldas Juska
 */
public class KnowledgeBaseException extends Exception {

    /**
     * Creates a new instance of <code>KnowledgeBaseException</code> without detail message.
     */
    public KnowledgeBaseException() {
    }


    /**
     * Constructs an instance of <code>KnowledgeBaseException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public KnowledgeBaseException(String msg) {
        super(msg);
    }

    /**
     * Constructs an instance of <code>KnowledgeBaseException</code> with the specified detail message.
     * @param msg the detail message.
     */
    public KnowledgeBaseException(String msg, Throwable cause) {
        super(msg, cause);
    }

    /**
     * Constructs an instance of <code>KnowledgeBaseException</code> with the specified detail message and cause.
     * @param msg the detail message.
     */
    public KnowledgeBaseException(Throwable cause) {
        super(cause);
    }
}

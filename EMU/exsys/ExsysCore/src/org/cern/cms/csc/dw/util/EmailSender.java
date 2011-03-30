/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.util;

import javax.mail.Message.RecipientType;
import javax.mail.MessagingException;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;

/**
 *
 * @author evka
 */
public class EmailSender {

    private static Session getMailcern() throws NamingException {
        Context c = new InitialContext();
        return (Session) c.lookup("mail/cern");
    }

    public static void send(String email, String subject, String body) throws NamingException, MessagingException {
        Session mailcern = getMailcern();
        MimeMessage message = new MimeMessage(mailcern);
        message.setSubject(subject);
        message.setRecipients(RecipientType.TO, InternetAddress.parse(email, false));
        message.setText(body);
        Transport.send(message);
    }
}

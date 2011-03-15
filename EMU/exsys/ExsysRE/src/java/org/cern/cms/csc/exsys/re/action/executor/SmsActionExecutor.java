/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.action.executor;

import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.mail.Address;
import javax.mail.Message.RecipientType;
import javax.mail.MessagingException;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.exsys.re.action.ActionExecutor;
import org.cern.cms.csc.exsys.re.exception.ActionExecutionException;
import org.cern.cms.csc.exsys.re.model.ActionExecution;
import org.cern.cms.csc.exsys.re.model.SmsAction;

/**
 *
 * @author evka
 */
public class SmsActionExecutor extends ActionExecutor {

    private static final Logger logger = SimpleLogger.getLogger(EmailActionExecutor.class);
    private static final Pattern cernNumberPattern = Pattern.compile("(16|076487|004176487|\\+4176487)(\\d{4})");

    public SmsActionExecutor(ActionExecution actionExec) {
        super(actionExec);
    }

    @Override
    protected void execute(ActionExecution actionExec) throws ActionExecutionException {
        try {
            SmsAction smsAction = (SmsAction) actionExec.getAction();
            logger.info("Sending SMS to " + smsAction.getRecipients() +
                       " (action name = \"" + smsAction.getName() +
                       "\",action execution ID=" + actionExec.getid() + ")");
            sendSms(smsAction.getRecipients(), 
                    actionExec.getTrigger().getConclusion().getTitle(),
                    actionExec.getTrigger().getType().toString());
        } catch (Exception ex) {
            throw new ActionExecutionException("Could not send SMS", ex);
        }
    }

    private String phoneNumbersToEmails(String phoneNumbers) {
        String[] phoneNumbersSplit = phoneNumbers.split(",");
        StringBuilder emails = new StringBuilder();

        for (int i=0; i < phoneNumbersSplit.length; i++) {
            if (i > 0) { emails.append(","); }

            String number = phoneNumbersSplit[i];
            Matcher m = cernNumberPattern.matcher(number);
            if (m.matches()) {
                emails.append("004176487");
                emails.append(m.group(2));
            } else {
                emails.append(number);
            }
            emails.append("@mail2sms.cern.ch");
        }
        return emails.toString();
    }

    private Session getMailcern() throws NamingException {
        Context c = new InitialContext();
        return (Session) c.lookup("mail/cern");
    }

    private void sendSms(String phoneNumbers, String subject, String body) throws NamingException, MessagingException {
        Session mailcern = getMailcern();
        MimeMessage message = new MimeMessage(mailcern);
        message.setFrom(InternetAddress.parse("ExpertSys@cern.ch", false)[0]);
        message.setReplyTo(InternetAddress.parse("ExpertSys@cern.ch", false));
        message.setSubject(subject);
        message.setRecipients(RecipientType.TO, InternetAddress.parse(phoneNumbersToEmails(phoneNumbers), false));
        message.setText(body);
        Transport.send(message);
    }

}

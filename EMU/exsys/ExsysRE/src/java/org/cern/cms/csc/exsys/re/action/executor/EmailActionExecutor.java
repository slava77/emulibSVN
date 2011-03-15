/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.action.executor;

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
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionTriggerType;
import org.cern.cms.csc.exsys.re.model.EmailAction;

/**
 *
 * @author evka
 */
public class EmailActionExecutor extends ActionExecutor {

    private static final Logger logger = SimpleLogger.getLogger(EmailActionExecutor.class);

    public EmailActionExecutor(ActionExecution actionExec) {
        super(actionExec);
    }

    @Override
    protected void execute(ActionExecution actionExec) throws ActionExecutionException {
        try {
            EmailAction emailAction = (EmailAction) actionExec.getAction();
            Conclusion concl = actionExec.getTrigger().getConclusion();
            ConclusionTriggerType triggerType = actionExec.getTrigger().getType();
            logger.info("Sending email to " + emailAction.getRecipients() +
                       " (action name = \"" + emailAction.getName() +
                       "\",action execution ID=" + actionExec.getid() + ")");

            String subject = "Expert System: \"" + concl.getTitle() + "\" " + triggerType;

            StringBuilder body = new StringBuilder();
            body.append("Conclusion for component \"");
            body.append(concl.getComponent().getName());
            body.append("\" has been ");
            if (triggerType.equals(ConclusionTriggerType.OPEN)) {
                body.append("*opened*");
            } else if (triggerType.equals(ConclusionTriggerType.UPDATE)) {
                body.append("*updated*");
            } else if (triggerType.equals(ConclusionTriggerType.CLOSE)) {
                body.append("*closed*");
            }
            body.append(": \"");
            body.append(concl.getTitle());
            body.append("\"\n\n");
            body.append(concl.toStringDetailed());
            body.append("\n\n");
            body.append("-------------------------------------------------------\n");
            body.append("This is an automatic message from CSC Expert System\n");
            body.append("You are getting this message because your email address is registered in an automatic action \"");
            body.append(emailAction.getName());
            body.append("\"\n");
            body.append("If you think you got this email by mistake, well, you're wrong ;) Expert System is always right!");

            sendMail(emailAction.getRecipients(), subject, body.toString());
        } catch (Exception ex) {
            throw new ActionExecutionException("Could not send an email", ex);
        }
    }

    private Session getMailcern() throws NamingException {
        Context c = new InitialContext();
        return (Session) c.lookup("mail/cern");
    }

    private void sendMail(String email, String subject, String body) throws NamingException, MessagingException {
        Session mailcern = getMailcern();
        MimeMessage message = new MimeMessage(mailcern);
        message.setSubject(subject);
        message.setRecipients(RecipientType.TO, InternetAddress.parse(email, false));
        message.setText(body);
        Transport.send(message);
    }

}

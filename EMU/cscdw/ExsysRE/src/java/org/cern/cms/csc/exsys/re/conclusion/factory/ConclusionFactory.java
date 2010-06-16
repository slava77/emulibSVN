/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion.factory;

import com.espertech.esper.client.EventBean;
import com.espertech.esper.client.StatementAwareUpdateListener;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.cern.cms.csc.exsys.re.model.ConclusionType;

/**
 * @author Evaldas Juska
 * This class is abstract conclusion factory which is used to create conclusions out
 * of the results of the rules.
 */
public abstract class ConclusionFactory implements StatementAwareUpdateListener {

    private static final Pattern paramPattern = Pattern.compile("\\$(\\S+)");

    /** Conclusion type that this factory is supposed to use to create conclusions. */
    private ConclusionType conclType;

    /**
     * @param conclType conclusion type that this factory is supposed to use to create conclusions.
     */
    public ConclusionFactory(ConclusionType conclType) {
        this.conclType = conclType;
    }

    /**
     * Substitutes all parameters (having format of $paramName) in a given string with their actual values from the event
     * @param str string where the parameter values will be substituted
     * @param event event where the parameter values should be taken from
     * @return resulting string
     */
    protected String substituteParams(String str, EventBean event) {
        StringBuffer ret = new StringBuffer();
        Matcher m = paramPattern.matcher(str);
        while(m.find()) {
            String param = m.group(1);
            m.appendReplacement(ret, event.get(param).toString());
        }
        m.appendTail(ret);
        return ret.toString();
    }

    /**
     * Get conclusion type that this factory is supposed to use to create conclusions.
     * @return conclusion type that this factory is supposed to use to create conclusions.
     */
    public ConclusionType getConclusionType() {
        return conclType;
    }

}

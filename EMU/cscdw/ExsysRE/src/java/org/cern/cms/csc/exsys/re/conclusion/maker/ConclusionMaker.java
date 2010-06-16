/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion.maker;

import com.espertech.esper.client.EventBean;
import com.espertech.esper.client.StatementAwareUpdateListener;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * @author evka
 * This class is abstract conclusion maker which is used to make conclusions out
 * of the results of the rules. The generated conclusions can be displayable or
 * persistable or both. See documentation of the constructors for more information.
 */
public abstract class ConclusionMaker implements StatementAwareUpdateListener {

    private String type;
    private String title;
    private String description;
    private boolean display;
    private boolean persist;
    private static final Pattern paramPattern = Pattern.compile("\\$(\\S+)");

    /**
     * Constructor (display and persist default to true - that means that all
     * conclusions created by this conclusion maker will be displayed to the user
     * as well as persisted to the data warehouse DB)
     * @param type type of the conclusion maker (should be unique!)
     * @param title title of the created conclusions (this may include properties 
     * of the facts that are selected by the associated rule in format $factType.property).
     * @param description description of the created conclusions (this may also
     * include properties of the facts that are selected by the associated rule.
     */
    public ConclusionMaker (String type, String title, String description) {
        this.type = type;
        this.title = title;
        this.description = description;
        this.display = true;
        this.persist = true;
    }

    /**
     * Constructor
     * @param type type of the conclusion maker (should be unique!)
     * @param title title of the created conclusions (this may include properties
     * of the facts that are selected by the associated rule in format $factType.property).
     * @param description description of the created conclusions (this may also
     * include properties of the facts that are selected by the associated rule.
     * @param display if this is true then conclusions created by this conclusion maker
     * will be displayed to the user (else they will be "invisible")
     * @param persist if this is true then conclusions created by this conclusion
     * maker will be persisted to the data warehouse DB (otherwise they will be
     * runtime only - useful just to display some information to the user e.g.
     * High Voltage is ramping and will reach setpoint voltage in ##s).
     */
    public ConclusionMaker(String type, String title, String description, boolean display, boolean persist) {
        this.type = type;
        this.title = title;
        this.description = description;
        this.display = display;
        this.persist = persist;
    }

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

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }
}

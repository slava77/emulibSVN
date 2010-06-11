/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.action;

import org.cern.cms.csc.dw.model.fact.DqmCscFact;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.EntityEditorManager;

/**
 *
 * @author valdo
 */

public class CreateFact extends EntityEditorManager {

    /** Creates a new instance of CreateFact */
    public CreateFact() {
        super(new DqmCscFact());
    }

      @Override
    public String doSave()  {
        return "navRules";
    }

}

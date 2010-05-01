/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import javax.ejb.Remote;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.exsys.re.model.Conclusion;

/**
 *
 * @author Evka
 */
@Remote
public interface RuleEngineInputRemote {

    void receiveFact(Fact fact);
    void receiveConclusion(Conclusion conclusion);
    
}

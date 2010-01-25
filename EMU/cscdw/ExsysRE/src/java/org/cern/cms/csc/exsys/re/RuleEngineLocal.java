/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import javax.ejb.Local;
import org.cern.cms.csc.dw.model.Fact;
import org.cern.cms.csc.exsys.re.conclusion.maker.ConclusionMaker;

/**
 *
 * @author evka
 */
@Local
public interface RuleEngineLocal {

    void sendFact(Fact fact);

    void addRule(String rule, String ruleName, ConclusionMaker conclusionMaker);
    
}
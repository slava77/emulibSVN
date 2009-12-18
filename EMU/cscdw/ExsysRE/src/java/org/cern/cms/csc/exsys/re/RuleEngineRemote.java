/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import javax.ejb.Remote;
import org.cern.cms.csc.dw.model.FactType;

/**
 *
 * @author evka
 */
@Remote
public interface RuleEngineRemote {

    void sendFact(FactType fact);
    
}

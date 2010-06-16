/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import com.espertech.esper.client.EPRuntime;
import javax.ejb.Local;

/**
 *
 * @author evka
 */
@Local
public interface RuleEngineManagerLocal {

    EPRuntime getEsperRuntime();
    void reconfigureEsper();
    
}

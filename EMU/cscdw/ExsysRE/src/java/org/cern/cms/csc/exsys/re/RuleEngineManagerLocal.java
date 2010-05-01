/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re;

import com.espertech.esper.client.EPRuntime;
import javax.ejb.Local;
import org.cern.cms.csc.exsys.re.conclusion.maker.ConclusionMaker;

/**
 *
 * @author evka
 */
@Local
public interface RuleEngineManagerLocal {

    EPRuntime getEsperRuntime();
    
}

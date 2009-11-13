/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.cdw.model.Severity;

/**
 *
 * @author valdo
 */
@Local
public interface SeverityFacadeLocal {

    void create(Severity severity);

    void edit(Severity severity);

    void remove(Severity severity);

    Severity find(Object id);

    List<Severity> findAll();

}

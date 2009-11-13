/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.cdw.model.Domain;

/**
 *
 * @author valdo
 */
@Local
public interface DomainFacadeLocal {

    void create(Domain domain);

    void edit(Domain domain);

    void remove(Domain domain);

    Domain find(Object id);

    List<Domain> findAll();

}

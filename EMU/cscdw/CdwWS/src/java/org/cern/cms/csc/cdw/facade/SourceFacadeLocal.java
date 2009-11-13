/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.cdw.model.Source;

/**
 *
 * @author valdo
 */
@Local
public interface SourceFacadeLocal {

    void create(Source source);

    void edit(Source source);

    void remove(Source source);

    Source find(Object id);

    List<Source> findAll();

}

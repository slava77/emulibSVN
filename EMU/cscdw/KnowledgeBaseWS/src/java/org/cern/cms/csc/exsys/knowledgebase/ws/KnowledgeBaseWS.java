package org.cern.cms.csc.exsys.knowledgebase.ws;

import java.util.HashSet;
import java.util.Set;
import javax.annotation.Resource;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.ejb.Stateless;
import org.cern.cms.csc.exsys.knowledgebase.KnowledgeBase;
import org.cern.cms.csc.exsys.knowledgebase.KnowledgeBaseException;
import org.semanticweb.owl.model.OWLIndividual;

/**
 *
 * @author Evaldas Juska
 */
@WebService()
@Stateless()
public class KnowledgeBaseWS {

    @Resource(name="cmsCscKnowledgeBase")
    private KnowledgeBase knowledgeBase;
    /**
     * Web service operation
     */
    @WebMethod(operationName = "getIndividuals")
    public java.util.Set getIndividuals(@WebParam(name = "dlQuery")
    final String dlQuery) throws KnowledgeBaseException {
        Set<String> ret = new HashSet<String>();
        try {
            Set<OWLIndividual> individuals = knowledgeBase.getIndividuals(dlQuery);
            for (OWLIndividual ind : individuals) {
                ret.add(ind.toString());
            }
        } catch (Exception ex) {
            throw new KnowledgeBaseException("Could not execute query \"" + dlQuery + "\"", ex);
        }
        return ret;
    }
}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf;

import java.util.ArrayList;
import java.util.List;
import javax.ejb.EJB;
import javax.faces.convert.Converter;
import javax.faces.model.SelectItem;
import javax.naming.NamingException;
import org.cern.cms.csc.exsys.re.dao.ConclusionDaoLocal;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.convert.LovConverter;
import org.cern.cms.csc.exsys.re.gui.jsf.util.JsfBeanBase;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionType;

/**
 *
 * @author Evka
 */

@EJB(name="ConclusionDaoRef", beanInterface=org.cern.cms.csc.exsys.re.dao.ConclusionDaoLocal.class)
public class ConclusionTypes extends JsfBeanBase {

    private ConclusionDaoLocal conclDao;

    private List<ConclusionType> conclTypesCache;
    private List<SelectItem> conclTypesSICache;
    private Converter allConclusionTypesConverter;

    /** Creates a new instance of RuleEngineFacade */
    public ConclusionTypes() {
        super();
    }

    public List<SelectItem> getAllConclusionTypes() throws Exception {
        if (conclTypesSICache == null) {
            conclTypesCache = getConclusionDao().getAllConclusionTypes();
            conclTypesSICache = new ArrayList<SelectItem>();
            for (ConclusionType conclType: conclTypesCache) {
                conclTypesSICache.add(new SelectItem(conclType, conclType.getEntityTitle()));
            }
        }
        return conclTypesSICache;
    }

    public Converter getAllConclusionTypesConverter() throws Exception {
        if (allConclusionTypesConverter == null) {
            getAllConclusionTypes(); // initialize the conclTypesCache
            allConclusionTypesConverter = new LovConverter(conclTypesCache);
        }
        return allConclusionTypesConverter;
    }

    private ConclusionDaoLocal getConclusionDao() throws NamingException {
        if (conclDao == null) {
            conclDao = (ConclusionDaoLocal) getEjb("ConclusionDaoRef");
        }
        return conclDao;
    }

}

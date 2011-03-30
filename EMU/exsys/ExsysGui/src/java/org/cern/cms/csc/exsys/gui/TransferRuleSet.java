/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.gui;

import java.util.List;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.RequestScoped;
import javax.faces.convert.Converter;
import javax.transaction.UserTransaction;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.util.EjbLookup;
import org.cern.cms.csc.exsys.gui.editor.converter.LovConverter;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoRemote;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.RuleSet;

/**
 *
 * @author evka
 */
@ManagedBean
@RequestScoped
public class TransferRuleSet {

    private static final Logger logger = ExsysLogger.getLogger(TransferRuleSet.class);

    @Resource
    private UserTransaction ut;

    @EJB
    private RuleEngineDaoLocal reDao;

    private List<RuleSet> allRuleSets;
    private Converter ruleSetConverter;

    private RuleSet ruleSet;
    private String hostname;
    private String port = "3700";

    public TransferRuleSet() {
    }

    public List<RuleSet> getAllRuleSets() {
        if (allRuleSets == null) {
            allRuleSets = reDao.getAllRuleSets();
        }
        return allRuleSets;
    }

    public void setAllRuleSets(List<RuleSet> allRuleSets) {
        this.allRuleSets = allRuleSets;
    }

    public Converter getRuleSetConverter() {
        if (ruleSetConverter == null) {
            ruleSetConverter = new LovConverter(getAllRuleSets());
        }
        return ruleSetConverter;
    }

    public void setRuleSetConverter(Converter ruleSetConverter) {
        this.ruleSetConverter = ruleSetConverter;
    }

    public String getHostname() {
        return hostname;
    }

    public void setHostname(String hostname) {
        this.hostname = hostname;
    }

    public String getPort() {
        return port;
    }

    public void setPort(String port) {
        this.port = port;
    }

    public RuleSet getRuleSet() {
        return ruleSet;
    }

    public void setRuleSet(RuleSet ruleSet) {
        this.ruleSet = ruleSet;
    }

    public String doTransfer() throws Exception {
        if (ruleSet != null) {
            ut.begin();
            ruleSet = (RuleSet) reDao.getEntityDao().refreshEntity(ruleSet);
            for (ConclusionType ct: ruleSet.getConclusionTypes()) { // load lazy relations
                for (Rule r: ct.getRules()) {
                    r.getConclusionType();
                }
            }
            ut.commit();

            logger.info("TransferRuleSet: rule set prapered for transfer");
            logger.info("TransferRuleSet: acquiring an instance of RuleEngineDaoRemote on " + hostname + ":" + Integer.parseInt(port));

            EjbLookup<RuleEngineDaoRemote> reDaoRemote = new EjbLookup<RuleEngineDaoRemote>(RuleEngineDaoRemote.class,
                                                                                            hostname,
                                                                                            Integer.parseInt(port));

            logger.info("TransferRuleSet: instance of RuleEngineDaoRemote on " + hostname + ":" + Integer.parseInt(port) + " has been acquired");
            logger.info("TransferRuleSet: calling reDaoRemote.ejb().cloneAndSaveRuleSet");

            reDaoRemote.ejb().cloneAndSaveRuleSet(ruleSet);
            
            logger.info("TransferRuleSet: DONE");
        }
        return "reRuleSets";
    }
}

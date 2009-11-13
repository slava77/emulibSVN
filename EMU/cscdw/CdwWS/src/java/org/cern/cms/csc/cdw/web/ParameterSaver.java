/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.web;

import javax.ejb.EJB;
import ch.cern.cms.csc.exsys.wsdl.GetParameterPortType;
import javax.ejb.Stateless;
import javax.jws.WebService;
import org.cern.cms.csc.cdw.model.*;
import org.cern.cms.csc.cdw.facade.*;
import java.util.Date;
import java.math.BigDecimal;

/**
 *
 * @author valdo
 */
@WebService(serviceName = "getParameterService", portName = "getParameterPort", endpointInterface = "ch.cern.cms.csc.exsys.wsdl.GetParameterPortType", targetNamespace = "http://www.cern.ch/cms/csc/exsys/wsdl", wsdlLocation = "META-INF/wsdl/getParameter/getParameterWrapper.wsdl")
@Stateless
public class ParameterSaver implements GetParameterPortType {

    @EJB private SourceFacadeLocal sourceLocal;
    @EJB private SeverityFacadeLocal severityLocal;
    @EJB private ParameterFacadeLocal paramLocal;
    @EJB private ParamValueFacadeLocal paramValueLocal;

    public boolean getParameterOperation(ch.cern.cms.csc.exsys.data.ParameterType parameter, ch.cern.cms.csc.exsys.data.SourceType source) {

        try {

            ParamValue param = new ParamValue();
            param.setPvaSrcId(sourceLocal.find(source.value()));
            param.setPvaSourceTime(parameter.getTime().toGregorianCalendar().getTime());
            param.setPvaParId(paramLocal.find(parameter.getParameterId().value()));
            param.setPvaCreateTime(new Date());
            param.setPvaComponentId(parameter.getComponent());

            //if (parameter.getRun() > 0) {
                param.setPvaRunNumber(parameter.getRun());
            //}

            //if (parameter.getStringValue() != null) {
                param.setPvaStringValue(parameter.getStringValue());
            //}

            //if (parameter.getNumericValue() != null) {
                param.setPvaNumericValue(parameter.getNumericValue());
            //}

            //if (parameter.isBoolValue() != null) {
                param.setPvaBooleanValue(parameter.isBoolValue());
            //}

            if (parameter.getLowerThreshold() != null) {
                param.setPvaLowerThreshold(new BigDecimal(parameter.getLowerThreshold()));
            }

            if (parameter.getUpperThreshold() != null) {
                param.setPvaUpperThreshold(new BigDecimal(parameter.getUpperThreshold()));
            }

            if (parameter.getFraction() != null) {
                param.setPvaFraction(new BigDecimal(parameter.getFraction()));
            }

            if (parameter.getSeverity() != null && parameter.getSeverity().value() != null) {
                param.setPvaSevId(severityLocal.find(parameter.getSeverity().value()));
            }

            param.setPvaDescr(parameter.getDescr());

            paramValueLocal.create(param);
            
        } catch (Exception e) {
          e.printStackTrace(System.err);
          return false;
        }

        return true;
    }

}

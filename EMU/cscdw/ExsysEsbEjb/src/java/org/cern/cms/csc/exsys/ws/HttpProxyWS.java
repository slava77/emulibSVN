/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.ws;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.MalformedURLException;
import java.net.URLConnection;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.ejb.Stateless;

/**
 *
 * @author Evka
 */
@WebService()
@Stateless()
public class HttpProxyWS {
    private static Logger logger = Logger.getLogger(HttpProxyWS.class.getName());

    /**
     * Web service operation
     */
    @WebMethod(operationName = "requestXml")
    public String requestXml(@WebParam(name = "url") final String url) {
        try {
            URLConnection conn = (new URL(url)).openConnection();
            String content = new String();
            BufferedReader reader = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            String line;
            while ((line = reader.readLine()) != null) {
                content += line + "\n";
            }
            reader.close();
            return content;
        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Exception while retrieving XML from " + url, ex);
            return "";
        }
    }

}

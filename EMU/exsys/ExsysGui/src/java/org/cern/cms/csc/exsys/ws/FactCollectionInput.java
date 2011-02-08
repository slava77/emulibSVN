package org.cern.cms.csc.exsys.ws;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.URL;
import javax.ejb.EJB;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.xml.XMLConstants;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.Unmarshaller;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.ws.FactCollectionInputLocal;

public class FactCollectionInput extends HttpServlet {

    private static final String SERVICE_NAME = "EXSYS Fact Collection Input Service";
    private static final String SCHEMA_ENTRY = "/org/cern/cms/csc/dw/schema/cdw.xsd";
    private static final Logger logger = SimpleLogger.getLogger(FactCollectionInput.class);

    @EJB
    private FactCollectionInputLocal wsInput;

    private SchemaFactory schemaFactory;
    private Schema schema;
    private JAXBContext jaxb;
    private Unmarshaller unmarshaller;

    @Override
    public void init() throws ServletException {
        try {
            super.init();

            // Create pointer to schema
            URL schemaUrl = FactCollectionInput.class.getResource(SCHEMA_ENTRY);
            this.schemaFactory = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
            this.schema = schemaFactory.newSchema(schemaUrl);

            // Create JAXB unmarshaler
            this.jaxb = JAXBContext.newInstance(FactCollection.class);
            this.unmarshaller = jaxb.createUnmarshaller();
            this.unmarshaller.setSchema(schema);

        } catch (Exception ex) {
            throw new ServletException(ex);
        }
    }

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        try {

            // Unmarshal and send object to WS
            JAXBElement<FactCollection> xcol = (JAXBElement<FactCollection>) unmarshaller.unmarshal(request.getReader());
            response.getWriter().print("OK:");
            response.getWriter().print(wsInput.getFactCollection(xcol.getValue()));

        } catch (Exception ex) {
            logger.error(ex);
            throw new ServletException(ex);
        }
    }

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        PrintWriter out = response.getWriter();
        out.println("<html><head><title>");
        out.println(SERVICE_NAME);
        out.println("<h1>");
        out.println(SERVICE_NAME);
        out.println("</h1><p>Service talks only POST!</p>");
        out.println("</body></html>");
    }

    @Override
    public String getServletInfo() {
        return SERVICE_NAME;
    }

}

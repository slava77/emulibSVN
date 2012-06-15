/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.StringReader;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdo
 */
public class BeanTableExportProcessorSecXslt extends BeanTableExportProcessorSecondary {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportProcessorSecXslt.class);
    private static final TransformerFactory tf = TransformerFactory.newInstance();
    
    public BeanTableExportProcessorSecXslt(BeanTable table, BeanTableExportTemplateSecondary template) {
        super(table, template);
    }

    public boolean export(InputStream in, OutputStream out) throws IOException {

        try {
            
            StreamSource ss = new StreamSource(in);
            StreamResult sr = new StreamResult(out);
            Templates translet = tf.newTemplates(new StreamSource(new StringReader(template.getTemplate())));
            Transformer transformer = translet.newTransformer();
            transformer.transform(ss, sr);
            
        } catch (Exception ex) {
            
            write(out, ex.getMessage());
            return false;
            
        }
        
        return true;
    }

}

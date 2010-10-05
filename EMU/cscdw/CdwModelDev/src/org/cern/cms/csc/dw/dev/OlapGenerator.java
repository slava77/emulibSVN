package org.cern.cms.csc.dw.dev;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.PosixParser;
import org.cern.cms.csc.dw.dev.olap.CubeDef;
import org.cern.cms.csc.dw.dev.olap.OlapHierarchy;
import org.cern.cms.csc.dw.metadata.FactMd;
import org.cern.cms.csc.dw.metadata.MetadataManager;
import org.cern.cms.csc.dw.model.ontology.graph.GServices;
import org.cern.cms.csc.dw.model.ontology.graph.GraphDevServices;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.SAXException;

public class OlapGenerator {

    public final static String SQL_ENDL = "\n/\n";
    
    private static final String OPTION_DB = "db";
    private static final String OPTION_GDB = "gdb";
    private static final String OPTION_DEST = "dest";
    private static final String OPTION_HIERARCHIES_XML = "hxml";

    private static final String FILENAME_SQL = "olap.sql";
    private static final String FILENAME_XML = "OlapExsys.xml";

    private TransformerFactory transFactory;
    private DocumentBuilderFactory docFactory;
    private DocumentBuilder docBuilder;
    private final MetadataManager mm = new MetadataManager();
    private GServices gdb;

    private List<CubeDef> cubes = new ArrayList<CubeDef>();
    private List<OlapHierarchy> componentHierarchies = new ArrayList<OlapHierarchy>();

    public OlapGenerator(String dbSchema, String graphdb, File hierarchiesXml) throws ParserConfigurationException, SAXException, IOException {

        this.transFactory = TransformerFactory.newInstance();
        this.docFactory = DocumentBuilderFactory.newInstance();
        this.docBuilder = this.docFactory.newDocumentBuilder();

        for (FactMd fact: mm.getFactMDs()) {
            cubes.add(new CubeDef(fact, dbSchema));
        }

        Document doc = docBuilder.parse(hierarchiesXml);
        gdb = new GraphDevServices(graphdb);
        
        for (Element el: OlapHierarchy.getElementsByTagName(doc.getDocumentElement(), "Hierarchy")) {
            OlapHierarchy h = new OlapHierarchy(gdb, el, dbSchema);
            componentHierarchies.add(h);
        }

    }

    protected void close() {
        gdb.close();
    }

    public void generateDDL(File file) throws FileNotFoundException {
        FileOutputStream fout = new FileOutputStream(file);
        PrintWriter out = new PrintWriter(fout);

        for (OlapHierarchy h: componentHierarchies) {
            h.generateDDL(out);
        }

        for (CubeDef cube : cubes) {
            if (cube.hasMeasureAndDimension()) {
                cube.generateDDL(out);
            }
        }

        out.flush();
        out.close();
    }

    public void generateSchemas(File file) throws TransformerException, SAXException, IOException {
        
        Document doc = docBuilder.newDocument();
        Element rootEl = doc.createElement("schema");
        rootEl.setAttribute("name", "CSC-EXSYS");
        doc.appendChild(rootEl);

        for (OlapHierarchy h: componentHierarchies) {
            rootEl.appendChild(h.getElement(doc));
        }

        for (CubeDef cube : cubes) {
            if (cube.hasMeasureAndDimension()) {
                cube.generateSchema(doc, rootEl);
            }
        }

        outputDomToFile(doc, file);
        
    }

    private void outputDomToFile(Document doc, File file) throws TransformerConfigurationException, TransformerException {
        DOMSource domSource = new DOMSource(doc);
        StreamResult result = new StreamResult(file);
        Transformer transformer = transFactory.newTransformer();
        transformer.setOutputProperty("method", "xml");
        transformer.setOutputProperty("indent", "yes");
        transformer.setOutputProperty("media-type", "text/xml");
        transformer.setOutputProperty("omit-xml-declaration", "no");
        transformer.transform(domSource, result);
    }

    @SuppressWarnings("static-access")
    public static void main(String[] args) {

        OlapGenerator generator = null;

        try {
            
            // create the command line parser
            CommandLineParser clparser = new PosixParser();
            Options opts = new Options();
            opts.addOption(OptionBuilder.hasArg().withDescription("DB schema").isRequired().create(OPTION_DB));
            opts.addOption(OptionBuilder.hasArg().withDescription("Path to graphdb").isRequired().create(OPTION_GDB));
            opts.addOption(OptionBuilder.hasArg().withDescription("Path to hierarchies xml").isRequired().create(OPTION_HIERARCHIES_XML));
            opts.addOption(OptionBuilder.hasArg().withDescription("Destination directory").isRequired().create(OPTION_DEST));
            CommandLine cmdLine = clparser.parse(opts, args);

            String dbSchema = cmdLine.getOptionValue(OPTION_DB).toUpperCase();
            String graphdb = cmdLine.getOptionValue(OPTION_GDB);
            File destDir = new File(cmdLine.getOptionValue(OPTION_DEST));
            File hierXml = new File(cmdLine.getOptionValue(OPTION_HIERARCHIES_XML));

            generator = new OlapGenerator(dbSchema, graphdb, hierXml);

            generator.generateDDL(new File(destDir, FILENAME_SQL));
            generator.generateSchemas(new File(destDir, FILENAME_XML));
            
        } catch (Exception e) {
            e.printStackTrace(System.err);
        } finally {
            if (generator != null) {
                generator.close();
            }
        }
            
    }

}

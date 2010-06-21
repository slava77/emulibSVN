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
import org.cern.cms.csc.dw.metadata.FactMd;
import org.cern.cms.csc.dw.metadata.MetadataManager;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.xml.sax.SAXException;

public class OlapGenerator {

    private static final String OPTION_DB = "db";
    private static final String OPTION_DEST = "dest";
    private static final String OPTION_XML_BASE = "base";
    private static final String FILENAME_SQL = "olap.sql";
    private static final String FILENAME_XML = "olap.xml";

    private final String dbSchema;
    private TransformerFactory transFactory;
    private DocumentBuilderFactory docFactory;
    private DocumentBuilder docBuilder;
    private final MetadataManager mm = new MetadataManager();

    private List<CubeDef> cubes = new ArrayList<CubeDef>();

    public OlapGenerator(String dbSchema) throws ParserConfigurationException {
        transFactory = TransformerFactory.newInstance();
        this.docFactory = DocumentBuilderFactory.newInstance();
        this.docBuilder = this.docFactory.newDocumentBuilder();
        this.dbSchema = dbSchema;
        for (FactMd fact: mm.getFactMDs()) {
            cubes.add(new CubeDef(fact));
        }
    }

    public void generateDDL(File file) throws FileNotFoundException {
        FileOutputStream fout = new FileOutputStream(file);
        PrintWriter out = new PrintWriter(fout);

        for (CubeDef cube : cubes) {
            if (cube.hasColumns()) {
                cube.generateDDL(out);
            }
        }

        out.flush();
        out.close();
    }

    public void generateSchemas(File file, File base) throws TransformerException, SAXException, IOException {
        
        Document doc = docBuilder.parse(base);
        Node rootNode = doc.getDocumentElement();
        for (CubeDef cube : cubes) {
            if (cube.hasColumns()) {
                cube.generateSchema(doc, rootNode, dbSchema);
            }
        }

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

        try {
            
            // create the command line parser
            CommandLineParser clparser = new PosixParser();
            Options opts = new Options();
            opts.addOption(OptionBuilder.hasArg().withDescription("DB schema").isRequired().create(OPTION_DB));
            opts.addOption(OptionBuilder.hasArg().withDescription("XML base").isRequired().create(OPTION_XML_BASE));
            opts.addOption(OptionBuilder.hasArg().withDescription("Destination directory").isRequired().create(OPTION_DEST));
            CommandLine cmdLine = clparser.parse(opts, args);

            String dbSchema = cmdLine.getOptionValue(OPTION_DB).toUpperCase();
            File destDir = new File(cmdLine.getOptionValue(OPTION_DEST));
            File baseXml = new File(cmdLine.getOptionValue(OPTION_XML_BASE));

            OlapGenerator generator = new OlapGenerator(dbSchema);
            
            generator.generateDDL(new File(destDir, FILENAME_SQL));
            generator.generateSchemas(new File(destDir, FILENAME_XML), baseXml);
            
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }
            
    }

}

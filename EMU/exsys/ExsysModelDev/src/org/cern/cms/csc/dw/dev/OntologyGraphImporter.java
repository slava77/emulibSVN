package org.cern.cms.csc.dw.dev;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.Vector;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.PosixParser;
import jsf.bean.gui.log.SimpleLogger;
import org.apache.xerces.parsers.SAXParser;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentSynonym;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import jsf.bean.gui.log.Logger;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

public class OntologyGraphImporter {

    private static final Logger log = SimpleLogger.getLogger(OntologyGraphImporter.class);
    private static final String OWL_URI = "http://www.w3.org/2006/12/owl2-xml#";
    private static final String DEFAULT_OWL_FILE = "../KnowledgeBase/ontology/csc-ontology.owl";
    private static final String [] uriNames = new String [] { "URI", "annotationURI", "datatypeURI" };

    private GOntologyFactory gFactory;
    
    enum AnnotationType {
        DESCRIPTION
    };

    private Map<String, GComponent> components = new HashMap<String, GComponent>();
    private Map<String, GComponentClass> componentClasses = new HashMap<String, GComponentClass>();
    private Map<String, GComponentLinkClass> componentLinkClasses = new HashMap<String, GComponentLinkClass>();

    private void processContentHandler(SAXParser parser, String filename, OntologyHandler handler) throws Exception {
        parser.setContentHandler(handler);
        log.info("Processing " + handler.getClass().getName() + "..");
        parser.parse(filename);
    }

    /**
     * Import OWL XML file to ontology model
     * @param filename OWL file to import
     * @param em
     * @throws java.lang.Exception
     */
    public void process(String filename) throws Exception {

        SAXParser parser = new SAXParser();

        processContentHandler(parser, filename, new DeclarationHandler());
        processContentHandler(parser, filename, new ClassAssertionHandler());
        processContentHandler(parser, filename, new LinkClassHandler());

        processContentHandler(parser, filename, new SubClassOfHandler());
        processContentHandler(parser, filename, new LinkUpdateHandler());

        gFactory.intermediateCommit();

        processContentHandler(parser, filename, new SameIndividualsHandler());
        processContentHandler(parser, filename, new AnnotationHandler());

        gFactory.intermediateCommit();

        processContentHandler(parser, filename, new DataPropertyAssertionHandler());

        gFactory.intermediateCommit();
        
        processContentHandler(parser, filename, new ObjectPropertyAssertionHandler(false));
        processContentHandler(parser, filename, new ObjectPropertyAssertionHandler(true));

        // Removing components without types and/or names (synonyms)
        log.info("Removing synonym components..");
        for (GComponent gc: components.values()) {
            if (!gc.isSetType() || !gc.isSetName()) {
                gFactory.removeGComponent(gc);
            }
        }

        log.info("Commit..");
        gFactory.close();

    }

    public OntologyGraphImporter(String destinationFolder) {
        gFactory = new GOntologyFactory(destinationFolder);
    }


    @SuppressWarnings("static-access")
    public static void main(String[] args) {

        try {

            // create the command line parser
            CommandLineParser clparser = new PosixParser();

            Options opts = new Options();
            opts.addOption(OptionBuilder.hasArg().withDescription("OWL file").create("owl"));
            opts.addOption(OptionBuilder.hasArg().withDescription("Destination folder").create("dest"));

            CommandLine cmdLine = clparser.parse(opts, args);

            String ontology_file = cmdLine.getOptionValue("owl", DEFAULT_OWL_FILE);
            String destinationFolder = cmdLine.getOptionValue("dest");

            OntologyGraphImporter importer = new OntologyGraphImporter(destinationFolder);
            importer.process(ontology_file);

        } catch (Exception e) {
            log.error(e);
            e.printStackTrace(System.err);
        }
    }

    private abstract class OntologyHandler extends DefaultHandler {

        private String blockName = null;
        private String localName = null;
        private String uriName = null;
        private Set<String> blockNames;
        private StringBuilder text = null;

        public OntologyHandler(String... blockNames) {
            this.blockNames = new HashSet<String>(blockNames.length);
            for (int i = 0; i < blockNames.length; i++) {
                this.blockNames.add(blockNames[i]);
            }
        }

        public void blockElement(String blockName, String localName, String name) {
        }

        public void elementText(String blockName, String localName, String name, String text) {
        }
        
        public void blockStart(String blockName) {
        }

        public void blockEnd(String blockName) {
        }

        @Override
        public void endElement(String uri, String localName, String qName) throws SAXException {

            if (!uri.equals(OWL_URI)) {
                return;
            }

            if (blockNames.contains(localName)) {
                if (text != null) {
                    String str = text.toString();
                    str = str.replaceAll("\\n", "");
                    if (!str.isEmpty()) {
                        elementText(blockName, this.localName, uriName, str);
                    }
                }
                blockEnd(blockName);
                blockName = null;
                uriName = null;
                localName = null;
            }
        }

        @Override
        public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
            text = null;
            
            if (!uri.equals(OWL_URI)) {
                return;
            }
            if (blockNames.contains(localName)) {
                blockName = localName;
                blockStart(blockName);
                return;
            }
            if (blockName != null) {
                uriName = getUri(attributes);
                if (uriName != null) {
                    this.localName = localName;
                    blockElement(blockName, localName, uriName);
                }
            }
        }

        @Override
        public void characters(char[] ch, int start, int length) throws SAXException {
            if (blockName != null && localName != null && uriName != null) {
                if (text == null) {
                    text = new StringBuilder();
                }
                text.append(ch, start, length);
            }
        }

        public String getUri(Attributes attributes) {
            String cname = null;

            for (String uName: uriNames) {
                cname = attributes.getValue(uName);
                if (cname != null) {
                    break;
                }
            }

            if (cname != null) {
                if (cname.contains("#")) {
                    cname = cname.substring(cname.indexOf("#") + 1);
                }
            }

            return cname;
        }
    }

    private class DeclarationHandler extends OntologyHandler {

        public DeclarationHandler() {
            super("Declaration", "SubClassOf");
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("Class")) {
                if (!componentClasses.containsKey(name)) {
                    try {
                        GComponentClass c = gFactory.createGComponentClass(ComponentClassType.fromValue(name));
                        componentClasses.put(name, c);
                    } catch (IllegalArgumentException e) {
                        log.warn("Component class type [" + name + "] not defined in schema. Skipping..");
                    }
                } else {
                    log.warn("Double component class [" + name + "] declaration. Ignoring..");
                }
            } else if (localName.equals("Individual")) {
                if (!components.containsKey(name)) {
                    GComponent c = gFactory.createGComponent(name);
                    components.put(name, c);
                } else {
                    log.warn("Double component [" + name + "] declaration. Ignoring..");
                }
            }
        }
    }

    private class ClassAssertionHandler extends OntologyHandler {

        private GComponent component = null;
        private GComponentClass componentClass = null;

        public ClassAssertionHandler() {
            super("ClassAssertion");
        }

        @Override
        public void blockEnd(String blockName) {
            if (component != null && componentClass != null) {
                if (!component.isSetType()) {
                    component.setType(componentClass);
                }
            }
        }

        @Override
        public void blockStart(String blockName) {
            component = null;
            componentClass = null;
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("Class")) {
                componentClass = componentClasses.get(name);
                if (componentClass == null) {
                    log.warn("Component class [" + name + "] not declared but found in " + blockName + ". Skipping..");
                }
            } else if (localName.equals("Individual")) {
                component = components.get(name);
                if (component == null) {
                    log.warn("Component [" + name + "] not declared but found in " + blockName + ". Skipping..");
                }
            }
        }
    }

    private class LinkClassHandler extends OntologyHandler {

        public LinkClassHandler() {
            super("ObjectPropertyAssertion", "SubObjectPropertyOf");
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("ObjectProperty")) {
                if (!componentLinkClasses.containsKey(name)) {
                    try {
                        GComponentLinkClass clc = gFactory.createGComponentLinkClass(ComponentLinkClassType.fromValue(name));
                        clc.setTransitive(false);
                        componentLinkClasses.put(name, clc);
                    } catch (IllegalArgumentException e) {
                        log.warn("Component link class type [" + name + "] not defined in schema but found in " + blockName + ". Skipping..");
                    }
                }
            }
        }
    }

    private class LinkUpdateHandler extends OntologyHandler {

        GComponentLinkClass links[] = {null, null};

        public LinkUpdateHandler() {
            super("TransitiveObjectProperty", "InverseObjectProperties", "SubObjectPropertyOf");
        }

        @Override
        public void blockEnd(String blockName) {
            if (blockName.equals("InverseObjectProperties")) {
                if (links[0] != null && links[1] != null) {
                    links[0].setInverse(links[1]);
                    links[1].setInverse(links[0]);
                }
                links[0] = links[1] = null;
            }
            if (blockName.equals("SubObjectPropertyOf")) {
                if (links[0] != null && links[1] != null) {
                    links[0].setParent(links[1]);
                }
                links[0] = null;
                links[1] = null;
            }
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("ObjectProperty")) {
                try {
                    if (blockName.equals("TransitiveObjectProperty")) {
                        componentLinkClasses.get(name).setTransitive(true);
                    } else {

                        if (links[0] == null) {
                            links[0] = componentLinkClasses.get(name);
                            links[0].getId();
                        } else {
                            links[1] = componentLinkClasses.get(name);
                            links[1].getId();
                        }
                    }
                } catch (NullPointerException e) {
                    log.warn("Component link class [" + name + "] not defined but found in " + blockName + ". Skipping..");
                }

            }
        }
    }

    private class SubClassOfHandler extends OntologyHandler {

        GComponentClass parent = null;
        GComponentClass child = null;

        public SubClassOfHandler() {
            super("SubClassOf");
        }

        @Override
        public void blockEnd(String blockName) {
            if (parent != null && child != null) {
                child.addParent(parent);
            }
            parent = null;
            child = null;
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("Class")) {
                try {
                    componentClasses.get(name).getId();
                    if (child == null) {
                        child = componentClasses.get(name);
                    } else {
                        parent = componentClasses.get(name);
                    }
                } catch (NullPointerException e) {
                    log.warn("Component class [" + name + "] not defined but found in " + blockName + ". Skipping..");
                }
            }
        }
    }

    private class DataPropertyAssertionHandler extends OntologyHandler {

        String propTypeName; // only used in case of an exception to provide more information
        GComponent.DataPropertyType propType;
        GComponent component;
        Object value;

        public DataPropertyAssertionHandler() {
            super("DataPropertyAssertion");
        }

        @Override
        public void blockEnd(String blockName) {
            if ((propType != null) && (component != null) && (value != null)) {
                component.setDataProperty(propType, value);
            }
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("DataProperty")) {
                propTypeName = name;
                propType = GComponent.DataPropertyType.fromOwlName(name);
            } else if (localName.equals("Individual")) {
                if (!components.containsKey(name)) {
                    throw new RuntimeException("DataProperty was declared on an unknown individual: " + name);
                } else {
                    component = components.get(name);
                }
            }
        }

        @Override
        public void elementText(String blockName, String localName, String name, String text) {
           if (text.isEmpty()) {
               log.warn("Found an empty data property value (" + propTypeName + " on individual " + component.getName() + ") - skipping");
           }
            if (localName.equals("Constant")) {
                if (name.equals("string")) {
                    value = text;
                } else if (name.equals("integer")) {
                    try {
                        text = text.replaceAll("\\s", "");
                        value = Integer.valueOf(text);
                    } catch (NumberFormatException nfEx) {
                        throw new RuntimeException("Couldn't parse an integer data property value: " + propTypeName + "=" + text + " on individual " + component.getName());
                    }
                } else if (name.equals("boolean")) {
                    text = text.replaceAll("\\s", "");
                    value = Boolean.valueOf(text);
                } else {
                    throw new RuntimeException("Unknown data property dataType: " + name);
                }
            }
        }
    }


    private class ObjectPropertyAssertionHandler extends OntologyHandler {

        GComponent comps[] = { null, null };
        GComponentLinkClass linkClass = null;
        boolean checkInverse = false;

        public ObjectPropertyAssertionHandler(boolean checkInverse) {
            super("ObjectPropertyAssertion");
            this.checkInverse = checkInverse;
        }

        private boolean isLinkExists(GComponent component, GComponent toComponent, GComponentLinkClass checkLinkClass) {
            return component.getGComponentLinks().hasLink(component, checkLinkClass.getType());
        }

        @Override
        public void blockEnd(String blockName) {
            if (comps[0] != null && comps[1] != null && linkClass != null && !comps[0].equals(comps[1])) {

                if (!isLinkExists(comps[0], comps[1], linkClass)) {
                    comps[0].getGComponentLinks().addLink(comps[1], linkClass.getType());
                }

                if (checkInverse && linkClass.getInverse() != null) {
                    if (!isLinkExists(comps[1], comps[0], linkClass.getInverse())) {
                        comps[1].getGComponentLinks().addLink(comps[0], linkClass.getInverse().getType());
                    }
                }

            }

            comps[0] = null;
            comps[1] = null;
            linkClass = null;
            
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("ObjectProperty")) {
                if (componentLinkClasses.containsKey(name)) {
                    linkClass = componentLinkClasses.get(name);
                } else {
                    log.warn("Component link class [" + name + "] not defined but found in " + blockName + ". Skipping..");
                }
            } else if (localName.equals("Individual")) {
                if (components.containsKey(name)) {
                    if (comps[0] == null) {
                        comps[0] = components.get(name);
                    } else {
                        comps[1] = components.get(name);
                    }
                } else {
                    log.warn("Component [" + name + "] not defined but found in " + blockName + ". Skipping..");
                }
            }
        }
    }

    private class SameIndividualsHandler extends OntologyHandler {

        Set<String> unique = new HashSet<String>();
        Vector<String> syns = new Vector<String>();

        public SameIndividualsHandler() {
            super("SameIndividuals");
        }

        @Override
        public void blockEnd(String blockName) {
            if (syns.size() > 0) {
                for (String s1: syns) {
                    GComponent c = components.get(s1);
                    if (c.isSetType()) {
                        for (String s2: syns) {
                            if (!s1.equals(s2)) {
                                GComponentSynonym syn = gFactory.createGComponentSynonym(s2);
                                c.addSynonym(syn);
                            }
                        }
                    }
                }
            }
            syns.clear();
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("Individual")) {
                if (components.containsKey(name)) {
                    if (!unique.contains(name)) {
                       syns.add(name);
                       unique.add(name);
                    } else {
                        log.warn("Dublicate synonym [" + name + "] found in " + blockName + ". Ignoring..");
                    }
                } else {
                    log.warn("Component [" + name + "] not defined but found in " + blockName + ". Skipping..");
                }
            }
        }
    }

    /*
        <EntityAnnotation>
            <Class URI="http://www.cern.ch/cms/csc.owl#Chamber"/>
            <Annotation annotationURI="dc:description">
                <Constant datatypeURI="xsd:string">A cathode strip chamber. Naming: ME&lt;e&gt;&lt;s&gt;/&lt;r&gt;/&lt;c&gt;, e: endcap [+-], s: station [1..4], r: ring [1..3], c: chamber [01..36]</Constant>
            </Annotation>
        </EntityAnnotation>
    */
    private class AnnotationHandler extends OntologyHandler {

        GComponentClass componentClass = null;
        String value = null;
        AnnotationType type = null;

        public AnnotationHandler() {
            super("EntityAnnotation");
        }

        @Override
        public void blockEnd(String blockName) {
            if (componentClass != null && value != null && type != null) {
                switch (type) {
                    case DESCRIPTION:
                        componentClass.setDescription(value);
                        break;
                }
            }
            componentClass = null;
            value = null;
            type = null;
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("Class")) {
                try {
                    componentClasses.get(name).getId();
                    componentClass = componentClasses.get(name);
                } catch (NullPointerException e) {
                    log.warn("Component class [" + name + "] not defined but found in " + blockName + ". Skipping..");
                }
            }
            if (localName.equals("Annotation")) {
                if (name.contains("description")) {
                    type = AnnotationType.DESCRIPTION;
                } else {
                    log.warn("Annotation type [" + name + "] not supported but found in " + blockName + ". Skipping..");
                }
            }
            if (localName.equals("Constant") && name != null) {
                value = null;
            }
        }

        @Override
        public void elementText(String blockName, String localName, String name, String text) {
            if (localName.equals("Constant") && name.equals("string") && type != null) {
                value = (value == null ? text : value.concat(text));
            }
        }

    }

}

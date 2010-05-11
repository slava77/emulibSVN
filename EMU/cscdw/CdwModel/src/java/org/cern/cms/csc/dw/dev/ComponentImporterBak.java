/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.cern.cms.csc.dw.dev;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.Vector;
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.PosixParser;
import org.apache.log4j.Logger;
import org.apache.xerces.parsers.SAXParser;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentLink;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentSynonym;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 *
 * @author valdo
 */
public class ComponentImporterBak {

    static Logger log = Logger.getLogger(ComponentImporterBak.class.getName());
    private static final String OWL_URI = "http://www.w3.org/2006/12/owl2-xml#";
    private static final String DEFAULT_OWL_FILE = "../KnowledgeBase/ontology/csc-ontology.owl";
    private static final String [] uriNames = new String [] { "URI", "annotationURI", "datatypeURI" };
    private static int flushNumber = 0;
    
    private EntityManager em = null;
    
    enum AnnotationType {
        DESCRIPTION
    };

    private Map<String, Component> components = new HashMap<String, Component>();
    private Map<String, ComponentClass> componentClasses = new HashMap<String, ComponentClass>();
    private Map<String, ComponentLinkClass> componentLinkClasses = new HashMap<String, ComponentLinkClass>();

    /**
     *
     * @return
     */
    public Map<String, Component> getComponents() {
        return components;
    }

    /**
     *
     * @return
     */
    public Map<String, ComponentClass> getComponentClasses() {
        return componentClasses;
    }

    /**
     *
     * @return
     */
    public Map<String, ComponentLinkClass> getComponentLinkClasses() {
        return componentLinkClasses;
    }

    private void processContentHandler(SAXParser parser, String filename, OntologyHandler handler) throws Exception {
        parser.setContentHandler(handler);
        log.info("Processing " + handler.getClass().getName() + "..");
        parser.parse(filename);
    }

    private void flush() {
        log.info("Intermediate flush (" + String.valueOf(++flushNumber) + ")..");
        em.flush();
        em.getTransaction().commit();
        em.getTransaction().begin();
        log.info("Intermediate flush (" + String.valueOf(flushNumber) + ") done");
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

        log.info("Persisting component classes..");
        for (ComponentClass c : getComponentClasses().values()) {
            log.debug(c.toString());
            em.persist(c);
        }

        log.info("Persisting component link classes..");
        for (ComponentLinkClass c : getComponentLinkClasses().values()) {
            log.debug(c.toString());
            em.persist(c);
        }

        log.info("Persisting components..");
        for (Component c : getComponents().values()) {
            log.debug(c.toString());
            if (c.isSetComponentClass()) {
                em.persist(c);
            }
        }

        processContentHandler(parser, filename, new SubClassOfHandler());
        processContentHandler(parser, filename, new LinkUpdateHandler());

        flush();

        processContentHandler(parser, filename, new ObjectPropertyAssertionHandler(false));

        flush();

        processContentHandler(parser, filename, new ObjectPropertyAssertionHandler(true));

        flush();

        processContentHandler(parser, filename, new SameIndividualsHandler());

        processContentHandler(parser, filename, new AnnotationHandler());
        
    }

    public ComponentImporterBak(EntityManager em) {
        this.em = em;
    }

    /**
     *
     * @throws java.lang.Throwable
     */
    @Override
    protected void finalize() throws Throwable {
    }

    /**
     * @param args the command line arguments
     */
    @SuppressWarnings("static-access")
    public static void main(String[] args) {

        try {

            // create the command line parser
            CommandLineParser clparser = new PosixParser();

            Options opts = new Options();
            opts.addOption(OptionBuilder.hasArg().withDescription("OWL file").create("owl"));

            CommandLine cmdLine = clparser.parse(opts, args);

            String ontology_file = cmdLine.getOptionValue("owl", DEFAULT_OWL_FILE);

            EntityManagerFactory emf = Persistence.createEntityManagerFactory("CdwUtilPU");
            EntityManager em = emf.createEntityManager();
            em.getTransaction().begin();

            ComponentImporterBak importer = new ComponentImporterBak(em);
            importer.process(ontology_file);

            log.info("Final ontology model flush..");
            em.flush();

            em.getTransaction().commit();
            em.close();
            emf.close();

        } catch (Exception e) {
            log.fatal(e);
            e.printStackTrace(System.err);
        }
    }

    private abstract class OntologyHandler extends DefaultHandler {

        private String blockName = null;
        private String localName = null;
        private String uriName = null;
        private Set<String> blockNames;
        private long flushFrequency = 0;
        private long flushCounter = 0;

        public OntologyHandler(long flushFrequency, String... blockNames) {
            this(blockNames);
            this.flushFrequency = flushFrequency;
        }

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
                blockEnd(blockName);
                blockName = null;
                uriName = null;
                localName = null;

                if (flushFrequency > 0) {
                    flushCounter += 1;
                    if ((flushCounter % flushFrequency) == 0) {
                        flush();
                    }
                }
            }
            
        }

        @Override
        public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
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
                String text = new String(ch, start, length);
                if (text != null) {
                    elementText(blockName, localName, uriName, text);
                }
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
                if (cname.contains(":")) {
                    cname = cname.substring(cname.indexOf(":") + 1);
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
                if (!getComponentClasses().containsKey(name)) {
                    try {
                        ComponentClass c = new ComponentClass();
                        c.setName(ComponentClassType.fromValue(name));
                        getComponentClasses().put(name, c);
                    } catch (IllegalArgumentException e) {
                        log.warn("Component class type [" + name + "] not defined in schema. Skipping..");
                    }
                } else {
                    log.warn("Double component class [" + name + "] declaration. Ignoring..");
                }
            } else if (localName.equals("Individual")) {
                if (!getComponents().containsKey(name)) {
                    Component c = new Component();
                    c.setName(name);
                    getComponents().put(name, c);
                } else {
                    log.warn("Double component [" + name + "] declaration. Ignoring..");
                }
            }
        }
    }

    private class ClassAssertionHandler extends OntologyHandler {

        private Component component = null;
        private ComponentClass componentClass = null;

        public ClassAssertionHandler() {
            super("ClassAssertion");
        }

        @Override
        public void blockEnd(String blockName) {
            if (component != null && componentClass != null) {
                component.setComponentClass(componentClass);
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
                componentClass = getComponentClasses().get(name);
                if (componentClass == null) {
                    log.warn("Component class [" + name + "] not declared but found in " + blockName + ". Skipping..");
                }
            } else if (localName.equals("Individual")) {
                component = getComponents().get(name);
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
                if (!getComponentLinkClasses().containsKey(name)) {
                    try {
                        ComponentLinkClass clc = new ComponentLinkClass();
                        clc.setName(ComponentLinkClassType.fromValue(name));
                        clc.setTransitive(false);
                        getComponentLinkClasses().put(name, clc);
                    } catch (IllegalArgumentException e) {
                        log.warn("Component link class type [" + name + "] not defined in schema but found in " + blockName + ". Skipping..");
                    }
                }
            }
        }
    }

    private class LinkUpdateHandler extends OntologyHandler {

        ComponentLinkClass links[] = {null, null};

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
                        getComponentLinkClasses().get(name).setTransitive(true);
                    } else {

                        if (links[0] == null) {
                            links[0] = getComponentLinkClasses().get(name);
                            links[0].getId();
                        } else {
                            links[1] = getComponentLinkClasses().get(name);
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

        ComponentClass parent = null;
        ComponentClass child = null;

        public SubClassOfHandler() {
            super("SubClassOf");
        }

        @Override
        public void blockEnd(String blockName) {
            if (parent != null && child != null) {
                child.getParents().add(parent);
            }
            parent = null;
            child = null;
        }

        @Override
        public void blockElement(String blockName, String localName, String name) {
            if (localName.equals("Class")) {
                try {
                    getComponentClasses().get(name).getId();
                    if (child == null) {
                        child = getComponentClasses().get(name);
                    } else {
                        parent = getComponentClasses().get(name);
                    }
                } catch (NullPointerException e) {
                    log.warn("Component class [" + name + "] not defined but found in " + blockName + ". Skipping..");
                }
            }
        }
    }

    private class ObjectPropertyAssertionHandler extends OntologyHandler {

        Component comps[] = {null, null};
        ComponentLinkClass linkClass = null;
        boolean checkInverse = false;
        private static final int FLUSH_INTERVAL = 3000;

        public ObjectPropertyAssertionHandler(boolean checkInverse) {
            super(FLUSH_INTERVAL, "ObjectPropertyAssertion");
            this.checkInverse = checkInverse;
        }

        private boolean isLinkExists(Component component, Component toComponent, ComponentLinkClass checkLinkClass) {
            for (ComponentLink link : component.getLinks()) {
                if (link.getComponent().equals(toComponent) && link.getComponentLinkClass().equals(checkLinkClass)) {
                    return true;
                }
            }
            return false;
        }

        @Override
        public void blockEnd(String blockName) {
            if (comps[0] != null && comps[1] != null && linkClass != null && !comps[0].equals(comps[1])) {

                if (!isLinkExists(comps[0], comps[1], linkClass)) {
                    ComponentLink link = new ComponentLink();
                    link.setComponent(comps[1]);
                    link.setComponentLinkClass(linkClass);
                    comps[0].getLinks().add(link);
                }

                if (checkInverse && linkClass.getInverse() != null) {
                    if (!isLinkExists(comps[1], comps[0], linkClass.getInverse())) {
                        ComponentLink link = new ComponentLink();
                        link.setComponent(comps[0]);
                        link.setComponentLinkClass(linkClass.getInverse());
                        comps[1].getLinks().add(link);
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
                if (getComponentLinkClasses().containsKey(name)) {
                    linkClass = getComponentLinkClasses().get(name);
                } else {
                    log.warn("Component link class [" + name + "] not defined but found in " + blockName + ". Skipping..");
                }
            } else if (localName.equals("Individual")) {
                if (getComponents().containsKey(name)) {
                    if (comps[0] == null) {
                        comps[0] = getComponents().get(name);
                    } else {
                        comps[1] = getComponents().get(name);
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
                    Component c = getComponents().get(s1);
                    if (c.isSetComponentClass()) {
                        for (String s2: syns) {
                            if (!s1.equals(s2)) {
                                ComponentSynonym synonym = new ComponentSynonym();
                                synonym.setSynonym(s2);
                                c.getSynonyms().add(synonym);
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
                if (getComponents().containsKey(name)) {
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

        ComponentClass componentClass = null;
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
                    getComponentClasses().get(name).getId();
                    componentClass = getComponentClasses().get(name);
                } catch (NullPointerException e) {
                    log.warn("Component class [" + name + "] not defined but found in " + blockName + ". Skipping..");
                }
            }
            if (localName.equals("Annotation")) {
                if (name.equals("description")) {
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

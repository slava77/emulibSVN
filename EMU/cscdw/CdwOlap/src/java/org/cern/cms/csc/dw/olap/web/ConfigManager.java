package org.cern.cms.csc.dw.olap.web;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

public class ConfigManager {

    private static final String DEFAULT_MDX_FILE = "WEB-INF/default.mdx";
    private static final String SCHEMA_FILE = "WEB-INF/OlapExsys.xml";
    private static final String CUBE_NAME_PATTERN = "%CUBE%";

    private static String queryPattern = null;
    private static Set<String> cubes = null;

    public static void init(String homePath) throws FileNotFoundException, IOException, ParserConfigurationException, SAXException {
        {
            File f = new File(homePath, DEFAULT_MDX_FILE);
            BufferedReader br = new BufferedReader(new FileReader(f));
            try {
                StringBuilder sb = new StringBuilder();
                String line = null;
                while ((line = br.readLine()) != null) {
                    sb.append(line);
                    sb.append("\n");
                }
                queryPattern = sb.toString();
            }
            finally {
                br.close();
            }
        }

        {
            cubes = new HashSet<String>();
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            DocumentBuilder db = dbf.newDocumentBuilder();
            File f = new File(homePath, SCHEMA_FILE);
            Document doc = db.parse(f);
            NodeList cubesNodes = doc.getElementsByTagName("Cube");
            for (int i = 0; i < cubesNodes.getLength(); i++) {
                Element cubeEl = (Element) cubesNodes.item(i);
                cubes.add(cubeEl.getAttribute("name"));
            }
        }

    }

    public static String getDefaultQuery(String cube) throws FileNotFoundException, IOException {
        String q = null;
        if (queryPattern != null) {
            q = queryPattern.replaceAll(CUBE_NAME_PATTERN, cube);
        }
        return q;
    }

    public static Set<String> getCubes() {
        return Collections.unmodifiableSet(cubes);
    }

}

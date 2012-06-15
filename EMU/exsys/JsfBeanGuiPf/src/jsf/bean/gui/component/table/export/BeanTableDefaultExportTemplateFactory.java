/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.ResourceBundle;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdor
 */
public class BeanTableDefaultExportTemplateFactory {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableDefaultExportTemplateFactory.class);
    
    private static final String TEMPLATES_BUNDLE = "jsf.bean.gui.component.table.template.templates";
    private static final String KEY_PRIMARY = "primary";
    private static final String KEY_PREV_SUFFIX = ".prev";
    private static final String KEY_SECONDARY = "secondary";

    private static List<BeanTableExportTemplate> templates;
    
    public static List<BeanTableExportTemplate> getTemplates() {
        
        if (templates == null) {
        
            templates = new ArrayList<BeanTableExportTemplate>();
            ResourceBundle bundle = ResourceBundle.getBundle(TEMPLATES_BUNDLE);

            Map<String, BeanTableExportTemplate> loaded = new HashMap<String, BeanTableExportTemplate>();

            {
                String s = bundle.getString(KEY_PRIMARY);
                if (s != null && s.trim().length() > 0) {
                    String[] keys = s.split(",");
                    for (String key: keys) {
                        try {
                            BeanTableDefaultExportTemplate t = new BeanTableDefaultPrimaryTemplate(key, bundle);
                            templates.add(t);
                            loaded.put(key, t);
                        } catch (IOException ex) {
                            logger.error("Error while loading default templates", ex);
                        }
                    }
                }
            }

            {
                String s = bundle.getString(KEY_SECONDARY);
                if (s != null && s.trim().length() > 0) {
                    String[] keys = s.split(",");
                    int numMissedToLoad = 0;
                    int numLoaded = 0;
                    do {
                        numMissedToLoad = 0;
                        numLoaded = 0;
                        for (String key: keys) {
                            if (!loaded.containsKey(key)) {
                                try {
                                    String prev = bundle.getString(key.concat(KEY_PREV_SUFFIX));
                                    if (loaded.containsKey(prev)) {
                                        BeanTableDefaultSecondaryTemplate t = new BeanTableDefaultSecondaryTemplate(key, loaded.get(prev), bundle);
                                        templates.add(t);
                                        loaded.put(key, t);
                                    }
                                    numLoaded += 1;
                                } catch (IOException ex) {
                                    logger.error("Error while loading default templates", ex);
                                }
                            } else {
                                numMissedToLoad += 1;
                            }
                        }
                    } while (numMissedToLoad == 0 || numLoaded == 0);
                }
            }
        }
        
        return templates;
        
    }


}

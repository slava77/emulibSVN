/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import java.io.IOException;
import java.util.ResourceBundle;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdor
 */
public abstract class BeanTableDefaultExportTemplate implements BeanTableExportTemplate {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableDefaultExportTemplate.class);
    
    protected static final String TEMPLATE = "/jsf/bean/gui/component/table/template/%s.ftl";
    private static final String KEY_MIME_SUFFIX = ".mime";
    private static final String KEY_EXT_SUFFIX = ".ext";

    protected final String key;
    private final String title;
    private final String ext;
    private final String mimeType;

    public BeanTableDefaultExportTemplate(String key, ResourceBundle bundle) throws IOException {
        this.key = key;
        this.title = bundle.getString(key);
        this.mimeType = bundle.getString(key.concat(KEY_MIME_SUFFIX));
        this.ext = bundle.getString(key.concat(KEY_EXT_SUFFIX));
    }

    public String getMimeType() {
        return mimeType;
    }

    public String getTitle() {
        return title;
    }

    public String getExt() {
        return ext;
    }

    public String getKey() {
        return key;
    }
    
}

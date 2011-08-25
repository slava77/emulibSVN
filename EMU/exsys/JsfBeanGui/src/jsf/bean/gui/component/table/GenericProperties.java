package jsf.bean.gui.component.table;

import java.io.IOException;
import java.io.Serializable;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Properties;
import javax.faces.model.SelectItem;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

public class GenericProperties implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(GenericProperties.class);
    private Properties properties;

    public GenericProperties(Properties properties) {
        this.properties = properties;
    }

    public Properties getProperties() {
        return properties;
    }

    public boolean hasKey(String key) {
        return properties.containsKey(key);
    }

    public String get(String key, String defaultValue) {
        if (hasKey(key)) {
            return properties.getProperty(key);
        }
        return defaultValue;
    }

    public void set(String key, String value) {
        if (value == null) {
            properties.remove(key);
        } else {
            properties.setProperty(key, value);
        }
    }

    public Integer get(String key, Integer defaultValue) {
        if (hasKey(key)) {
            String s = get(key, (String) null);
            if (s != null) {
                return Integer.valueOf(s);
            }
        }
        return defaultValue;
    }

    public void set(String key, Integer value) {
        set(key, (value != null ? value.toString() : (String) null));
    }

    public List<String> getStrList(String key) {
        List<String> ret = new ArrayList<String>();
        if (hasKey(key)) {
            Collections.addAll(ret, get(key, "").split(","));
        }
        return ret;
    }

    public void setStrList(String key, List<String> value) {
        StringBuilder sb = new StringBuilder();
        for (String s: value) {
            if (sb.length() > 0) {
                sb.append(",");
            }
            sb.append(s);
        }
        properties.setProperty(key, sb.toString());
    }

    public List<Integer> getIntList(String key) {
        if (hasKey(key)) {
            List<Integer> ret = new ArrayList<Integer>();
            for (String s: get(key, "").split(",")) {
                ret.add(Integer.valueOf(s));
            }
            return ret;
        }
        return Collections.EMPTY_LIST;
    }

    public void setIntList(String key, List<Integer> value) {
        StringBuilder sb = new StringBuilder();
        for (Integer i: value) {
            if (sb.length() > 0) {
                sb.append(",");
            }
            sb.append(i.toString());
        }
        properties.setProperty(key, sb.toString());
    }

    public Collection<SelectItem> getSelectItemList(String key) {
        if (hasKey(key)) {
            List<SelectItem> ret = new ArrayList<SelectItem>();
            for (String s: getStrList(key)) {
                ret.add(new SelectItem(s, s));
            }
            return ret;
        }
        return Collections.EMPTY_LIST;
    }

    public Collection<SelectItem> getSelectItemList(String key, Collection<SelectItem> defaultValue) {
        Collection<SelectItem> ret = getSelectItemList(key);
        if (!ret.isEmpty()){
            return ret;
        }
        return defaultValue;
    }

    public Boolean get(String key, Boolean defaultValue) {
        if (hasKey(key)) {
            return Boolean.valueOf(get(key, defaultValue.toString()));
        }
        return defaultValue;
    }

    public void set(String key, Boolean value) {
        properties.setProperty(key, value.toString());
    }

    public String getData() {
        StringWriter sw = new StringWriter();
        try {
            properties.store(sw, null);
        } catch (IOException ex) {
            logger.error(ex);
        }
        return sw.toString();
    }

}

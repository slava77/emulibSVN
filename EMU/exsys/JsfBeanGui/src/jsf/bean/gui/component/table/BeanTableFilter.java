package jsf.bean.gui.component.table;

import java.util.ArrayList;
import java.util.List;

public class BeanTableFilter {

  public static enum Operator {

        AND("and"),
        OR("or");

        private final String value;

        Operator(String value) {
            this.value = value;
        }

        public String getValue() {
            return this.value;
        }

        public static Operator fromValue(String opvalue) {
            for (Operator o: Operator.values()) {
                if (o.getValue().equalsIgnoreCase(opvalue)) {
                    return o;
                }
            }
            return null;
        }

    };

    public static enum Operation {

        EQUAL("="),
        MORE(">"),
        LESS("<"),
        MORE_EQUAL(">="),
        LESS_EQUAL("<="),
        NOT_EQUAL("<>"),
        LIKE("like"),
        NOTLIKE("notlike"),
        ISNULL("isnull"),
        ISNOTNULL("isnotnull"),
        IN("in"),
        NOTIN("notin");

        private final String value;

        Operation(String value) {
            this.value = value;
        }

        public String getValue() {
            return this.value;
        }

        public static Operation fromValue(String opvalue) {
            for (Operation o: Operation.values()) {
                if (o.getValue().equalsIgnoreCase(opvalue)) {
                    return o;
                }
            }
            return null;
        }

    };

    private List<BeanTableFilterItem> items = new ArrayList<BeanTableFilterItem>();

    public List<BeanTableFilterItem> getItems() {
        return items;
    }
    private String string = null;

    public String getString() {
        return this.string;
    }

    public void setString(String string) {
        this.string = string;
    }
    private boolean defaultFilter = false;

    public boolean isDefault() {
        return this.defaultFilter;
    }

    public void setDefault(boolean defaultFilter) {
        this.defaultFilter = defaultFilter;
    }

    public boolean isEmpty() {
        return (string == null || "".equals(string));
    }

}
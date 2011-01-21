package org.cern.cms.csc.dw.dao.table;

import java.util.List;

public interface BeanTableFilterIf {

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

    List<BeanTableFilterItemIf> getItems();

}

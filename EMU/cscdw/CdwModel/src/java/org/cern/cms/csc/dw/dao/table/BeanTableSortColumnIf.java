package org.cern.cms.csc.dw.dao.table;

public interface BeanTableSortColumnIf {

    public enum SORT_DIR {

        ASCENDING("asc"),
        DESCENDING("desc");

        private final String value;

        SORT_DIR(String value) {
            this.value = value;
        }

        public String getValue() {
            return this.value;
        }

    }

    boolean isAscending();
    String getPropertyName();

}

package org.cern.cms.csc.dw.dao.table;

public interface BeanTableColumnIf {

    public enum TYPE {

        SIMPLE("simple"),
        ENTITY("entity");

        private final String value;

        TYPE(String value) {
            this.value = value;
        }

        public String getValue() {
            return this.value;
        }

    }

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
    String getName();
    String getTitle();
    boolean isFilterSet();
    BeanTableFilterIf getFilter();
    void setFilter(BeanTableFilterIf filter);
    TYPE getType();

}

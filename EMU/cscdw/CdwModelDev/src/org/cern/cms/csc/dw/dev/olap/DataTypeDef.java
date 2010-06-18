package org.cern.cms.csc.dw.dev.olap;

import java.math.BigDecimal;

public class DataTypeDef {

    private final Class type;
    private String typeName;

    public DataTypeDef(Class type) {
        this.type = type;
        this.typeName = type.getSimpleName();
        
        if (type.equals(Long.class)) {
            typeName = "Integer";
        }

        if (type.equals(BigDecimal.class)) {
            typeName = "Numeric";
        }

    }

    public String getTypeName() {
        return typeName;
    }

    public Class getType() {
        return type;
    }

}

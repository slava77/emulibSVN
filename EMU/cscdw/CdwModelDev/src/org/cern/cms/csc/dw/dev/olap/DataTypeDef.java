package org.cern.cms.csc.dw.dev.olap;

import java.math.BigDecimal;
import java.math.BigInteger;

public class DataTypeDef {

    private final Class type;
    private String typeName;

    public DataTypeDef(Class type) {
        this.type = type;
        this.typeName = type.getSimpleName();
        
        if (type.equals(Long.class) || type.equals(BigInteger.class)) {
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

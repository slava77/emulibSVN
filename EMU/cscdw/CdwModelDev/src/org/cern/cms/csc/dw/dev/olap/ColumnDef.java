package org.cern.cms.csc.dw.dev.olap;

import java.lang.reflect.Method;
import javax.persistence.Column;

public abstract class ColumnDef {

    protected Method method;
    protected Column column;
    protected DataTypeDef type;

    protected ColumnDef(Method method) {
        this.method = method;
        this.column = method.getAnnotation(Column.class);
        this.type = new DataTypeDef(method.getReturnType());
    }

    public DataTypeDef getType() {
        return type;
    }

    public Column getColumn() {
        return column;
    }

    public Method getMethod() {
        return method;
    }

}

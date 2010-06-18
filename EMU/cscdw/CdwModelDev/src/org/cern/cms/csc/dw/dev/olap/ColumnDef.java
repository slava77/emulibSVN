package org.cern.cms.csc.dw.dev.olap;

import java.lang.reflect.Method;
import javax.persistence.Column;
import javax.persistence.JoinColumn;

public abstract class ColumnDef {

    protected Method method;
    protected String columnName;
    protected DataTypeDef type;

    protected ColumnDef(Method method) {
        this.method = method;

        if (method.isAnnotationPresent(Column.class)) {
            this.columnName = method.getAnnotation(Column.class).name();
        } else

        if (method.isAnnotationPresent(JoinColumn.class)) {
            this.columnName = method.getAnnotation(JoinColumn.class).name();
        }

        this.type = new DataTypeDef(method.getReturnType());
    }

    public DataTypeDef getType() {
        return type;
    }

    public String getColumnName() {
        return columnName;
    }

    public Method getMethod() {
        return method;
    }

}

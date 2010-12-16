package org.cern.cms.csc.dw.dev.olap;

import java.io.PrintWriter;
import java.lang.reflect.Method;
import javax.persistence.Column;
import javax.persistence.JoinColumn;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public abstract class ColumnDef {

    protected final CubeDef cube;
    protected final Method method;
    protected final String name;
    protected String columnName;
    protected DataTypeDef type;

    protected ColumnDef(CubeDef cube, Method method, String name) {

        this.cube = cube;
        this.method = method;
        this.name = name;

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

    public Element getElement(Document doc) {
        return null;
    }

    public void getDDL(PrintWriter out) {       
    }

}

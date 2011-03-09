package jsf.bean.gui.converter;

import javax.faces.component.UIComponent;
import javax.faces.context.FacesContext;
import javax.faces.convert.Converter;
import jsf.bean.gui.ClassFinderIf;

public class ClassConverter implements Converter  {

    private final ClassFinderIf classFinder;

    public ClassConverter(ClassFinderIf classFinder) {
        this.classFinder = classFinder;
    }

    @Override
    public Object getAsObject(FacesContext context, UIComponent component, String value) {
        if (value == null) {
            return (Class) null;
        }
        try {
            return classFinder.getClassForName(value);
        } catch (ClassNotFoundException ex) {
            throw new RuntimeException(ex);
        }
    }

    @Override
    public String getAsString(FacesContext context, UIComponent component, Object value) {
        if (value == null) {
            return (String) null;
        }
        Class c = (Class) value;
        return c.getName();
    }

}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.complex;

import com.icesoft.faces.component.selectinputtext.SelectInputText;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import javax.faces.convert.Converter;
import javax.faces.event.ValueChangeEvent;
import javax.faces.model.SelectItem;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;

/**
 *
 * @author valdo
 */
public class ComponentEditor extends ManyToOneEditor {

    private static final int MATCHES_TO_DISPLAY = 15;

    private String componentName = null;
    private List<SelectItem> componentMatches = null;

    public ComponentEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, EntityDaoLocal entityDao) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor, entityDao);
    }

    @Override
    protected Converter createConverter() {
        return null;
    }

    @Override
    protected InputType createInputType() {
        return InputType.COMPONENT;
    }

    @Override
    public List<SelectItem> getListOfValues() throws Exception {
        return Collections.EMPTY_LIST;
    }

    public int getMatchesToDisplay() {
        return MATCHES_TO_DISPLAY;
    }

    public String getComponentName() {
        return componentName;
    }

    public void setComponentName(String componentName) {
        this.componentName = componentName;
    }

    public List<SelectItem> getComponentMatches() {
        return componentMatches;
    }

    public void componentInputValueChanged(ValueChangeEvent event) throws IllegalAccessException, InvocationTargetException, ComponentNotFoundException {

        if (event.getComponent() instanceof SelectInputText) {

            SelectInputText autoComplete = (SelectInputText) event.getComponent();
            String newWord = (String) event.getNewValue();

            componentMatches = new ArrayList<SelectItem>();
            setValue(null);

            if (newWord != null && newWord.length() > 0) {

                for (GComponent gc: getEntityDao().getOntologyDao().getGComponentsByNameMatches(newWord, getComponentClassLimit(), MATCHES_TO_DISPLAY)) {
                    componentMatches.add(new SelectItem(gc, gc.getName()));
                }

                // if there is a selected item then find the object of the
                // same name
                if (autoComplete.getSelectedItem() != null) {
                    Component c = getEntityDao().getOntologyDao().getComponentByName(((GComponent) autoComplete.getSelectedItem().getValue()).getName());
                    setValue(c);
                }

                // if there was no selection we still want to see if a proper
                // object was typed and update our selectedObject instance.
                else {
                    if (componentMatches != null) {
                        GComponent tmp;
                        for(int i = 0, max = componentMatches.size(); i < max; i++){
                            tmp = (GComponent) componentMatches.get(i).getValue();
                            if (tmp.getName().compareToIgnoreCase(newWord) == 0) {
                                setValue(getEntityDao().getOntologyDao().getComponentByName(tmp.getName()));
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    public Collection<GComponentClass> getComponentClassLimit() {
        return Collections.EMPTY_LIST;
    }

    public boolean isComponentClassLimited() {
        return getComponentClassLimit().isEmpty() == false;
    }

}

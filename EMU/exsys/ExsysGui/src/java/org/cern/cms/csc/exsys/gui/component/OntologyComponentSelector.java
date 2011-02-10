package org.cern.cms.csc.exsys.gui.component;

import com.icesoft.faces.component.selectinputtext.SelectInputText;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.faces.event.ValueChangeEvent;
import javax.faces.model.SelectItem;
import org.cern.cms.csc.dw.dao.GOntologyDaoLocal;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;

public abstract class OntologyComponentSelector {

    private static final int ROWS = 15;

    private int rows = ROWS;
    private List<SelectItem> componentMatches;

    private Collection<GComponentClass> limitTo = Collections.EMPTY_SET;
    private GComponent component = null;
    private String componentName = null;

    protected abstract GOntologyDaoLocal getGOntologyDao();
    
    public OntologyComponentSelector() {
    }

    public void componentInputValueChanged(ValueChangeEvent event) {

        if (event.getComponent() instanceof SelectInputText) {

            // get the number of displayable records from the component
            SelectInputText autoComplete = (SelectInputText) event.getComponent();
            // get the new value typed by component user.
            String newWord = (String) event.getNewValue();

            componentMatches = new ArrayList<SelectItem>();
            component = null;

            if (newWord != null && newWord.length() > 0) {

                for (GComponent gc: getGOntologyDao().getGComponentsByNameMatches(newWord, limitTo, getRows())) {
                    componentMatches.add(new SelectItem(gc, gc.getName()));
                }

                // if there is a selected item then find the object of the
                // same name
                if (autoComplete.getSelectedItem() != null) {
                    component = (GComponent) autoComplete.getSelectedItem().getValue();
                }

                // if there was no selection we still want to see if a proper
                // object was typed and update our selectedObject instance.
                else {
                    if (componentMatches != null) {
                        GComponent tmp;
                        for(int i = 0, max = componentMatches.size(); i < max; i++){
                            tmp = (GComponent) componentMatches.get(i).getValue();
                            if (tmp.getName().compareToIgnoreCase(newWord) == 0) {
                                component = tmp;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    public int getRows() {
        return rows;
    }

    public void setRows(int rows) {
        this.rows = rows;
    }

    public List<SelectItem> getComponentMatches() {
        return componentMatches;
    }

    public GComponent getComponent() {
        return component;
    }

    public void setComponent(GComponent component) {
        this.component = component;
    }

    public void setComponentId(Long componentId) throws ComponentNotFoundException {
        this.component = getGOntologyDao().getGComponent(componentId);
    }

    public Collection<GComponentClass> getLimitTo() {
        return limitTo;
    }

    public void setLimitTo(Collection<GComponentClass> limitTo) {
        this.limitTo = limitTo;
    }

    public boolean isLimited() {
        return limitTo != null && !limitTo.isEmpty();
    }

    public int getLimitToSize() {
        if (isLimited()) {
            return limitTo.size();
        }
        return 0;
    }

    public String getComponentName() {
        return componentName;
    }

    public void setComponentName(String componentName) {
        this.componentName = componentName;
    }

    public Set<Map.Entry<GComponent.DataPropertyType, Object>> getComponentDataProperties() {
        return getComponent().getAllDataProperties().entrySet();
    }
}

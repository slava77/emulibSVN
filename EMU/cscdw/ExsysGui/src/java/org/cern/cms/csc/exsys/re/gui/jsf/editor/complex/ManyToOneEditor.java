/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.complex;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;
import javax.faces.convert.Converter;
import javax.faces.model.SelectItem;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.base.metadata.EntityManyToOnePropertyMD;
import org.cern.cms.csc.dw.model.base.metadata.EntityPropertyMD;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor.InputType;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.base.EntityEditor;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.convert.LovConverter;

/**
 * This type of editor is used for many-to-one relations between entities.
 * @author Evka
 */
public class ManyToOneEditor extends EntityEditor {

    /** Cache of list of available values (used only for enum, many-to-one and many-to-many properties). */
    private List<Object> lovCache = null;
    /** Cache of list of available select items (used only for enum, many-to-one and many-to-many properties). */
    private List<SelectItem> lovSelectItemsCache = null;

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public ManyToOneEditor(EntityBase entity, EntityPropertyMD metadata, Editor parentEditor) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor);
        if (!(metadata instanceof EntityManyToOnePropertyMD)) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a ManyToOneEditor for a property which is not a many-to-one relation: " + metadata.getName());
        }
    }

    /**
     * Calls super.getValue() and if that comes back null then given that LOV (list of values) isn't null, returns the first LOV element.
     * @see org.cern.cms.csc.exsys.re.gui.jsf.editor.base.Editor.setValue()
     */
    @Override
    public Object getValue() throws IllegalAccessException, InvocationTargetException {
        Object value = super.getValue();
        if ((value == null) && (lovCache != null)) {
            if (lovCache.size() > 0) {
                return lovCache.get(0);
            }
        }
        return value;
    }



    @Override
    public EntityManyToOnePropertyMD getMetadata() {
        return (EntityManyToOnePropertyMD) super.getMetadata();
    }

    @Override
    protected Converter createConverter() {
        if (lovCache == null) {
            try {
                getListOfValues();
            } catch(Exception ex) {
                throw new RuntimeException(ex);
            }
        }
        return new LovConverter(lovCache);
    }

    @Override
    protected InputType createInputType() {
        return InputType.SELECT_ONE_MENU;
    }

    /**
     * Get list of available values as List<SelectItem> (this is cached, to refresh cache use refreshListOfValues())
     * @return list of available values as List<SelectItem> (this is cached, to refresh cache use refreshListOfValues())
     * @throws Exception rethrown from EntityPropertyMD.getListOfValues()
     * @see org.cern.cms.csc.dw.model.base.EntityPropertyMD.getListOfValues()
     */
    public List<SelectItem> getListOfValues() throws Exception {
        if (lovSelectItemsCache == null) {
            refreshListOfValues(true);
        }
        return lovSelectItemsCache;
    }

    /**
     * Get list of available values as List<SelectItem>
     * @param includeNewValue if this is set to true, then a new value is also included in the list
     * @return list of available values as List<SelectItem>
     * @throws Exception rethrown from EntityPropertyMD.getListOfValues()
     * @see org.cern.cms.csc.dw.model.base.EntityPropertyMD.getListOfValues()
     */
    public void refreshListOfValues(boolean includeNewValue) throws Exception {
        lovCache = getMetadata().getListOfValues();
        if (lovCache != null) {
            List<SelectItem> ret = new ArrayList<SelectItem>();
            for (Object lovObject: lovCache) {
                SelectItem lovItem;
                if (lovObject instanceof EntityBase) {
                    EntityBase lovEntityObj = (EntityBase) lovObject;
                    lovItem = new SelectItem(lovObject, lovEntityObj.getEntityTitle());
                } else {
                    lovItem = new SelectItem(lovObject, lovObject.toString());
                }
                ret.add(lovItem);
            }
            if (includeNewValue) {
                lovCache.add(0, getMetadata().getNewValue());
                SelectItem createNewSI = new SelectItem(getMetadata().getNewValue(), "Create New...");
                ret.add(0, createNewSI);
            }
            lovSelectItemsCache = ret;
        }
    }

}

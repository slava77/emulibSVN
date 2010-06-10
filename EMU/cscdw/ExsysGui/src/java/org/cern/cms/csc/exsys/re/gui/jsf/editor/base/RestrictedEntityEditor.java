/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.gui.jsf.editor.base;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import javax.faces.convert.Converter;
import javax.faces.event.ActionEvent;
import javax.faces.model.SelectItem;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.metadata.RestrictedPropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;
import org.cern.cms.csc.exsys.re.gui.jsf.editor.convert.LovConverter;

/**
 *
 * @author Evka
 */
public abstract class RestrictedEntityEditor extends EntityEditor {

    /** Cache of list of available values (used only for enum, many-to-one and many-to-many properties). */
    private List<Object> lovCache = null;
    /** Cache of list of available select items (used only for enum, many-to-one and many-to-many properties). */
    private List<SelectItem> lovSelectItemsCache = null;
    /** List of newly created (and not saved) list of value items. */
    private List<Object> newLovItems = new ArrayList<Object>();

    /**
     * Constructor
     * @param entity entity that the property to be edited belongs to.
     * @param metadata metadata of the property to be edited.
     * @param parentEditor parent editor that this editor belongs to.
     * @throws InvalidEntityBeanPropertyException thrown if property is incompatible with this kind of editor
     */
    public RestrictedEntityEditor(EntityBase entity, PropertyMd metadata, Editor parentEditor, EntityDaoLocal entityDao) throws InvalidEntityBeanPropertyException {
        super(entity, metadata, parentEditor, entityDao);
        if (!(metadata instanceof RestrictedPropertyMd)) {
            throw new InvalidEntityBeanPropertyException("Attempt to create a RestrictedEntityEditor for a property which is not a restricted relation (not a many-to-one or many-to-many or enum): " + metadata.getName());
        }
    }

    public abstract boolean isRestrictionStrict();

    @Override
    public RestrictedPropertyMd getMetadata() {
        return (RestrictedPropertyMd) super.getMetadata();
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

    /**
     * Returns list of value objects cache.
     * @return list of value objects cache.
     */
    protected List<Object> getLovCache() {
        return lovCache;
    }

    /**
     * Get list of available values as List<SelectItem> (this is cached, to refresh cache use refreshListOfValues())
     * @return list of available values as List<SelectItem> (this is cached, to refresh cache use refreshListOfValues())
     * @throws Exception rethrown from PropertyMd.getListOfValues()
     * @see org.cern.cms.csc.dw.model.base.PropertyMd.getListOfValues()
     */
    public List<SelectItem> getListOfValues() throws Exception {
        if (lovSelectItemsCache == null) {
            rebuildLovSelectItemsCache();
        }
        return lovSelectItemsCache;
    }

    /**
     * Get list of available values as List<SelectItem>
     * @param includeNewValue if this is set to true, then a new value is also included in the list
     * @return list of available values as List<SelectItem>
     * @throws Exception rethrown from PropertyMd.getListOfValues()
     * @see org.cern.cms.csc.dw.model.base.PropertyMd.getListOfValues()
     */
    public void refreshListOfValues() throws Exception {
        lovCache = getMetadata().getListOfValues(getEntityDao());
        newLovItems.clear();
        resetConverter();
        lovSelectItemsCache =  null;
    }

    /**
     * Rebuilds lovSelectItemsCache (list of select items returned to gui) according to lovCache.
     * @throws Exception rethrown from PropertyMd.getListOfValues()
     */
    private void rebuildLovSelectItemsCache() throws Exception {
        if (lovCache == null) {
            refreshListOfValues();
        }
        if (lovCache != null) {
            List<SelectItem> ret = new ArrayList<SelectItem>();
            for (Object lovObject: lovCache) {
                SelectItem lovItem;
                if (lovObject instanceof EntityBase) {
                    EntityBase lovEntityObj = (EntityBase) lovObject;
                    String title = lovEntityObj.getEntityTitle();
                    if (title.equals("null")) {
                        title = "New";
                    }
                    lovItem = new SelectItem(lovObject, title);
                } else {
                    lovItem = new SelectItem(lovObject, lovObject.toString());
                }
                ret.add(lovItem);
            }
            lovSelectItemsCache = ret;
        }
    }

    /**
     * Creates new List Of Values Item and assigns it to the value of the editor
     * @throws Exception
     */
    public void createAndAssignNewLovItem(ActionEvent e) throws Exception {
        Object newValue = getMetadata().getNewValue();
        setValue(newValue);
        lovCache.add(0, newValue);
        newLovItems.add(newValue);
        resetConverter();
        rebuildLovSelectItemsCache();
    }

    /**
     * Tells if the currently assigned value is new (not from DB) or not.
     * @param value
     * @return
     */
    public boolean isValueNew() {
        try {
            return getNewLovItems().contains(getValue());
        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Exception in RestrictedEntityEditor.isValueNew()", ex);
            throw new RuntimeException(ex);
        }
    }

    /**
     * Get list of newly created (and not saved) list of value items.
     * @return list of newly created (and not saved) list of value items.
     */
    public List<Object> getNewLovItems() {
        return newLovItems;
    }

}

package jsf.bean.gui.component.table;

import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Collection;
import javax.faces.model.SelectItem;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.BeanTableManager;

public class BeanTablePack {

    private Collection<SelectItem> classes = new ArrayList<SelectItem>();
    private Class<? extends EntityBeanBase> selectedClass;
    private BeanTable table = null;
    private final BeanTableManager manager;
    private final String title;
    private final String prefix;

    public BeanTablePack(BeanTableManager manager, Class<? extends EntityBeanBase> rowClass) throws Exception {
        this(null, null, manager, rowClass);
    }

    public BeanTablePack(String nextPrefix, String nextTitle, BeanTableManager manager, Class<? extends EntityBeanBase> rowClass) throws Exception {

        this.title = manager.buildTableTitle(nextTitle);
        this.prefix = manager.buildTablePrefix(nextPrefix);
        this.manager = manager;

        Class firstClass = null;
        if (Modifier.isAbstract(rowClass.getModifiers())) {
            for (Class cl: manager.getClassFinder().findSubclassesInSamePackage(rowClass)) {
                if (!Modifier.isAbstract(cl.getModifiers())) {
                    if (firstClass == null) {
                        firstClass = cl;
                    }
                    this.classes.add(new SelectItem(cl, cl.getSimpleName()));
                }
            }
            this.setSelectedClass(firstClass);
        } else {
            this.setSelectedClass(rowClass);
        }
    }

    public Collection<SelectItem> getClasses() {
        return classes;
    }

    public Class<? extends EntityBeanBase> getSelectedClass() {
        return selectedClass;
    }

    public final void setSelectedClass(Class<? extends EntityBeanBase> selectedClass) throws Exception {
        if (selectedClass != null && (this.selectedClass == null || !this.selectedClass.equals(selectedClass))) {
            this.selectedClass = selectedClass;
            this.table = new BeanTable(this, selectedClass);
            if (classes.isEmpty()) {
                this.table.getProperties().setTablePrefix(prefix);
            } else {
                String p = "{".concat(selectedClass.getSimpleName()).concat("}.");
                if (prefix != null) {
                    p = prefix.concat(p);
                }
                this.table.getProperties().setTablePrefix(p);
            }
        }
    }

    public BeanTable getTable() {
        return table;
    }

    public String getTitle() {
        return title;
    }

    public BeanTableManager getManager() {
        return manager;
    }

    public boolean isSingleClass() {
        return classes.isEmpty();
    }

}

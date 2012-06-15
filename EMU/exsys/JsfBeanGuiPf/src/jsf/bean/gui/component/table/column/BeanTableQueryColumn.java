/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.column;

/**
 *
 * @author valdo
 */
public class BeanTableQueryColumn implements Comparable<BeanTableQueryColumn> {
    
    private final String title;
    private final String name;
    private final Class type;
    private final boolean direct;
    private final boolean sortable;

    public BeanTableQueryColumn(String title, String name, Class type, boolean direct, boolean sortable) {
        this.title = title;
        this.name = name;
        this.type = type;
        this.direct = direct;
        this.sortable = sortable;
    }

    public String getName() {
        return name;
    }

    public Class getType() {
        return type;
    }

    public String getTypeName() {
        return type.getSimpleName();
    }
    
    public String getTitle() {
        return title;
    }

    public boolean isDirect() {
        return direct;
    }

    public boolean isSortable() {
        return sortable;
    }
    
    @Override
    public boolean equals(Object o) {
        if (o instanceof BeanTableQueryColumn) {
            return ((BeanTableQueryColumn) o).getName().equals(this.name);
        }
        return super.equals(o);
    }

    public int compareTo(BeanTableQueryColumn t) {
        return t.getName().compareTo(this.getName());
    }
    
}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.api;

/**
 *
 * @author valdo
 */
public class BeanTableApiColumn implements Comparable<BeanTableApiColumn> {
    
    private final String name;
    private final String type;

    public BeanTableApiColumn(String name, String type) {
        this.name = name;
        this.type = type;
    }

    public String getName() {
        return name;
    }

    public String getType() {
        return type;
    }

    @Override
    public boolean equals(Object o) {
        if (o instanceof BeanTableApiColumn) {
            return ((BeanTableApiColumn) o).getName().equals(this.name);
        }
        return super.equals(o);
    }

    public int compareTo(BeanTableApiColumn t) {
        return t.getName().compareTo(this.getName());
    }
    
}

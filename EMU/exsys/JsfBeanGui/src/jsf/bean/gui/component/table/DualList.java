package jsf.bean.gui.component.table;

import java.util.Collection;
import java.util.LinkedList;
import java.util.List;

public class DualList<T> {

    private List<T> source;
    private List<T> target;

    public DualList(List<T> source, List<T> target) {
        this.source = source;
        this.target = target;
    }

    public DualList() {
        this.source = new LinkedList<T>();
        this.target = new LinkedList<T>();
    }

    public List<T> getSource() {
        return source;
    }

    public List<T> getTarget() {
        return target;
    }

    public void setSource(List<T> source) {
        this.source = source;
    }

    public void setTarget(List<T> target) {
        this.target = target;
    }

    public void setTargetExceptSource(Collection<T> list) {
        setItemsExcept(list, this.target, this.source);
    }

    public void setSourceExceptTarget(Collection<T> list) {
        setItemsExcept(list, this.source, this.target);
    }

    private void setItemsExcept(Collection<T> list,
                                Collection<T> targetList,
                                Collection<T> baseList) {
        for (T t: list) {
            if (!baseList.contains(t)) {
                targetList.add(t);
            }
        }
    }

}

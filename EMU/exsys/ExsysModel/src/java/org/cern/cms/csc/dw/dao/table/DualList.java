package org.cern.cms.csc.dw.dao.table;

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

}

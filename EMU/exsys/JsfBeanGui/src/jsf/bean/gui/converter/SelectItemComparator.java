/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.converter;

import java.util.Comparator;
import javax.faces.model.SelectItem;

/**
 *
 * @author valdor
 */
public class SelectItemComparator implements Comparator<SelectItem> {

    public int compare(SelectItem o1, SelectItem o2) {
        return o1.getLabel().compareTo(o2.getLabel());
    }

}

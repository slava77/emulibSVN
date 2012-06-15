/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.fm;

import java.util.Comparator;
import javax.faces.model.SelectItem;

/**
 *
 * @author valdor
 */
public class TemplateHelpComparator implements Comparator<SelectItem> {

    public int compare(SelectItem s1, SelectItem s2) {
        return s1.getLabel().compareTo(s2.getLabel());
    }

}

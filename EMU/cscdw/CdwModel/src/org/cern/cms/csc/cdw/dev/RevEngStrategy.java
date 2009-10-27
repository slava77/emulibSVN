package org.cern.cms.csc.cdw.dev;

/**
 *
 * @author valdo
 */
import java.util.regex.Pattern;
import java.util.regex.Matcher;
import org.hibernate.cfg.reveng.*;

public class RevEngStrategy extends DelegatingReverseEngineeringStrategy {

  /**
   * Utilities
   */

  private static Pattern prefix = Pattern.compile("^(...)_(.+)$");

  private String removePrefix(String name) {
    Matcher m = prefix.matcher(name);
    if (m.find()) name = m.group(2);
    return name;
  }

  private String singularize(String name) {
    Pattern p = null;
    Matcher m = null;

    p = Pattern.compile("^(.+)(IES|ies)$");
    m = p.matcher(name);
    if (m.find()) name = m.group(1) + "y";

    p = Pattern.compile("^(.+)(SES|ses)$");
    m = p.matcher(name);
    if (m.find()) {
      name = m.group(1) + "s";
    } else {
      p = Pattern.compile("^(.+)[Ss]$");
      m = p.matcher(name);
      if (m.find()) name = m.group(1);
    }

    return name;
  }

  private String toUpperCamelCase(String s) {
    return ReverseEngineeringStrategyUtil.toUpperCamelCase(s);
  }

  /**
   * Extenssions
   */

  public RevEngStrategy(ReverseEngineeringStrategy delegate) {
    super(delegate);
  }

  public String columnToPropertyName(TableIdentifier table, String column) {
    return super.columnToPropertyName(table, removePrefix(column));
  }

  public String tableToClassName(TableIdentifier tableIdentifier) {
    String name = singularize(removePrefix(tableIdentifier.getName()));
    return super.tableToClassName(new TableIdentifier(name));
  }

  public String classNameToCompositeIdName(String className) {
    return className + "Key";
  }

 /*
  public String foreignKeyToCollectionName(String keyname, TableIdentifier fromTable, List fromColumns, TableIdentifier referencedTable, List referencedColumns, boolean uniqueReference) {
    String name = singularize(fromTable.getName());
    return super.foreignKeyToCollectionName(keyname, new TableIdentifier(name), fromColumns, referencedTable, referencedColumns, uniqueReference);
  }

  public String foreignKeyToEntityName(String keyname, TableIdentifier fromTable, List fromColumnNames, TableIdentifier referencedTable, List referencedColumnNames, boolean uniqueReference)
{
    String name = singularize(fromTable.getName());
    return super.foreignKeyToEntityName(keyname, new TableIdentifier(name), fromColumnNames, referencedTable, referencedColumnNames, uniqueReference);
  }
*/

}

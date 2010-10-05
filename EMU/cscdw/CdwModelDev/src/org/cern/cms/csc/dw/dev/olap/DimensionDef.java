package org.cern.cms.csc.dw.dev.olap;

import java.io.PrintWriter;
import java.lang.reflect.Method;
import java.util.Date;
import org.cern.cms.csc.dw.dev.OlapGenerator;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

class DimensionDef extends ColumnDef {

    private boolean timeDimension = false;
    private String tableName = null;
    private boolean baseField = false;
    private boolean shared = false;

    public DimensionDef(CubeDef cube, Method method, String name, String sharedTable, boolean baseField) {
        super(cube, method, name);

        if (method.getReturnType().equals(Date.class)) {
            setTableName(cube.getFact().getTableName(), cube.getTableSuffix());
            this.timeDimension = true;
        }

        if (!sharedTable.equals("")) {
            setTableName(sharedTable);
            this.baseField = baseField;
            this.shared = true;
        }

    }

    private void setTableName(String tableName) {
        setTableName(tableName, "");
    }

    private void setTableName(String tableName, String suffix) {
        if (!suffix.equals("")) {
            suffix = "$".concat(suffix);
        }
        if ((tableName.length() + suffix.length()) > 30) {
            tableName = tableName.substring(0, 30 - suffix.length());
        }
        this.tableName = tableName + suffix;
    }

    @Override
    public void getDDL(PrintWriter out) {

        if (timeDimension) {
            if (!shared || (shared && baseField)) {

                out.println("DROP VIEW " + tableName);
                out.print(OlapGenerator.SQL_ENDL);

                out.println("\nCREATE VIEW " + tableName + " AS");
                out.println("SELECT");
                out.println("\t  a.time_data \"TIME\"");
                out.println("\t, to_number(TO_CHAR(a.time_data, \'YYYY\')) \"YEAR\"");
                out.println("\t, to_number(TO_CHAR(a.time_data, \'MM\')) \"MONTH\"");
                out.println("\t, initcap(trim(TO_CHAR(a.time_data, \'MONTH\'))) \"MONTH_NAME\"");
                out.println("\t, to_number(TO_CHAR(a.time_data, \'W\')) \"WEEK\"");
                out.println("\t, \'Week \' || TO_CHAR(a.time_data, \'W\') \"WEEK_NAME\"");
                out.println("\t, to_number(TO_CHAR(a.time_data, \'DD\')) \"DAY\"");
                out.println("\t, to_number(TO_CHAR(a.time_data, \'D\')) \"WEEKDAY\"");
                out.println("\t, initcap(trim(TO_CHAR(a.time_data, \'DAY\'))) \"WEEKDAY_NAME\"");
                out.println("FROM");
                out.println("\t(SELECT DISTINCT " + columnName + " as time_data FROM " + cube.getFact().getTableName());
                out.println("\tWHERE " + columnName + " is not null) a");
                out.print(OlapGenerator.SQL_ENDL);

            }
        } else {
            if (shared && baseField) {

                out.println("DROP VIEW " + tableName);
                out.print(OlapGenerator.SQL_ENDL);

                out.println("\nCREATE VIEW " + tableName + " AS");
                out.println("SELECT DISTINCT");
                out.println("\t  " + columnName);
                out.println("FROM");
                out.println("\t" + cube.getFact().getTableName());
                out.print(OlapGenerator.SQL_ENDL);
                
            }
        }
    }

    /**
     * Generating Shared dimension usage XML element, i.e.
     * <DimensionUsage source="Time" name="Run Created" caption="Run Created" foreignKey="RUN_CREATE_TIME"></DimensionUsage>
     * @param doc
     * @return element or null if dimension is not shared
     */
    public Element getUsageElement(Document doc) {
        Element usageEl = null;
        if (shared) {
            usageEl = doc.createElement("DimensionUsage");
            usageEl.setAttribute("name", name);
            usageEl.setAttribute("source", name);
            usageEl.setAttribute("foreignKey", columnName);
        }
        return usageEl;
    }

    /**
     * Generating dimension XML element, i.e.:
     * Standard:
     *
     *  <Dimension type="StandardDimension" name="Trg. DT Status">
     *      <Hierarchy name="Trg. DT Status" hasAll="true" allMemberName="All Trg. DT Statuses">
     *          <Level name="Trg. DT Status" column="RUN_TRG_DT" type="String" uniqueMembers="true" levelType="Regular" hideMemberIf="Never"></Level>
     *      </Hierarchy>
     *  </Dimension>
     *
     *  or Time:
     *
     *  <Dimension type="TimeDimension" name="Time">
     *      <Hierarchy name="Time" hasAll="true" allMemberName="All Periods" primaryKey="TIME">
     *          <Table name="RR2_OLAP_RUN_DATE_GLOBAL" schema="CMS_VALDAS" alias=""></Table>
     *          <Level name="Year" column="YEAR" type="Integer" uniqueMembers="true" levelType="TimeYears" hideMemberIf="Never"></Level>
     *          <Level name="Month" column="MONTH" nameColumn="MONTH_NAME" type="Integer" uniqueMembers="true" levelType="TimeMonths" hideMemberIf="Never"></Level>
     *          <Level name="Day" column="DAY" type="Integer" uniqueMembers="true" levelType="TimeDays" hideMemberIf="Never"></Level>
     *      </Hierarchy>
     *  </Dimension>
     * @param doc
     * @return XML element
     */
    @Override
    public Element getElement(Document doc) {

        Element dimEl = doc.createElement("Dimension");
        dimEl.setAttribute("name", name);
        if (timeDimension) {
            dimEl.setAttribute("type", "TimeDimension");
        } else {
            dimEl.setAttribute("type", "StandardDimension");
        }

        Element hierarchyEl = doc.createElement("Hierarchy");
        hierarchyEl.setAttribute("name", name);
        hierarchyEl.setAttribute("hasAll", "true");

        dimEl.appendChild(hierarchyEl);

        if (shared || timeDimension) {
            Element dateTableEl = doc.createElement("Table");
            dateTableEl.setAttribute("name", tableName);
            dateTableEl.setAttribute("schema", cube.getDbSchema());
            hierarchyEl.appendChild(dateTableEl);
        }

        if (timeDimension) {

            hierarchyEl.setAttribute("allMemberName", "All Periods");
            hierarchyEl.setAttribute("primaryKey", "TIME");

            if (!shared) {
                dimEl.setAttribute("foreignKey", columnName);
            }

            Element yEl = doc.createElement("Level");
            yEl.setAttribute("name", "Year");
            yEl.setAttribute("column", "YEAR");
            yEl.setAttribute("type", "Integer");
            yEl.setAttribute("uniqueMembers", "true");
            yEl.setAttribute("levelType", "TimeYears");
            yEl.setAttribute("hideMemberIf", "Never");
            hierarchyEl.appendChild(yEl);

            Element mEl = doc.createElement("Level");
            mEl.setAttribute("name", "Month");
            mEl.setAttribute("column", "MONTH");
            mEl.setAttribute("nameColumn", "MONTH_NAME");
            mEl.setAttribute("type", "Integer");
            mEl.setAttribute("uniqueMembers", "true");
            mEl.setAttribute("levelType", "TimeMonths");
            mEl.setAttribute("hideMemberIf", "Never");
            hierarchyEl.appendChild(mEl);
            
            Element dEl = doc.createElement("Level");
            dEl.setAttribute("name", "Day");
            dEl.setAttribute("column", "DAY");
            dEl.setAttribute("type", "Integer");
            dEl.setAttribute("uniqueMembers", "true");
            dEl.setAttribute("levelType", "TimeDays");
            dEl.setAttribute("hideMemberIf", "Never");
            hierarchyEl.appendChild(dEl);

        } else {

            if (shared) {
                hierarchyEl.setAttribute("primaryKey", columnName);
            }

            Element el = doc.createElement("Level");
            el.setAttribute("name", name);
            el.setAttribute("column", columnName);
            el.setAttribute("type", type.getTypeName());
            el.setAttribute("uniqueMembers", "true");
            el.setAttribute("levelType", "Regular");
            el.setAttribute("hideMemberIf", "Never");
            hierarchyEl.appendChild(el);
            
        }

        return dimEl;

    }

    public boolean isBaseField() {
        return baseField;
    }

    public boolean isShared() {
        return shared;
    }

    public String getTableName() {
        return tableName;
    }

    public boolean isTimeDimension() {
        return timeDimension;
    }

}

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<%@page session="true" contentType="text/html; charset=utf-8" import="org.cern.cms.csc.dw.olap.web.ConfigManager" %>
<%@taglib uri="http://www.tonbeller.com/jpivot" prefix="jp" %>
<%@taglib uri="http://www.tonbeller.com/wcf" prefix="wcf" %>
<%@taglib uri="http://java.sun.com/jstl/core" prefix="c"%>

<html>
    <head>
        <title>CMS CSC DW OLAP</title>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <link rel="stylesheet" type="text/css" href="jpivot/table/mdxtable.css">
        <link rel="stylesheet" type="text/css" href="jpivot/navi/mdxnavi.css">
        <link rel="stylesheet" type="text/css" href="wcf/form/xform.css">
        <link rel="stylesheet" type="text/css" href="wcf/table/xtable.css">
        <link rel="stylesheet" type="text/css" href="wcf/tree/xtree.css">
        <link rel="stylesheet" type="text/css" href="include/olap.css"/>
        <link rel="stylesheet" type="text/css" href="include/custom-theme/jquery-ui-1.8.5.custom.css"/>

        <script src="include/jquery-1.4.2.min.js" type="text/javascript"></script>
        <script src="include/json2.js" type="text/javascript"></script>
        <script src="include/jquery.json-2.2.min.js" type="text/javascript"></script>
        <script src="include/jquery-ui.min.js" type="text/javascript"></script>
        <script src="include/jquery.rest.js" type="text/javascript"></script>
        <script src ="include/cookie.js" type="text/javascript"></script>
        <script src="include/olap.js" type="text/javascript"></script>
    </head>
    <body style="background-color:#FEFEFE;">

        <%
            if (request.getSession().isNew()) {
        %>
            <jp:mondrianQuery id="query01" dataSource="jdbc/cdw" catalogUri="/WEB-INF/OlapExsys.xml">
                <%=ConfigManager.getDefaultQuery()%>
            </jp:mondrianQuery>
        <%
            }
        %>

        <jp:chart id="chart01" query="#{query01}" visible="false"/>
        <form>
            <jp:print id="query01"/>
            <jp:table id="table01" query="#{query01}"/>
            <jp:navigator id="navi01" query="#{query01}" visible="true"/>
            <wcf:form id="mdxedit01" xmlUri="/WEB-INF/jpivot/table/mdxedit.xml" model="#{query01}" visible="true"/>
            <wcf:form id="sortform01" xmlUri="/WEB-INF/jpivot/table/sortform.xml" model="#{table01}" visible="true"/>
            <jp:print id="print01"/>
            <wcf:form id="printform01" xmlUri="/WEB-INF/jpivot/print/printpropertiesform.xml" model="#{print01}" visible="true"/>
            <wcf:form id="chartform01" xmlUri="/WEB-INF/jpivot/chart/chartpropertiesform.xml" model="#{chart01}" visible="true"/>
            <wcf:table id="query01.drillthroughtable"  visible="true" selmode="none" editable="true"/>

            <jp:selectproperties id="selectprop01" table="#{table01}" visible="true"/>

            <%-- define a toolbar --%>
            <wcf:toolbar id="toolbar01" bundle="org.cern.cms.csc.dw.olap.resource.messages">

                <wcf:imgbutton id="cubes" tooltip="Cubes.button" img="cubes" href="cubes"/>

                <wcf:separator/>

                <wcf:imgbutton id="navi" tooltip="CubeNavi.button" img="cube" href="navi"/>
                <wcf:imgbutton id="mdxedit" tooltip="MdxEdit.button" img="mdx-edit" href="mdxedit"/>
                <wcf:imgbutton id="sort" tooltip="SortConfig.button" img="sort-asc" href="sort"/>

                <wcf:separator/>

                <wcf:scriptbutton id="levelStyle" tooltip="LevelStyle.button" img="level-style" model="#{table01.extensions.axisStyle.levelStyle}"/>
                <wcf:scriptbutton id="hideSpans" tooltip="HideSpans.button" img="hide-spans" model="#{table01.extensions.axisStyle.hideSpans}"/>

                <wcf:scriptbutton id="propertiesButton" tooltip="Propeperties.button"  img="properties" model="#{table01.rowAxisBuilder.axisConfig.propertyConfig.showProperties}"/>
                <wcf:imgbutton id="selectProp" tooltip="SelectProperties.button"  img="properties-config" href="selectprop"/>

                <wcf:scriptbutton id="nonEmpty" tooltip="RemoveEmpty.button" img="non-empty" model="#{table01.extensions.nonEmpty.buttonPressed}"/>
                <wcf:scriptbutton id="swapAxes" tooltip="SwapAxis.button"  img="swap-axes" model="#{table01.extensions.swapAxes.buttonPressed}"/>

                <wcf:separator/>

                <wcf:scriptbutton model="#{table01.extensions.drillMember.enabled}"	 tooltip="DrillMember.button" radioGroup="navi" id="drillMember"   img="navi-member"/>
                <wcf:scriptbutton model="#{table01.extensions.drillPosition.enabled}" tooltip="DrillPosition.button" radioGroup="navi" id="drillPosition" img="navi-position"/>
                <wcf:scriptbutton model="#{table01.extensions.drillReplace.enabled}" tooltip="DrillReplace.button" radioGroup="navi" id="drillReplace"  img="navi-replace"/>
                <wcf:scriptbutton model="#{table01.extensions.drillThrough.enabled}"  tooltip="DrillThrough.button" id="drillthrough"  img="navi-through"/>

                <wcf:separator/>

                <wcf:scriptbutton id="chartButton01" tooltip="Chart.button" img="chart" model="#{chart01.visible}"/>
                <wcf:imgbutton id="chartprop" tooltip="ChartProperties.button" img="chart-config" href="chartprop"/>

                <wcf:separator/>

                <wcf:imgbutton id="printprop" tooltip="PrintProperties.button" img="print-config" href="printprop"/>
                <wcf:imgbutton id="printpdf" tooltip="PrintPdf.button" img="print" href="./Print?cube=01&type=1"/>
                <wcf:imgbutton id="printxls" tooltip="PrintXls.button" img="excel" href="./Print?cube=01&type=0"/>
                <wcf:separator/>

                <wcf:imgbutton id="refresh" tooltip="Refresh.button" img="refresh" href="action?id=refresh"/>
                <wcf:imgbutton id="reset" tooltip="Reset.button" img="reset" href="action?id=reset"/>

            </wcf:toolbar>

            <jsp:setProperty name="navi01" property="visible" value="true"/>
            <jsp:setProperty name="selectprop01" property="visible" value="true"/>
            <jsp:setProperty name="query01.drillthroughtable"  property="visible" value="true"/>

            <%-- printing error messages here --%>
            <div id="msg">
                <center>
                </center>
            </div>

            <%-- render toolbar --%>
            <div id="toolb" class="buttonPanel">
                <table width="100%">
                    <tr>
                        <td width="250" align="center">
                            <div class="tableNameLabel">CSC Data Warehouse OLAP</div>
                        </td>
                        <td>
                            <wcf:render ref="toolbar01" xslUri="/WEB-INF/jpivot/toolbar/htoolbar.xsl" xslCache="true"/>
                        </td>
                    </tr>
                </table>
            </div>

            <%-- render table --%>

            <div style="margin: 10px">
                
                <div id="chart">
                    <wcf:render ref="chart01" xslUri="/WEB-INF/jpivot/chart/chart.xsl" xslCache="true"/>
                </div>

                <wcf:render ref="table01" xslUri="/WEB-INF/jpivot/table/mdxtable.xsl" xslCache="true"/>

                <div id="slicer">
                    <wcf:render ref="table01" xslUri="/WEB-INF/jpivot/table/mdxslicer.xsl" xslCache="true"/>
                </div>

            </div>

            <%-- drill through table --%>
            <div id="drillthrough" class="window" style="width: 300px; overflow: auto; display: none">
                <wcf:render ref="query01.drillthroughtable" xslUri="/WEB-INF/wcf/wcf.xsl" xslCache="true"/>
            </div>

            <%-- render navigator --%>
            <div id="navi" style="max-height: 98%; overflow: auto; display: none">
                <wcf:render ref="navi01" xslUri="/WEB-INF/jpivot/navi/navigator.xsl" xslCache="true"/>
             <%--   <wcf:render ref="selectprop01" xslUri="/WEB-INF/jpivot/navi/navigator.xsl" xslCache="true"/> --%>
            </div>

            <div id="selectprop" style="display: none">
                <wcf:render ref="selectprop01" xslUri="/WEB-INF/jpivot/navi/navigator.xsl" xslCache="true"/>
            </div>
            <%-- edit mdx --%>
            <div id="mdxedit" class="window" style="display: none">
                <wcf:render ref="mdxedit01" xslUri="/WEB-INF/wcf/wcf.xsl" xslCache="true"/>
            </div>

            <%-- sort properties --%>
            <div id="sort" class="window" style="display: none">
                <wcf:render ref="sortform01" xslUri="/WEB-INF/wcf/wcf.xsl" xslCache="true"/>
            </div>

            <%-- chart properties --%>
            <div id="chartprop" class="window" style="display: none">
                <wcf:render ref="chartform01" xslUri="/WEB-INF/wcf/wcf.xsl" xslCache="true"/>
            </div>

            <%-- print properties --%>
            <div id="printprop" class="window" style="display: none">
                <wcf:render ref="printform01" xslUri="/WEB-INF/wcf/wcf.xsl" xslCache="true"/>
            </div>

            <div id="cubes" style="display: none">
            </div>

            <div id="bookmarks" style="display: none">
                <select id="bookmarkList" style="width: 100%">
                </select>
                <p>
                <button title="Load" style="width: 31%">Load</button>
                <button title="Save" style="width: 31%">Save</button>
                <button title="Delete" style="width: 31%">Delete</button>
            </div>

            <div id="bookmarkName" style="display: none">
                <input>
                <p>
                <button title="OK" style="width: 31%">OK</button>
                <button title="Cancel" style="width: 31%">Cancel</button>
            </div>

        </form>
    </body>
</html>
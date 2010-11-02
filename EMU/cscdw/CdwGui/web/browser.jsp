<!--
<%@taglib prefix="ice" uri="http://www.icesoft.com/icefaces/component" %>
<%@taglib prefix="f" uri="http://java.sun.com/jsf/core" %>
<%@taglib prefix="ui" uri="http://java.sun.com/jsf/facelets" %>
-->

<ui:composition
    xmlns:cdw="http://www.cern.ch/cms/csc/dw/gui/ontology"
    xmlns:h="http://java.sun.com/jsf/html"
    xmlns:f="http://java.sun.com/jsf/core"
    xmlns:ice="http://www.icesoft.com/icefaces/component"
    xmlns:c="http://java.sun.com/jstl/core"
    xmlns:ui="http://java.sun.com/jsf/facelets"
    template="WEB-INF/resources/main.jsp">

    <ui:define name="content">

        <ice:outputLabel value="Table" for="tableRowClass"/>
        <ice:selectOneListbox size="1" id="tableRowClass" value="#{BeanTableController.tableRowClassName}">
            <f:selectItems value="#{BeanTableController.entities}"/>
        </ice:selectOneListbox>

        <ice:commandButton value="Apply"/>

        <ice:panelGroup rendered="#{BeanTableController.tableSet}">

            <table border="0" cellspacing="0" width="100%" class="iceDatTblColHdr">
                <tr>
                    <td>
                        <ice:panelGrid columns="8" cellspacing="2" cellpadding="2">

                            <ice:outputText value="#{BeanTableController.table.title}" styleClass="tableNameLabel" style="margin-right: 5px"/>

                            <ice:commandLink actionListener="#{BeanTableController.table.refreshListener}" value="Refresh" title="Refresh" style="font-weight: bold"/>

                            |

                            <ice:commandLink value="Filter" title="Filter" style="font-weight: bold">
                                <f:setPropertyActionListener target="#{BeanTableController.displayFilter}" value="#{!BeanTableController.displayFilter}"/>
                            </ice:commandLink>

                            |

                            <ice:commandLink value="Columns" title="Columns" style="font-weight: bold">
                                <f:setPropertyActionListener target="#{BeanTableController.showSelectColumns}" value="#{!BeanTableController.showSelectColumns}"/>
                            </ice:commandLink>

                            |

                            <ice:commandLink value="Sorting" title="Sorting" style="font-weight: bold">
                                <f:setPropertyActionListener target="#{BeanTableController.showSortByColumns}" value="#{!BeanTableController.showSortByColumns}"/>
                            </ice:commandLink>

                        </ice:panelGrid>
                    </td>
                    <td align="right">

                        <ice:panelGrid columns="10" border="0">

                            <ice:commandButton
                                image="/xmlhttp/css/xp/css-images/arrow-first.gif"
                                actionListener="#{BeanTableController.table.firstPageListener}"
                                title="First Page" />

                            <ice:commandButton
                                image="/xmlhttp/css/xp/css-images/arrow-fr.gif"
                                actionListener="#{BeanTableController.table.fastbackwardPageListener}"
                                title="Fast Backward" />

                            <ice:commandButton
                                image="/xmlhttp/css/xp/css-images/arrow-previous.gif"
                                actionListener="#{BeanTableController.table.prevPageListener}"
                                title="Previous Page" />

                            <ice:panelGroup>
                                <ice:outputFormat value="{0} items. Show {1} from {2} to {3}. Page {4} / {5}">
                                    <f:param value="#{BeanTableController.table.dataCount}"/>
                                    <f:param value="#{BeanTableController.table.dataSize}"/>
                                    <f:param value="#{BeanTableController.table.dataFirstIndex}"/>
                                    <f:param value="#{BeanTableController.table.dataLastIndex}"/>
                                    <f:param value="#{BeanTableController.table.pageIndex}"/>
                                    <f:param value="#{BeanTableController.table.pageCount}"/>
                                </ice:outputFormat>
                            </ice:panelGroup>

                            <ice:commandButton
                                image="/xmlhttp/css/xp/css-images/arrow-next.gif"
                                actionListener="#{BeanTableController.table.nextPageListener}"
                                title="Next Page" />

                            <ice:commandButton
                                image="/xmlhttp/css/xp/css-images/arrow-ff.gif"
                                actionListener="#{BeanTableController.table.fastforwardPageListener}"
                                title="Fast Forward" />

                            <ice:commandButton
                                image="/xmlhttp/css/xp/css-images/arrow-last.gif"
                                actionListener="#{BeanTableController.table.lastPageListener}"
                                title="Last Page" />

                            <ice:panelGrid columns="2" cellpadding="0" cellspacing="0">
                                <ice:outputLabel value="#{msgs['table.selectPage']}"/>
                                <ice:inputText value="#{BeanTableController.table.pageIndex}" style="width: 35px; margin: 0px" title="Select Page">
                                    <f:converter converterId="javax.faces.Integer"/>
                                </ice:inputText>
                            </ice:panelGrid>

                            <ice:panelGrid columns="2" cellpadding="0" cellspacing="0">
                                <ice:outputLabel value="Page Size"/>
                                <ice:selectOneMenu value="#{BeanTableController.table.pageSize}" partialSubmit="true" title="Page Size" style="margin: 0px">
                                    <f:selectItems value="#{BeanTableController.table.pageSizeItems}"/>
                                </ice:selectOneMenu>
                            </ice:panelGrid>

                        </ice:panelGrid>

                    </td>
                </tr>
            </table>

            <ice:dataTable id="beanTable"
                           var="item"
                           width="100%"
                           value="#{BeanTableController.table.data}"
                           varStatus="itemStatus"
                           style="margin-top: 0px; width: 100%"
                           columnWidths="#{BeanTableController.table.columnWidths}"
                           sortColumn="#{BeanTableController.table.sortColumn}"
                           sortAscending="#{BeanTableController.table.ascending}">

                <ice:columns value="#{BeanTableController.table.columns}" var="selColumn">

                    <f:facet name="header">

                        <ice:panelGroup style="width: #{selColumn.width}px; overflow: hidden">

                            <ice:panelGrid columns="1" width="100%" cellpadding="0" cellspacing="0" style="height: 100%" border="0">

                                <ice:panelGroup style="height: 100%">
                                    <ice:commandSortHeader columnName="#{selColumn.propertyMd.name}">
                                        <ice:outputText value="#{selColumn.propertyMd.title}" title="#{selColumn.propertyMd.title}"/>
                                    </ice:commandSortHeader>
                                </ice:panelGroup>

                                <ice:panelGroup rendered="#{BeanTableController.displayFilter}">

                                    <ice:panelGroup panelTooltip="filterInputBox" styleClass="tableFilterBox">
                                        <ice:outputText
                                            converter="#{selColumn.filterConverter}"
                                            value="#{BeanTableController.table.filter[selColumn.propertyMd.name]}"/>
                                    </ice:panelGroup>

                                    <ice:panelTooltip id="filterInputBox"
                                                      displayOn="click" hideOn="mousedown"
                                                      hoverDelay="100" draggable="false">

                                        <f:facet name="header">
                                            <ice:outputText value="[#{selColumn.propertyMd.title}] filter"/>
                                        </f:facet>

                                        <f:facet name="body">
                                            <ice:panelGroup>
                                                <ice:inputText
                                                    size="50"
                                                    required="false"
                                                    partialSubmit="true"
                                                    converter="#{selColumn.filterConverter}"
                                                    actionListener="#{BeanTableController.table.filterListener}"
                                                    value="#{BeanTableController.table.filter[selColumn.propertyMd.name]}"/>
                                            </ice:panelGroup>
                                        </f:facet>

                                    </ice:panelTooltip>
                                </ice:panelGroup>

                            </ice:panelGrid>

                        </ice:panelGroup>
                    </f:facet>
  
                    <ice:panelGroup style="width: #{selColumn.width}px; overflow: hidden">

                        <ice:outputText style="width: 100%; text-align: left"
                                        converter="#{selColumn.converter}"
                                        value="#{BeanTableController.table.cellValue}"/>

                    </ice:panelGroup>

                </ice:columns>

            </ice:dataTable>

            <c:if test="#{BeanTableController.table.setColumnWidth}">

                <ice:inputHidden id="beanTable_input" value="#{BeanTableController.table.columnWidths}"/>

                <script type="text/javascript">
                    // <![CDATA[
                    //jQuery.noConflict();
                    jQuery(document).ready( function () {

                        var tableId = ("beanTable").toString();
                        var table = 0;

                        jQuery('table').each(
                            function() {
                                if (table == 0) {
                                    var a = this.id.toString().split(":");
                                    if (a.length > 0 && a[a.length - 1] == tableId) {
                                        table = this;
                                    }
                                }
                            }
                        );

                        if (table != 0) {

                            var columnWidths = "";

                            jQuery(table).find("th").each(
                                function() {
                                    if (columnWidths != "") {
                                        columnWidths += ",";
                                    }
                                    columnWidths += jQuery(this).width();
                                }
                            );

                            var input = 0;

                            jQuery("input[type=hidden]").each(
                                function() {
                                    if (input == 0) {
                                        var a = this.id.toString().split(":");
                                        if (a.length > 0 && a[a.length - 1] == (tableId + "_input")) {
                                            input = this;
                                        }
                                    }
                                }
                            );

                            if (input != 0) {
                                jQuery(input).val(columnWidths);
                            }

                        }

                    });
                    // ]]>
                </script>

            </c:if>

            <ice:panelPopup autoCentre="true" modal="false"
                            style="z-index: 1000; overflow: hidden; height: 490px; width: 440px"
                            rendered="#{BeanTableController.showSelectColumns}">

                <f:facet name="header">
                    <ice:panelGroup>
                        <table width="100%">
                            <tr>
                                <td width="100%">
                                    <ice:outputText value="Table Columns" />
                                </td>
                                <td width="1%">
                                    <ice:commandButton type="button"
                                                       image="/xmlhttp/css/rime/css-images/cal_off.gif"
                                                       style="vertical-align: middle; text-align: right"
                                                       title="Close" alt="Close">
                                        <f:setPropertyActionListener target="#{BeanTableController.showSelectColumns}" value="#{!BeanTableController.showSelectColumns}"/>
                                    </ice:commandButton>
                                </td>
                            </tr>
                        </table>
                    </ice:panelGroup>
                </f:facet>

                <f:facet name="body">

                    <ice:panelGroup>
                        <ice:panelGrid columns="2">

                            <ice:outputText value="Available Columns"/>

                            <ice:outputText value="Selected Columns"/>

                            <ice:panelPositioned var="selColumn"
                                                 value="#{BeanTableController.table.availableColumns}"
                                                 styleClass="positionPanelContainer"
                                                 style="width:200px; height:400px; overflow-x: hidden; overflow-y: auto; border:2px solid #8b8c8b;">

                                <ice:panelGroup style="cursor:move;" styleClass="container" >
                                    <ice:outputText value="#{selColumn.propertyMd.title}"/>
                                </ice:panelGroup>

                            </ice:panelPositioned>

                            <ice:panelPositioned var="selColumn"
                                                 value="#{BeanTableController.table.selectedColumns}"
                                                 styleClass="positionPanelContainer"
                                                 listener="#{BeanTableController.table.applySelectedColumnsListener}"
                                                 style="width:200px; height:400px; overflow-x:hidden; overflow-y:auto; border:2px solid red;">

                                <ice:panelGroup style="cursor:move;" styleClass="container">
                                    <ice:panelGrid columns="2" width="100%" cellpadding="0" cellspacing="0" border="0">
                                        <ice:panelGroup style="width: 100px;">
                                            <ice:outputText value="#{selColumn.propertyMd.title}"/>
                                        </ice:panelGroup>
                                        <ice:panelGrid columns="2" style="width: 10px" border="0">
                                            <ice:outputText value="#{selColumn.width}px">
                                                <f:convertNumber integerOnly="true" groupingUsed="false"/>
                                            </ice:outputText>
                                            <ice:panelGrid columns="1" width="10px">
                                                <ice:commandButton image="/media/image/spin_up.gif" actionListener="#{BeanTableController.table.columnWidthListener}">
                                                    <f:attribute name="colName" value="#{selColumn.propertyMd.name}"/>
                                                    <f:attribute name="step" value="#{3}"/>
                                                </ice:commandButton>
                                                <ice:commandButton image="/media/image/spin_down.gif" actionListener="#{BeanTableController.table.columnWidthListener}">
                                                    <f:attribute name="colName" value="#{selColumn.propertyMd.name}"/>
                                                    <f:attribute name="step" value="#{-3}"/>
                                                </ice:commandButton>
                                            </ice:panelGrid>
                                        </ice:panelGrid>
                                    </ice:panelGrid>
                                </ice:panelGroup>

                            </ice:panelPositioned>

                        </ice:panelGrid>

                        <ice:panelGroup>
                            <ice:commandButton value="Reset Column Width"
                                               style="margin-right: 5px"
                                               actionListener="#{BeanTableController.table.resetColumnWidthListener}">
                                <f:setPropertyActionListener target="#{BeanTableController.showSelectColumns}" value="#{!BeanTableController.showSelectColumns}"/>
                            </ice:commandButton>
                            <ice:commandButton value="Close"
                                               style="margin-right: 5px">
                                <f:setPropertyActionListener target="#{BeanTableController.showSelectColumns}" value="#{!BeanTableController.showSelectColumns}"/>
                            </ice:commandButton>
                        </ice:panelGroup>

                    </ice:panelGroup>

                </f:facet>
            </ice:panelPopup>

            <ice:panelPopup autoCentre="true" modal="false"
                            style="z-index: 1000; overflow: hidden; height: 500px; width: 440px"
                            rendered="#{BeanTableController.showSortByColumns}">

                <f:facet name="header">
                    <ice:panelGroup>
                        <table width="100%">
                            <tr>
                                <td width="100%">
                                    <ice:outputText value="Table Sorting Columns" />
                                </td>
                                <td width="1%">
                                    <ice:commandButton type="button"
                                                       image="/xmlhttp/css/rime/css-images/cal_off.gif"
                                                       style="vertical-align: middle; text-align: right"
                                                       actionListener="#{BeanTableController.table.cancelSortListener}"
                                                       title="Close" alt="Close">
                                        <f:setPropertyActionListener target="#{BeanTableController.showSortByColumns}" value="#{!BeanTableController.showSortByColumns}"/>
                                    </ice:commandButton>
                                </td>
                            </tr>
                        </table>
                    </ice:panelGroup>
                </f:facet>

                <f:facet name="body">

                    <ice:panelGroup>

                        <ice:panelGrid columns="2">

                            <ice:outputText value="Available Columns"/>

                            <ice:outputText value="Selected Columns"/>

                            <ice:panelPositioned var="sortByColumn"
                                                 value="#{BeanTableController.table.availableSortByColumns}"
                                                 styleClass="positionPanelContainer"
                                                 style="width:200px; height:400px; overflow-x:hidden; overflow-y:auto; border:2px solid #8b8c8b;">

                                <ice:panelGroup style="cursor:move;" styleClass="container" >
                                    <ice:outputText value="#{sortByColumn.propertyMd.title}"/>
                                </ice:panelGroup>

                            </ice:panelPositioned>

                            <ice:panelPositioned var="sortByColumn"
                                                 value="#{BeanTableController.table.sortByColumns}"
                                                 styleClass="positionPanelContainer"
                                                 style="width:200px; height:400px; overflow-x:hidden; overflow-y:auto; border:2px solid red;">

                                <ice:panelGroup style="cursor:move;" styleClass="container">
                                    <ice:selectBooleanCheckbox partialSubmit="true"
                                                               title="Ascending?"
                                                               value="#{sortByColumn.ascending}"/>
                                    <ice:outputText value="#{sortByColumn.propertyMd.title}"/>
                                </ice:panelGroup>

                            </ice:panelPositioned>

                        </ice:panelGrid>

                        <ice:panelGroup>
                            <ice:commandButton value="Apply"
                                               style="margin-right: 5px"
                                               actionListener="#{BeanTableController.table.applySortListener}">
                                <f:setPropertyActionListener target="#{BeanTableController.showSortByColumns}" value="#{!BeanTableController.showSortByColumns}"/>
                            </ice:commandButton>
                            <ice:commandButton value="Cancel"
                                               actionListener="#{BeanTableController.table.cancelSortListener}">
                                <f:setPropertyActionListener target="#{BeanTableController.showSortByColumns}" value="#{!BeanTableController.showSortByColumns}"/>
                            </ice:commandButton>
                        </ice:panelGroup>

                    </ice:panelGroup>

                </f:facet>
            </ice:panelPopup>

        </ice:panelGroup>

    </ui:define>

</ui:composition>
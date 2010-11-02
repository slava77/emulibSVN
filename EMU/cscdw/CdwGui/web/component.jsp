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

        <ice:panelGroup rendered="#{ComponentClassTreeController.selectedComponent != null}">

            <h3>
                <ice:outputFormat value="Component {0} (class: {1})">
                    <f:param value="#{ComponentClassTreeController.selectedComponent.name}" />
                    <f:param value="#{ComponentClassTreeController.selectedComponent.type.type.value}" />
                </ice:outputFormat>
            </h3>

            <ice:panelGrid columns="2">

                <ice:outputLabel value="Direct class:"/>
                <cdw:componentClassLink value="#{ComponentClassTreeController.selectedComponent.type}" />

                <ice:outputLabel value="All classes:"/>
                <ice:panelGroup>
                    <ice:panelSeries value="#{ComponentClassTreeController.selectedComponent.types}" var="type" varStatus="status">
                        <cdw:componentClassLink value="#{type}" />
                        <ice:outputText value="," rendered="#{!(status.index == status.end)}"/>
                    </ice:panelSeries>
                </ice:panelGroup>

                <ice:outputLabel value="Component:"/>
                <cdw:componentLink value="#{ComponentClassTreeController.selectedComponent}" />

                <ice:outputLabel value="Synonyms:"/>
                <ice:panelGroup>
                    <ice:panelSeries value="#{ComponentClassTreeController.selectedComponent.synonyms}" var="synonym" varStatus="status">
                        <ice:outputText value="#{synonym.name}"/>
                        <ice:outputText value="," rendered="#{!(status.index == status.end)}"/>
                    </ice:panelSeries>
                </ice:panelGroup>

            </ice:panelGrid>

            <ice:outputLabel value="Component Links:"/>

            <ice:dataTable id="componentLinkTable" value="#{ComponentClassTreeController.selectedComponent.links}" var="link" rows="15">

                <f:facet name="header">
                    <ice:columnGroup>
                        <ice:headerRow>
                            <ice:column>
                                <ice:outputText value="ID"/>
                            </ice:column>
                            <ice:column>
                                <ice:outputText value="From Component"/>
                            </ice:column>
                            <ice:column>
                                <ice:outputText value="Link"/>
                            </ice:column>
                            <ice:column>
                                <ice:outputText value="To Component"/>
                            </ice:column>
                            <ice:column>
                                <ice:outputText value="To Class"/>
                            </ice:column>
                        </ice:headerRow>
                    </ice:columnGroup>
                </f:facet>

                <ice:column>
                    <ice:outputText value="#{link.id}"/>
                </ice:column>
                <ice:column>
                    <cdw:componentLink value="#{ComponentClassTreeController.selectedComponent}" />
                </ice:column>
                <ice:column>
                    <ice:outputText value="#{link.type.value}"/>
                </ice:column>
                <ice:column>
                    <cdw:componentLink value="#{link.component}" />
                </ice:column>
                <ice:column>
                    <cdw:componentClassLink value="#{link.component.type}" />
                </ice:column>

            </ice:dataTable>

            <ice:panelGrid columns="2">

                 <ice:dataPaginator for="componentLinkTable"
                     fastStep="10"
                     pageCountVar="pageCount"
                     pageIndexVar="pageIndex"
                     paginator="true"
                     paginatorMaxPages="9">
                     <f:facet name="first" >
                             <ice:graphicImage url="media/css/rime/css-images/arrow-first.gif" style="border:none;"/>
                    </f:facet>
                     <f:facet name="last">
                            <ice:graphicImage url="media/css/rime/css-images/arrow-last.gif" style="border:none;" />
                    </f:facet>
                     <f:facet name="previous">
                            <ice:graphicImage url="media/css/rime/css-images/arrow-previous.gif" style="border:none;" />
                    </f:facet>
                     <f:facet name="next">
                            <ice:graphicImage url="media/css/rime/css-images/arrow-next.gif" style="border:none;" />
                    </f:facet>
                     <f:facet name="fastforward">
                            <ice:graphicImage url="media/css/rime/css-images/arrow-ff.gif" style="border:none;" />
                    </f:facet>
                     <f:facet name="fastrewind">
                            <ice:graphicImage url="media/css/rime/css-images/arrow-fr.gif" style="border:none;" />
                    </f:facet>
                </ice:dataPaginator>

                <ice:dataPaginator
                    for="componentLinkTable"
                    rowsCountVar="rowsCount"
                    displayedRowsCountVar="displayedRowsCountVar"
                    firstRowIndexVar="firstRowIndex"
                    lastRowIndexVar="lastRowIndex"
                    pageCountVar="pageCount"
                    pageIndexVar="pageIndex">
                    <ice:outputFormat value="{0} links found, displaying {1}, from {2} to {3}. Page {4} / {5}">
                        <f:param value="#{rowsCount}" />
                        <f:param value="#{displayedRowsCountVar}" />
                        <f:param value="#{firstRowIndex}" />
                        <f:param value="#{lastRowIndex}" />
                        <f:param value="#{pageIndex}" />
                        <f:param value="#{pageCount}" />
                    </ice:outputFormat>
                 </ice:dataPaginator>

            </ice:panelGrid>


        </ice:panelGroup>

    </ui:define>
</ui:composition>

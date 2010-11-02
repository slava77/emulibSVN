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

        <ice:panelGrid columns="2">

            <ice:panelGroup>

                <ice:commandLink action="explorer" value="Reset" actionListener="#{ComponentClassTreeController.resetAction}"/>

                <h4><ice:outputText value="Component Classes" /></h4>
                <ice:tree id="componentClassTree"
                    value="#{ComponentClassTreeController.model}"
                    var="node"
                    hideRootNode="true"
                    hideNavigation="false">
                    <ice:treeNode>
                        <f:facet name="icon">
                            <ice:panelGroup style="display: inline" >
                                <ice:graphicImage value="media/css/rime/css-images/#{node.userObject.icon}"/>
                            </ice:panelGroup>
                        </f:facet>
                        <f:facet name="content">
                            <ice:panelGroup style="display: inline">
                                <cdw:componentClassLink value="#{node.userObject.componentClass}" />
                            </ice:panelGroup>
                        </f:facet>
                    </ice:treeNode>
                </ice:tree>

                <h4><ice:outputText value="Component Link Classes" /></h4>
                <ice:tree id="componentLinkClassTree"
                    value="#{ComponentLinkClassTreeController.model}"
                    var="node"
                    hideRootNode="true"
                    hideNavigation="false">
                    <ice:treeNode>
                        <f:facet name="icon">
                            <ice:panelGroup style="display: inline" >
                                <ice:graphicImage value="media/css/rime/css-images/#{node.userObject.icon}"/>
                            </ice:panelGroup>
                        </f:facet>
                        <f:facet name="content">
                            <ice:panelGroup style="display: inline">
                                <cdw:componentLinkClassLink value="#{node.userObject.componentLinkClass}" />
                            </ice:panelGroup>
                        </f:facet>
                    </ice:treeNode>
                </ice:tree>

            </ice:panelGroup>

            <ice:panelGroup>

                <ice:panelGrid columns="2" columnClasses="componentMatchesSelect componentMatchesButton">
                    <ice:panelGrid columns="1">
                        <ice:selectInputText
                            rows="#{ComponentClassTreeController.componentMatchesToDisplay}"
                            width="300"
                            value="#{ComponentClassTreeController.selectedComponentName}"
                            valueChangeListener="#{ComponentClassTreeController.componentInputValueChanged}"
                            listVar="cmp"
                            listValue="#{ComponentClassTreeController.componentMatches}">
                            <f:facet name="selectInputText">
                                <ice:panelGrid columns="2" columnClasses="componentMatchesName componentMatchesType">
                                    <ice:outputText value="#{cmp.name}"/>
                                    <ice:outputText value="#{cmp.type.type.value}" style="font-style: italic" />
                                </ice:panelGrid>
                            </f:facet>
                        </ice:selectInputText>
                        <ice:outputFormat value="Limited to class {0}" rendered="#{ComponentClassTreeController.selectedComponentClass != null}">
                            <f:param value="#{ComponentClassTreeController.selectedComponentClass.type.value}"/>
                        </ice:outputFormat>
                    </ice:panelGrid>
                    <ice:commandButton
                        value="Details"
                        action="component"
                        disabled="#{ComponentClassTreeController.selectedComponent == null}"/>
                </ice:panelGrid>

                <ice:panelGroup rendered="#{ComponentClassTreeController.selectedComponentClass != null}">

                    <h2>
                        <ice:outputFormat value="Component Class: {0}">
                            <f:param value="#{ComponentClassTreeController.selectedComponentClass.type.value}"/>
                        </ice:outputFormat>
                    </h2>

                    <ice:panelGrid columns="2">

                        <ice:outputLabel value="Subclass Of:"/>
                        <ice:panelGroup>
                            <ice:panelSeries value="#{ComponentClassTreeController.selectedComponentClass.parentsRecursive}" var="parent" varStatus="status">
                                <cdw:componentClassLink value="#{parent}" />
                                <ice:outputText value="," rendered="#{!(status.index == status.end)}"/>
                            </ice:panelSeries>
                        </ice:panelGroup>

                        <ice:outputLabel value="Has subclasses:"/>
                        <ice:panelGroup>
                            <ice:panelSeries value="#{ComponentClassTreeController.selectedComponentClass.childrenRecursive}" var="child" varStatus="status">
                                <cdw:componentClassLink value="#{child}" />
                                <ice:outputText value="," rendered="#{!(status.index == status.end)}"/>
                            </ice:panelSeries>
                        </ice:panelGroup>

                        <ice:outputLabel value="Description:"/>
                        <ice:outputText value="#{ComponentClassTreeController.selectedComponentClass.description}"/>

                    </ice:panelGrid>

                    <ice:panelGroup rendered="#{!ComponentClassTreeController.selectedComponentClass.hasComponentsRecursive}">
                        <ice:outputText value="Components not found"/>
                    </ice:panelGroup>

                    <ice:panelGroup rendered="#{ComponentClassTreeController.selectedComponentClass.hasComponentsRecursive}">
                        <ice:dataTable id="componentTable"
                            value="#{ComponentClassTreeController.selectedComponentClass.componentsRecursive}"
                            var="comp" rows="15">

                            <f:facet name="header">
                                <ice:columnGroup>
                                    <ice:headerRow>
                                        <ice:column>
                                            <ice:outputText value="ID"/>
                                        </ice:column>
                                        <ice:column>
                                            <ice:outputText value="Name"/>
                                        </ice:column>
                                        <ice:column>
                                            <ice:outputText value="Class"/>
                                        </ice:column>
                                    </ice:headerRow>
                                </ice:columnGroup>
                            </f:facet>

                            <ice:column>
                                <ice:outputText value="#{comp.id}"/>
                            </ice:column>
                            <ice:column>
                                <cdw:componentLink value="#{comp}" />
                            </ice:column>
                            <ice:column>
                                <cdw:componentClassLink value="#{comp.type}" />
                            </ice:column>

                        </ice:dataTable>

                        <ice:panelGrid columns="2">

                             <ice:dataPaginator for="componentTable"
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
                                for="componentTable"
                                rowsCountVar="rowsCount"
                                displayedRowsCountVar="displayedRowsCountVar"
                                firstRowIndexVar="firstRowIndex"
                                lastRowIndexVar="lastRowIndex"
                                pageCountVar="pageCount"
                                pageIndexVar="pageIndex">
                                <ice:outputFormat value="{0} components found, displaying {1}, from {2} to {3}. Page {4} / {5}">
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

                </ice:panelGroup>

            </ice:panelGroup>
                                
        </ice:panelGrid>

    </ui:define>

</ui:composition>
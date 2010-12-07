<!--
<%@taglib prefix="ice" uri="http://www.icesoft.com/icefaces/component" %>
<%@taglib prefix="f" uri="http://java.sun.com/jsf/core" %>
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

        <ice:panelTabSet>
            <ice:panelTab label="Current Status">

                <ice:panelGrid columns="1">

                    <ice:panelGrid columns="3">

                        <ice:panelGroup>

                            <h2>Database</h2>

                            <ice:panelGrid columns="2">
                                <ice:outputLabel value="Status: "/>
                                <ice:outputText value="OK" rendered="#{MonitorController.databaseDate != null}" style="color: green"/>
                                <ice:outputText value="Not OK" rendered="#{MonitorController.databaseDate == null}" style="color: red"/>
                                <ice:outputLabel value="Sysdate: "/>
                                <ice:outputText value="#{MonitorController.databaseDate}"/>
                            </ice:panelGrid>

                        </ice:panelGroup>

                        <ice:panelGroup>
                            <h2>Queues</h2>

                            <table border="0">
                                <tr>
                                    <td>
                                        <ice:outputLabel value="Name"/>
                                    </td><td>
                                        <ice:outputLabel value="Size"/>
                                    </td><td>
                                        <ice:outputLabel value="Message"/>
                                    </td>
                                </tr>
                                <ui:repeat value="#{MonitorController.queues}" var="q">
                                    <tr>
                                        <td>
                                            <ice:outputText value="#{q.name}"/>
                                        </td><td>
                                            <ice:outputText value="#{q.size}"/>
                                        </td><td>
                                            <ice:commandLink value="flush" actionListener="#{q.flushMessageListener}"/>
                                        </td>
                                    </tr>
                                </ui:repeat>
                            </table>

                        </ice:panelGroup>
                        <ice:panelGroup>
                            <h2>Chart</h2>

                            <ice:outputLabel value="Last hours"/>
                            <ice:selectOneMenu value="#{MonitorController.chartLastHours}" immediate="true" partialSubmit="true">
                                <f:selectItems value="#{MonitorController.chartLastHoursItems}"/>
                            </ice:selectOneMenu>

                            <br/>

                            <ice:commandLink value="Refresh" action="monitor"/>

                        </ice:panelGroup>
                    </ice:panelGrid>

                    <ice:panelGroup>

                        <ice:graphicImage value="#{MonitorController.chartImage}"/>

                    </ice:panelGroup>

                </ice:panelGrid>

            </ice:panelTab>
            <ice:panelTab label="Monitor">

                <cdw:beanTable controller="#{MonitorController}"/>

            </ice:panelTab>
        </ice:panelTabSet>

    </ui:define>

</ui:composition>
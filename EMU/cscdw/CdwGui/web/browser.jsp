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

        <cdw:beanTable controller="#{MainBeanTableController}"/>

    </ui:define>

</ui:composition>
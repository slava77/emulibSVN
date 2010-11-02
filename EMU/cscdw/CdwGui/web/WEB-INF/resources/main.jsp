<!--
<%@taglib prefix="ice" uri="http://www.icesoft.com/icefaces/component" %>
<%@taglib prefix="f" uri="http://java.sun.com/jsf/core" %>
-->

<f:view
    xmlns:cdw="http://www.cern.ch/cms/csc/dw/gui/ontology"
    xmlns:h="http://java.sun.com/jsf/html"
    xmlns:f="http://java.sun.com/jsf/core"
    xmlns:ice="http://www.icesoft.com/icefaces/component"
    xmlns:c="http://java.sun.com/jstl/core"
    xmlns:ui="http://java.sun.com/jsf/facelets">

    <ice:outputDeclaration doctypeRoot="HTML"
                           doctypePublic="-//W3C//DTD XHTML 1.0 Transitional//EN"
                           doctypeSystem="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"/>

    <ice:outputHtml>
        <ice:outputHead>
            <title>CMS CSC Data Warehouse</title>
            <link href="media/css/rime/rime.css" rel="stylesheet" type="text/css"/>
            <link href="media/css/cdw.css" rel="stylesheet" type="text/css"/>
            <link href="media/css/cdw-print.css" rel="stylesheet" type="text/css" media="print"/>
            <script src="media/script/jquery-1.4.2.min.js" type="text/javascript"></script>
            <script type="text/javascript">
                jQuery.noConflict();
            </script>
        </ice:outputHead>
        <ice:outputBody style="padding: 0px; margin: 0px">
            <ice:form>

                <ice:panelGrid columns="2" width="100%" cellpadding="10" bgcolor="#E1E1CD">

                    <ice:outputText value="CMS CSC Data Warehouse" styleClass="mainTitle" />

                    <ice:menuBar style="font-size: 12px">
                        <ice:menuItem action="home" value="Home" styleClass=""/>
                        <ice:menuItem action="browser" value="Browser"/>
                        <ice:menuItem action="explorer" value="Ontology"/>
                    </ice:menuBar>

                </ice:panelGrid>

                <ice:panelGroup style="padding: 15px;">
                    <ui:insert name="content"/>
                </ice:panelGroup>

            </ice:form>                
        </ice:outputBody>
    </ice:outputHtml>

</f:view> 
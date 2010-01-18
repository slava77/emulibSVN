<ice:commandLink
    xmlns:f="http://java.sun.com/jsf/core"
    xmlns:ice="http://www.icesoft.com/icefaces/component"
    title="Click for details"
    action="explorer"
    actionListener="#{ComponentClassTreeController.componentClassNodeSelectedAction}">
    <f:param name="componentClassId" value="#{value.idItem}" />
    <ice:outputText value="#{value.idItem}"/>
</ice:commandLink>


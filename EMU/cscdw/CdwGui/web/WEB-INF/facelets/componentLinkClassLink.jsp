<ice:commandLink
    xmlns:f="http://java.sun.com/jsf/core"
    xmlns:ice="http://www.icesoft.com/icefaces/component"
    title="Click for details"
    action="explorer"
    actionListener="#{ComponentLinkClassTreeController.componentLinkClassNodeSelectedAction}">
    <f:param name="componentLinkClassId" value="#{value.id}" />
    <ice:outputText value="#{value.type.value}"/>
</ice:commandLink>


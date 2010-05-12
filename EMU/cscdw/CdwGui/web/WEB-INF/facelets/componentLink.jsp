<ice:commandLink
    xmlns:f="http://java.sun.com/jsf/core"
    xmlns:ice="http://www.icesoft.com/icefaces/component"
    title="Click for details"
    action="component">
    <f:setPropertyActionListener target="#{ComponentClassTreeController.selectedComponentId}" value="#{value.id}"/>
    <ice:outputText value="#{value.name}"/>
</ice:commandLink>


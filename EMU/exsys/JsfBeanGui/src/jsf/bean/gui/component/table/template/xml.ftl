<?xml version='1.0' encoding='UTF-8' ?>
<items>
${}SEPARATOR${}
<${item.class.simpleName}>
<#list columns as c>
<${c.name?html}><#t>
<#if c.embedType>
<#assign embedItem=c.columnValue(item)>
<#list c.embeddedProperties as embedCol>

<${embedCol.name?html}><#t>
<#if embedCol.entityType>
${embedCol.columnValue(embedItem).entityTitle}<#t>
<#elseif embedCol.boolean>
${embedCol.columnValue(embedItem)?string}<#t>
<#else>
${embedCol.columnValue(embedItem)?html}<#t>
</#if>
</${embedCol.name?html}><#t>
</#list>

<#elseif c.entityType>
${c.columnValue(item).entityTitle}<#t>
<#elseif c.boolean>
${c.columnValue(item)?string}<#t>
<#else>
${c.columnValue(item)?html}<#t>
</#if>
</${c.name?html}>
</#list>
</${item.class.simpleName}>
${}SEPARATOR${}
</items>
<#list columns as c>
<#if c.embedType>
<#list c.embeddedProperties as ec>
${c.name} ${ec.name}<#if ec_has_next>	</#if><#t>
</#list>
<#else>
${c.name}<#t>
</#if><#if c_has_next>	</#if></#list>
${}SEPARATOR${}
<#list columns as c>
<#if c.embedType><#assign embedItem=c.columnValue(item)><#list c.embeddedProperties as embedCol>
<#if embedCol.entityType>
${embedCol.columnValue(embedItem).entityTitle}<#t>
<#elseif embedCol.columnValue(embedItem)?is_string && ( embedCol.columnValue(embedItem)?contains("\n") )>
"${embedCol.columnValue(embedItem)}"<#t>
<#elseif embedCol.boolean>
${embedCol.columnValue(embedItem)?string}<#t>
<#elseif embedCol.date>
${embedCol.columnValue(embedItem)}<#t>
<#elseif embedCol.numeric>
${embedCol.columnValue(embedItem)?c}<#t>
<#elseif embedCol.columnValue(embedItem)?contains(",") || embedCol.columnValue(embedItem)?contains("\"") || embedCol.columnValue(embedItem)?contains("\n")>
"${embedCol.columnValue(embedItem)?replace('"','\"\"')}"<#t>
<#else>
${embedCol.columnValue(embedItem)}<#t>
</#if><#t><#if embedCol_has_next>	</#if><#t></#list><#t>
<#elseif c.entityType>
${c.columnValue(item).entityTitle}<#t>
<#elseif c.columnValue(item)?is_string && ( c.columnValue(item)?contains("\n") )>
"${c.columnValue(item)}"<#t>
<#elseif c.boolean>
${c.columnValue(item)?string}<#t>
<#elseif c.date>
${c.columnValue(item)}<#t>
<#elseif c.numeric>
${c.columnValue(item)}<#t>
<#elseif c.columnValue(item)?contains(",") || c.columnValue(item)?contains("\"") || c.columnValue(item)?contains("\n")>
"${c.columnValue(item)?replace('"','\"\"')}"<#t>
<#else>
${c.columnValue(item)}<#t>
</#if><#if c_has_next>	<#t></#if><#t>
</#list>

${}SEPARATOR${}
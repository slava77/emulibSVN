<#list columns as c>
<#if c.embedType>
<#list c.embeddedProperties as ec>
${c.name} ${ec.name}<#if ec_has_next>, </#if><#t>
</#list>
<#else>
${c.name}<#t></#if><#if c_has_next>, </#if></#list>
${}SEPARATOR${}
<#list columns as c>
<#if c.embedType><#assign embItem=c.columnValue(item)><#list c.embeddedProperties as embCol>
<#if embCol.entityType>
${embCol.columnValue(embItem).entityTitle}<#t>
<#elseif embCol.columnValue(embItem)?is_string && ( embCol.columnValue(embItem)?contains(",") || embCol.columnValue(embItem)?contains("\"") || embCol.columnValue(embItem)?contains("\n") )>
"${embCol.columnValue(embItem)?replace('"','\"\"')}"<#t>
<#elseif embCol.boolean>
${embCol.columnValue(embItem)?string}<#t>
<#elseif embCol.numeric>
${embCol.columnValue(embItem)?c}<#t>
<#elseif embCol.date>
"${embCol.columnValue(embItem)?string(embCol.outputFormat)}"<#t>
<#elseif embCol.columnValue(embItem)?contains(",") || embCol.columnValue(embItem)?contains("\"") || embCol.columnValue(embItem)?contains("\n")>
"${embCol.columnValue(embItem)?replace('"','\"\"')}"<#t>
<#else>
${embCol.columnValue(embItem)}<#t>
</#if><#if embCol_has_next>, </#if><#t></#list><#t>
<#elseif c.entityType>
${c.columnValue(item).entityTitle}<#t>
<#elseif c.columnValue(item)?is_string && ( c.columnValue(item)?contains(",") || c.columnValue(item)?contains("\"") || c.columnValue(item)?contains("\n") )>
"${c.columnValue(item)?replace('"','\"\"')}"<#t>
<#elseif c.boolean>
${c.columnValue(item)?string}<#t>
<#elseif c.numeric>
${c.columnValue(item)?c}<#t>
<#elseif c.date>
"${c.columnValue(item)}"<#t>
<#elseif c.columnValue(item)?contains(",") || c.columnValue(item)?contains("\"") || c.columnValue(item)?contains("\n")>
"${c.columnValue(item)?replace('"','\"\"')}"<#t>
<#else>
${c.columnValue(item)}<#t>
</#if><#if c_has_next>, <#t></#if><#t>
</#list>

${}SEPARATOR${}
<#list columns as c>
${c.name}<#if c_has_next>, </#if><#t>
</#list>
${}SEPARATOR${}

<#list columns as c>
<#if c.columnValue(item)?contains(",") || c.columnValue(item)?contains("\"") || c.columnValue(item)?contains("\n")>
"${c.columnValue(item)?replace('"','\"\"')}"<#if c_has_next>, </#if><#t>
<#else>
${c.columnValue(item)}<#if c_has_next>, </#if><#t>
</#if>
</#list>
${}SEPARATOR${}
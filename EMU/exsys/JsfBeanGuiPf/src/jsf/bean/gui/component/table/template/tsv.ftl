<#list columns as c>
<#if c.isEmbedType>
<#list c.embeddedProperties as ec>
${c.name} ${ec.name}<#if ec_has_next>	</#if><#t>
</#list>
<#else>
${c.name}<#t>
</#if><#if c_has_next>	</#if></#list>
${}SEPARATOR${}
<#list columns as c>
<#if c.isListType>
{list}<#t>
<#elseif !c.columnValue(item)?has_content>
<#t>
<#elseif c.isEmbedType><#assign embedItem=c.columnValue(item)><#list c.embeddedProperties as embedCol>
<#if embedCol.isListType>
{list}<#t>
<#elseif !embedCol.columnValue(embedItem)?has_content>
<#t>
<#elseif embedCol.isEntityType && !embedCol.isListType>
${embedCol.columnValue(embedItem).entityTitle}<#t>
<#elseif embedCol.columnValue(embedItem)?is_string && ( embedCol.columnValue(embedItem)?contains("\n") )>
"${embedCol.columnValue(embedItem)?replace('\n',' ')}"<#t>
<#elseif embedCol.isNumeric || embedCol.isDate>
${embedCol.columnValue(embedItem)}<#t>
<#elseif embedCol.isBoolean>
${embedCol.columnValue(embedItem)?string}<#t>
<#elseif embedCol.columnValue(embedItem)?contains(",") || embedCol.columnValue(embedItem)?contains("\"") || embedCol.columnValue(embedItem)?contains("\n")>
"${embedCol.columnValue(embedItem)?replace('"','\"\"')?replace('\n',' ')}"<#t>
<#else>
${embedCol.columnValue(embedItem)?replace('\n',' ')}<#t>
</#if><#t><#if embedCol_has_next>	</#if><#t></#list><#t>
<#elseif c.isEntityType && !c.isListType>
${c.columnValue(item).entityTitle}<#t>
<#elseif c.columnValue(item)?is_string && ( c.columnValue(item)?contains("\n") )>
"${c.columnValue(item)?replace('\n',' ')}"<#t>
<#elseif c.isNumeric || c.isDate>
${c.columnValue(item)}<#t>
<#elseif c.isBoolean>
${c.columnValue(item)?string}<#t>
<#elseif c.columnValue(item)?contains(",") || c.columnValue(item)?contains("\"") || c.columnValue(item)?contains("\n")>
"${c.columnValue(item)?replace('"','\"\"')?replace('\n',' ')}"<#t>
<#else>
${c.columnValue(item)?replace('\n',' ')}<#t>
</#if><#if c_has_next>	<#t></#if><#t>
</#list>

${}SEPARATOR${}
<#list columns as c>
<#if c.isEmbedType>
<#list c.embeddedProperties as ec>
${c.name} ${ec.name}<#if ec_has_next>, </#if><#t>
</#list>
<#else>
${c.name}<#t></#if><#if c_has_next>, </#if></#list>
${}SEPARATOR${}
<#list columns as c>
<#if c.isListType>
{list}<#t>
<#elseif !c.columnValue(item)?has_content>
<#t>
<#elseif c.isEmbedType><#assign embItem=c.columnValue(item)><#list c.embeddedProperties as embCol>
<#if !embCol.columnValue(embItem)?has_content>
<#t>
<#elseif embCol.isEntityType && !embCol.isListType>
${embCol.columnValue(embItem).entityTitle}<#t>
<#elseif embCol.isListType>
{list}<#t>
<#elseif embCol.columnValue(embItem)?is_string && ( embCol.columnValue(embItem)?contains(",") || embCol.columnValue(embItem)?contains("\"") || embCol.columnValue(embItem)?contains("\n") )>
"${embCol.columnValue(embItem)?replace('"','\"\"')?replace('\n',' ')}"<#t>
<#elseif embCol.isNumeric || embCol.isDate>
${embCol.columnValue(embItem)}<#t>
<#elseif embCol.isBoolean>
${embCol.columnValue(embItem)?string}<#t>
<#elseif embCol.columnValue(embItem)?contains(",") || embCol.columnValue(embItem)?contains("\"") || embCol.columnValue(embItem)?contains("\n")>
"${embCol.columnValue(embItem)?replace('"','\"\"')?replace('\n',' ')}"<#t>
<#else>
${embCol.columnValue(embItem)}<#t>
</#if><#if embCol_has_next>, </#if><#t></#list><#t>
<#elseif c.isEntityType && !c.isListType>
${c.columnValue(item).entityTitle}<#t>
<#elseif c.columnValue(item)?is_string && ( c.columnValue(item)?contains(",") || c.columnValue(item)?contains("\"") || c.columnValue(item)?contains("\n") )>
"${c.columnValue(item)?replace('"','\"\"')?replace('\n',' ')}"<#t>
<#elseif c.isNumeric || c.isDate>
${c.columnValue(item)}<#t>
<#elseif c.isBoolean>
${c.columnValue(item)?string}<#t>
<#elseif c.columnValue(item)?contains(",") || c.columnValue(item)?contains("\"") || c.columnValue(item)?contains("\n")>
"${c.columnValue(item)?replace('"','\"\"')?replace('\n',' ')}"<#t>
<#else>
${c.columnValue(item)}<#t>
</#if><#if c_has_next>, <#t></#if><#t>
</#list>

${}SEPARATOR${}
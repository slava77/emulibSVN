[
${}SEPARATOR${}
    {
<#list columns as c>
<#if !c.columnValue(item)?has_content>
        "${c.name}": null<#rt>
<#elseif c.isEmbedType>
        "${c.name}": {
<#assign embItem=c.columnValue(item)>
<#list c.embeddedProperties as embCol>
<#if !embCol.columnValue(embItem)?has_content>
            "${embCol.name}": null<#rt>
<#elseif c.isEntityType>
             "${embCol.name}": "${embCol.columnValue(embItem).entityTitle}"<#rt>
<#elseif embCol.isNumeric || embCol.isBoolean>
            "${embCol.name}": ${embCol.columnValue(embItem)}<#rt>
<#else>
            "${embCol.name}": "${embCol.columnValue(embItem)}"<#rt>
</#if>
<#if embCol_has_next>,</#if>
</#list>        }<#rt>
<#elseif c.isEntityType>
        "${c.name}": "${c.columnValue(item).entityTitle}"<#rt>
<#elseif c.isNumeric || c.isBoolean>
        "${c.name}": ${c.columnValue(item)}<#rt>
<#else>
        "${c.name}": "${c.columnValue(item)}"<#rt>
</#if><#if c_has_next>,</#if>
</#list>
        }<#if !isLast>,</#if>
${}SEPARATOR${}
]
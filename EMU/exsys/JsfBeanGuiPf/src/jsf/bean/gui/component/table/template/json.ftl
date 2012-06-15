[
${}SEPARATOR${}
    {
<#list columns as c>
<#if c.isListType>
             "${c.name}": "{list}"<#rt>
<#elseif !c.columnValue(item)?has_content>
        "${c.name}": null<#rt>
<#elseif c.isEmbedType>
        "${c.name}": {
<#assign embItem=c.columnValue(item)>
<#list c.embeddedProperties as embCol>
<#if embCol.isListType>
             "${embCol.name}": "{list}"<#rt>
<#elseif !embCol.columnValue(embItem)?has_content>
            "${embCol.name}": null<#rt>
<#elseif embCol.isEntityType && !embCol.isListType>
             "${embCol.name}": "${embCol.columnValue(embItem).entityTitle}"<#rt>
<#elseif embCol.isNumeric>
            "${embCol.name}": ${embCol.columnValue(embItem)}<#rt>
<#elseif embCol.isBoolean>
            "${embCol.name}": ${embCol.columnValue(embItem)?string}<#rt>
<#else>
            "${embCol.name}": "${embCol.columnValue(embItem)}"<#rt>
</#if>
<#if embCol_has_next>,</#if>
</#list>        }<#rt>
<#elseif c.isEntityType && !c.isListType>
        "${c.name}": "${c.columnValue(item).entityTitle}"<#rt>
<#elseif c.isNumeric>
        "${c.name}": ${c.columnValue(item)}<#rt>
<#elseif c.isBoolean>
        "${c.name}": ${c.columnValue(item)?string}<#rt>
<#else>
        "${c.name}": "${c.columnValue(item)}"<#rt>
</#if><#if c_has_next>,</#if>
</#list>
        }<#if !isLast>,</#if>
${}SEPARATOR${}
]
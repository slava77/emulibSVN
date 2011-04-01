[
${}SEPARATOR${}
    {
<#list columns as c>
<#if c.embedType>
        "${c.name}": {
<#assign embItem=c.columnValue(item)>
<#list c.embeddedProperties as embCol>
<#if c.entityType>
             "${embCol.name}": "${embCol.columnValue(embItem).entityTitle}"<#rt>
<#elseif embCol.numeric>
            "${embCol.name}": ${embCol.columnValue(embItem)?c}<#rt>
<#elseif embCol.boolean>
            "${embCol.name}": ${embCol.columnValue(embItem)?string}<#rt>
<#elseif !embCol.columnValue(embItem)?has_content>
            "${embCol.name}": null<#rt>
<#else>
            "${embCol.name}": "${embCol.columnValue(embItem)}"<#rt>
</#if>
<#if embCol_has_next>,</#if>
</#list>        }<#rt>
<#elseif c.entityType>
        "${c.name}": "${c.columnValue(item).entityTitle}"<#rt>
<#elseif c.numeric>
        "${c.name}": ${c.columnValue(item)?c}<#rt>
<#elseif c.boolean>
        "${c.name}": ${c.columnValue(item)?string}<#rt>
<#elseif !c.columnValue(item)?has_content>
        "${c.name}": null<#rt>
<#else>
        "${c.name}": "${c.columnValue(item)}"<#rt>
</#if><#if c_has_next>,</#if>
</#list>
        }<#if !isLast>,</#if>
${}SEPARATOR${}
]
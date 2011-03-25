[
${}SEPARATOR${}
        {
<#list columns as c>
<#if c.columnValue(item)?is_number>
------------"${c.name}": ${c.columnValue(item)?number}<#t>
<#elseif c.columnValue(item)?is_boolean>
            "${c.name}": ${c.columnValue(item)}<#t>
<#elseif !c.columnValue(item)?has_content>
            "${c.name}": null<#t>
<#else>
            "${c.name}": "${c.columnValue(item)}"<#t>
</#if><#if c_has_next>,</#if>
</#list>
        }<#if !isLast>,</#if>
${}SEPARATOR${}
]
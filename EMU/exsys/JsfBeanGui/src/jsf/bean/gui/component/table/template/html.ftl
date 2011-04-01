<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<head>
<style type="text/css">
table
{
font-family:"Trebuchet MS", Arial, Helvetica, sans-serif;
width:100%;
border-collapse:collapse;
}
td, th
{
border:1px solid #000000;
padding:3px 7px 2px 7px;
}
th
{
text-align:left;
padding-top:5px;
padding-bottom:5px;
background-color:#999999;
color:#ffffff;
}
</style>
</head>
<html>
    <body>
        <table border="1">
            <tr>
<#list columns as c>
<#if c.embedType>
<#list c.embeddedProperties as ec>
                <th>${c.name?cap_first} ${ec.name}</th>
</#list>
<#else>
                <th>${c.name?cap_first}</th>
</#if>
</#list>
            </tr>
${}SEPARATOR${}
<#list columns as c>
<#if c.embedType>
<#assign embedItem=c.columnValue(item)>
<#list c.embeddedProperties as embedCol>
                <td><#rt>
<#if embedCol.entityType>
                    ${embedCol.columnValue(embedItem).entityTitle}<#t>
<#elseif embedCol.boolean>
                    ${embedCol.columnValue(embedItem)?string}<#t>
<#else>
                    ${embedCol.columnValue(embedItem)?html}<#t>
</#if>
                </td><#lt>
</#list>
<#elseif c.entityType>
                <td>${c.columnValue(item).entityTitle}</td>
<#elseif c.boolean>
                <td>${c.columnValue(item)?string}</td>
<#else>
                <td>${c.columnValue(item)?html}</td>
</#if>
</#list>
            </tr>
${}SEPARATOR${}
        </table>
    </body>
</html>
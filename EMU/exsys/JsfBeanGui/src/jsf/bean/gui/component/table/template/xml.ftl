<?xml version='1.0' encoding='UTF-8' ?>
<items>
${}SEPARATOR${}
<${item.class.simpleName}>
<#list columns as c>
<${c.name?html}>${c.columnValue(item)?html}</${c.name?html}>
</#list>
</${item.class.simpleName}>
${}SEPARATOR${}
</items>
<?xml version="1.0" encoding="UTF-8"?>

<!--
    Document   : persistenceProperties.xsl
    Created on : January 4, 2010, 1:31 PM
    Author     : valdo
    Description:
        Purpose of transformation follows.
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" xmlns:jpa="http://java.sun.com/xml/ns/persistence">

    <xsl:output media-type="text/plain" omit-xml-declaration="yes" />

    <xsl:template match="/">
        <xsl:apply-templates select="/jpa:persistence/jpa:persistence-unit[@name='CdwUtilPU']/jpa:properties/jpa:property"/>
    </xsl:template>

    <xsl:template match="jpa:property">
        <xsl:value-of select="@name"/>
        <xsl:text>=</xsl:text>
        <xsl:value-of select="@value"/>
        <xsl:text>
</xsl:text>
    </xsl:template>

</xsl:stylesheet>

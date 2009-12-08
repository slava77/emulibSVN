<?xml version="1.0" encoding="UTF-8"?>

<!--
    Document   : emuCountersToParameterCollection.xsl
    Created on : December 8, 2009, 12:45 PM
    Author     : Evka
    Description: This transformation is used to transform EMU counters (sent from Xmas monitoring)
                 to parameter collection for further use in the ExSys
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
                xmlns:tns="http://www.cern.ch/cms/csc/exsys/data">
    <xsl:output method="xml" encoding="UTF-8" indent="yes"/>
    
    <!-- ======= config ======= -->
    <xsl:variable name="sourceName" select="'Xmas'"/>
    <!-- ====================== -->

    <!-- workaround for lack of upper-case and lower-case functions in XSLT 1.0 -->
    <xsl:variable name="lowercase" select="'abcdefghijklmnopqrstuvwxyz'" />
    <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />

    <!-- parameter collection -->
    <xsl:template match="/">
        <xsl:variable name="countersTimestamp" select="child::*[1]/@dateTime"/>
        <tns:parameterCollection>
            <tns:source>
                <xsl:value-of select="$sourceName"/>
            </tns:source>
            <xsl:for-each select="//sample/count/@*">
                <xsl:if test="local-name() != 'chamber'">
                    <xsl:call-template name="createParameter">
                        <xsl:with-param name="timestamp" select="$countersTimestamp"/>
                        <xsl:with-param name="paramId"
                                        select="translate(concat(../../@name, '_', local-name(), '_counter'), $lowercase, $uppercase)"/>
                        <xsl:with-param name="component" select="../@chamber"/>
                        <xsl:with-param name="value" select="."/>
                    </xsl:call-template>
                </xsl:if>
            </xsl:for-each>
        </tns:parameterCollection>
    </xsl:template>

    <!-- parameters -->
    <xsl:template name="createParameter">
        <xsl:param name="timestamp"/>
        <xsl:param name="component"/>
        <xsl:param name="paramId"/>
        <xsl:param name="value"/>
        <tns:parameter>
            <tns:time>
                <xsl:value-of select="$timestamp"/>
            </tns:time>
            <tns:run>-1</tns:run>
            <tns:component>
                <xsl:value-of select="$component"/>
            </tns:component>
            <tns:parameterId>
                <xsl:value-of select="$paramId"/>
            </tns:parameterId>
            <tns:numericValue>
                <xsl:value-of select="$value"/>
            </tns:numericValue>
            <tns:severity>INFO</tns:severity>
        </tns:parameter>

    </xsl:template>
</xsl:stylesheet>

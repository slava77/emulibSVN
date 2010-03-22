<?xml version="1.0" encoding="UTF-8"?>

<!--
    Document   : emuCountersToParameterCollection.xsl
    Created on : December 8, 2009, 12:45 PM
    Author     : Evka
    Description: This transformation is used to transform EMU counters (sent from Xmas monitoring)
                 to parameter collection for further use in the ExSys
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
                xmlns:tns="http://www.cern.ch/cms/csc/dw/model/dse">
    <xsl:output method="xml" encoding="UTF-8" indent="yes"/>

    <!-- workaround for lack of upper-case and lower-case functions in XSLT 1.0 -->
    <xsl:variable name="lowercase" select="'abcdefghijklmnopqrstuvwxyz'" />
    <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />

    <!-- fact collection -->
    <xsl:template match="/emuCounterNames">
        <xsl:variable name="countersTimestamp" select="@dateTime"/>
        <xsl:variable name="countersVersion" select="@version"/>
        <tns:dataServiceEntityCollection>
            <tns:tmbCounterDescriptionDSE>
                <tns:version>
                    <xsl:value-of select="$countersVersion"/>
                </tns:version>
                <xsl:for-each select="count">
                    <xsl:if test="contains(@name, 'TC')">
                        <xsl:element name="{concat('tns:', translate(@name, $uppercase, $lowercase), 'description')}">
                            <xsl:value-of select="normalize-space(.)"/>
                        </xsl:element>
                    </xsl:if>
                </xsl:for-each>
            </tns:tmbCounterDescriptionDSE>
        </tns:dataServiceEntityCollection>
    </xsl:template>

</xsl:stylesheet>

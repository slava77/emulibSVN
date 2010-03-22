<?xml version="1.0" encoding="UTF-8"?>

<!--
    Document   : emuCountersToParameterCollection.xsl
    Created on : December 8, 2009, 12:45 PM
    Author     : Evka
    Description: This transformation is used to transform EMU counters (sent from Xmas monitoring)
                 to parameter collection for further use in the ExSys
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
                xmlns:tns="http://www.cern.ch/cms/csc/exsys/bpel/pullTmbCounters/emuCountersNamingVersion">
    <xsl:output method="xml" encoding="UTF-8" indent="yes"/>

    <!-- fact collection -->
    <xsl:template match="emuCounters">
        <tns:namingVersion>
            <xsl:value-of select="@version"/>
        </tns:namingVersion>
    </xsl:template>

</xsl:stylesheet>

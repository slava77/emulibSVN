<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:c="http://cms.cern.ch/emu/afeb/teststand/configuration">
  <xsl:output method="xml" indent="yes"/>

  <!-- Remove blanks from device ids -->
  <xsl:template match="/c:configuration/c:inputs/c:testedDevice/@id">
    <xsl:attribute name="{name(.)}">
      <xsl:value-of select="translate(.,' ','')"/>
    </xsl:attribute>
  </xsl:template>

  <!-- Copy everything else  -->
  <xsl:template match="*|@*|text()|comment()">
    <xsl:copy >
      <xsl:apply-templates select="*|@*|text()|comment()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>

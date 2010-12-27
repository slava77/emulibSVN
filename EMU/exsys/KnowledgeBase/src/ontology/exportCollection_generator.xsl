<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fn="http://www.w3.org/2005/02/xpath-functions"
    xmlns:owl="http://www.w3.org/2006/12/owl2-xml#"
    xmlns:csc="http://www.cern.ch/cms/csc.owl#"
    xmlns:exp="http://www.cern.ch/cms/csc/dw/model"
    exclude-result-prefixes="xsl fn owl csc">

  <xsl:output method="xml" omit-xml-declaration="no" indent="yes"/>
  <xsl:strip-space elements="*"/>

  <xsl:template match="/">
    <exp:exportComponentCollection>
      <xsl:apply-templates select="owl:Ontology/owl:ClassAssertion" />
    </exp:exportComponentCollection>
  </xsl:template>

  <xsl:template match="owl:ClassAssertion">
    <exp:exportComponent>
        <xsl:variable name="id" select="substring-after(owl:Individual/@URI,'#')" />
        <xsl:variable name="fullid" select="owl:Individual/@URI" />
        <exp:id>
            <xsl:value-of select="$id"/>
        </exp:id>
        <exp:type>
            <xsl:value-of select="substring-after(owl:Class/@URI,'#')"/>
        </exp:type>
        <xsl:for-each select="/owl:Ontology/owl:SameIndividuals[owl:Individual/@URI = $fullid]">
            <xsl:for-each select="owl:Individual">
                <xsl:if test="owl:Individual/@URI != $fullid">
                    <exp:synonym>
                        <xsl:value-of select="substring-after(owl:Individual/@URI,'#')"/>
                    </exp:synonym>
                </xsl:if>
            </xsl:for-each>
        </xsl:for-each>
    </exp:exportComponent>
  </xsl:template>

</xsl:stylesheet>

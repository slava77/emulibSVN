<?xml version="1.0"?>
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:XSL="alias-namespace" xmlns:ad="http://cms.cern.ch/emu/afeb/teststand/analyzeddevice" xmlns:s="http://cms.cern.ch/emu/afeb/teststand/selection" version="1.0">

  <!-- This xsl should produce another xsl with a template like this one for each cut in SelectionCuts.xml -->
  <!-- <xsl:template match="/ad:device/ad:measurement[@type='count_vs_dac' and @capacitor='external' and number(translate(@setThreshold,'+',''))=46]/ad:threshold/@mean" mode="cut"> -->
  <!--   <xsl:if test="translate(.,'+','') >= number(document('SelectionCuts.xml')/s:selection/s:cut[number(@id)=$CUTID]/@high) or number(document('SelectionCuts.xml')/s:selection/s:cut[number(@id)=$CUTID]/@low) >= translate(.,'+','')">1</xsl:if> -->
  <!-- </xsl:template> -->

  <xsl:output method="xml" indent="yes"/>

  <xsl:namespace-alias stylesheet-prefix="XSL" result-prefix="xsl" />

  <xsl:template match="s:selection">
    <XSL:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ad="http://cms.cern.ch/emu/afeb/teststand/analyzeddevice" xmlns:s="http://cms.cern.ch/emu/afeb/teststand/selection" version="1.0">
      <xsl:apply-templates select="s:cut"/>
    </XSL:transform>
  </xsl:template>

  <xsl:template match="s:cut">
    <XSL:template match="{@xpath}" mode="cut">
      <XSL:param name="CUTID"><xsl:value-of select="@id"/></XSL:param>
      <XSL:if test="translate(.,'+','') >= number(document('SelectionCuts.xml')/s:selection/s:cut[number(@id)=$CUTID]/@high) or number(document('SelectionCuts.xml')/s:selection/s:cut[number(@id)=$CUTID]/@low) >= translate(.,'+','')"><XSL:text xml:space="preserve"> </XSL:text><xsl:value-of select="@id"/></XSL:if>
    </XSL:template>
  </xsl:template>

</xsl:transform>

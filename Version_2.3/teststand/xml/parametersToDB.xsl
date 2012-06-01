<?xml version="1.0"?>
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ad="http://cms.cern.ch/emu/afeb/teststand/analyzeddevice" version="1.0">

  <xsl:output method="text"/>

  <xsl:include href="failedCutsList.xsl"/>

  <xsl:template match="@*" mode="cut"/>

  <xsl:template match="*" mode="cut">
    <xsl:apply-templates select="*|@*" mode="cut"/>
  </xsl:template>

  <!-- Q 	Ns 	Gn 	Offs 	Qm 	U20 	Cint 	Q 	Ns 	Mt 	Rt 	Slt -->
  <xsl:template match="ad:device">DeviceId <xsl:value-of select="@id"/>
FailedCuts <xsl:apply-templates select="*" mode="cut"/>
Q <xsl:value-of select="/ad:device/ad:measurement[@type='count_vs_dac' and @capacitor='external' and number(translate(@setThreshold,'+',''))=46]/ad:threshold/@mean"/>
Ns <xsl:value-of select="/ad:device/ad:measurement[@type='count_vs_dac' and @capacitor='external' and number(translate(@setThreshold,'+',''))=46]/ad:noise/@mean"/>
Gn <xsl:value-of select="/ad:device/ad:gain/@mean"/>
Offs <xsl:value-of select="/ad:device/ad:offset/@mean"/>
Qm  <xsl:value-of select="/ad:device/ad:maxMeasuredThreshold[number(translate(@atSetThreshold,'+',''))=0]/@value"/>
U20 <xsl:value-of select="/ad:device/ad:averageSetThreshold[number(translate(@atCharge,'+',''))=20]/@value"/>
Cint <xsl:value-of select="/ad:device/ad:C_int/@mean"/>
Q <xsl:value-of select="/ad:device/ad:measurement[@type='count_vs_dac' and @capacitor='internal' and number(translate(@setThreshold,'+',''))=46]/ad:threshold/@mean"/>
Ns <xsl:value-of select="/ad:device/ad:measurement[@type='count_vs_dac' and @capacitor='internal' and number(translate(@setThreshold,'+',''))=46]/ad:noise/@mean"/>
Mt <xsl:value-of select="/ad:device/ad:measurement[@type='time_vs_dac']/ad:times[number(translate(@nominalInputCharge,'+',''))=100]/ad:mean/@mean"/>
Rt <xsl:value-of select="/ad:device/ad:measurement[@type='time_vs_dac']/ad:times[number(translate(@nominalInputCharge,'+',''))=100]/ad:rms/@mean"/>
Slt <xsl:value-of select="/ad:device/ad:measurement[@type='time_vs_dac']/ad:slew/ad:spanOfMeans/@mean"/><xsl:text>
</xsl:text>
  </xsl:template>

</xsl:transform>

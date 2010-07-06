<?xml version="1.0" encoding="UTF-8"?>

<!--
    Document   : emuCountersToParameterCollection.xsl
    Created on : December 8, 2009, 12:45 PM
    Author     : Evka
    Description: This transformation is used to transform EMU counters (sent from Xmas monitoring)
                 to parameter collection for further use in the ExSys
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
                xmlns:tns="http://www.cern.ch/cms/csc/dw/model"
                xmlns:ser="http://www.cern.ch/cms/csc/dw/service">
    <xsl:output method="xml" encoding="UTF-8" indent="yes"/>
    
    <!-- ======= config ======= -->
    <xsl:variable name="sourceName" select="'XmasFactService'"/>
    <!-- ====================== -->

    <!-- workaround for lack of upper-case and lower-case functions in XSLT 1.0 -->
    <xsl:variable name="lowercase" select="'abcdefghijklmnopqrstuvwxyz'" />
    <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />

    <!-- fact collection -->
    <xsl:template match="/">
        <xsl:variable name="countersTimestamp" select="child::*[1]/@dateTime"/>
        <tns:factCollection>
            <!--ser:serviceInstructions async="false" strict="false" persist="true"/-->
            <tns:source>
                <xsl:value-of select="$sourceName"/>
            </tns:source>
            <xsl:choose>
                <xsl:when test="//sample">
                    <!-- these are TMB trigger counters -->
                    <xsl:for-each select="//sample/count">
                        <xsl:variable name="component" select="concat(@chamber, '/TMB')"/>
                        <!-- Sliding Tmb Trigger Counters fact (DROPPED) -->
                        <!--xsl:if test="../@name = 'sliding'">
                            <xsl:call-template name="slidingTmbTriggerCounterFact">
                                <xsl:with-param name="timestamp" select="$countersTimestamp"/>
                                <xsl:with-param name="component" select="$component"/>
                            </xsl:call-template>
                        </xsl:if-->

                        <!-- Cumulative Tmb Trigger Counters fact -->
                        <xsl:if test="../@name = 'cumulative'">
                            <xsl:call-template name="cumulativeTmbTriggerCounterFact">
                                <xsl:with-param name="timestamp" select="$countersTimestamp"/>
                                <xsl:with-param name="component" select="$component"/>
                            </xsl:call-template>
                        </xsl:if>
                    </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                    <!-- these are TMB non-trigger counters -->
                    <xsl:for-each select="/emuCounters/count">
                        <xsl:variable name="component" select="concat(@chamber, '/TMB')"/>
                        <xsl:call-template name="tmbTriggerCounterFact">
                            <xsl:with-param name="timestamp" select="$countersTimestamp"/>
                            <xsl:with-param name="component" select="$component"/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:otherwise>
            </xsl:choose>
        </tns:factCollection>
    </xsl:template>

    <!-- Sliding Tmb Trigger Counters fact -->
    <xsl:template name="slidingTmbTriggerCounterFact">
        <xsl:param name="component"/>
        <xsl:param name="timestamp"/>
        
        <tns:slidingTmbTriggerCounterFact>
            <!-- general part -->
            <tns:time>
                <xsl:value-of select="$timestamp"/>
            </tns:time>
            <tns:component_id>
                <xsl:value-of select="$component"/>
            </tns:component_id>
            <tns:severity>INFO</tns:severity>

            <!-- concrete part -->
            <tns:alctCount>
                <xsl:value-of select="@alct"/>
            </tns:alctCount>
            <tns:clctCount>
                <xsl:value-of select="@clct"/>
            </tns:clctCount>
            <tns:lctCount>
                <xsl:value-of select="@lct"/>
            </tns:lctCount>
            <tns:l1aCount>
                <xsl:value-of select="@l1a"/>
            </tns:l1aCount>
            <tns:windowWidth>
                <xsl:value-of select="../@delta_t"/>
            </tns:windowWidth>
        </tns:slidingTmbTriggerCounterFact>
    </xsl:template>

    <!-- Cumulative Tmb Trigger Counters fact -->
    <xsl:template name="cumulativeTmbTriggerCounterFact">
        <xsl:param name="component"/>
        <xsl:param name="timestamp"/>

        <tns:cumulativeTmbTriggerCounterFact>
            <!-- general part -->
            <tns:time>
                <xsl:value-of select="$timestamp"/>
            </tns:time>
            <tns:component_id>
                <xsl:value-of select="$component"/>
            </tns:component_id>
            <tns:severity>INFO</tns:severity>

            <!-- concrete part -->
            <tns:alctCount>
                <xsl:value-of select="@alct"/>
            </tns:alctCount>
            <tns:clctCount>
                <xsl:value-of select="@clct"/>
            </tns:clctCount>
            <tns:lctCount>
                <xsl:value-of select="@lct"/>
            </tns:lctCount>
            <tns:l1aCount>
                <xsl:value-of select="@l1a"/>
            </tns:l1aCount>
        </tns:cumulativeTmbTriggerCounterFact>
    </xsl:template>

    <!-- All Tmb Counters fact -->
    <xsl:template name="tmbTriggerCounterFact">
        <xsl:param name="component"/>
        <xsl:param name="timestamp"/>

        <tns:tmbCounterFact>
            <!-- general part -->
            <tns:time>
                <xsl:value-of select="$timestamp"/>
            </tns:time>
            <tns:component_id>
                <xsl:value-of select="$component"/>
            </tns:component_id>
            <tns:severity>INFO</tns:severity>

            <!-- concrete part -->
            <tns:version>
                <xsl:value-of select="../@version"/>
            </tns:version>
            <xsl:for-each select="@*">
                <xsl:if test="contains(local-name(), 'TC')">
                    <xsl:element name="{concat('tns:', translate(local-name(), $uppercase, $lowercase))}">
                        <xsl:value-of select="."/>
                    </xsl:element>
                </xsl:if>
            </xsl:for-each>
        </tns:tmbCounterFact>
    </xsl:template>

</xsl:stylesheet>

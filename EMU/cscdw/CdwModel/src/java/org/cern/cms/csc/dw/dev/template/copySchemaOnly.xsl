<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"

    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:tns="http://www.cern.ch/cms/csc/dw/model"
    xmlns:ont="http://www.cern.ch/cms/csc/dw/ontology"

    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xmlns:jaxb="http://java.sun.com/xml/ns/jaxb"
	xmlns:hj="http://hyperjaxb3.jvnet.org/ejb/schemas/customizations"
	xmlns:orm="http://java.sun.com/xml/ns/persistence/orm"
    exclude-result-prefixes="xsi jaxb hj orm">

    <xsl:output method="xml" encoding="utf-8" indent="yes" omit-xml-declaration="no"/>

    <xsl:template match="/">
        <xsl:apply-templates select="xsd:schema" mode="root"/>
    </xsl:template>

    <xsl:template match="xsd:schema" mode="root">
        <xsl:copy>
            <xsl:apply-templates select="xsd:*[name() != 'xsd:annotation']|@*[ contains(name(),':') = false() ]" mode="child"/>
        </xsl:copy>
    </xsl:template>

    <xsl:template match="xsd:*|@*" mode="child">
        <xsl:copy>
            <xsl:apply-templates select="xsd:*[name() != 'xsd:annotation']|@*[ contains(name(),':') = false() ]" mode="child"/>
        </xsl:copy>
    </xsl:template>

</xsl:stylesheet>

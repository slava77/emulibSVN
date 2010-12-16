<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:jaxb="http://java.sun.com/xml/ns/jaxb"
    xmlns:hj="http://hyperjaxb3.jvnet.org/ejb/schemas/customizations"
    xmlns:orm="http://java.sun.com/xml/ns/persistence/orm"
    xmlns:xjc="http://java.sun.com/xml/ns/jaxb/xjc"
    xmlns:ann="http://jaxb.dev.java.net/plugin/annotate"
    xmlns:tns="http://www.cern.ch/cms/csc/dw/model"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:dqm="http://www.cern.ch/cms/csc/dw/model/dqm"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

    <xsl:output media-type="text/xml" omit-xml-declaration="no" indent="yes" />

    <xsl:template match="/">

         <xsd:schema 
            targetNamespace="http://www.cern.ch/cms/csc/dw/model"
            elementFormDefault="qualified"
            xsi:schemaLocation="
                http://java.sun.com/xml/ns/jaxb ext/bindingschema_2_0.xsd
                http://java.sun.com/xml/ns/persistence/orm ext/orm_1_0.xsd
                http://hyperjaxb3.jvnet.org/ejb/schemas/customizations ext/customizations.xsd"
            jaxb:extensionBindingPrefixes="xjc orm hj ann"
            jaxb:version="2.0">

            <xsl:apply-templates select="/dqm:classes/dqm:class"/>

        </xsd:schema>

    </xsl:template>

    <xsl:template match="dqm:class">

        <xsd:element name="{@name}"
                     type="tns:{@name}Type"
                     substitutionGroup="tns:fact"/>

        <xsd:complexType name="{@name}Type">
            <xsd:annotation>
                <xsd:appinfo>
                    <jaxb:class name="{@name}" />
                    <hj:entity>
                        <orm:table name="{substring(@table,1,30)}"/>
                    </hj:entity>
                    <ann:annotate xmlns:a="org.cern.cms.csc.dw.model.annotation">
                        <a:FactAnn limitComponents="{@componentClass}" title="{@factTitle}"/>
                    </ann:annotate>
                    <xsl:apply-templates select="dqm:annotations"/>
                </xsd:appinfo>
            </xsd:annotation>
            <xsd:complexContent>
                <xsd:extension base="tns:factType">
                    <xsd:sequence>

                        <xsd:element name="online" type="xsd:boolean">
                            <xsd:annotation>
                                <xsd:appinfo>
                                    <hj:basic>
                                        <orm:column name="FCT_ONLINE"/>
                                    </hj:basic>
                                </xsd:appinfo>
                            </xsd:annotation>
                        </xsd:element>

                        <xsl:apply-templates select="./dqm:param">
                            <xsl:sort select="." order="ascending"/>
                        </xsl:apply-templates>

                    </xsd:sequence>
                </xsd:extension>
            </xsd:complexContent>
        </xsd:complexType>

    </xsl:template>

    <xsl:template match="dqm:param">

        <xsd:element name="{.}" type="xsd:long" minOccurs="0">
            <xsd:annotation>
                <xsd:appinfo>
                    <hj:basic>
                        <orm:column name="{substring(@column,1,30)}"/>
                    </hj:basic>
                    <jaxb:property name="{@property}"/>
                    <xsl:if test="@measure != ''">
                        <ann:annotateGet xmlns:a="org.cern.cms.csc.dw.model.annotation">
                            <a:OlapMeasure name="{@property}" aggregator="{@measure}"/>
                        </ann:annotateGet>
                    </xsl:if>
                </xsd:appinfo>
            </xsd:annotation>
        </xsd:element>

        <xsl:if test="@fraction = 1">
            <xsd:element name="{.}_F" type="xsd:float" minOccurs="0">
                <xsd:annotation>
                    <xsd:appinfo>
                        <hj:basic>
                            <orm:column name="{substring(@column,1,28)}_F"/>
                        </hj:basic>
                        <jaxb:property name="{@property}Fraction"/>
                    </xsd:appinfo>
                </xsd:annotation>
            </xsd:element>
        </xsl:if>

    </xsl:template>

    <xsl:template match="dqm:annotations">
      <xsl:copy>
        <xsl:apply-templates/>
      </xsl:copy>
    </xsl:template>

</xsl:stylesheet>

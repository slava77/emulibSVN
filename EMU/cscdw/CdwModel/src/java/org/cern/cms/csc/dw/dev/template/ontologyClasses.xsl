<?xml version="1.0" encoding="UTF-8"?>

<!--
    Document   : ontologyClasses.xsl
    Created on : December 29, 2009, 12:17 AM
    Author     : valdo
    Description:
        Purpose of transformation follows.
-->

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:owl="http://www.w3.org/2006/12/owl2-xml#"
    version="1.0" exclude-result-prefixes="xsl owl">

    <xsl:output method="xml" omit-xml-declaration="no" encoding="utf-8" indent="yes"/>

    <xsl:key name="ClassURI" match="owl:Class" use="@URI"/>
    <xsl:key name="ObjectPropertyURI" match="owl:ObjectProperty" use="@URI"/>

    <xsl:template match="/">
        <xsd:schema
            targetNamespace="http://www.cern.ch/cms/csc/dw/ontology"
            xmlns:xsd="http://www.w3.org/2001/XMLSchema"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tns="http://www.cern.ch/cms/csc/dw/ontology"
            xmlns:jaxb="http://java.sun.com/xml/ns/jaxb"
            xmlns:hj="http://hyperjaxb3.jvnet.org/ejb/schemas/customizations"
            xmlns:orm="http://java.sun.com/xml/ns/persistence/orm"
            xmlns:xjc="http://java.sun.com/xml/ns/jaxb/xjc"
            xsi:schemaLocation="http://java.sun.com/xml/ns/jaxb http://java.sun.com/xml/ns/jaxb/bindingschema_2_0.xsd http://java.sun.com/xml/ns/persistence/orm http://java.sun.com/xml/ns/persistence/orm_1_0.xsd http://hyperjaxb3.jvnet.org/ejb/schemas/customizations jar://home/valdo/Documents/CSCDW/EMU/cscdw/lib/hyperjaxb3/hyperjaxb3-ejb-schemas-customizations-0.5.4.jar!customizations.xsd"
            elementFormDefault="qualified"
            jaxb:extensionBindingPrefixes="xjc orm hj"
            jaxb:version="2.0">

            <xsl:for-each select="/owl:Ontology/owl:*/owl:Class[generate-id() = generate-id(key('ClassURI', @URI))]">
                <xsl:sort select="@URI"/>

                <xsl:variable name="uri" select="@URI"/>
                <xsl:variable name="name" select="translate(substring-after($uri,'#'),' /','_')"/>
                <xsl:variable name="pre_table" select="translate($name,'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
                <xsl:variable name="table" select="substring(concat('CDW_',$pre_table,'_COMPONENTS'), 1, 30)"/>

                <xsd:element name="{$name}" substitutionGroup="tns:component">
                    <xsd:complexType>
                        <xsd:annotation>
                            <xsd:appinfo>
                                <jaxb:class name="{$name}" />
                                <hj:entity>
                                    <orm:table name="{$table}"/>
                                </hj:entity>
                            </xsd:appinfo>
                        </xsd:annotation>
                        <xsd:complexContent>
                            <xsd:extension base="tns:componentType">
                                <xsd:sequence>
                                    <xsl:comment xml:space="4">
                                    <xsl:for-each select="//owl:ClassAssertion/owl:Individual[../owl:Class/@URI = $uri]">
                                        <xsl:value-of select="@URI"/>
                                    </xsl:for-each>
                                    </xsl:comment>
                                </xsd:sequence>
                            </xsd:extension>
                        </xsd:complexContent>
                    </xsd:complexType>
                </xsd:element>

            </xsl:for-each>
            

        </xsd:schema>
    </xsl:template>

</xsl:stylesheet>

<?xml version="1.0" encoding="UTF-8"?>

<!--
    Document   : ontologyClassifiers.xsl
    Created on : December 28, 2009, 2:22 PM
    Author     : valdo
    Description:
        Generate ontology classifiers from ontology XML
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
            xmlns:cu="http://jaxb.dev.java.net/plugin/classutils"
            xmlns:xjc="http://java.sun.com/xml/ns/jaxb/xjc"
            xsi:schemaLocation="
                http://java.sun.com/xml/ns/jaxb ext/bindingschema_2_0.xsd
                http://java.sun.com/xml/ns/persistence/orm ext/orm_1_0.xsd
                http://hyperjaxb3.jvnet.org/ejb/schemas/customizations ext/customizations.xsd"
            elementFormDefault="qualified"
            jaxb:extensionBindingPrefixes="xjc orm hj cu"
            jaxb:version="2.0">

          <xsd:simpleType name="componentClassIdType">
            <xsd:annotation>
                <xsd:appinfo>
                    <jaxb:typesafeEnumClass name="ComponentClassType"/>
                </xsd:appinfo>
            </xsd:annotation>
            <xsd:restriction base="xsd:string">
                <xsl:for-each select="/owl:Ontology/owl:*/owl:Class[generate-id() = generate-id(key('ClassURI', @URI))]">
                    <xsl:sort select="@URI"/>
                    <xsd:enumeration value="{substring-after(@URI,'#')}"/>
                </xsl:for-each>
            </xsd:restriction>
          </xsd:simpleType>

          <xsd:simpleType name="componentLinkClassIdType">
            <xsd:annotation>
                <xsd:appinfo>
                    <jaxb:typesafeEnumClass name="ComponentLinkClassType"/>
                    <cu:implements name="org.neo4j.graphdb.RelationshipType"/>
                </xsd:appinfo>
            </xsd:annotation>
            <xsd:restriction base="xsd:string">
                <xsl:for-each select="/owl:Ontology/owl:*/owl:ObjectProperty[generate-id() = generate-id(key('ObjectPropertyURI', @URI))]">
                    <xsl:sort select="@URI"/>
                    <xsd:enumeration value="{substring-after(@URI,'#')}"/>
                </xsl:for-each>
            </xsd:restriction>
          </xsd:simpleType>

        </xsd:schema>

    </xsl:template>

</xsl:stylesheet>

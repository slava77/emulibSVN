<?xml version="1.0" encoding="UTF-8"?>
<!-- List class declarations without annotation and annotations without class declaration. -->

<!DOCTYPE stylesheet [
   <!ENTITY xsd "http://www.w3.org/2001/XMLSchema#" >
   <!ENTITY rdfs "http://www.w3.org/2000/01/rdf-schema#" >
   <!ENTITY csc "http://www.cern.ch/cms/csc.owl#" >
   <!ENTITY LF "&#10;" >
]>

<xsl:transform 
    xmlns="http://www.w3.org/2006/12/owl2-xml#"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:owl2xml="http://www.w3.org/2006/12/owl2-xml#"
    xmlns:templ="http://www.cern.ch/cms/csc/owl-template"
    xmlns:dc="http://purl.org/dc/elements/1.1/"
    version="1.0">
 
  <xsl:output method="text" encoding="UTF-8" indent="yes"/>

  <xsl:template match="owl2xml:Ontology">

Declarations missing annotation
    <xsl:for-each select="//owl2xml:Declaration">
      <xsl:variable name="CLASS" select="owl2xml:Class/@URI"/>
      <xsl:if test="$CLASS and not(//owl2xml:EntityAnnotation/owl2xml:Class[@URI=$CLASS])"><xsl:value-of select="$CLASS"/>    is not annotated
</xsl:if>
    </xsl:for-each>

Annotations without declaration
    <xsl:for-each select="//owl2xml:EntityAnnotation">
      <xsl:variable name="CLASS" select="owl2xml:Class/@URI"/>
      <xsl:if test="$CLASS and not(//owl2xml:Declaration/owl2xml:Class[@URI=$CLASS])"><xsl:value-of select="$CLASS"/>    is not declared
</xsl:if>
    </xsl:for-each>

  </xsl:template>

</xsl:transform>

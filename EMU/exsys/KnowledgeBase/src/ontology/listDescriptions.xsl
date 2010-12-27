<?xml version="1.0" encoding="UTF-8"?>
<!-- List in alphabetic order all declarations together with their descriptions. -->

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
 
  <xsl:output method="text" encoding="UTF-8"/>

  <xsl:template match="owl2xml:Ontology">
    <xsl:apply-templates select="owl2xml:Declaration">
      <xsl:sort select="translate(substring-after(child::*[position()=1]/@URI,'#'),'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="owl2xml:Declaration">
    <xsl:variable name="CLASS" select="substring-after(child::*[position()=1]/@URI,'#')"/>
    <xsl:value-of select="$CLASS"/><xsl:text>			</xsl:text><xsl:value-of select="//owl2xml:EntityAnnotation[substring-after(child::*[position()=1]/@URI,'#')=$CLASS]/owl2xml:Annotation/owl2xml:Constant"/><xsl:text>
</xsl:text>
  </xsl:template>  

</xsl:transform>

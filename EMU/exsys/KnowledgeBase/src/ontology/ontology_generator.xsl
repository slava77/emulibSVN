<?xml version="1.0" encoding="UTF-8"?>

<!DOCTYPE stylesheet [
   <!ENTITY xsd "http://www.w3.org/2001/XMLSchema#" >
   <!ENTITY rdfs "http://www.w3.org/2000/01/rdf-schema#" >
   <!ENTITY csc "http://www.cern.ch/cms/csc.owl#" >
   <!ENTITY LF "&#10;" >
]>

<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:owl2xml="http://www.w3.org/2006/12/owl2-xml#"
    xmlns:templ="http://www.cern.ch/cms/csc/owl-template"
    xmlns:dc="http://purl.org/dc/elements/1.1/"
    version="1.0">
 
  <xsl:output method="xml" encoding="UTF-8" indent="yes"/>

  <xsl:template match="owl2xml:Ontology">

<!--     <xsl:text disable-output-escaping="yes"><![CDATA[ -->
<!-- <!DOCTYPE Ontology [ -->
<!--    <!ENTITY xsd "http://www.w3.org/2001/XMLSchema#" > -->
<!--    <!ENTITY rdfs "http://www.w3.org/2000/01/rdf-schema#" > -->
<!--    <!ENTITY csc "http://www.cern.ch/cms/csc.owl#" > -->
<!-- ]> -->
<!-- ]]> -->
<!-- </xsl:text> -->

    <xsl:copy>
      <xsl:apply-templates/>
    </xsl:copy>

  </xsl:template>


  <xsl:template match="owl2xml:Ontology/owl2xml:*">
    <xsl:copy-of select="."/>
  </xsl:template>


  <xsl:template match="templ:individuals">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declarations and relations of individuals</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="templ:chambers">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declaration of CSC hierarchy down to layers</xsl:comment>
    <xsl:text>&LF;</xsl:text>
   <xsl:variable name="SOURCE" select="@source"/>

    <Declaration><Individual URI="&csc;ME"/></Declaration>
    <ClassAssertion><Class URI="&csc;CSC"/><Individual URI="&csc;ME"/></ClassAssertion>

    <xsl:value-of select="count(document($SOURCE)//*)"/>

    <xsl:for-each select="document($SOURCE)//Chamber">
      <xsl:sort select="@endcap"/>
      <xsl:variable name="ENDCAP" select="@endcap"/>
      <xsl:if test="not(preceding::Chamber/@endcap=$ENDCAP)">
	<!-- Endcaps -->
	<Declaration><Individual URI="&csc;ME{$ENDCAP}"/></Declaration>
	<ClassAssertion><Class URI="&csc;Endcap"/><Individual URI="&csc;ME{$ENDCAP}"/></ClassAssertion>
	<DataPropertyAssertion><DataProperty URI="&csc;_Name"/><Individual URI="&csc;ME{$ENDCAP}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="concat('ME',$ENDCAP)"/></Constant></DataPropertyAssertion>
	<DataPropertyAssertion><DataProperty URI="&csc;_Name"/><Individual URI="&csc;ME{$ENDCAP}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="concat('ME',translate($ENDCAP,'+-','pm'))"/></Constant></DataPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}"/><Individual URI="&csc;ME"/>
	</ObjectPropertyAssertion>

	<xsl:for-each select="document($SOURCE)//Chamber[@endcap=$ENDCAP]">
	  <xsl:sort select="@station"/>
	  <xsl:variable name="STATION" select="@station"/>
	  <xsl:if test="not(preceding::Chamber[@endcap=$ENDCAP]/@station=$STATION)">
	    <!-- Stations -->
	    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}"/></Declaration>
	    <ClassAssertion><Class URI="&csc;Station"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}"/></ClassAssertion>
	    <ObjectPropertyAssertion>
	      <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}"/><Individual URI="&csc;ME{$ENDCAP}"/>
	    </ObjectPropertyAssertion>

	    <xsl:for-each select="document($SOURCE)//Chamber[@endcap=$ENDCAP and @station=$STATION]">
	      <xsl:sort select="@type"/>
	      <xsl:variable name="RING" select="@type"/>
	      <xsl:if test="not(preceding::Chamber[@endcap=$ENDCAP and @station=$STATION]/@type=$RING)">
		<!-- Rings -->
		<Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}"/></Declaration>
		<ClassAssertion><Class URI="&csc;Ring"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}"/></ClassAssertion>
		<ObjectPropertyAssertion>
		  <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}"/>
		</ObjectPropertyAssertion>
		
		<xsl:for-each select="document($SOURCE)//Chamber[@endcap=$ENDCAP and @station=$STATION and @type=$RING]">
		  <xsl:sort data-type="number" select="@number"/>
		  <xsl:variable name=       "CHAMBER" select=              "@number"/>
		  <xsl:variable name="PADDED_CHAMBER" select="format-number(@number,'00')"/>
		  <xsl:if test="not(preceding::Chamber[@endcap=$ENDCAP and @station=$STATION and @type=$RING]/@number=$CHAMBER)">

		    <!-- Chambers -->
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/></Declaration>
		    <xsl:if test="$CHAMBER != $PADDED_CHAMBER">
		      <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/></Declaration>
		      <SameIndividuals>
			<Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/>
			<Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"       />
		      </SameIndividuals>
		    </xsl:if>
		    <ClassAssertion><Class URI="&csc;Chamber"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/></ClassAssertion>
		    <ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}"/></ObjectPropertyAssertion>
		    <!-- Chamber's Cooling -->
		    <ObjectPropertyAssertion><ObjectProperty URI="&csc;cools"/><Individual URI="&csc;CoolingCircuit2"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/></ObjectPropertyAssertion>

		    <!-- Layers -->
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/1"/></Declaration>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/2"/></Declaration>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/3"/></Declaration>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/4"/></Declaration>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/5"/></Declaration>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/6"/></Declaration>
		    <xsl:if test="$CHAMBER != $PADDED_CHAMBER">
		      <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/1"/></Declaration>
		      <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/2"/></Declaration>
		      <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/3"/></Declaration>
		      <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/4"/></Declaration>
		      <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/5"/></Declaration>
		      <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/6"/></Declaration>
		      <SameIndividuals><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/1"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/1"/></SameIndividuals>
		      <SameIndividuals><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/2"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/2"/></SameIndividuals>
		      <SameIndividuals><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/3"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/3"/></SameIndividuals>
		      <SameIndividuals><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/4"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/4"/></SameIndividuals>
		      <SameIndividuals><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/5"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/5"/></SameIndividuals>
		      <SameIndividuals><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/6"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/6"/></SameIndividuals>
		    </xsl:if>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/1"/></ClassAssertion>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/2"/></ClassAssertion>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/3"/></ClassAssertion>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/4"/></ClassAssertion>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/5"/></ClassAssertion>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/6"/></ClassAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/1"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/2"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/3"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/4"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/5"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}/6"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$PADDED_CHAMBER}"/>
		    </ObjectPropertyAssertion>
		  </xsl:if>
		</xsl:for-each>
		
	      </xsl:if>
	    </xsl:for-each>
	    
	  </xsl:if>
	</xsl:for-each>
	
      </xsl:if>
    </xsl:for-each>

  </xsl:template>




  <xsl:template match="templ:SectorProc">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declaration of PCrates and SectorProcessors</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>

    <!-- SectorProcessors -->
    <xsl:for-each select="document($SOURCE)//SectorProcessor[@id!='']">
      <xsl:sort select="@id"/>

      <xsl:variable name="PADDED_SP_INSTANCE" select="format-number(@id,'00')"/>
      <Declaration><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/></Declaration>
      <ClassAssertion><Class URI="&csc;SectorProcessor"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/></ClassAssertion>

      <!-- PeripheralCrates -->
      <xsl:for-each select="PeripheralCrate">
	<xsl:sort select="@endcap"/>
	<xsl:sort select="@station"/>
	<xsl:sort select="@VMEcrate"/>
	<xsl:variable name="PM_ENDCAP"><xsl:choose><xsl:when test="@endcap = '+'">p</xsl:when><xsl:otherwise>m</xsl:otherwise></xsl:choose></xsl:variable>
	<xsl:variable name="PADDED_VME_NUMBER" select="format-number(@VMEcrate,'00')"/>
	<xsl:variable name="VME_NUMBER" select="@VMEcrate"/>
	<Declaration><Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}"/></Declaration>
	<Declaration><Individual URI="&csc;VME{$PM_ENDCAP}{@station}_{$PADDED_VME_NUMBER}"/></Declaration>
	<SameIndividuals>
	  <Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}"/>
	  <Individual URI="&csc;VME{$PM_ENDCAP}{@station}_{$PADDED_VME_NUMBER}"/>
	</SameIndividuals>
	<xsl:if test="$VME_NUMBER != $PADDED_VME_NUMBER">
	  <Declaration><Individual URI="&csc;VME{@endcap}{@station}/{$VME_NUMBER}"/></Declaration>
	  <Declaration><Individual URI="&csc;VME{$PM_ENDCAP}{@station}_{$VME_NUMBER}"/></Declaration>
	  <SameIndividuals>
	    <Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}"/>
	    <Individual URI="&csc;VME{@endcap}{@station}/{$VME_NUMBER}"/>
	    <Individual URI="&csc;VME{$PM_ENDCAP}{@station}_{$VME_NUMBER}"/>
	  </SameIndividuals>
	</xsl:if>
	<ClassAssertion><Class URI="&csc;PeripheralCrate"/><Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}"/></ClassAssertion>
	<!-- SectorProcessor inputs -->
	<xsl:variable name="FPGA" select="position()"/>
	<Declaration><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M1"/></Declaration>
	<Declaration><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M2"/></Declaration>
	<Declaration><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M3"/></Declaration>
	<ClassAssertion><Class URI="&csc;SectorProcessorInput"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M1"/></ClassAssertion>
	<ClassAssertion><Class URI="&csc;SectorProcessorInput"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M2"/></ClassAssertion>
	<ClassAssertion><Class URI="&csc;SectorProcessorInput"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M3"/></ClassAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M1"/></ObjectPropertyAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M2"/></ObjectPropertyAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M3"/></ObjectPropertyAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M1"/><Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}/MPC"/></ObjectPropertyAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M2"/><Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}/MPC"/></ObjectPropertyAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}InputF{$FPGA}/M3"/><Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}/MPC"/></ObjectPropertyAssertion>
      </xsl:for-each>

    </xsl:for-each>
  </xsl:template>




  <xsl:template match="templ:FEDCrates">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declaration of FEDCrates</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>

    <xsl:for-each select="document($SOURCE)//DDU/@crate">
      <xsl:sort/>
      <xsl:variable name="FEDCRATE_NUMBER"><xsl:value-of select="."/></xsl:variable>
      <xsl:if test="not(preceding::DDU[@crate=$FEDCRATE_NUMBER])">
	<Declaration><Individual URI="&csc;FEDCrate{$FEDCRATE_NUMBER}"/></Declaration>
	<ClassAssertion><Class URI="&csc;FEDCrate"/><Individual URI="&csc;FEDCrate{$FEDCRATE_NUMBER}"/></ClassAssertion>
      </xsl:if>
    </xsl:for-each>
    
  </xsl:template>


  <xsl:template match="templ:PSideFMM|templ:MSideFMM">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declaration of slinks and their relation to DDUs and DCCs; FMMs of DDUs and DCCs</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>

    <!-- FED system name(s) -->
    <xsl:variable name="FEDSYSTEM" select="document($SOURCE)/FEDSystem/@NAME"/>
    <Declaration><Individual URI="&csc;{$FEDSYSTEM}"/></Declaration>
    <xsl:variable name="FEDSYSTEM_CANONICAL"><xsl:if test="name()='templ:PSideFMM'">FED/ME+</xsl:if><xsl:if test="name()='templ:MSideFMM'">FED/ME-</xsl:if></xsl:variable>
    <Declaration><Individual URI="&csc;{$FEDSYSTEM_CANONICAL}"/></Declaration>
    <ClassAssertion><Class URI="&csc;FEDSystem"/><Individual URI="&csc;{$FEDSYSTEM_CANONICAL}"/></ClassAssertion>
    <SameIndividuals><Individual URI="&csc;{$FEDSYSTEM}"/><Individual URI="&csc;{$FEDSYSTEM_CANONICAL}"/></SameIndividuals>

    <xsl:for-each select="document($SOURCE)//FEDCrate">
      <!-- FED Crate -->
      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;isIn"/>
	<Individual URI="&csc;FEDCrate{@CRATE_NUMBER}"/><Individual URI="&csc;{$FEDSYSTEM_CANONICAL}"/>
      </ObjectPropertyAssertion>
      <!-- DCC -->
      <Declaration><Individual URI="&csc;DCC{DCC/@FMM_ID}"/></Declaration>
      <Declaration><Individual URI="&csc;Slink{DCC/@SLINK1_ID}"/></Declaration>
      <Declaration><Individual URI="&csc;Slink{DCC/@SLINK2_ID}"/></Declaration>
      <ClassAssertion><Class URI="&csc;DCC"/><Individual URI="&csc;DCC{DCC/@FMM_ID}"/></ClassAssertion>
      <ClassAssertion><Class URI="&csc;Slink"/><Individual URI="&csc;Slink{DCC/@SLINK1_ID}"/></ClassAssertion>
      <ClassAssertion><Class URI="&csc;Slink"/><Individual URI="&csc;Slink{DCC/@SLINK2_ID}"/></ClassAssertion>
      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;isIn"/>
	<Individual URI="&csc;Slink{DCC/@SLINK1_ID}"/><Individual URI="&csc;DCC{DCC/@FMM_ID}"/>
      </ObjectPropertyAssertion>
      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;isIn"/>
	<Individual URI="&csc;Slink{DCC/@SLINK2_ID}"/><Individual URI="&csc;DCC{DCC/@FMM_ID}"/>
      </ObjectPropertyAssertion>
      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;isIn"/>
	<Individual URI="&csc;DCC{DCC/@FMM_ID}"/><Individual URI="&csc;FEDCrate{@CRATE_NUMBER}"/>
      </ObjectPropertyAssertion>
      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;receivesL1AFrom"/>
	<Individual URI="&csc;DCC{DCC/@FMM_ID}"/><Individual URI="&csc;TTC"/>
      </ObjectPropertyAssertion>
      <DataPropertyAssertion>
	<DataProperty URI="&csc;_FMM"/><Individual URI="&csc;DCC{DCC/@FMM_ID}"/>
	<Constant datatypeURI="&xsd;integer"><xsl:value-of select="DCC/@FMM_ID"/></Constant>
      </DataPropertyAssertion>

      <!-- DDUs -->
      <xsl:for-each select="DDU">
	<xsl:variable name="PADDED_RUI_INSTANCE" select="format-number(@RUI,'00')"/>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesL1AFrom"/>
	  <Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/><Individual URI="&csc;DCC{../DCC/@FMM_ID}"/>
	</ObjectPropertyAssertion>
 	<DataPropertyAssertion>
	  <DataProperty URI="&csc;_FMM"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/>
	  <Constant datatypeURI="&xsd;integer"><xsl:value-of select="@FMM_ID"/></Constant>
	</DataPropertyAssertion>
      </xsl:for-each>

    </xsl:for-each>
  </xsl:template>


  <!-- FMM of TF DDU -->
  <xsl:template match="templ:TFFMM">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>FMMs of TF DDU</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>

    <!-- FED system name(s) -->
    <xsl:variable name="FEDSYSTEM" select="document($SOURCE)/FEDSystem/@NAME"/>
    <Declaration><Individual URI="&csc;{$FEDSYSTEM}"/></Declaration>
    <xsl:variable name="FEDSYSTEM_CANONICAL">FED/TF</xsl:variable>
    <Declaration><Individual URI="&csc;{$FEDSYSTEM_CANONICAL}"/></Declaration>
    <ClassAssertion><Class URI="&csc;FEDSystem"/><Individual URI="&csc;{$FEDSYSTEM_CANONICAL}"/></ClassAssertion>
    <SameIndividuals><Individual URI="&csc;{$FEDSYSTEM}"/><Individual URI="&csc;{$FEDSYSTEM_CANONICAL}"/></SameIndividuals>

    <xsl:for-each select="document($SOURCE)//FEDCrate">
      <!-- FED Crate -->
      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;isIn"/>
	<Individual URI="&csc;FEDCrate{@CRATE_NUMBER}"/><Individual URI="&csc;{$FEDSYSTEM_CANONICAL}"/>
      </ObjectPropertyAssertion>
    </xsl:for-each>

<!--     <ObjectPropertyAssertion> -->
<!--       <ObjectProperty URI="&csc;receivesL1AFrom"/> -->
<!--       <Individual URI="&csc;DDU00"/><Individual URI="&csc;TODO"/> -->
<!--     </ObjectPropertyAssertion> -->
    <DataPropertyAssertion>
      <DataProperty URI="&csc;_FMM"/><Individual URI="&csc;DDU00"/>
      <Constant datatypeURI="&xsd;integer"><xsl:value-of select="document($SOURCE)//DDU/@FMM_ID"/></Constant>
    </DataPropertyAssertion>

  </xsl:template>


  <xsl:template match="templ:lDAQChain">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>DAQ Manager</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <Declaration><Individual URI="&csc;emu::daq::manager::Application"/></Declaration>
    <Declaration><Individual URI="&csc;DAQManager"/></Declaration>
    <SameIndividuals><Individual URI="&csc;emu::daq::manager::Application"/><Individual URI="&csc;DAQManager"/></SameIndividuals>
    <!-- <ClassAssertion><Class URI="&csc;LocalDAQ"/><Individual URI="&csc;DAQManager"/></ClassAssertion> -->
    <ClassAssertion><Class URI="&csc;Software"/><Individual URI="&csc;DAQManager"/></ClassAssertion>    
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declaration of RUIs and DDUs and their relation to chambers</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>DDU readout</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:for-each select="document($SOURCE)//RUI">
      <xsl:variable name="RUI_INSTANCE" select="@instance"/>
      <xsl:variable name="PADDED_RUI_INSTANCE" select="format-number(@instance,'00')"/>

      <Declaration><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/></Declaration>
      <Declaration><Individual URI="&csc;emu::daq::rui::Application{$PADDED_RUI_INSTANCE}"/></Declaration>
      <Declaration><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/></Declaration>
      <SameIndividuals><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/><Individual URI="&csc;emu::daq::rui::Application{$PADDED_RUI_INSTANCE}"/></SameIndividuals>
      <xsl:if test="$RUI_INSTANCE != $PADDED_RUI_INSTANCE">
	<Declaration><Individual URI="&csc;RUI{$RUI_INSTANCE}"/></Declaration>
	<Declaration><Individual URI="&csc;DDU{$RUI_INSTANCE}"/></Declaration>
	<SameIndividuals><Individual URI="&csc;RUI{$RUI_INSTANCE}"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/></SameIndividuals>
	<SameIndividuals><Individual URI="&csc;DDU{$RUI_INSTANCE}"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/></SameIndividuals>
      </xsl:if>
      <ClassAssertion><Class URI="&csc;RUI"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/></ClassAssertion>
      <ClassAssertion><Class URI="&csc;DDU"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/></ClassAssertion>

      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;controls"/><Individual URI="&csc;DAQManager"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/>
      </ObjectPropertyAssertion>
      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;monitors"/><Individual URI="&csc;DAQManager"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/>
      </ObjectPropertyAssertion>

      <xsl:if test="$PADDED_RUI_INSTANCE != '00'">
        <Declaration><Individual URI="&csc;EmuMonitor{$PADDED_RUI_INSTANCE}"/></Declaration>
        <ClassAssertion><Class URI="&csc;EmuMonitor"/><Individual URI="&csc;EmuMonitor{$PADDED_RUI_INSTANCE}"/></ClassAssertion>
        <ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;EmuMonitor{$PADDED_RUI_INSTANCE}"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/>
        </ObjectPropertyAssertion>
        <ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;sendsDataTo"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/><Individual URI="&csc;EmuMonitor{$PADDED_RUI_INSTANCE}"/>
        </ObjectPropertyAssertion>
      </xsl:if>

      <xsl:choose>
	<xsl:when test="@instance=0">
	  <ClassAssertion><Class URI="&csc;TFDDU"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/></ClassAssertion>
	  <ClassAssertion><Class URI="&csc;TFRUI"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/></ClassAssertion>
	</xsl:when>
	<xsl:otherwise>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;Slink{DCC/@slink}"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/>
	  </ObjectPropertyAssertion>
	</xsl:otherwise>
      </xsl:choose>

      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/><Individual URI="&csc;FEDCrate{DDU/@crate}"/>
      </ObjectPropertyAssertion>

      <!-- DDU inputs -->
      <xsl:for-each select="DDU/input">
	<xsl:variable name="DDU_INPUT" select="@id"/>
	<xsl:variable name="PADDED_DDU_INPUT" select="format-number(@id,'00')"/>

	<Declaration><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/></Declaration>
	<xsl:if test="$DDU_INPUT != $PADDED_DDU_INPUT">
	  <Declaration><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$DDU_INPUT}"/></Declaration>
	  <SameIndividuals><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$DDU_INPUT}"/></SameIndividuals>
	</xsl:if>
	<xsl:if test="$RUI_INSTANCE != $PADDED_RUI_INSTANCE">
	  <Declaration><Individual URI="&csc;DDU{$RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/></Declaration>
	  <SameIndividuals><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/><Individual URI="&csc;DDU{$RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/></SameIndividuals>
	  <xsl:if test="$DDU_INPUT != $PADDED_DDU_INPUT">
	    <Declaration><Individual URI="&csc;DDU{$RUI_INSTANCE}Input{$DDU_INPUT}"/></Declaration>
	    <SameIndividuals><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/><Individual URI="&csc;DDU{$RUI_INSTANCE}Input{$DDU_INPUT}"/></SameIndividuals>
	  </xsl:if>
	</xsl:if>
	<ClassAssertion><Class URI="&csc;DDUInput"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/></ClassAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/></ObjectPropertyAssertion>

	<xsl:choose>
	  <!-- TF -->
	  <xsl:when test="../../@instance=0">
	    <xsl:if test="SectorProcessor/@id!=''">
	      <xsl:variable name="PADDED_SP_INSTANCE" select="format-number(SectorProcessor/@id,'00')"/>
	      <ObjectPropertyAssertion>
		<ObjectProperty URI="&csc;receivesTriggerFrom"/>
		<Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/>
		<Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/>
	      </ObjectPropertyAssertion>
	    </xsl:if>
	  </xsl:when>
	  <!-- chambers (only those in PCrate source xml file, i.e., already installed) -->
	  <xsl:otherwise>
	    <xsl:variable name=       "CHAMBER_NAME">ME<xsl:value-of select="Chamber/@endcap"/><xsl:value-of select="Chamber/@station"/>/<xsl:value-of select="Chamber/@type"/>/<xsl:value-of select=              "Chamber/@number"/></xsl:variable>
	    <xsl:variable name="PADDED_CHAMBER_NAME">ME<xsl:value-of select="Chamber/@endcap"/><xsl:value-of select="Chamber/@station"/>/<xsl:value-of select="Chamber/@type"/>/<xsl:value-of select="format-number(Chamber/@number,'00')"/></xsl:variable>
	    <xsl:if test="document(document('ontology_template.xml')//templ:PSidePCrates/@source)//CSC[@label=$CHAMBER_NAME] | document(document('ontology_template.xml')//templ:MSidePCrates/@source)//CSC[@label=$CHAMBER_NAME]">
	      <ObjectPropertyAssertion>
		<ObjectProperty URI="&csc;receivesDataFrom"/>
		<Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/>
		<Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB"/>
	      </ObjectPropertyAssertion>
	    </xsl:if>
	  </xsl:otherwise>
	</xsl:choose>

      </xsl:for-each>

    </xsl:for-each>
  </xsl:template>

  <!-- Peripheral crates -->
  <xsl:template match="templ:PSidePCrates|templ:MSidePCrates">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declaration of ALCTs, CFEBs, RATs, TMBs and DMBs, and their relation to chambers and PCrates</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declaration of VCCs, MPCs and CCBs</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>
    <xsl:text>&LF;</xsl:text>
    <xsl:for-each select="document($SOURCE)/EmuSystem/PeripheralCrate">
      <!-- VCC, CCB, MPC -->
      <xsl:variable name="VME_NAME"><xsl:value-of select="translate(@label,'mp_','-+/')"/></xsl:variable>
      <xsl:variable name="PADDED_VME_NAME"><xsl:value-of select="substring($VME_NAME,1,6)"/><xsl:if test="string-length($VME_NAME)=7">0</xsl:if><xsl:value-of select="substring($VME_NAME,7,string-length($VME_NAME)-6)"/></xsl:variable>
      <!--       <xsl:message><xsl:value-of select="$VME_NAME"/><xsl:text>  </xsl:text><xsl:value-of select="$PADDED_VME_NAME"/></xsl:message> -->
      <Declaration><Individual URI="&csc;{$PADDED_VME_NAME}/VCC"/></Declaration>
      <Declaration><Individual URI="&csc;{$PADDED_VME_NAME}/CCB"/></Declaration>
      <Declaration><Individual URI="&csc;{$PADDED_VME_NAME}/MPC"/></Declaration>
      <ClassAssertion><Class URI="&csc;VCC"/><Individual URI="&csc;{$PADDED_VME_NAME}/VCC"/></ClassAssertion>
      <ClassAssertion><Class URI="&csc;CCB"/><Individual URI="&csc;{$PADDED_VME_NAME}/CCB"/></ClassAssertion>
      <ClassAssertion><Class URI="&csc;MPC"/><Individual URI="&csc;{$PADDED_VME_NAME}/MPC"/></ClassAssertion>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_VME_NAME}/VCC"/><Individual URI="&csc;{$PADDED_VME_NAME}"/></ObjectPropertyAssertion>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_VME_NAME}/CCB"/><Individual URI="&csc;{$PADDED_VME_NAME}"/></ObjectPropertyAssertion>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_VME_NAME}/MPC"/><Individual URI="&csc;{$PADDED_VME_NAME}"/></ObjectPropertyAssertion>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;getsLVFrom"/><Individual URI="&csc;{$PADDED_VME_NAME}/VCC"/><Individual URI="&csc;{$PADDED_VME_NAME}"/></ObjectPropertyAssertion>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;getsLVFrom"/><Individual URI="&csc;{$PADDED_VME_NAME}/CCB"/><Individual URI="&csc;{$PADDED_VME_NAME}"/></ObjectPropertyAssertion>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;getsLVFrom"/><Individual URI="&csc;{$PADDED_VME_NAME}/MPC"/><Individual URI="&csc;{$PADDED_VME_NAME}"/></ObjectPropertyAssertion>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;receivesL1AFrom"/><Individual URI="&csc;{$PADDED_VME_NAME}/CCB"/><Individual URI="&csc;TTC"/></ObjectPropertyAssertion>
      <!-- ALCT, RAT, TMB, DMB -->
      <xsl:for-each select="CSC">
	<xsl:variable name="PADDED_CHAMBER_NAME"><xsl:value-of select="substring(@label,1,7)"/><xsl:if test="string-length(@label)=8">0</xsl:if><xsl:value-of select="substring(@label,8,string-length(@label)-7)"/></xsl:variable>
	<xsl:variable name="CHAMBER_NAME"><xsl:value-of select="@label"/></xsl:variable>
	<!-- 	<xsl:message><xsl:value-of select="@label"/><xsl:text>  </xsl:text><xsl:value-of select="$PADDED_CHAMBER_NAME"/></xsl:message> -->
	<Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/></Declaration>
	<Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT" /></Declaration>
	<Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB" /></Declaration>
	<Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB" /></Declaration>
	<xsl:if test="$CHAMBER_NAME != $PADDED_CHAMBER_NAME">
	  <Declaration><Individual URI="&csc;{$CHAMBER_NAME}/ALCT"/></Declaration>
	  <Declaration><Individual URI="&csc;{$CHAMBER_NAME}/RAT" /></Declaration>
	  <Declaration><Individual URI="&csc;{$CHAMBER_NAME}/TMB" /></Declaration>
	  <Declaration><Individual URI="&csc;{$CHAMBER_NAME}/DMB" /></Declaration>
	  <SameIndividuals><Individual URI="&csc;{$CHAMBER_NAME}/ALCT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/></SameIndividuals>
	  <SameIndividuals><Individual URI="&csc;{$CHAMBER_NAME}/RAT" /><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT" /></SameIndividuals>
	  <SameIndividuals><Individual URI="&csc;{$CHAMBER_NAME}/TMB" /><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB" /></SameIndividuals>
	  <SameIndividuals><Individual URI="&csc;{$CHAMBER_NAME}/DMB" /><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB" /></SameIndividuals>
	</xsl:if>
	<ClassAssertion><Class URI="&csc;ALCT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/></ClassAssertion>
	<ClassAssertion><Class URI="&csc;RAT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/> </ClassAssertion>
	<ClassAssertion><Class URI="&csc;TMB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/> </ClassAssertion>
	<ClassAssertion><Class URI="&csc;DMB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB"/> </ClassAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/><Individual URI="&csc;{$PADDED_VME_NAME}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/><Individual URI="&csc;{$PADDED_VME_NAME}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB"/><Individual URI="&csc;{$PADDED_VME_NAME}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;getsLVFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;getsLVFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/><Individual URI="&csc;{$PADDED_VME_NAME}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;getsLVFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/><Individual URI="&csc;{$PADDED_VME_NAME}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;getsLVFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB"/><Individual URI="&csc;{$PADDED_VME_NAME}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/>
	</ObjectPropertyAssertion>
<!-- 	<ObjectPropertyAssertion> -->
<!-- 	  <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/> -->
<!-- 	</ObjectPropertyAssertion> -->
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_VME_NAME}/MPC"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesL1AFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/><Individual URI="&csc;{$PADDED_VME_NAME}/CCB"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesL1AFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesL1AFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesL1AFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB"/><Individual URI="&csc;{$PADDED_VME_NAME}/CCB"/>
	</ObjectPropertyAssertion>

	<!-- AFEB -->
	<xsl:for-each select="TMB/ALCT/AnodeChannel">
	  <xsl:variable name="PADDED_AFEB_NUMBER" select="format-number(@afeb_number,'00')"/>
	  <xsl:variable name="AFEB_NUMBER" select="@afeb_number"/>
	  <Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/></Declaration>
	  <xsl:if test="$AFEB_NUMBER != $PADDED_AFEB_NUMBER">
	    <Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$AFEB_NUMBER}"/></Declaration>
	    <SameIndividuals>
	      <Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$AFEB_NUMBER}"/>
	      <Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/>
	    </SameIndividuals>
	  </xsl:if>
	  <xsl:if test="$CHAMBER_NAME != $PADDED_CHAMBER_NAME">
	    <Declaration><Individual URI="&csc;{$CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/></Declaration>
	    <SameIndividuals>
	      <Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/>
	      <Individual URI="&csc;{$CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/>
	    </SameIndividuals>
	    <xsl:if test="$AFEB_NUMBER != $PADDED_AFEB_NUMBER">
	      <Declaration><Individual URI="&csc;{$CHAMBER_NAME}/AFEB{$AFEB_NUMBER}"/></Declaration>
	      <SameIndividuals>
		<Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/>
		<Individual URI="&csc;{$CHAMBER_NAME}/AFEB{$AFEB_NUMBER}"/>
	      </SameIndividuals>
	    </xsl:if>
	  </xsl:if>
	  <ClassAssertion><Class URI="&csc;AFEB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/></ClassAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/>
	  </ObjectPropertyAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/>
	  </ObjectPropertyAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/AFEB{$PADDED_AFEB_NUMBER}"/>
	  </ObjectPropertyAssertion>
	  
	</xsl:for-each>

	<!-- CFEB -->
	<xsl:for-each select="DAQMB/CFEB">
	  <Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/></Declaration>
	  <xsl:if test="$CHAMBER_NAME != $PADDED_CHAMBER_NAME">
	    <Declaration><Individual URI="&csc;{$CHAMBER_NAME}/CFEB{@cfeb_number}"/></Declaration>
	    <SameIndividuals><Individual URI="&csc;{$CHAMBER_NAME}/CFEB{@cfeb_number}"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/></SameIndividuals>
	  </xsl:if>
	  <ClassAssertion><Class URI="&csc;CFEB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/></ClassAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/>
	  </ObjectPropertyAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/>
	  </ObjectPropertyAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/>
	  </ObjectPropertyAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/>
	  </ObjectPropertyAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/>
	  </ObjectPropertyAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;receivesL1AFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB"/>
	  </ObjectPropertyAssertion>
	</xsl:for-each>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:template>



  <xsl:template match="templ:lDAQFarm">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declaration of local DAQ computers and their relation to RUIs</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>

    <xsl:for-each select="document($SOURCE)//computer">
      <Declaration><Individual URI="&csc;{@alias}"/></Declaration>
      <Declaration><Individual URI="&csc;{@host}"/></Declaration>
      <Declaration><Individual URI="&csc;{substring-before(@alias,'.cms')}"/></Declaration>
      <Declaration><Individual URI="&csc;{substring-before(@host,'.cms')}"/></Declaration>
      <SameIndividuals><Individual URI="&csc;{@alias}"/><Individual URI="&csc;{@host}"/></SameIndividuals>
      <xsl:variable name="UNQUALIFIED_HOST"><xsl:value-of select="substring-before(@host,'.cms')"/></xsl:variable>
      <xsl:variable name="UNQUALIFIED_ALIAS"><xsl:value-of select="substring-before(@alias,'.cms')"/></xsl:variable>
      <xsl:if test="$UNQUALIFIED_HOST != @host">
	<Declaration><Individual URI="&csc;{$UNQUALIFIED_HOST}"/></Declaration>
	<SameIndividuals><Individual URI="&csc;{$UNQUALIFIED_HOST}"/><Individual URI="&csc;{@host}"/></SameIndividuals>
      </xsl:if>
      <xsl:if test="$UNQUALIFIED_ALIAS != @alias">
	<Declaration><Individual URI="&csc;{$UNQUALIFIED_ALIAS}"/></Declaration>
	<SameIndividuals><Individual URI="&csc;{$UNQUALIFIED_ALIAS}"/><Individual URI="&csc;{@alias}"/></SameIndividuals>
      </xsl:if>
      <ClassAssertion><Class URI="&csc;Computer"/><Individual URI="&csc;{@alias}"/></ClassAssertion>
      
      <xsl:for-each select="RUI">
	<!-- schar devices -->
	<xsl:variable name="INSTANCE"><xsl:if test="string-length(@instance)=1">0</xsl:if><xsl:value-of select="@instance"/></xsl:variable>
	<Declaration><Individual URI="&csc;{../@alias}:/dev/schar{@schar}"/></Declaration>
	<ClassAssertion><Class URI="&csc;/dev/schar"/><Individual URI="&csc;{../@alias}:/dev/schar{@schar}"/></ClassAssertion>
	<ObjectPropertyAssertion>
	  <xsl:choose>
	    <xsl:when test="$INSTANCE = '00'"><ObjectProperty URI="&csc;receivesTriggerFrom"/></xsl:when>
	    <xsl:otherwise><ObjectProperty URI="&csc;receivesDataFrom"/></xsl:otherwise>
	  </xsl:choose>
	  <Individual URI="&csc;{../@alias}:/dev/schar{@schar}"/><Individual URI="&csc;DDU{$INSTANCE}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <xsl:choose>
	    <xsl:when test="$INSTANCE = '00'"><ObjectProperty URI="&csc;receivesTriggerFrom"/></xsl:when>
	    <xsl:otherwise><ObjectProperty URI="&csc;receivesDataFrom"/></xsl:otherwise>
	  </xsl:choose>
	  <Individual URI="&csc;RUI{$INSTANCE}"/><Individual URI="&csc;{../@alias}:/dev/schar{@schar}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;isIn"/>
	  <Individual URI="&csc;{../@alias}:/dev/schar{@schar}"/><Individual URI="&csc;{../@alias}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;isIn"/>
	  <Individual URI="&csc;RUI{$INSTANCE}"/><Individual URI="&csc;{../@alias}"/>
	</ObjectPropertyAssertion>
        <xsl:if test="$INSTANCE != '00'">
            <ObjectPropertyAssertion>
              <ObjectProperty URI="&csc;isIn"/>
              <Individual URI="&csc;EmuMonitor{$INSTANCE}"/><Individual URI="&csc;{../@alias}"/>
            </ObjectPropertyAssertion>
        </xsl:if>
	<DataPropertyAssertion>
	  <DataProperty URI="&csc;_isInConfiguration"/><Individual URI="&csc;RUI{$INSTANCE}"/>
	  <Constant datatypeURI="&xsd;boolean"><xsl:choose><xsl:when test="@status = 'in'">true</xsl:when><xsl:otherwise>false</xsl:otherwise></xsl:choose></Constant>
	</DataPropertyAssertion>

      </xsl:for-each>

    </xsl:for-each>
  </xsl:template>

  <!-- LTC and TTC -->
  <xsl:template match="templ:LTCandTTC">
    <Declaration><Individual URI="&csc;LTC"/></Declaration>
    <Declaration><Individual URI="&csc;TTC"/></Declaration>
    <ClassAssertion><Class URI="&csc;LTC"/><Individual URI="&csc;LTC"/></ClassAssertion>
    <ClassAssertion><Class URI="&csc;TTC"/><Individual URI="&csc;TTC"/></ClassAssertion>
  </xsl:template>

  <!-- HV -->
  <xsl:template match="templ:HV">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>HV</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE_DISTR" select="@source"/>
    <xsl:variable name="SOURCE_SEGMENT" select="../templ:HVSegments/@source"/>
    <!--     <xsl:message><xsl:value-of select="$SOURCE_SEGMENT"/></xsl:message> -->
    <xsl:for-each select="document($SOURCE_DISTR)/hv/hvPrimary">
      <!-- HV Primary -->
      <xsl:variable name="HVPRIMARY">HVPrimary<xsl:value-of select="@id"/></xsl:variable>
      <Declaration><Individual URI="&csc;{$HVPRIMARY}"/></Declaration>
      <ClassAssertion><Class URI="&csc;HVPrimary"/><Individual URI="&csc;{$HVPRIMARY}"/></ClassAssertion>
      <DataPropertyAssertion><DataProperty URI="&csc;_Name"/><Individual URI="&csc;{$HVPRIMARY}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@id"/></Constant></DataPropertyAssertion>
      <DataPropertyAssertion><DataProperty URI="&csc;_DCSId"/><Individual URI="&csc;{$HVPRIMARY}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@dcsId"/></Constant></DataPropertyAssertion>
      <xsl:for-each select="hvMaster">
	<!-- HV Master -->
	<xsl:variable name="PADDED_MASTER">HVMaster<xsl:value-of select="format-number(@id,'00')"/></xsl:variable>
	<Declaration><Individual URI="&csc;{$PADDED_MASTER}"/></Declaration>
	<ClassAssertion><Class URI="&csc;HVMaster"/><Individual URI="&csc;{$PADDED_MASTER}"/></ClassAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;getsHVFrom"/><Individual URI="&csc;{$PADDED_MASTER}"/><Individual URI="&csc;{$HVPRIMARY}"/>
	</ObjectPropertyAssertion>
      <DataPropertyAssertion><DataProperty URI="&csc;_DCSId"/><Individual URI="&csc;{$PADDED_MASTER}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@dcsId"/></Constant></DataPropertyAssertion>

	<xsl:for-each select="hvMasterChannel">
	  <!-- HV Master Channel -->
	  <xsl:variable name="MASTER_CHANNEL"><xsl:value-of select="$PADDED_MASTER"/>Channel<xsl:value-of select="@channelName"/></xsl:variable>
	  <Declaration><Individual URI="&csc;{$MASTER_CHANNEL}"/></Declaration>
	  <ClassAssertion><Class URI="&csc;HVMasterChannel"/><Individual URI="&csc;{$MASTER_CHANNEL}"/></ClassAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;getsHVFrom"/><Individual URI="&csc;{$MASTER_CHANNEL}"/><Individual URI="&csc;{$PADDED_MASTER}"/>
	  </ObjectPropertyAssertion>
	  <DataPropertyAssertion><DataProperty URI="&csc;_DCSId"/><Individual URI="&csc;{$MASTER_CHANNEL}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@dcsId"/></Constant></DataPropertyAssertion>

	  <xsl:for-each select="hvDistribution">
	    <!-- HV Distribution Board -->
	    <xsl:variable name="PADDED_DISTRIBUTION">HVDistribution<xsl:value-of select="format-number(@id,'000')"/></xsl:variable>
	    <Declaration><Individual URI="&csc;{$PADDED_DISTRIBUTION}"/></Declaration>
	    <ClassAssertion><Class URI="&csc;HVDistribution"/><Individual URI="&csc;{$PADDED_DISTRIBUTION}"/></ClassAssertion>
	    <ObjectPropertyAssertion>
	      <ObjectProperty URI="&csc;getsHVFrom"/><Individual URI="&csc;{$PADDED_DISTRIBUTION}"/><Individual URI="&csc;{$MASTER_CHANNEL}"/>
	    </ObjectPropertyAssertion>
	    <DataPropertyAssertion><DataProperty URI="&csc;_DCSId"/><Individual URI="&csc;{$PADDED_DISTRIBUTION}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@dcsId"/></Constant></DataPropertyAssertion>
	    <xsl:for-each select="hvSegment">
	      <xsl:variable name="CHANNEL" select="@moduleChannel"/>
	      <xsl:variable name="CHAMBER_TYPE"><xsl:choose><xsl:when test="substring(@isInChamber,4,1)!='1' and substring(@isInChamber,6,1)='2'">large</xsl:when><xsl:otherwise>small</xsl:otherwise></xsl:choose></xsl:variable>
	      <xsl:variable name="LAYER" select="document($SOURCE_SEGMENT)/HV/chamber[@type=$CHAMBER_TYPE]/channel[@OUT=$CHANNEL or @OUT1=$CHANNEL or @OUT2=$CHANNEL]/@layer"/>
	      <xsl:variable name="SEGMENT" select="document($SOURCE_SEGMENT)/HV/chamber[@type=$CHAMBER_TYPE]/channel[@OUT=$CHANNEL or @OUT1=$CHANNEL or @OUT2=$CHANNEL]/@segment"/>
	      <!-- HV Segment -->
	      <xsl:variable name="PADDED_CHAMBER_NAME" select="@isInChamber"/>
	      <xsl:variable name=       "CHAMBER_NAME" select="concat(substring(@isInChamber,1,7),number(substring(@isInChamber,8,2)))"/>
	      <!-- <xsl:message><xsl:value-of select="$PADDED_CHAMBER_NAME"/><xsl:text>  </xsl:text><xsl:value-of select="$CHAMBER_NAME"/></xsl:message> -->
	      <Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/{$LAYER}/HVSegment{$SEGMENT}"/></Declaration>
	      <xsl:if test="$CHAMBER_NAME != $PADDED_CHAMBER_NAME">
		<Declaration><Individual URI="&csc;{$CHAMBER_NAME}/{$LAYER}/HVSegment{$SEGMENT}"/></Declaration>
		<SameIndividuals><Individual URI="&csc;{$CHAMBER_NAME}/{$LAYER}/HVSegment{$SEGMENT}"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/{$LAYER}/HVSegment{$SEGMENT}"/></SameIndividuals>
	      </xsl:if>
	      <ClassAssertion><Class URI="&csc;HVSegment"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/{$LAYER}/HVSegment{$SEGMENT}"/></ClassAssertion>
	      <DataPropertyAssertion><DataProperty URI="&csc;_HVChannel"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/{$LAYER}/HVSegment{$SEGMENT}"/><Constant datatypeURI="&xsd;integer"><xsl:value-of select="$CHANNEL"/></Constant></DataPropertyAssertion>
	      <DataPropertyAssertion><DataProperty URI="&csc;_DCSId"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/{$LAYER}/HVSegment{$SEGMENT}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@dcsId"/></Constant></DataPropertyAssertion>
	      <ObjectPropertyAssertion>
		<ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/{$LAYER}/HVSegment{$SEGMENT}"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/{$LAYER}"/>
	      </ObjectPropertyAssertion>
	      <ObjectPropertyAssertion>
		<ObjectProperty URI="&csc;getsHVFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/{$LAYER}/HVSegment{$SEGMENT}"/><Individual URI="&csc;{$PADDED_DISTRIBUTION}"/>
	      </ObjectPropertyAssertion>
	    </xsl:for-each>
	  </xsl:for-each>

	</xsl:for-each>

      </xsl:for-each>
    </xsl:for-each>
  </xsl:template>


  <!-- HV for Dubna chambers -->
  <xsl:template match="templ:HVDubna">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>HV for Dubna chambers</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>
    <xsl:for-each select="document($SOURCE)/HV/rack">
      <!-- Rack -->
      <xsl:variable name="RACK">Rack<xsl:value-of select="@name"/></xsl:variable>
      <Declaration><Individual URI="&csc;{$RACK}"/></Declaration>
      <ClassAssertion><Class URI="&csc;HVRack"/><Individual URI="&csc;{$RACK}"/></ClassAssertion>
      <DataPropertyAssertion><DataProperty URI="&csc;_Name"/><Individual URI="&csc;{$RACK}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@name"/></Constant></DataPropertyAssertion>
      <xsl:for-each select="crate">
	<!-- HV Crate -->
	<xsl:variable name="CRATE_NAME"><xsl:value-of select="@name"/></xsl:variable>
	<Declaration><Individual URI="&csc;{$CRATE_NAME}"/></Declaration>
	<ClassAssertion><Class URI="&csc;HVCrate"/><Individual URI="&csc;{$CRATE_NAME}"/></ClassAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$CRATE_NAME}"/><Individual URI="&csc;{$RACK}"/></ObjectPropertyAssertion>
	<xsl:for-each select="connection">
	  <!-- HV Board and Channel -->
	  <xsl:variable name="PADDED_BOARD_NAME"><xsl:value-of select="$CRATE_NAME"/>/Board<xsl:value-of select="format-number(@board,'00')"/></xsl:variable>
	  <xsl:variable name="PADDED_CHANNEL_NAME"><xsl:value-of select="$PADDED_BOARD_NAME"/>/Channel<xsl:value-of select="format-number(@channel,'000')"/></xsl:variable>
	  <xsl:variable name="PADDED_LAYER_NAME"><xsl:value-of select="../@ringName"/>/<xsl:value-of select="format-number(@chamber,'00')"/>/<xsl:value-of select="@layer"/></xsl:variable>
	  <Declaration><Individual URI="&csc;{$PADDED_BOARD_NAME}"/></Declaration>
	  <Declaration><Individual URI="&csc;{$PADDED_CHANNEL_NAME}"/></Declaration>
	  <ClassAssertion><Class URI="&csc;HVBoard"/><Individual URI="&csc;{$PADDED_BOARD_NAME}"/></ClassAssertion>
	  <ClassAssertion><Class URI="&csc;HVChannel"/><Individual URI="&csc;{$PADDED_CHANNEL_NAME}"/></ClassAssertion>
	  <ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_BOARD_NAME}"/><Individual URI="&csc;{$CRATE_NAME}"/></ObjectPropertyAssertion>
	  <ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_CHANNEL_NAME}"/><Individual URI="&csc;{$PADDED_BOARD_NAME}"/></ObjectPropertyAssertion>
	  <DataPropertyAssertion><DataProperty URI="&csc;_DCSId"/><Individual URI="&csc;{$PADDED_CHANNEL_NAME}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@dcsId"/></Constant></DataPropertyAssertion>
	  <!-- Chamber layer -->
	  <ObjectPropertyAssertion><ObjectProperty URI="&csc;getsHVFrom"/><Individual URI="&csc;{$PADDED_LAYER_NAME}"/><Individual URI="&csc;{$PADDED_CHANNEL_NAME}"/></ObjectPropertyAssertion>
	</xsl:for-each>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:template>


  <!-- LV -->
  <xsl:template match="templ:LV">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>LV</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>
    <xsl:for-each select="document($SOURCE)/lv/maraton">
      <!-- Maratons -->
      <xsl:variable name="PADDED_MARATON_INSTANCE" select="format-number(@id,'00')"/>
      <Declaration><Individual URI="&csc;Maraton{$PADDED_MARATON_INSTANCE}"/></Declaration>
      <ClassAssertion><Class URI="&csc;Maraton"/><Individual URI="&csc;Maraton{$PADDED_MARATON_INSTANCE}"/></ClassAssertion>
      <DataPropertyAssertion><DataProperty URI="&csc;_DCSId"/><Individual URI="&csc;Maraton{$PADDED_MARATON_INSTANCE}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@dcsId"/></Constant></DataPropertyAssertion>
      <xsl:variable name="MARATONS_STATION">ME<xsl:value-of select="translate(@side,'MP','-+')"/><xsl:value-of select="@station"/></xsl:variable>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;Maraton{$PADDED_MARATON_INSTANCE}"/><Individual URI="&csc;{$MARATONS_STATION}"/></ObjectPropertyAssertion>
      <xsl:for-each select="peripheralCrate">
	<!-- Peripheral Crates -->
	<xsl:variable name="PADDED_VME_NAME"><xsl:value-of select="substring(@id,1,6)"/><xsl:if test="string-length(@id)=7">0</xsl:if><xsl:value-of select="substring(@id,7,string-length(@id)-6)"/></xsl:variable>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;getsLVFrom"/><Individual URI="&csc;{$PADDED_VME_NAME}"/><Individual URI="&csc;Maraton{$PADDED_MARATON_INSTANCE}"/></ObjectPropertyAssertion>
	<DataPropertyAssertion><DataProperty URI="&csc;_DCSId"/><Individual URI="&csc;{$PADDED_VME_NAME}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@dcsId"/></Constant></DataPropertyAssertion>
	<!-- PCrate Racks -->
	<xsl:variable name="RACK_NAME" select="@rackName"/>
	<!-- <xsl:message><xsl:value-of select="$RACK_NAME"/>: <xsl:value-of select="count(preceding::peripheralCrate[@rackName = $RACK_NAME])"/>/<xsl:value-of select="count(//peripheralCrate)"/></xsl:message> -->
	<xsl:if test="not(preceding::peripheralCrate[@rackName = $RACK_NAME])">
	  <Declaration><Individual URI="&csc;Rack{$RACK_NAME}"/></Declaration>
	  <ClassAssertion><Class URI="&csc;PCrateRack"/><Individual URI="&csc;Rack{$RACK_NAME}"/></ClassAssertion>
	  <DataPropertyAssertion><DataProperty URI="&csc;_Name"/><Individual URI="&csc;Rack{$RACK_NAME}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="$RACK_NAME"/></Constant></DataPropertyAssertion>
	  <!-- PCrate Rack Cooling -->
	  <xsl:call-template name="RackCooling">
	    <xsl:with-param name="RACK_NAME_" select="$RACK_NAME"/>
	  </xsl:call-template>
	</xsl:if>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;cools"/><Individual URI="&csc;Rack{$RACK_NAME}"/><Individual URI="&csc;{$PADDED_VME_NAME}"/></ObjectPropertyAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{$PADDED_VME_NAME}"/><Individual URI="&csc;Rack{$RACK_NAME}"/></ObjectPropertyAssertion>
	<DataPropertyAssertion><DataProperty URI="&csc;_PositionInRack"/><Individual URI="&csc;{$PADDED_VME_NAME}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@placeInRack"/></Constant></DataPropertyAssertion>
	<DataPropertyAssertion><DataProperty URI="&csc;_ELMBId"/><Individual URI="&csc;{$PADDED_VME_NAME}"/><Constant datatypeURI="&xsd;integer"><xsl:value-of select="@elmbId"/></Constant></DataPropertyAssertion>
	<DataPropertyAssertion><DataProperty URI="&csc;_DCSCrateNumber"/><Individual URI="&csc;{$PADDED_VME_NAME}"/><Constant datatypeURI="&xsd;string"><xsl:value-of select="@dcsCrateNumber"/></Constant></DataPropertyAssertion>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:template>

  <!-- Gas -->
  <xsl:template match="templ:Gas">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>Gas</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>
    <!-- gas mixer input lines -->
    <Declaration><Individual URI="&csc;GasMixerInputLine1"/></Declaration>
    <Declaration><Individual URI="&csc;GasMixerInputLine2"/></Declaration>
    <Declaration><Individual URI="&csc;GasMixerInputLine3"/></Declaration>
    <ClassAssertion><Class URI="&csc;GasMixerInputLine"/><Individual URI="&csc;GasMixerInputLine1"/></ClassAssertion>
    <ClassAssertion><Class URI="&csc;GasMixerInputLine"/><Individual URI="&csc;GasMixerInputLine2"/></ClassAssertion>
    <ClassAssertion><Class URI="&csc;GasMixerInputLine"/><Individual URI="&csc;GasMixerInputLine3"/></ClassAssertion>
    <DataPropertyAssertion><DataProperty URI="&csc;_Gas"/><Individual URI="&csc;GasMixerInputLine1"/><Constant datatypeURI="&xsd;string">CO2</Constant></DataPropertyAssertion>
    <DataPropertyAssertion><DataProperty URI="&csc;_Gas"/><Individual URI="&csc;GasMixerInputLine2"/><Constant datatypeURI="&xsd;string">Ar</Constant></DataPropertyAssertion>
    <DataPropertyAssertion><DataProperty URI="&csc;_Gas"/><Individual URI="&csc;GasMixerInputLine3"/><Constant datatypeURI="&xsd;string">CF4</Constant></DataPropertyAssertion>
    <ObjectPropertyAssertion><ObjectProperty URI="&csc;deliversGasTo"/><Individual URI="&csc;GasMixerInputLine1"/><Individual URI="&csc;GasMixer"/></ObjectPropertyAssertion>
    <ObjectPropertyAssertion><ObjectProperty URI="&csc;deliversGasTo"/><Individual URI="&csc;GasMixerInputLine2"/><Individual URI="&csc;GasMixer"/></ObjectPropertyAssertion>
    <ObjectPropertyAssertion><ObjectProperty URI="&csc;deliversGasTo"/><Individual URI="&csc;GasMixerInputLine3"/><Individual URI="&csc;GasMixer"/></ObjectPropertyAssertion>
    <!-- gas mixer -->
    <Declaration><Individual URI="&csc;GasMixer"/></Declaration>
    <ClassAssertion><Class URI="&csc;GasDevice"/><Individual URI="&csc;GasMixer"/></ClassAssertion>
    <xsl:for-each select="document($SOURCE)/gasSystem/gasChannel">
      <!-- gas racks -->
      <xsl:variable name="GAS_RACK_NUMBER" select="@rackNumber"/>
      <xsl:if test="not(preceding-sibling::gasChannel[@rackNumber=$GAS_RACK_NUMBER])">
	<Declaration><Individual URI="&csc;GasRack{$GAS_RACK_NUMBER}"/></Declaration>
	<ClassAssertion><Class URI="&csc;GasRack"/><Individual URI="&csc;GasRack{$GAS_RACK_NUMBER}"/></ClassAssertion>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;deliversGasTo"/><Individual URI="&csc;GasMixer"/><Individual URI="&csc;GasRack{$GAS_RACK_NUMBER}"/></ObjectPropertyAssertion>
      </xsl:if>
      <!-- gas channels -->
      <Declaration><Individual URI="&csc;GasRack{$GAS_RACK_NUMBER}Channel{@number}"/></Declaration>
      <ClassAssertion><Class URI="&csc;GasChannel"/><Individual URI="&csc;GasRack{$GAS_RACK_NUMBER}Channel{@number}"/></ClassAssertion>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;deliversGasTo"/><Individual URI="&csc;GasRack{$GAS_RACK_NUMBER}"/><Individual URI="&csc;GasRack{$GAS_RACK_NUMBER}Channel{@number}"/></ObjectPropertyAssertion>
      <ObjectPropertyAssertion><ObjectProperty URI="&csc;deliversGasTo"/><Individual URI="&csc;GasRack{$GAS_RACK_NUMBER}Channel{@number}"/><Individual URI="&csc;{chamber[1]/@name}"/></ObjectPropertyAssertion>
      <xsl:for-each select="chamber[position()!=last()]">
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;deliversGasTo"/><Individual URI="&csc;{@name}"/><Individual URI="&csc;{following-sibling::chamber/@name}"/></ObjectPropertyAssertion>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:template>

  <!-- Cooling -->
  <xsl:template match="templ:CoolingCircuits">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>Cooling</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <Declaration><Individual URI="&csc;CoolingCircuit1"/></Declaration>
    <Declaration><Individual URI="&csc;CoolingCircuit2"/></Declaration>
    <ClassAssertion><Class URI="&csc;CoolingCircuit"/><Individual URI="&csc;CoolingCircuit1"/></ClassAssertion>
    <ClassAssertion><Class URI="&csc;CoolingCircuit"/><Individual URI="&csc;CoolingCircuit2"/></ClassAssertion>
    <DataPropertyAssertion><DataProperty URI="&csc;_Name"/><Individual URI="&csc;CoolingCircuit1"/><Constant datatypeURI="&xsd;string">Racks Circuit</Constant></DataPropertyAssertion>
    <DataPropertyAssertion><DataProperty URI="&csc;_Name"/><Individual URI="&csc;CoolingCircuit2"/><Constant datatypeURI="&xsd;string">Endcap Circuit</Constant></DataPropertyAssertion>
  </xsl:template>

  <xsl:template name="RackCooling">
    <xsl:param name="RACK_NAME_"/>
    <!-- Rack Turbines -->
    <Declaration><Individual URI="&csc;Rack{$RACK_NAME_}Turbine1"/></Declaration>
    <Declaration><Individual URI="&csc;Rack{$RACK_NAME_}Turbine2"/></Declaration>
    <ClassAssertion><Class URI="&csc;RackTurbine"/><Individual URI="&csc;Rack{$RACK_NAME_}Turbine1"/></ClassAssertion>
    <ClassAssertion><Class URI="&csc;RackTurbine"/><Individual URI="&csc;Rack{$RACK_NAME_}Turbine2"/></ClassAssertion>
    <ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;Rack{$RACK_NAME_}Turbine1"/><Individual URI="&csc;Rack{$RACK_NAME_}"/></ObjectPropertyAssertion>
    <ObjectPropertyAssertion><ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;Rack{$RACK_NAME_}Turbine2"/><Individual URI="&csc;Rack{$RACK_NAME_}"/></ObjectPropertyAssertion>
    <ObjectPropertyAssertion><ObjectProperty URI="&csc;cools"/><Individual URI="&csc;Rack{$RACK_NAME_}Turbine1"/><Individual URI="&csc;Rack{$RACK_NAME_}"/></ObjectPropertyAssertion>
    <ObjectPropertyAssertion><ObjectProperty URI="&csc;cools"/><Individual URI="&csc;Rack{$RACK_NAME_}Turbine2"/><Individual URI="&csc;Rack{$RACK_NAME_}"/></ObjectPropertyAssertion>
    <!-- Rack Water Circuit -->
    <xsl:variable name="DISK" select="number(substring($RACK_NAME_,4,1))-2"/>
    <xsl:choose>
      <xsl:when test="$DISK='1'">
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;cools"/><Individual URI="&csc;CoolingCircuit1"/><Individual URI="&csc;Rack{$RACK_NAME_}"/></ObjectPropertyAssertion>	
      </xsl:when>
      <xsl:otherwise>
	<ObjectPropertyAssertion><ObjectProperty URI="&csc;cools"/><Individual URI="&csc;CoolingCircuit2"/><Individual URI="&csc;Rack{$RACK_NAME_}"/></ObjectPropertyAssertion>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- FactProviders -->
  <xsl:template match="templ:FactProviders">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>FactProviders</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>
    <xsl:for-each select="document($SOURCE)/FactProviders/Provider">
      <xsl:variable name="provider" select="text()"/>
      <Declaration><Individual URI="&csc;{$provider}"/></Declaration>
      <ClassAssertion><Class URI="&csc;FactProvider"/><Individual URI="&csc;{$provider}"/></ClassAssertion>
    </xsl:for-each>
  </xsl:template>

  <!-- DCS Software -->
  <xsl:template match="templ:DcsSoftware">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>DcsSoftware</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>
    <xsl:for-each select="document($SOURCE)/DCS/SoftwareComponents/DcsSoftwareComponent">
      <xsl:variable name="sw" select="text()"/>
      <Declaration><Individual URI="&csc;{$sw}"/></Declaration>
      <ClassAssertion><Class URI="&csc;DCS"/><Individual URI="&csc;{$sw}"/></ClassAssertion>
    </xsl:for-each>
    <xsl:for-each select="document($SOURCE)/DCS/Computers/DcsComputer">
      <xsl:variable name="computer" select="text()"/>
      <Declaration><Individual URI="&csc;{$computer}"/></Declaration>
      <ClassAssertion><Class URI="&csc;Computer"/><Individual URI="&csc;{$computer}"/></ClassAssertion>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="templ:Dummies">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>Various dummy individuals</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <!-- of FEDSystem -->
    <Declaration><Individual URI="&csc;Test"/></Declaration>
    <ClassAssertion><Class URI="&csc;FEDSystem"/><Individual URI="&csc;Test"/></ClassAssertion>
    <Declaration><Individual URI="&csc;Test-Crate"/></Declaration>
    <SameIndividuals>
      <Individual URI="&csc;Test-Crate"/>
      <Individual URI="&csc;Test"/>
    </SameIndividuals>
    <Declaration><Individual URI="&csc;unnamed"/></Declaration>
    <ClassAssertion><Class URI="&csc;FEDSystem"/><Individual URI="&csc;unnamed"/></ClassAssertion>
  </xsl:template>

  <xsl:template name="Annotate">
    <!-- Copies the class annotation to the individual. Not used now. -->
    <xsl:param name="CLASS"/>
    <xsl:param name="INDIVIDUAL"/>
    <EntityAnnotation>
      <Individual URI="&csc;{$INDIVIDUAL}"/><xsl:copy-of select="document('ontology_template.xml')//owl2xml:EntityAnnotation[substring-after(owl2xml:Class/@URI,'#')=$CLASS]/owl2xml:Annotation"/>
    </EntityAnnotation>
  </xsl:template>

</xsl:transform>

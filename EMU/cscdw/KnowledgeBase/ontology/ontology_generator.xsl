<?xml version="1.0" encoding="UTF-8"?>

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
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}"/><Individual URI="&csc;ME"/>
	</ObjectPropertyAssertion>

	<xsl:for-each select="document($SOURCE)//Chamber[@endcap=$ENDCAP]">
	  <xsl:sort select="@station"/>
	  <xsl:variable name="STATION" select="@station"/>
	  <xsl:if test="not(preceding::Chamber[@endcap=$ENDCAP]/@station=$STATION)">
	    <!-- Stations -->
	    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}"/></Declaration>
	    <ClassAssertion><Class URI="&csc;Station"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}"/></ClassAssertion>
	    <ObjectPropertyAssertion>
	      <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}"/><Individual URI="&csc;ME{$ENDCAP}"/>
	    </ObjectPropertyAssertion>

	    <xsl:for-each select="document($SOURCE)//Chamber[@endcap=$ENDCAP and @station=$STATION]">
	      <xsl:sort select="@type"/>
	      <xsl:variable name="RING" select="@type"/>
	      <xsl:if test="not(preceding::Chamber[@endcap=$ENDCAP and @station=$STATION]/@type=$RING)">
		<!-- Rings -->
		<Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}"/></Declaration>
		<ClassAssertion><Class URI="&csc;Ring"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}"/></ClassAssertion>
		<ObjectPropertyAssertion>
		  <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}"/>
		</ObjectPropertyAssertion>
		
		<xsl:for-each select="document($SOURCE)//Chamber[@endcap=$ENDCAP and @station=$STATION and @type=$RING]">
		  <xsl:sort select="@number"/>
		  <xsl:variable name="CHAMBER" select="@number"/>
		  <xsl:if test="not(preceding::Chamber[@endcap=$ENDCAP and @station=$STATION and @type=$RING]/@number=$CHAMBER)">

		    <!-- Chambers -->
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/></Declaration>
		    <ClassAssertion><Class URI="&csc;Chamber"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/></ClassAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}"/>
		    </ObjectPropertyAssertion>

		    <!-- Layers -->
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/1"/></Declaration>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/1"/></ClassAssertion>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/2"/></Declaration>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/2"/></ClassAssertion>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/3"/></Declaration>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/3"/></ClassAssertion>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/4"/></Declaration>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/4"/></ClassAssertion>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/5"/></Declaration>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/5"/></ClassAssertion>
		    <Declaration><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/6"/></Declaration>
		    <ClassAssertion><Class URI="&csc;Layer"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/6"/></ClassAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/1"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/2"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/3"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/4"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/5"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/>
		    </ObjectPropertyAssertion>
		    <ObjectPropertyAssertion>
		      <ObjectProperty URI="&csc;isPartOf"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}/6"/><Individual URI="&csc;ME{$ENDCAP}{$STATION}/{$RING}/{$CHAMBER}"/>
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

      <xsl:variable name="PADDED_SP_INSTANCE"><xsl:if test="string-length(@id)=1">0</xsl:if><xsl:value-of select="@id"/></xsl:variable>
      <Declaration><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/></Declaration>
      <ClassAssertion><Class URI="&csc;SectorProcessor"/><Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/></ClassAssertion>

      <!-- PeripheralCrates -->
      <xsl:for-each select="PeripheralCrate">
	<xsl:sort select="@endcap"/>
	<xsl:sort select="@station"/>
	<xsl:sort select="@VMEcrate"/>
	<xsl:variable name="PM_ENDCAP"><xsl:choose><xsl:when test="@endcap = '+'">p</xsl:when><xsl:otherwise>m</xsl:otherwise></xsl:choose></xsl:variable>
	<xsl:variable name="PADDED_VME_NUMBER"><xsl:if test="string-length(@VMEcrate)=1">0</xsl:if><xsl:value-of select="@VMEcrate"/></xsl:variable>
	<Declaration><Individual URI="&csc;VME{@endcap}{@station}/{@VMEcrate}"/></Declaration>
	<Declaration><Individual URI="&csc;VME{$PM_ENDCAP}{@station}_{@VMEcrate}"/></Declaration>
	<Declaration><Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}"/></Declaration>
	<Declaration><Individual URI="&csc;VME{$PM_ENDCAP}{@station}_{$PADDED_VME_NUMBER}"/></Declaration>
	<SameIndividuals>
	  <Individual URI="&csc;VME{@endcap}{@station}/{@VMEcrate}"/>
	  <Individual URI="&csc;VME{$PM_ENDCAP}{@station}_{@VMEcrate}"/>
	  <Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}"/>
	  <Individual URI="&csc;VME{$PM_ENDCAP}{@station}_{$PADDED_VME_NUMBER}"/>
	</SameIndividuals>
	<ClassAssertion><Class URI="&csc;PeripheralCrate"/><Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}"/></ClassAssertion>
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

    <!-- DCC -->
    <xsl:for-each select="document($SOURCE)//FEDCrate">
      <Declaration><Individual URI="&csc;DCC{DCC/@FMM_ID}"/></Declaration>
      <Declaration><Individual URI="&csc;Slink{DCC/@SLINK1_ID}"/></Declaration>
      <Declaration><Individual URI="&csc;Slink{DCC/@SLINK2_ID}"/></Declaration>
      <ClassAssertion><Class URI="&csc;DCC"/><Individual URI="&csc;DCC{DCC/@FMM_ID}"/></ClassAssertion>
      <ClassAssertion><Class URI="&csc;Slink"/><Individual URI="&csc;Slink{DCC/@SLINK1_ID}"/></ClassAssertion>
      <ClassAssertion><Class URI="&csc;Slink"/><Individual URI="&csc;Slink{DCC/@SLINK2_ID}"/></ClassAssertion>
<!--       <ObjectPropertyAssertion> -->
<!-- 	<ObjectProperty URI="&csc;receivesDataFrom"/> -->
<!-- 	<Individual URI="&csc;Slink{DCC/@SLINK1_ID}"/><Individual URI="&csc;DCC{DCC/@FMM_ID}"/> -->
<!--       </ObjectPropertyAssertion> -->
<!--       <ObjectPropertyAssertion> -->
<!-- 	<ObjectProperty URI="&csc;receivesDataFrom"/> -->
<!-- 	<Individual URI="&csc;Slink{DCC/@SLINK2_ID}"/><Individual URI="&csc;DCC{DCC/@FMM_ID}"/> -->
<!--       </ObjectPropertyAssertion> -->
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
      <DataPropertyAssertion>
	<DataProperty URI="&csc;hasFMM"/><Individual URI="&csc;DCC{DCC/@FMM_ID}"/>
	<Constant datatypeURI="&xsd;integer"><xsl:value-of select="DCC/@FMM_ID"/></Constant>
      </DataPropertyAssertion>

      <!-- DDUs -->
      <xsl:for-each select="DDU">
	<xsl:variable name="PADDED_RUI_INSTANCE"><xsl:if test="string-length(@RUI)=1">0</xsl:if><xsl:value-of select="@RUI"/></xsl:variable>
<!-- 	<ObjectPropertyAssertion> -->
<!-- 	  <ObjectProperty URI="&csc;receivesDataFrom"/> -->
<!-- 	  <Individual URI="&csc;DCC{../DCC/@FMM_ID}"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/> -->
<!-- 	  <Individual URI="&csc;DCC{../DCC/@FMM_ID}"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/> -->
<!-- 	</ObjectPropertyAssertion> -->
 	<DataPropertyAssertion>
	  <DataProperty URI="&csc;hasFMM"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/>
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
    <DataPropertyAssertion>
      <DataProperty URI="&csc;hasFMM"/><Individual URI="&csc;DDU00"/>
      <Constant datatypeURI="&xsd;integer"><xsl:value-of select="document($SOURCE)//DDU/@FMM_ID"/></Constant>
    </DataPropertyAssertion>
  </xsl:template>


  <xsl:template match="templ:lDAQChain">
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>declaration of RUIs and DDUs and their relation to chambers</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:variable name="SOURCE" select="@source"/>
    <xsl:text>&LF;</xsl:text>
    <xsl:comment>DDU readout</xsl:comment>
    <xsl:text>&LF;</xsl:text>
    <xsl:for-each select="document($SOURCE)//RUI">
      <xsl:variable name="PADDED_RUI_INSTANCE"><xsl:if test="string-length(@instance)=1">0</xsl:if><xsl:value-of select="@instance"/></xsl:variable>

      <Declaration><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/></Declaration>
      <ClassAssertion><Class URI="&csc;RUI"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/></ClassAssertion>
      <DataPropertyAssertion>
	<DataProperty URI="&csc;hasInstance"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/>
	<Constant datatypeURI="&xsd;integer"><xsl:value-of select="@instance"/></Constant>
      </DataPropertyAssertion>

      <Declaration><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/></Declaration>
      <ClassAssertion><Class URI="&csc;DDU"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/></ClassAssertion>
      <DataPropertyAssertion>
	<DataProperty URI="&csc;hasInstance"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/>
	<Constant datatypeURI="&xsd;integer"><xsl:value-of select="@instance"/></Constant>
      </DataPropertyAssertion>

      <xsl:choose>
	<xsl:when test="@instance=0">
	  <ClassAssertion><Class URI="&csc;TFDDU"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/></ClassAssertion>
	  <ClassAssertion><Class URI="&csc;TFRUI"/><Individual URI="&csc;RUI{$PADDED_RUI_INSTANCE}"/></ClassAssertion>
	</xsl:when>
	<xsl:otherwise>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;receivesDataFrom"/>
	    <Individual URI="&csc;Slink{DCC/@slink}"/>
	    <Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/>
	  </ObjectPropertyAssertion>
	</xsl:otherwise>
      </xsl:choose>

      <ObjectPropertyAssertion>
	<ObjectProperty URI="&csc;isIn"/>
	<Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/>
	<Individual URI="&csc;FEDCrate{DDU/@crate}"/>
      </ObjectPropertyAssertion>

      <xsl:for-each select="DDU/input">
	<xsl:variable name="PADDED_DDU_INPUT"><xsl:if test="string-length(@id)=1">0</xsl:if><xsl:value-of select="@id"/></xsl:variable>
	<Declaration><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/></Declaration>
	<ClassAssertion><Class URI="&csc;DDUInput"/><Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/></ClassAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;isIn"/>
	  <Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/>
	  <Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesDataFrom"/>
	  <Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}"/>
	  <Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/>
	</ObjectPropertyAssertion>

	<xsl:choose>
	  <!-- TF -->
	  <xsl:when test="../../@instance=0 and SectorProcessor/@id!=''">
	    <xsl:variable name="PADDED_SP_INSTANCE"><xsl:if test="string-length(SectorProcessor/@id)=1">0</xsl:if><xsl:value-of select="SectorProcessor/@id"/></xsl:variable>
	    <ObjectPropertyAssertion>
	      <ObjectProperty URI="&csc;receivesTriggerFrom"/>
	      <Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/>
	      <Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/>
	    </ObjectPropertyAssertion>
	    <xsl:for-each select="SectorProcessor/PeripheralCrate">
	      <xsl:variable name="PADDED_VME_NUMBER"><xsl:if test="string-length(@VMEcrate)=1">0</xsl:if><xsl:value-of select="@VMEcrate"/></xsl:variable>
	      <ObjectPropertyAssertion>
		<ObjectProperty URI="&csc;receivesTriggerFrom"/>
		<Individual URI="&csc;SectorProcessor{$PADDED_SP_INSTANCE}"/>
		<Individual URI="&csc;VME{@endcap}{@station}/{$PADDED_VME_NUMBER}/MPC"/>
	      </ObjectPropertyAssertion>
	    </xsl:for-each>
	  </xsl:when>
	  <!-- chambers -->
	  <xsl:otherwise>
	    <ObjectPropertyAssertion>
	      <ObjectProperty URI="&csc;receivesDataFrom"/>
	      <Individual URI="&csc;DDU{$PADDED_RUI_INSTANCE}Input{$PADDED_DDU_INPUT}"/>
	      <Individual URI="&csc;ME{Chamber/@endcap}{Chamber/@station}/{Chamber/@type}/{Chamber/@number}/DMB"/>
	    </ObjectPropertyAssertion>
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
      <!-- ALCT, RAT, TMB, DMB -->
      <xsl:for-each select="CSC">
	<xsl:variable name="PADDED_CHAMBER_NAME"><xsl:value-of select="substring(@label,1,7)"/><xsl:if test="string-length(@label)=8">0</xsl:if><xsl:value-of select="substring(@label,8,string-length(@label)-7)"/></xsl:variable>
	<!-- 	<xsl:message><xsl:value-of select="@label"/><xsl:text>  </xsl:text><xsl:value-of select="$PADDED_CHAMBER_NAME"/></xsl:message> -->
	<Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/></Declaration>
	<Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/> </Declaration>
	<Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/> </Declaration>
	<Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/DMB"/> </Declaration>
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
	  <ObjectProperty URI="&csc;receivesDataFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/>
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
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/ALCT"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/RAT"/>
	</ObjectPropertyAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;receivesTriggerFrom"/><Individual URI="&csc;{$PADDED_VME_NAME}/MPC"/><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/TMB"/>
	</ObjectPropertyAssertion>
	<xsl:for-each select="DAQMB/CFEB">
	  <!-- CFEB -->
	  <Declaration><Individual URI="&csc;{$PADDED_CHAMBER_NAME}/CFEB{@cfeb_number}"/></Declaration>
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
	<DataPropertyAssertion>
	  <DataProperty URI="&csc;isOn"/><Individual URI="&csc;RUI{$INSTANCE}"/>
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
    <xsl:message><xsl:value-of select="$SOURCE_SEGMENT"/></xsl:message>
    <xsl:for-each select="document($SOURCE_DISTR)/hv/hvPrimary">
      <!-- HV Primary -->
      <xsl:variable name="HVPRIMARY">HVPrimary<xsl:value-of select="@id"/></xsl:variable>
      <Declaration><Individual URI="&csc;{$HVPRIMARY}"/></Declaration>
      <ClassAssertion><Class URI="&csc;HVPrimary"/><Individual URI="&csc;{$HVPRIMARY}"/></ClassAssertion>
      <xsl:for-each select="hvMaster">
	<!-- HV Master -->
	<xsl:variable name="MASTER">Master<xsl:value-of select="@id"/></xsl:variable>
	<Declaration><Individual URI="&csc;{$HVPRIMARY}/{$MASTER}"/></Declaration>
	<ClassAssertion><Class URI="&csc;HVMaster"/><Individual URI="&csc;{$HVPRIMARY}/{$MASTER}"/></ClassAssertion>
	<ObjectPropertyAssertion>
	  <ObjectProperty URI="&csc;getsHVFrom"/><Individual URI="&csc;{$HVPRIMARY}/{$MASTER}"/><Individual URI="&csc;{$HVPRIMARY}"/>
	</ObjectPropertyAssertion>
	<xsl:for-each select="hvDistribution">
	  <!-- HV Distribution Board -->
	  <xsl:variable name="DISTRIBUTION">Distribution<xsl:value-of select="@id"/></xsl:variable>
	  <Declaration><Individual URI="&csc;{$HVPRIMARY}/{$MASTER}/{$DISTRIBUTION}"/></Declaration>
	  <ClassAssertion><Class URI="&csc;HVDistribution"/><Individual URI="&csc;{$HVPRIMARY}/{$MASTER}/{$DISTRIBUTION}"/></ClassAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;getsHVFrom"/><Individual URI="&csc;{$HVPRIMARY}/{$MASTER}/{$DISTRIBUTION}"/><Individual URI="&csc;{$HVPRIMARY}/{$MASTER}"/>
	  </ObjectPropertyAssertion>
	  <xsl:for-each select="hvSegment">
	    <xsl:variable name="CHANNEL" select="@moduleChannel"/>
	    <xsl:variable name="CHAMBER_TYPE"><xsl:choose><xsl:when test="substring(@isInChamber,4,1)!='1' and substring(@isInChamber,6,1)='2'">large</xsl:when><xsl:otherwise>small</xsl:otherwise></xsl:choose></xsl:variable>
	    <xsl:variable name="LAYER" select="document($SOURCE_SEGMENT)/HV/chamber[@type=$CHAMBER_TYPE]/channel[@OUT=$CHANNEL or @OUT2=$CHANNEL or @OUT2=$CHANNEL]/@layer"/>
<!-- 	    <xsl:variable name="LAYER"><xsl:choose><xsl:when test="$CHAMBER_TYPE='small'"><xsl:value-of select="document($SOURCE_SEGMENT)/HV/chamber[@type=$CHAMBER_TYPE]/channel[@OUT1=$CHANNEL or @OUT2=$CHANNEL]/@layer"/></xsl:when><xsl:otherwise><xsl:value-of select="document($SOURCE_SEGMENT)/HV/chamber[@type=$CHAMBER_TYPE]/channel[@OUT=$CHANNEL]/@layer"/></xsl:otherwise></xsl:choose> -->
            <xsl:variable name="SEGMENT" select="document($SOURCE_SEGMENT)/HV/chamber[@type=$CHAMBER_TYPE]/channel[@OUT=$CHANNEL or @OUT2=$CHANNEL or @OUT2=$CHANNEL]/@segment"/>
	    <!-- HV Segment -->
	    <Declaration><Individual URI="&csc;{@isInChamber}/{$LAYER}/HVSegment{$SEGMENT}"/></Declaration>
	    <ClassAssertion><Class URI="&csc;HVSegment"/><Individual URI="&csc;{@isInChamber}/{$LAYER}/HVSegment{$SEGMENT}"/></ClassAssertion>
	    <DataPropertyAssertion>
	      <DataProperty URI="&csc;hasHVChannel"/><Individual URI="&csc;{@isInChamber}/{$LAYER}/HVSegment{$SEGMENT}"/>
	      <Constant datatypeURI="&xsd;integer"><xsl:value-of select="$CHANNEL"/></Constant>
	    </DataPropertyAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;isIn"/><Individual URI="&csc;{@isInChamber}/{$LAYER}/HVSegment{$SEGMENT}"/><Individual URI="&csc;{@isInChamber}/{$LAYER}"/>
	  </ObjectPropertyAssertion>
	  <ObjectPropertyAssertion>
	    <ObjectProperty URI="&csc;getsHVFrom"/><Individual URI="&csc;{@isInChamber}/{$LAYER}/HVSegment{$SEGMENT}"/><Individual URI="&csc;{$HVPRIMARY}/{$MASTER}/{$DISTRIBUTION}"/>
	  </ObjectPropertyAssertion>
	  </xsl:for-each>
	</xsl:for-each>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:template>

</xsl:transform>

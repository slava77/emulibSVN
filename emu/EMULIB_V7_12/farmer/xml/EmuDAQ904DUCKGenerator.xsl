<?xml version="1.0"?>
<!-- This XSL transformation is used for generating DUCK config file for EmuDAQ and EmuDQM from a RUI-to-computer mapping. -->
<!-- Usage example:  -->
<!--     xsltproc <hyphen><hyphen>stringparam NAME 'DAQConfig' <hyphen><hyphen>stringparam FARM 'DAQ' EmuDAQ904DUCKGenerator.xsl RUI-to-computer_mapping_bdg904.xml > DAQ.duck -->
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0" xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">

  <!-- Parameter SIDE is to be optionally set from the command line. -->
  <!-- If it's set to 'M', only the minus side will be generated; if 'P', only the plus side; if 'B', both sides; otherwise both sides. -->
<!--   <xsl:param name="SIDE"/> -->

  <!-- Parameter DIR is to be set from the command line. -->
  <!-- It's the system directory containing the XDAQ configuration files. -->
  <xsl:param name="DIR">/home/cscdev/config/conf_generator/r9/out</xsl:param>

  <!-- Parameter NAME is to be set from the command line. -->
  <!-- It's the name of the XDAQ configuration file (without path or extension) -->
  <xsl:param name="NAME"/>

  <!-- Parameter FARM is to be set from the command line. -->
  <!-- It's either DAQ or DQM or DQM_Display-->
  <xsl:param name="FARM"/>

  <xsl:param name="CONFIG_FILE"><xsl:value-of select="$DIR"/>/<xsl:value-of select="$NAME"/>.xml</xsl:param>
  <xsl:param name="FM_CONFIG_PATH">
    <xsl:if test="$FARM='DAQ'">DAQ/<xsl:value-of select="$NAME"/></xsl:if>
    <xsl:if test="$FARM='DQM'">DQM/DQM</xsl:if>
    <xsl:if test="$FARM='DQM_Display'">DQM/DQM_Display</xsl:if>
  </xsl:param>
  <xsl:param name="PATHTOEXECUTIVE">/opt/xdaq/bin/xdaq.exe</xsl:param>
  <xsl:param name="DAQ_ENVIRONMENTSTRING">HOME=/home/cscdaq LD_LIBRARY_PATH=/home/cscdaq/TriDAS/x86/lib:/opt/xdaq/lib XDAQ_ROOT=/opt/xdaq XDAQ_SETUP_ROOT=/opt/xdaq/share BUILD_HOME=/home/cscdaq/TriDAS XDAQ_DOCUMENT_ROOT=/opt/xdaq/htdocs XDAQ_PLATFORM=x86 XDAQ_OS=linux XDAQ_ZONE=emu904 PATH=/bin:/usr/bin</xsl:param>
  <xsl:param name="DQM_ENVIRONMENTSTRING">HOME=/home/cscdev LD_LIBRARY_PATH=/home/cscdev/root/lib:/opt/xdaq/lib XDAQ_ROOT=/opt/xdaq XDAQ_SETUP_ROOT=/opt/xdaq/share BUILD_HOME=/home/cscdev/TriDAS XDAQ_DOCUMENT_ROOT=/opt/xdaq/htdocs XDAQ_PLATFORM=x86_slc4 XDAQ_OS=linux XDAQ_ZONE=emu904 ROOTSYS=/home/cscdev/root</xsl:param>

  <xsl:output method="xml" indent="yes"/>

  <!-- Generate DUCK files for EmuDAQ and EmuDQM -->
  <xsl:template match="RUI-to-computer_mapping">
    <xsl:comment>Generated by EmuDAQ904DUCKGenerator.xsl from RUI-to-computer_mapping_bdg904.xml of <xsl:value-of select="@date"/></xsl:comment>
    <Configuration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" user="cscdev" path="{$FM_CONFIG_PATH}">
      <FunctionManager name="config" hostname="emuslice02" port="12000"
		       qualifiedResourceType="rcms.fm.resource.qualifiedresource.FunctionManager"
		       sourceURL="http://emuslice02.cern.ch:12000/functionmanagers/cscLevelOneFM.jar"
		       className="rcms.fm.app.cscLevelOne.MyFunctionManager"
		       role="CSC" >
	<xsl:call-template name="JobControls"/>
	<xsl:if test="$FARM='DAQ'">
	  <xsl:call-template name="DAQManager"/>
	  <xsl:call-template name="EVM_and_TA"/>
<!-- 	  <xsl:if test="$SIDE!='M'"> -->
<!-- 	    <xsl:call-template name="TF"/> -->
<!-- 	  </xsl:if> -->
	  <xsl:call-template name="RUIs"/>
	</xsl:if>
	<xsl:if test="$FARM='DQM'">
	  <xsl:call-template name="Monitors"/>
	</xsl:if>
	<xsl:if test="$FARM='DQM_Display'">
	  <xsl:call-template name="DisplayClient"/>
	</xsl:if>
      </FunctionManager>
    </Configuration>
  </xsl:template>

  <!-- emu::daq::manager -->
  <xsl:template name="DAQManager">
    <xsl:comment>emu::daq::manager</xsl:comment>
    <xsl:variable name="HTTP_PORT">20200</xsl:variable>
    <XdaqExecutive hostname="emudaq02.cern.ch" port="{$HTTP_PORT}"
		   urn="urn:xdaq-application:lid=0"
		   qualifiedResourceType="rcms.fm.resource.qualifiedresource.XdaqExecutive"
		   instance="0"
		   pathToExecutive="{$PATHTOEXECUTIVE}"
		   unixUser="cscdaq"
		   logLevel="WARN"
		   logURL="file:/tmp/xdaq-daqmanager-cscdaq.log"
		   environmentString="{$DAQ_ENVIRONMENTSTRING}">
      <configFile location="file"><xsl:value-of select="$CONFIG_FILE"/></configFile>
    </XdaqExecutive>
    <XdaqApplication className="emu::daq::manager::Application" hostname="emudaq02.cern.ch" port="{$HTTP_PORT}"
		     urn="urn:xdaq-application:lid=12"
		     qualifiedResourceType="rcms.fm.resource.qualifiedresource.XdaqApplication"
		     instance="0" />
  </xsl:template>

  <!-- EVM and EmuTA -->
  <xsl:template name="EVM_and_TA">
    <xsl:comment >EVM and EmuTA</xsl:comment>
    <xsl:variable name="HTTP_PORT">20201</xsl:variable>
    <XdaqExecutive hostname="emudaq02.cern.ch" port="{$HTTP_PORT}"
		   urn="urn:xdaq-application:lid=0"
		   qualifiedResourceType="rcms.fm.resource.qualifiedresource.XdaqExecutive"
		   instance="0"
		   pathToExecutive="{$PATHTOEXECUTIVE}"
		   unixUser="cscdaq"
		   logLevel="WARN"
		   logURL="file:/tmp/xdaq-evm_ta-cscdaq.log"
		   environmentString="{$DAQ_ENVIRONMENTSTRING}">
      <configFile location="file"><xsl:value-of select="$CONFIG_FILE"/></configFile>
    </XdaqExecutive>
  </xsl:template>

  <!-- Track Finder's RUI -->
  <xsl:template name="TF">
    <xsl:comment >RUI 0 (TF)</xsl:comment>
    <XdaqExecutive hostname="{//RUI[@instance='0']/../@alias}" port="{//RUI[@instance='0']/@port}"
		   urn="urn:xdaq-application:lid=0"
		   qualifiedResourceType="rcms.fm.resource.qualifiedresource.XdaqExecutive"
		   instance="0"
		   pathToExecutive="{$PATHTOEXECUTIVE}"
		   unixUser="cscdaq"
		   logLevel="WARN"
		   logURL="file:/tmp/xdaq-rui0-cscdaq.log"
		   environmentString="{$DAQ_ENVIRONMENTSTRING}">
      <configFile location="file"><xsl:value-of select="$CONFIG_FILE"/></configFile>
    </XdaqExecutive>
  </xsl:template>

  <!-- RUIs -->
  <xsl:template name="RUIs">
    <xsl:for-each select="//RUI[@instance!='0']">
<!--       <xsl:if test="($SIDE!='P' and $SIDE!='M') or $SIDE='B' or ($SIDE='P' and number(@instance)&lt;=18) or ($SIDE='M' and number(@instance)&gt;18)"> -->
	
	<xsl:comment >RUI <xsl:value-of select="@instance"/></xsl:comment>
	<XdaqExecutive hostname="{../@alias}" port="{@port}"
		       urn="urn:xdaq-application:lid=0"
		       qualifiedResourceType="rcms.fm.resource.qualifiedresource.XdaqExecutive"
		       instance="0"
		       pathToExecutive="{$PATHTOEXECUTIVE}"
		       unixUser="cscdaq"
		       logLevel="WARN"
		       logURL="file:/tmp/xdaq-rui{@instance}-cscdaq.log"
		       environmentString="{$DAQ_ENVIRONMENTSTRING}">
	  <configFile location="file"><xsl:value-of select="$CONFIG_FILE"/></configFile>
	</XdaqExecutive>

<!--       </xsl:if> -->
    </xsl:for-each>
  </xsl:template>

  <!-- DQM Display Client -->
  <xsl:template name="DisplayClient">
    <xsl:comment >DQM Display Client</xsl:comment>
    <XdaqExecutive hostname="emudaq02.cern.ch" port="20550"
		   urn="urn:xdaq-application:lid=0"
		   qualifiedResourceType="rcms.fm.resource.qualifiedresource.XdaqExecutive"
		   instance="0"
		   pathToExecutive="{$PATHTOEXECUTIVE}"
		   unixUser="cscdev"
		   logLevel="INFO"
		   logURL="file:/tmp/xdaq-dqmdisplay-cscdev.log"
		   environmentString="{$DQM_ENVIRONMENTSTRING}">
      <configFile location="file"><xsl:value-of select="$CONFIG_FILE"/></configFile>
    </XdaqExecutive>

  </xsl:template>

  <!-- DQM Monitors -->
  <xsl:template name="Monitors">
    <xsl:for-each select="//RUI">
<!--       <xsl:if test="($SIDE!='P' and $SIDE!='M') or $SIDE='B' or ($SIDE='P' and number(@instance)&lt;=18) or ($SIDE='M' and number(@instance)&gt;18)"> -->

	<xsl:variable name="PORT"><xsl:value-of select="20500+number(@instance)"/></xsl:variable>
	<xsl:comment >DQM Monitor <xsl:value-of select="@instance"/></xsl:comment>
	<XdaqExecutive hostname="{../@alias}" port="{$PORT}"
		       urn="urn:xdaq-application:lid=0"
		       qualifiedResourceType="rcms.fm.resource.qualifiedresource.XdaqExecutive"
		       instance="0"
		       pathToExecutive="{$PATHTOEXECUTIVE}"
		       unixUser="cscdev"
		       logLevel="WARN"
		       logURL="file:/tmp/xdaq-dqmmonitor{@instance}-cscdev.log"
		       environmentString="{$DQM_ENVIRONMENTSTRING}">
	  <configFile location="file"><xsl:value-of select="$CONFIG_FILE"/></configFile>
	</XdaqExecutive>

<!--       </xsl:if>	 -->
    </xsl:for-each>
  </xsl:template>

  <!-- JobControls -->
  <xsl:template name="JobControls">
    <xsl:comment>JobControls</xsl:comment>
    <xsl:for-each select="//RUI[not(../@alias = preceding::RUI/../@alias) and not(string-length(../@alias)=0) and not(../@alias = document('')//XdaqExecutive/@hostname)]">
      <Service name="JobControl" hostname="{../@alias}" port="9999" urn="urn:xdaq-application:lid=10" qualifiedResourceType="rcms.fm.resource.qualifiedresource.JobControl"/>
    </xsl:for-each>
    <xsl:for-each select="document('')//XdaqExecutive[not(@hostname = preceding::XdaqExecutive/@hostname)]">
      <xsl:if test="not(contains(@hostname,'{'))">
      <Service name="JobControl" hostname="{@hostname}" port="9999" urn="urn:xdaq-application:lid=10" qualifiedResourceType="rcms.fm.resource.qualifiedresource.JobControl"/>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

</xsl:transform>
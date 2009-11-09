<%@ page language="java" contentType="text/html"%>
<%@ page import="java.util.*"%>
<%@ page import="rcms.gui.servlet.pilot.FMPilotConstants"%>
<%@ page isELIgnored="false"%>

<%@ taglib prefix="rcms.menu" uri="../tld/menu.tld"%>
<%@ taglib prefix="rcms.control" uri="../tld/control.tld"%>
<%@ taglib prefix="rcms.globalParameter" uri="../tld/globalParameter.tld"%>
<%@ taglib prefix="rcms.notification" uri="../tld/notification.tld"%>

<!-- Optional Section to set the visibility of available commands at a given state begin -->
<rcms.control:menuCreator />
<html>

<head>
<meta Http-Equiv="Cache-Control" Content="no-cache">
<meta Http-Equiv="Pragma" Content="no-cache">
<meta Http-Equiv="Expires" Content="0">

<TITLE>"CSC Function Manager"</TITLE>

<link rel="StyleSheet" href="../css/common.css" type="text/css" />
<link rel="StyleSheet" href="../css/control.css" type="text/css" />

<rcms.control:customResourceRenderer indentation="1" type="css" path="/css/myControlPanel.css" />
<rcms.control:customResourceRenderer indentation="1" type="js" path="/js/myControl.js" />
<rcms.control:customResourceRenderer indentation="1" type="js" path="/js/ajaxRequest.js" />

<script type="text/javascript" src="../js/stateNotification.js"></script>
<script type="text/javascript" src="../js/common.js"></script>
<script type="text/javascript" src="../js/globalParameters.js"></script>
<script type="text/javascript" src="../js/control.js"></script>
<script type="text/javascript" src="../yui/yahoo-dom-event/yahoo-dom-event.js"></script>
<script type="text/javascript" src="../yui/animation/animation-min.js"></script>

<!-- Custom javascript section begin -->
<script type="text/javascript">
		<rcms.control:onLoadJSRenderer 
		reloadOnStateChange="false" 
		commandButtonCssClass="MyControlButton" 
		commandParameterCheckBoxTitle="&nbsp;Show Command Parameter Section"
		commandParameterCssClass="label_left_black" 
		indentation="2"/>
		
		<rcms.control:buttonsJSRenderer indentation="2"/>
		<rcms.notification:jSRenderer indentation="2"/>
		<rcms.globalParameter:jSRenderer indentation="2"/>
</script>

<!-- Custom javascript section end -->
</head>
<body onLoad="myInit();" class="body">
<!--  switch back to a bigger font size -->
<font size=3> <!-- Table T1 begin -->

<p>
<rcms.globalParameter:getParameterMap fmParameterContainer="pars" />
</p>

<!--  conditional dummy span for sound support -->
<script type="text/javascript"></script>

<div class="ParaTableName"></div>	
<form name="FMPilotForm" id="FMPilotForm" method="POST" action="../../gui/servlet/FMPilotServlet?PAGE=/gui/jsp/controlPanel.jsp">
	
	<input type="hidden" id="globalParameterName3 "name="globalParameterName3" value="" />
	<input type="hidden" id="globalParameterValue3" name="globalParameterValue3" value="" />
	<input type="hidden" id="globalParameterType3" name="globalParameterType3" value="" />
	
	<rcms.control:actionHiddenInputRenderer indentation="4" /> 
	<rcms.control:commandHiddenInputRenderer indentation="4" /> 
	<rcms.notification:hiddenInputRenderer indentation="4" /> 

<TABLE class="HeaderTable" BORDER="4" CELLPADDING="2" CELLSPACING="2" WIDTH="100%">

	<tr>
	<TD bgcolor="#000000">
	<button id="showStatusTableButton" class="MenuButton"
			value="Status Table" name="Status Table"
			onClick="onShowStatusTableButton()">Status Table </button>
	</TD>

	<td><a class="MenuLinkEnabled" href="./DiagnosticServlet"><B>Diagnostic Page</B></a></td>
        <td><a class="MenuLinkEnabled" href="../../../Collector/Collector"><B>Logging Collector</B></a></td>
        <td><a class="MenuLinkEnabled" href="./MonitoringToolsServlet"><B>Monitoring Tools</B></a></td>
        <td><a class="MenuLinkEnabled" href="./RunInfoServlet"><B>Run Info</B></a></td>
        <td><a class="MenuLinkEnabled" href="./RunningConfigurationServlet"><B>Running Configurations</B></a></td>
        <td><a class="MenuLinkEnabled" href="./LogoutServlet"><B>Logout</B></a></td>
	
	<!--  REFRESH BUTTON --> 
	<TD bgcolor="#696969">
	<rcms.control:refreshButtonRenderer
		cssClass="button1" onClickFunction="onUpdatedRefreshButton()"
		name="Refresh" indentation="10" />
	</TD>
	
	<TD bgcolor="#4682B4">
	<rcms.control:createButtonRenderer cssClass="button1"
	onClickFunction="onCreateButton()" name="Create" indentation="10" /> 
	<rcms.control:attachButtonRenderer cssClass="button1" 
	onClickFunction="onAttachButton()" name="Attach"indentation="10" /> 
	<rcms.control:detachButtonRenderer cssClass="button1" 
	onClickFunction="onDetachButton()" name="Detach" indentation="10" /> 
	<rcms.control:destroyButtonRenderer cssClass="button1" 
	onClickFunction="onMyDestroyButton()" name="Destroy" indentation="10" />
	</TD>
	</tr>

</TABLE>
<br>
<TABLE class="paraTableName" BORDERCOLOR="black" BORDER="4" CELLPADDING="2" CELLSPACING="2" WIDTH="70%" align="center">
<tr>
<TD align="center" bgcolor="#000000" class="paraTableName" >
<rcms.control:stateRenderer titleClass="control_label1" label="<B>State<B>:&nbsp;" contentClass="control_labe13" indentation="10"/></TD>
</tr>
</TABLE>
<TABLE style="width: 70%;" align="center">
	<tr>
		<td  bgcolor="#B0C4DE" align="center">
		<div id="commandSection">
			<rcms.control:commandButtonsRenderer cssClass="button1" indentation="11"/>
		</div>
		<br>
		<div id="commandParameterCheckBoxSection" class="control_label1">
			<rcms.control:commandParameterCheckboxRenderer title="&nbsp;Show Command Parameter Section" indentation="11"/>
		</div>
	</td>
	</tr>
	
</TABLE>	

<table style="width: 1190px;">
<tr>
	
	<!--  INFO TABLE -->
	<td bgcolor="#DCDCDC">
	<table style="color: #FFFFFF; width: 700px; background-color: #87CEFA;">

	        <tr>
		<TD colspan="2" class="paraTableName">
			<rcms.control:configurationPathRenderer
				titleClass="control_label1" label="Configuration : &nbsp;"
				contentClass="control_label2" indentation="10" />
	
			<rcms.control:configurationKeyRenderer titleClass="control_label1"
				hidden="true" label="Configuration Keys:&nbsp;"
				contentClass="control_label2" indentation="10" />
		</TD>
		</tr>
		
		<tr>
			<td style="width: 120px;" class="paraTableName">SID</td>
			<td style="width: 400px;" class="paraTableValue" id="SID">${pars.SID}</td>
		</tr>

		<tr>
			<td style="width: 120px;" class="paraTableName">Run Number</td>
			<td style="width: 400px;" class="paraTableValue" id="RUN_NUMBER">${pars.RUN_NUMBER}</td>
		</tr>

		<tr>
			<td style="width: 120px;" class="paraTableName">STATE</td>
			<td style="width: 400px;" class="paraTableValue" id="STATE">${pars.STATE}</td>
		</tr>
	
		<tr>
			<td style="width: 110px;"  class="paraTableName">Global Key</td>
			<td style="width: 400px;" class="paraTableValue" id="GLOBAL_CONF_KEY">${pars.GLOBAL_CONF_KEY}</td>
		</tr>

		<tr>
			<td style="width: 110px;"  class="paraTableName">HWCFG Key</td>
			<td style="width: 400px;" class="paraTableValue" id="HWCFG_KEY">${pars.HWCFG_KEY}</td>
		</tr>
	
		<tr>
			<td style="width: 110px;"  class="paraTableName">Action</td>
			<td style="width: 400px;" class="paraTableValue" id="ACTION_MSG">${pars.ACTION_MSG}</td>
		</tr>	
		<tr>
			<td style="width: 110px;"  class="paraTableName">Error</td>
			<td style="width: 400px;" class="paraTableValue" id="ERROR_MSG">${pars.ERROR_MSG}</td>
		</tr>
		</td>

	</table>
	</td>
	
<td class="TitleMargin"><rcms.control:customResourceRenderer indentation="1" type="img" 
	path="/img/YE1.gif" align="middle" width="300"  height="250"  border="0" htmlId="YE1_left"/></td>
</tr>	
</table>

	<!--  SUBDET PANEL -->

	<div style="border: 1px solid white; bgcolor: green;">${pars.GUI_SUBDET_PANEL_HTML}</div>
</form>
	
	<div style="color: black; font-family: monospace;">

	<script type="text/javascript">
	<!--
	document.write(' ' + BrowserDetect.browser + ' ' + BrowserDetect.version + ' ' + BrowserDetect.OS );
	// -->
	</script>
	
</body>
</html>

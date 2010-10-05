<%@ page session="true" contentType="text/html; charset=ISO-8859-1" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jstl/core" %>

<html>
<head>
  <title>Run Regsitry OLAP is loading ...</title>
  <meta http-equiv="refresh" content="1; URL=<c:out value="${requestSynchronizer.resultURI}"/>">
</head>
<body style="background-color:#E9F0F2">
    <center>
  <h2>Run Registry OLAP is loading ...</h2>

  Please wait until your results are ready to display.
  <p>Click
  <a href="<c:out value="${requestSynchronizer.resultURI}"/>">here</a>
  if your browser does not support redirects.
    </center>
</body>
</html>

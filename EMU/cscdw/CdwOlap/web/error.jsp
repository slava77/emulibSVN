<%@page contentType="text/html" pageEncoding="UTF-8" isErrorPage="true"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
    "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>Error!</title>
    </head>
    <body style="background-color:#E9F0F2">

        <center>
            <h1 style="color:red">Error!</h1>
            <a onclick="javascript: window.parent.location.href = window.parent.location.href"> Click here to return. </a>
            <%
                if (session.isNew() == false) {
                    session.invalidate();
                }
                out.println("<p>" + exception.getMessage() + "<p>");
            %>
        </center>
        <%exception.printStackTrace(new java.io.PrintWriter(out));%>


    </body>
</html>

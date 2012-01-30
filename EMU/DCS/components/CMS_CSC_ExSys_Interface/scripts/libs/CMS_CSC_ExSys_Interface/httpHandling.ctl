/*
#uses "http.ctl"
#uses "proj_http.ctl"
#uses "as.ctl"
#uses "es.ctl"
#uses "dpGroups.ctl"
  */
#uses "CtrlHTTP"
#uses "CMS_CSC_common/emu_common.ctl"

private const string EXSYS_HOST = "csc-expert";
private const string EXSYS_PORT = 8080;
private const string EXSYS_URL = "/ExsysGui/factcollectioninput";

private global int exsys_g_socket_handler = -1;

/** Closes connection with ExSys. */
private void exsys_closeConnection() {
  dyn_string ex;
  int socket = exsys_getSocket(ex, false);
  if (emu_checkException(ex)) {
    emu_errorHandled(ex, "exsys_closeConnection()");
    return;
  }
  if (socket == -1) {
    return;
  }
  int ret = tcpClose(socket);
  exsys_g_socket_handler = -1;
  if (ret != 0) {
    dyn_errClass err = getLastError();
    emu_info("ExSys: Error closing socket: " + err);
  } else if (ret == 0) {
    emu_info("ExSys: Connetion to the Expert System closed successfully");
  }
}

/** Does an HTTP post to expert system. */
public void exsys_httpPost(string data, dyn_string &exceptionInfo) {
  string header = "POST " + EXSYS_URL + " HTTP/1.1\n" +
                  "User-Agent: PVSS\n" +
                  "Host: " + EXSYS_HOST + "\n" +
                  "Content-Type: text/xml\n" +
                  "Connection: Keep-Alive\n" +
                  "Content-Length: " + strlen(data);
  data = header + "\n\n" + data;
  
  int socketHdl = exsys_tcpWrite(data, exceptionInfo);
  if (emu_checkException(exceptionInfo)) { // try to close the socket and reopen it
    emu_errorHandled(exceptionInfo, "exsys_httpPost");
    exsys_closeConnection();
    socketHdl = exsys_tcpWrite(data, exceptionInfo);
  }
  if (emu_checkException(exceptionInfo)) { return; }
  
  // read the response from Expert System
  string responseData;
  time maxTimeout = 1000;
  tcpRead(socketHdl, responseData, maxTimeout);
  int okPos = strpos(responseData, "OK:");
  if (okPos >= 0) {
    int numSavedPos = okPos + 3;
    int savedFactsCount = substr(responseData, numSavedPos, strlen(responseData) - numSavedPos);
    emu_debug("Exsys: Expert System response OK, number of facts saved: " + savedFactsCount);
  } else {
    if (strlen(responseData) > 5) {
      emu_addError("Exsys: unexpected Expert System response: \"" + responseData + "\"", exceptionInfo);
    }
  }
  exsys_closeConnection();
}

/** 
  * Calls exsys_getSocket to acquire a socket and then writes the provided data to it.
  * @return socket handler that was used to send the data.
  */
private int exsys_tcpWrite(string data, dyn_string &exceptionInfo) {
  dyn_errClass err;
  int socketHdl = exsys_getSocket(exceptionInfo);
  if (emu_checkException(exceptionInfo)) { return -1; }
  if (socketHdl == -1) {
    emu_addError("ExSys: exsys_getSocket returned socket handler = -1", exceptionInfo);
    return socketHdl;
  }

  int ret = tcpWrite(socketHdl, data);
  dyn_errClass err=getLastError();
  if ((dynlen(err) > 0) || (ret < 0)) {
    emu_addError("ExSys: Error while writing to socket: " + err, exceptionInfo);
  }
  
  return socketHdl;
}

/** 
  * Opens a connection to the Expert System.
  * @return socket handler.
  */
private int exsys_getSocket(dyn_string &exceptionInfo, bool openIfClosed = true) {
  if ((exsys_g_socket_handler == -1) && openIfClosed) {
    exsys_g_socket_handler = _exsys_openSocket(EXSYS_HOST, EXSYS_PORT, exceptionInfo);
  }
  return exsys_g_socket_handler;
}

/** Opens TCP/IP connection and returns the socket handler. If there's an error -1 is returned. */
private int _exsys_openSocket(string host, unsigned port, dyn_string &exceptionInfo) {
  dyn_errClass err;
  int socket;
  socket = tcpOpen(host, port);
  err = getLastError();
  if (dynlen(err) > 0) {
    emu_addError("ExSys: Error opening connection to " + host + ":" + port + ": " + err, exceptionInfo);
    emu_info("ExSys: Trying to close the connection...");
    tcpClose(socket);
    return -1;
  } else {
    emu_info("ExSys Info: Connection to " + host + ":" + port + " has been established successfully");
  }
  return socket;
}

#uses "CtrlXmlRpc"

main() {
  string id = "CSC_Service_Bus";
  string func = "DcsTestHttpOperation";
  dyn_mixed args = makeDynMixed("agi");
  mixed res;
  string host = "localhost";
  int port = "1234";
  bool secure = FALSE;
  xmlrpcClient();
  xmlrpcConnectToServer(id, host, port, secure);
  DebugTN("calling function '" + func + "on server " + host + ":" + port);
  xmlrpcCall(id, func, args, res);
  DebugTN("call finished");
  DebugN("Result of XmlRpc call", res);
  xmlrpcCloseServer(id);
  DebugTN("disconnected");
}

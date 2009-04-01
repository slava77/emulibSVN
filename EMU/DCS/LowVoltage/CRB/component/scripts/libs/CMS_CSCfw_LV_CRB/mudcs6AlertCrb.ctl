mudcsAlertCrbMain()
{


  bool isAck;


  mudcsCrbPnlInit();
  isAck = CSC_fwCAN1_g_ISACK;
  mudcsCrbAiAlertSet(true, isAck);
  mudcsCrbElmbAlertSet(true, isAck);
  mudcsCrbErrorStatusAlertSet(true, isAck);
  mudcsCrbSummaryAlertSet(true);
//mudcsLvDebug("Done");

}


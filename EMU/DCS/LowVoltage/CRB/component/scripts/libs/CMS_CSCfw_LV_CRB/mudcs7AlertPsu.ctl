mudcsAlertPsuMain()
{


  bool isAck;


  mudcsCrbPnlInit();
  isAck = CSC_fwCAN1_g_ISACK;
  mudcsPsuAiAlertSet(true, isAck);
  mudcsCrbElmbAlertSet(true, isAck); // the same is performed in the CRB alert config (double work)
  mudcsPsuFsmAlertSet(true, isAck);
  mudcsPsuSummaryAlertSet(true);
//mudcsLvDebug("Done");

}


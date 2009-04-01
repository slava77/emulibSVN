mudcsAlertMrtnMain()
{


  bool isAck;


  mudcsMrtnPnlInit();
  isAck = CSC_fwCAN2_g_ISACK;
  mudcsMrtnCrateAlertSet(true, isAck);
  mudcsMrtnChannelAlertSet(true, isAck);
  mudcsMrtnSummaryAlertSet(true);

  mudcsPL500CrateAlertSet(true, isAck);
  mudcsPL500ChannelAlertSet(true, isAck);
  mudcsPL500SummaryAlertSet(true);

//  mudcsLvDebug("Done");
}


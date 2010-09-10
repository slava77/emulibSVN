mudcsCrbDbCreateMain(){
int i;


mudcsCrbPnlInit(0);
mudcsCrb_create_pcmb_database_type();
dpCreate("db_pcmb","Db_PCMB");
dpCreate("pcmb_mrtn","Db_PCMB");
dpCreate("db_pcmb_geographic","Db_PCMB");
mudcsCrb_fill_pcmb_database();

 //mudcsLvDebug("Done");
}
//============================

mudcsCrb_create_pcmb_database_type()
{
string dtype;
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

dtype="Db_PCMB";

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","list");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_DYN_STRING);

dpTypeCreate(xxdepes,xxdepei);
}
//============================
mudcsCrb_fill_pcmb_database(){

  dyn_string db,db_geog;
 if(CSC_fwCAN1_g_DEV_MODE){ // corr++
dynAppend(db,"CR1_PSU;00;3f"); dynAppend(db_geog,"n.a.");// HW crate number and CAN address   
dynAppend(db,"DEV_PC1;13"); dynAppend(db_geog,"DEV Bld"); // 37 
dynAppend(db,"DEV_PC2;02"); dynAppend(db_geog,"DEV Bld");// 38   
   
 }
 else {
dynAppend(db,"CR1_PSU;00;3f"); dynAppend(db_geog,"n.a.");// HW crate number and CAN address 
// 37 38 39 3A 3B 3C
dynAppend(db,"M4_PC1;37"); dynAppend(db_geog,"X5E51/bot;1"); // 37 
dynAppend(db,"M4_PC2;38"); dynAppend(db_geog,"X5L51/bot;2");// 38
dynAppend(db,"M4_PC3;39"); dynAppend(db_geog,"X3S51/top;3");// 39
dynAppend(db,"M4_PC4;3a"); dynAppend(db_geog,"X1L51/top;4");// 3a
dynAppend(db,"M4_PC5;3b"); dynAppend(db_geog,"X1E51/bot;5");// 3b
dynAppend(db,"M4_PC6;3c"); dynAppend(db_geog,"X3V51/bot;6");// 3c
// 31 32 33 34 35 36
dynAppend(db,"M3_PC1;31"); dynAppend(db_geog,"X5E41/bot;1");// 31
dynAppend(db,"M3_PC2;32"); dynAppend(db_geog,"X5L41/bot;2");// 32
dynAppend(db,"M3_PC3;33"); dynAppend(db_geog,"X3S41/top;3");// 33// 15
dynAppend(db,"M3_PC4;34"); dynAppend(db_geog,"X1L41/top;4");// 34
dynAppend(db,"M3_PC5;35"); dynAppend(db_geog,"X1E41/bot;5");// 35
dynAppend(db,"M3_PC6;36"); dynAppend(db_geog,"X3V41/bot;6");// 36
//2B 2C 2D 2E 2F 30
dynAppend(db,"M2_PC1;2b"); dynAppend(db_geog,"X5E41/top;1");// 2b
dynAppend(db,"M2_PC2;2c"); dynAppend(db_geog,"X5L41/top;2");// 2c
dynAppend(db,"M2_PC3;2d"); dynAppend(db_geog,"X3S41/bot;3");// 2d //  0f
dynAppend(db,"M2_PC4;2e"); dynAppend(db_geog,"X1L41/bot;4");// 2e
dynAppend(db,"M2_PC5;2f"); dynAppend(db_geog,"X1E41/top;5");// 2f
dynAppend(db,"M2_PC6;30"); dynAppend(db_geog,"X3V41/top;6");// 30

//20 21 | 23 22 | 25 24 | 27 26 | 28 29 | 2A 1F
dynAppend(db,"M1_PC1A;20"); dynAppend(db_geog,"X5E31/bot;2");// 20 // phi 2
dynAppend(db,"M1_PC1B;21"); dynAppend(db_geog,"X5E31/top;3");// 21 // phi 3
dynAppend(db,"M1_PC2A;23"); dynAppend(db_geog,"X5L31/bot;5");// 23 // phi 5
dynAppend(db,"M1_PC2B;22"); dynAppend(db_geog,"X5L31/top;4");// 22 // phi 4
dynAppend(db,"M1_PC3A;25"); dynAppend(db_geog,"X3S31/bot;7");// 25 // phi 7
dynAppend(db,"M1_PC3B;24"); dynAppend(db_geog,"X3S31/top;6");// 24 // phi 6
dynAppend(db,"M1_PC4A;27"); dynAppend(db_geog,"X1L31/bot;9");// 27 // phi 9
dynAppend(db,"M1_PC4B;26"); dynAppend(db_geog,"X1L31/top;8");// 26 // phi 8
dynAppend(db,"M1_PC5A;28"); dynAppend(db_geog,"X1E31/bot;10");// 28 // phi 10
dynAppend(db,"M1_PC5B;29"); dynAppend(db_geog,"X1E31/top;11");// 29 // phi 11
dynAppend(db,"M1_PC6A;2a"); dynAppend(db_geog,"X3V31/bot;12");// 2a // phi 12
dynAppend(db,"M1_PC6B;1f"); dynAppend(db_geog,"X3V31/top;1");// 1f // phi 1

//2 3 | 5 4 | 7 6 | 9 8 | A B | C 1
dynAppend(db,"P1_PC1A;02"); dynAppend(db_geog,"X5U31/bot;2");// 02  //sec   2
dynAppend(db,"P1_PC1B;03"); dynAppend(db_geog,"X5U31/top;3");// 03  //sec   3
dynAppend(db,"P1_PC2A;05"); dynAppend(db_geog,"X5R31/bot;5");//// 05  //sec 5
dynAppend(db,"P1_PC2B;04"); dynAppend(db_geog,"X5R31/top;4");//// 04  //sec 4
dynAppend(db,"P1_PC3A;07"); dynAppend(db_geog,"X3A31/bot;7");//// 07  //sec 7
dynAppend(db,"P1_PC3B;06"); dynAppend(db_geog,"X3A31/top;6");//// 06  //sec 6
dynAppend(db,"P1_PC4A;09"); dynAppend(db_geog,"X1R31/bot;9");//// 09  //sec 9
dynAppend(db,"P1_PC4B;08"); dynAppend(db_geog,"X1R31/top;8");//// 08  //sec 8
dynAppend(db,"P1_PC5A;0a"); dynAppend(db_geog,"X1U31/bot;10");// 0a  //sec   10
dynAppend(db,"P1_PC5B;0b"); dynAppend(db_geog,"X1U31/top;11");// 0b  //sec5  11
dynAppend(db,"P1_PC6A;0c"); dynAppend(db_geog,"X3J31/bot;12");// 0c   //sec6 12
dynAppend(db,"P1_PC6B;01"); dynAppend(db_geog,"X3J31/top;1");// 01  //sec6  1

//D E F 10 11 12
dynAppend(db,"P2_PC1;0d"); dynAppend(db_geog,"X5U41/top;1");// 0d  //sec1
dynAppend(db,"P2_PC2;0e"); dynAppend(db_geog,"X5R41/top;2");//// 0e  //sec2
dynAppend(db,"P2_PC3;0f"); dynAppend(db_geog,"X3A41/top;3");//// 0f  //sec3
dynAppend(db,"P2_PC4;10"); dynAppend(db_geog,"X1R41/bot;4");//// 10  //sec4
dynAppend(db,"P2_PC5;11"); dynAppend(db_geog,"X1U41/top;5");// 11  //sec5
dynAppend(db,"P2_PC6;12"); dynAppend(db_geog,"X3J41/bot;6");// 12  //sec6

//13 14 15 16 17 18
dynAppend(db,"P3_PC1;13"); dynAppend(db_geog,"X5U41/bot;1");// 13  //sec1
dynAppend(db,"P3_PC2;14"); dynAppend(db_geog,"X5R41/bot;2");//// 14  //sec2
dynAppend(db,"P3_PC3;15"); dynAppend(db_geog,"X3A41/bot;3");//// 15  //sec3
dynAppend(db,"P3_PC4;16"); dynAppend(db_geog,"X1R41/top;4");//// 16  //sec4
dynAppend(db,"P3_PC5;17"); dynAppend(db_geog,"X1U41/bot;5");// 17  //sec5
dynAppend(db,"P3_PC6;18"); dynAppend(db_geog,"X3J41/top;6");// 18  //sec6

//19 1A 1B 1C 1D 1E
dynAppend(db,"P4_PC1;19"); dynAppend(db_geog,"X5U51/bot;1");// 19   //sec1
dynAppend(db,"P4_PC2;1a"); dynAppend(db_geog,"X5R51/bot;2");//// 1a  //sec2
dynAppend(db,"P4_PC3;1b"); dynAppend(db_geog,"X3A51/top;3");//// 1b  //sec3
dynAppend(db,"P4_PC4;1c"); dynAppend(db_geog,"X1R51/top;4");//// 1c  //sec4
dynAppend(db,"P4_PC5;1d"); dynAppend(db_geog,"X1U51/bot;5");// 1d  //sec5
dynAppend(db,"P4_PC6;1e"); dynAppend(db_geog,"X3J51/bot;6");// 1e  //sec6
}
dpSetWait("db_pcmb.list",db);
dpSetWait("db_pcmb_geographic.list",db_geog);
  
//==============================================
dyn_string pcmb_mrtn;
//ME-4
dynAppend(pcmb_mrtn,"55;38");
dynAppend(pcmb_mrtn,"56;37");
dynAppend(pcmb_mrtn,"57;37");
dynAppend(pcmb_mrtn,"58;37");
dynAppend(pcmb_mrtn,"59;38");
dynAppend(pcmb_mrtn,"60;38");
//ME-3
dynAppend(pcmb_mrtn,"49;29");
dynAppend(pcmb_mrtn,"50;32");
dynAppend(pcmb_mrtn,"51;34");
dynAppend(pcmb_mrtn,"52;34");
dynAppend(pcmb_mrtn,"53;35");
dynAppend(pcmb_mrtn,"54;29");
//ME-2
dynAppend(pcmb_mrtn,"43;30");
dynAppend(pcmb_mrtn,"44;31");
dynAppend(pcmb_mrtn,"45;33");
dynAppend(pcmb_mrtn,"46;34");
dynAppend(pcmb_mrtn,"47;36");
dynAppend(pcmb_mrtn,"48;29");
//ME-1
dynAppend(pcmb_mrtn,"32;21");
dynAppend(pcmb_mrtn,"33;22");
dynAppend(pcmb_mrtn,"35;24");
dynAppend(pcmb_mrtn,"34;23");
dynAppend(pcmb_mrtn,"37;26");
dynAppend(pcmb_mrtn,"36;25");
dynAppend(pcmb_mrtn,"39;26");
dynAppend(pcmb_mrtn,"38;26");
dynAppend(pcmb_mrtn,"40;27");
dynAppend(pcmb_mrtn,"41;28");
dynAppend(pcmb_mrtn,"42;21");
dynAppend(pcmb_mrtn,"31;21");
//======

//ME+1
dynAppend(pcmb_mrtn,"2;1");
dynAppend(pcmb_mrtn,"3;2");
dynAppend(pcmb_mrtn,"5;4");
dynAppend(pcmb_mrtn,"4;3");
dynAppend(pcmb_mrtn,"7;6");
dynAppend(pcmb_mrtn,"6;5");
dynAppend(pcmb_mrtn,"9;6");
dynAppend(pcmb_mrtn,"8;6");
dynAppend(pcmb_mrtn,"10;7");
dynAppend(pcmb_mrtn,"11;8");
dynAppend(pcmb_mrtn,"12;1");
dynAppend(pcmb_mrtn,"1;1");

//ME+2
dynAppend(pcmb_mrtn,"13;10");
dynAppend(pcmb_mrtn,"14;11");
dynAppend(pcmb_mrtn,"15;13");
dynAppend(pcmb_mrtn,"16;14");
dynAppend(pcmb_mrtn,"17;16");
dynAppend(pcmb_mrtn,"18;9");
//ME+3
dynAppend(pcmb_mrtn,"19;9");
dynAppend(pcmb_mrtn,"20;12");
dynAppend(pcmb_mrtn,"21;14");
dynAppend(pcmb_mrtn,"22;14");
dynAppend(pcmb_mrtn,"23;15");
dynAppend(pcmb_mrtn,"24;9");
//ME+4
dynAppend(pcmb_mrtn,"25;18");
dynAppend(pcmb_mrtn,"26;19");// "26;17"
dynAppend(pcmb_mrtn,"27;17"); //
dynAppend(pcmb_mrtn,"28;17"); //
dynAppend(pcmb_mrtn,"29;18");
dynAppend(pcmb_mrtn,"30;18");


dpSetWait("pcmb_mrtn.list",pcmb_mrtn);



  


}
//============================

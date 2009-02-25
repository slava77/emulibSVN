mudcsCrbDbCreateMain(){
int i;


mudcsCrbPnlInit(0);
mudcsCrb_create_pcmb_database_type();
dpCreate("db_pcmb","Db_PCMB");
dpCreate("pcmb_mrtn","Db_PCMB");
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

  dyn_string db;
  
dynAppend(db,"CR1_PSU;00;3f"); // HW crate number and CAN address 
// 37 38 39 3A 3B 3C
dynAppend(db,"M4_PC1;37"); // 37
dynAppend(db,"M4_PC2;38"); // 38
dynAppend(db,"M4_PC3;39"); // 39
dynAppend(db,"M4_PC4;3a"); // 3a
dynAppend(db,"M4_PC5;3b"); // 3b
dynAppend(db,"M4_PC6;3c"); // 3c
// 31 32 33 34 35 36
dynAppend(db,"M3_PC1;31"); // 31
dynAppend(db,"M3_PC2;32"); // 32
dynAppend(db,"M3_PC3;33"); // 33// 15
dynAppend(db,"M3_PC4;34"); // 34
dynAppend(db,"M3_PC5;35"); // 35
dynAppend(db,"M3_PC6;36"); // 36
//2B 2C 2D 2E 2F 30
dynAppend(db,"M2_PC1;2b"); // 2b
dynAppend(db,"M2_PC2;2c"); // 2c
dynAppend(db,"M2_PC3;2d"); // 2d //  0f
dynAppend(db,"M2_PC4;2e"); // 2e
dynAppend(db,"M2_PC5;2f"); // 2f
dynAppend(db,"M2_PC6;30"); // 30

//20 21 | 23 22 | 25 24 | 27 26 | 28 29 | 2A 1F
dynAppend(db,"M1_PC1A;20"); // 20
dynAppend(db,"M1_PC1B;21"); // 21
dynAppend(db,"M1_PC2A;23"); // 23
dynAppend(db,"M1_PC2B;22"); // 22
dynAppend(db,"M1_PC3A;25"); // 25
dynAppend(db,"M1_PC3B;24"); // 24
dynAppend(db,"M1_PC4A;27"); // 27
dynAppend(db,"M1_PC4B;26"); // 26
dynAppend(db,"M1_PC5A;28"); // 28
dynAppend(db,"M1_PC5B;29"); // 29
dynAppend(db,"M1_PC6A;2a"); // 2a
dynAppend(db,"M1_PC6B;1f"); // 1f

//2 3 | 5 4 | 7 6 | 9 8 | A B | C 1
dynAppend(db,"P1_PC1A;02"); // 02  //sec1
dynAppend(db,"P1_PC1B;03"); // 03  //sec1
dynAppend(db,"P1_PC2A;05"); //// 05  //sec2
dynAppend(db,"P1_PC2B;04"); //// 04  //sec2
dynAppend(db,"P1_PC3A;07"); //// 07  //sec3
dynAppend(db,"P1_PC3B;06"); //// 06  //sec3
dynAppend(db,"P1_PC4A;09"); //// 09  //sec4
dynAppend(db,"P1_PC4B;08"); //// 08  //sec4
dynAppend(db,"P1_PC5A;0a"); // 0a  //sec5
dynAppend(db,"P1_PC5B;0b"); // 0b  //sec5
dynAppend(db,"P1_PC6A;0c"); // 0c   //sec6
dynAppend(db,"P1_PC6B;01"); // 01  //sec6

//D E F 10 11 12
dynAppend(db,"P2_PC1;0d"); // 0d  //sec1
dynAppend(db,"P2_PC2;0e"); //// 0e  //sec2
dynAppend(db,"P2_PC3;0f"); //// 0f  //sec3
dynAppend(db,"P2_PC4;10"); //// 10  //sec4
dynAppend(db,"P2_PC5;11"); // 11  //sec5
dynAppend(db,"P2_PC6;12"); // 12  //sec6

//13 14 15 16 17 18
dynAppend(db,"P3_PC1;13"); // 13  //sec1
dynAppend(db,"P3_PC2;14"); //// 14  //sec2
dynAppend(db,"P3_PC3;15"); //// 15  //sec3
dynAppend(db,"P3_PC4;16"); //// 16  //sec4
dynAppend(db,"P3_PC5;17"); // 17  //sec5
dynAppend(db,"P3_PC6;18"); // 18  //sec6

//19 1A 1B 1C 1D 1E
dynAppend(db,"P4_PC1;19"); // 19   //sec1
dynAppend(db,"P4_PC2;1a"); //// 1a  //sec2
dynAppend(db,"P4_PC3;1b"); //// 1b  //sec3
dynAppend(db,"P4_PC4;1c"); //// 1c  //sec4
dynAppend(db,"P4_PC5;1d"); // 1d  //sec5
dynAppend(db,"P4_PC6;1e"); // 1e  //sec6
 
dpSetWait("db_pcmb.list",db);
  
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
dynAppend(pcmb_mrtn,"26;17");
dynAppend(pcmb_mrtn,"27;17");
dynAppend(pcmb_mrtn,"28;17");
dynAppend(pcmb_mrtn,"29;18");
dynAppend(pcmb_mrtn,"30;18");


dpSetWait("pcmb_mrtn.list",pcmb_mrtn);



  


}
//============================

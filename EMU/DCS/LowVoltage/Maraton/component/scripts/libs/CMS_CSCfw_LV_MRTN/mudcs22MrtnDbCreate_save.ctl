mudcsMrtnDbCreateMain(){
int i;


mudcsMrtnPnlInit();
mudcsMrtn_create_mrtn_database_type();
dpCreate("db_mrtn","Db_MRTN");
mudcsMrtn_fill_mrtn_database();

// mudcsLvDebug("Done");
}
//============================

mudcsMrtn_create_mrtn_database_type()
{
string dtype;
dyn_dyn_string xxdepes;
dyn_dyn_int xxdepei;

dtype="Db_MRTN";

xxdepes[1] = makeDynString (dtype,"");
xxdepes[2] = makeDynString ("","list");

xxdepei[1]=makeDynInt (DPEL_STRUCT);
xxdepei[2]=makeDynInt (0,DPEL_DYN_STRING);

dpTypeCreate(xxdepes,xxdepei);
}
//============================
mudcsMrtn_fill_mrtn_database(){

  dyn_string db, db_geog;

//dynAppend(db,"CR1_PL500;2");
  
// addresses are decimal !!!!!!!!!  
  
  
dynAppend(db,"M4_CR1;37");dynAppend(db_geog,"X4S51");//37
dynAppend(db,"M4_CR2;38");dynAppend(db_geog,"X2V51");//38
dynAppend(db,"M4_CR3;");dynAppend(db_geog,"");//
dynAppend(db,"M4_CR4;");dynAppend(db_geog,"");//
dynAppend(db,"M4_CR5;");dynAppend(db_geog,"");//
dynAppend(db,"M4_CR6;");dynAppend(db_geog,"");//
dynAppend(db,"M4_CR7;");dynAppend(db_geog,"");//
dynAppend(db,"M4_CR8;");dynAppend(db_geog,"");//
/* should not be filled : the M2 should be filled instead: the maratons of M2and M3 are the same
dynAppend(db,"M3_CR1;");
dynAppend(db,"M3_CR2;");
dynAppend(db,"M3_CR3;");
dynAppend(db,"M3_CR4;");
dynAppend(db,"M3_CR5;");
dynAppend(db,"M3_CR6;");
dynAppend(db,"M3_CR7;");
dynAppend(db,"M3_CR8;");
*/

dynAppend(db,"M2_CR1;29");dynAppend(db_geog,"X4V41_L");//29
dynAppend(db,"M2_CR2;30");dynAppend(db_geog,"X4V41_U");//30
dynAppend(db,"M2_CR3;32");dynAppend(db_geog,"X4S41_L");//32
dynAppend(db,"M2_CR4;31");dynAppend(db_geog,"X4S41_U");//31
dynAppend(db,"M2_CR5;34");dynAppend(db_geog,"X2S43_L");//34
dynAppend(db,"M2_CR6;33");dynAppend(db_geog,"X2S43_U");//33
dynAppend(db,"M2_CR7;35");dynAppend(db_geog,"X2V41_L");//35
dynAppend(db,"M2_CR8;36");dynAppend(db_geog,"X2V41_U");//36

dynAppend(db,"M1_CR1;21");dynAppend(db_geog,"X4V31_L");//21
dynAppend(db,"M1_CR2;22");dynAppend(db_geog,"X4V31_U");//22
dynAppend(db,"M1_CR3;24");dynAppend(db_geog,"X4S31_L");//24
dynAppend(db,"M1_CR4;23");dynAppend(db_geog,"X4S31_U");//23
dynAppend(db,"M1_CR5;26");dynAppend(db_geog,"X2S33_L");//26
dynAppend(db,"M1_CR6;25");dynAppend(db_geog,"X2S33_U");//25
dynAppend(db,"M1_CR7;27");dynAppend(db_geog,"X2V31_L");//27
dynAppend(db,"M1_CR8;28");dynAppend(db_geog,"X2V31_U");//28

dynAppend(db,"P1_CR1;1");dynAppend(db_geog,"X4J31_L");//1
dynAppend(db,"P1_CR2;2");dynAppend(db_geog,"X4J31_U");//2
dynAppend(db,"P1_CR3;4");dynAppend(db_geog,"X4A31_L");//4
dynAppend(db,"P1_CR4;3");dynAppend(db_geog,"X2A31_U");//3
dynAppend(db,"P1_CR5;6");dynAppend(db_geog,"X2A33_L");//6
dynAppend(db,"P1_CR6;5");dynAppend(db_geog,"X2A33_U");//5
dynAppend(db,"P1_CR7;7");dynAppend(db_geog,"X2J31_L");//7
dynAppend(db,"P1_CR8;8");dynAppend(db_geog,"X2J31_U");//8

dynAppend(db,"P2_CR1;9");dynAppend(db_geog,"X4J41_L");//9
dynAppend(db,"P2_CR2;10");dynAppend(db_geog,"X4J41_U");//10
dynAppend(db,"P2_CR3;12");dynAppend(db_geog,"X4A41_L");//12
dynAppend(db,"P2_CR4;11");dynAppend(db_geog,"X4A41_U");//11
dynAppend(db,"P2_CR5;14");dynAppend(db_geog,"X2A43_L");//14
dynAppend(db,"P2_CR6;13");dynAppend(db_geog,"X2A43_U");//13
dynAppend(db,"P2_CR7;15");dynAppend(db_geog,"X2J41_L");//15
dynAppend(db,"P2_CR8;16");dynAppend(db_geog,"X2J41_U");//16

/* should not be filled : the P2 should be filled instead: the maratons of P2 and P3 are the same
dynAppend(db,"P3_CR1;");
dynAppend(db,"P3_CR2;");
dynAppend(db,"P3_CR3;");
dynAppend(db,"P3_CR4;");
dynAppend(db,"P3_CR5;");
dynAppend(db,"P3_CR6;");
dynAppend(db,"P3_CR7;");
dynAppend(db,"P3_CR8;");
*/

dynAppend(db,"P4_CR1;17");dynAppend(db_geog,"X4A51");//17
dynAppend(db,"P4_CR2;18");dynAppend(db_geog,"X2J51");//18
dynAppend(db,"P4_CR3;");dynAppend(db_geog,"");//
dynAppend(db,"P4_CR4;");dynAppend(db_geog,"");//
dynAppend(db,"P4_CR5;");dynAppend(db_geog,"");//
dynAppend(db,"P4_CR6;");dynAppend(db_geog,"");//
dynAppend(db,"P4_CR7;");dynAppend(db_geog,"");//
dynAppend(db,"P4_CR8;");dynAppend(db_geog,"");//

dpSetWait("db_mrtn.list",db);
dpSetWait("db_mrtn_geographic.list",db_geog); 
  
}
//============================

mudcsMrtnDbCreateMain() {
  int i;


  mudcsMrtnPnlInit();
  mudcsMrtn_create_mrtn_database_type();
  dpCreate("db_mrtn", "Db_MRTN");
  mudcsMrtn_fill_mrtn_database();

// mudcsLvDebug("Done");
}
//============================

mudcsMrtn_create_mrtn_database_type() {
  string dtype;
  dyn_dyn_string xxdepes;
  dyn_dyn_int xxdepei;

  dtype = "Db_MRTN";

  xxdepes[1] = makeDynString (dtype, "");
  xxdepes[2] = makeDynString ("", "list");

  xxdepei[1] = makeDynInt (DPEL_STRUCT);
  xxdepei[2] = makeDynInt (0, DPEL_DYN_STRING);

  dpTypeCreate(xxdepes, xxdepei);
}
//============================
mudcsMrtn_fill_mrtn_database() {

  dyn_string db;

//dynAppend(db,"CR1_PL500;2");

// addresses are decimal !!!!!!!!!


  dynAppend(db, "M4_CR1;37");//37
  dynAppend(db, "M4_CR2;38");//38
  dynAppend(db, "M4_CR3;");//
  dynAppend(db, "M4_CR4;");//
  dynAppend(db, "M4_CR5;");//
  dynAppend(db, "M4_CR6;");//
  dynAppend(db, "M4_CR7;");//
  dynAppend(db, "M4_CR8;");//
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

  dynAppend(db, "M2_CR1;29");//29
  dynAppend(db, "M2_CR2;30");//30
  dynAppend(db, "M2_CR3;32");//32
  dynAppend(db, "M2_CR4;31");//31
  dynAppend(db, "M2_CR5;34");//34
  dynAppend(db, "M2_CR6;33");//33
  dynAppend(db, "M2_CR7;35");//35
  dynAppend(db, "M2_CR8;36");//36

  dynAppend(db, "M1_CR1;21");//21
  dynAppend(db, "M1_CR2;22");//22
  dynAppend(db, "M1_CR3;24");//24
  dynAppend(db, "M1_CR4;23");//23
  dynAppend(db, "M1_CR5;26");//26
  dynAppend(db, "M1_CR6;25");//25
  dynAppend(db, "M1_CR7;27");//27
  dynAppend(db, "M1_CR8;28");//28

  dynAppend(db, "P1_CR1;1");//1
  dynAppend(db, "P1_CR2;2");//2
  dynAppend(db, "P1_CR3;4");//4
  dynAppend(db, "P1_CR4;3");//3
  dynAppend(db, "P1_CR5;6");//6
  dynAppend(db, "P1_CR6;5");//5
  dynAppend(db, "P1_CR7;7");//7
  dynAppend(db, "P1_CR8;8");//8

  dynAppend(db, "P2_CR1;9");//9
  dynAppend(db, "P2_CR2;10");//10
  dynAppend(db, "P2_CR3;12");//12
  dynAppend(db, "P2_CR4;11");//11
  dynAppend(db, "P2_CR5;14");//14
  dynAppend(db, "P2_CR6;13");//13
  dynAppend(db, "P2_CR7;15");//15
  dynAppend(db, "P2_CR8;16");//16

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

  dynAppend(db, "P4_CR1;17");//17
  dynAppend(db, "P4_CR2;18");//18
  dynAppend(db, "P4_CR3;");//
  dynAppend(db, "P4_CR4;");//
  dynAppend(db, "P4_CR5;");//
  dynAppend(db, "P4_CR6;");//
  dynAppend(db, "P4_CR7;");//
  dynAppend(db, "P4_CR8;");//

  dpSetWait("db_mrtn.list", db);


}
//============================

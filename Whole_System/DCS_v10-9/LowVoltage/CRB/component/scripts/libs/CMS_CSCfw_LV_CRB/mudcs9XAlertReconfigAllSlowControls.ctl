 //=========================
dyn_string mudcsTempAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new){

dyn_string ilist, f_inds;;

  string  s_j;
  int i, j, pos, ind, f_ind;

 
   for(j=1;j<=dynlen(off_channels);j++){
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
     if(off_channels[j] <=5){
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],"cfeb.v"+s_j)>=0){f_ind=ind;break;}
       } // for ind
     }
     else if(off_channels[j] ==6){
      for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],"alct.")>=0){f_ind=ind;break;}
       } // for ind      
     }
     else if(off_channels[j] ==7){
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],"tmb.")>=0){f_ind=ind;break;}
       } // for ind   
     }     
//---------------------------     
     if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
  
}
//========================

dyn_string mudcsLvCfebAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new, string dpe){
  
 
dyn_string ilist, f_inds;;

  string  s_j;
  int i, j, pos, ind, f_ind;

 
   for(j=1;j<=dynlen(off_channels);j++){
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
     if(off_channels[j] <=5){
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],dpe+".v"+s_j)>=0){dynAppend(f_inds,ind);}         
       } // for ind
     }

      
 
//---------------------------     
 //    if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
   
  
}
//========================
//========================

dyn_string mudcsLvAlctAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new){
  
 
dyn_string ilist, f_inds;;

  string  s_j;
  int i, j, pos, ind, f_ind;

 
   for(j=1;j<=dynlen(off_channels);j++){
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
 
     if(off_channels[j] ==6){
      for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],"Alct_o.")>=0){dynAppend(f_inds,ind);}                                      
       } // for ind      
     }
 
//---------------------------     
 //    if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
   
  
}
//========================
dyn_string mudcsHvAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new){
  

dyn_string ilist, f_inds;;

  string  s_j;
  int i, j, pos, ind, f_ind;

 
   for(j=1;j<=dynlen(off_channels);j++){
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
     
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         if(strpos(ilist_new[ind],".v"+s_j)>=0 && strpos(ilist_new[ind],"master")<0){f_ind=ind;break;}
       } // for ind
     
 
//---------------------------     
     if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
    
  
}//========================
dyn_string mudcsCrbAlertReconfig(dyn_int &off_channels, dyn_string &ilist_new){

//-------------------------  
  mudcsCrbPnlInit();  
//-------------------------
dyn_string ilist, f_inds, dp_frag;

  string  s_j;
  int i, j, pos, ind, f_ind, i10, chn;

 
   for(j=1;j<=dynlen(off_channels);j++){
     chn=off_channels[j];
     
     s_j=off_channels[j];
     f_ind=0;
 //-------------------------    
     if(off_channels[j] <=9){
   //   mudcsDebug(chn+" ; "+off_channels[j] );
      dp_frag=makeDynString("ai_"+CSC_fwCAN1_g_LIST_5V[chn],
                      "ai_"+CSC_fwCAN1_g_LIST_3V1[chn],"ai_"+CSC_fwCAN1_g_LIST_3V2[chn]);       
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         for(i10=1;i10<=dynlen(dp_frag);i10++){
          if(strpos(ilist_new[ind],dp_frag[i10])>=0){dynAppend(f_inds,ind);}
         }
       } // for ind
     }
     else if(off_channels[j] ==10){
      dp_frag=makeDynString("ai_"+CSC_fwCAN1_g_LIST_MPC[1],"ai_"+CSC_fwCAN1_g_LIST_MPC[2]);  
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         for(i10=1;i10<=dynlen(dp_frag);i10++){
          if(strpos(ilist_new[ind],dp_frag[i10])>=0){dynAppend(f_inds,ind);}
         }
       } // for ind
     }
     else if(off_channels[j] ==11){
      dp_frag=makeDynString("ai_"+CSC_fwCAN1_g_LIST_CC[1],"ai_"+CSC_fwCAN1_g_LIST_CC[2]);
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         for(i10=1;i10<=dynlen(dp_frag);i10++){
           if(strpos(ilist_new[ind],dp_frag[i10])>=0){dynAppend(f_inds,ind);}
         }
       } // for ind
     }
     else if(off_channels[j] ==12){
      dp_frag=makeDynString("ai_"+CSC_fwCAN1_g_LIST_15V[1],"ai_"+CSC_fwCAN1_g_LIST_15V[2]);   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
         for(i10=1;i10<=dynlen(dp_frag);i10++){
          if(strpos(ilist_new[ind],dp_frag[i10])>=0){dynAppend(f_inds,ind);}
         }
       } // for ind
     }

//---------------------------     
//     if(f_ind!=0)dynAppend(f_inds,f_ind);
     
   } // for j
   
       for(ind=1;ind<=dynlen(ilist_new);ind++){
             if(!dynContains(f_inds,ind))dynAppend(ilist,ilist_new[ind]);
            // else mudcsDebug(""); 
       } // for ind          
   
  return ilist;
    
  
}
//========================

//=========================
mudcsAlertReconfig(string type, string dpe, bool isLocal=false){
  
 
  
  string data, system_name, s_j, dpe2="";
  int i, j, pos, ind, f_ind;
  dyn_string dps, split;
  dyn_int off_channels;
  
bool configExists;
dyn_string alertTexts;
dyn_string ilist, ilist_new;
string alertPanel;
dyn_string alertPanelParameters;
string alertHelp;
bool isActive;
dyn_string exceptionInfo;

if(strpos(type,";")>=0){
  split=strsplit(type,";");
  type=split[1];
  dpe2=split[2]; 
}
  if(isLocal)dps=dpNames("*",type);
  else dps=dpNames("*:*",type); 
  

    
  for(i=1;i<=dynlen(dps);i++){
  //  DebugTN("aler:     "+dps[i]);
   pos=strpos(dps[i],":");
   if(pos>0)system_name=substr(dps[i],0,pos+1); // with ":"
   else system_name="";
   if(type=="fwCrb_CSC_LV")data=dps[i]+".";
   else if(dpe2!="")data = system_name+substr(dps[i],strpos(dps[i],"/")+1)+dpe2;
   else data=system_name+substr(dps[i],strpos(dps[i],"/")+1)+".";
       
   dpGet(dps[i]+dpe,off_channels);
   
  fwAlertConfig_getSummary(data,configExists,alertTexts,ilist,
  alertPanel,alertPanelParameters,alertHelp,isActive,exceptionInfo);
  
  if(dynlen(off_channels)>0){
   //DebugTN(ilist); 
  }
  
   ilist_new=ilist;
   dynClear(ilist);
   //========================================
   
  if(type=="TEMP_1")ilist=mudcsTempAlertReconfig(off_channels, ilist_new);
  else if(type=="LV_1"&& dpe2=="")ilist=mudcsLvAlctAlertReconfig(off_channels, ilist_new);
  else if(type=="LV_1"&& dpe2!="")ilist=mudcsLvCfebAlertReconfig(off_channels, ilist_new, dpe2);  
  else if(type=="HV_1")ilist=mudcsHvAlertReconfig(off_channels, ilist_new);
  else if(type=="fwCrb_CSC_LV")ilist=mudcsCrbAlertReconfig(off_channels, ilist_new);
        
  
 //========================================  
   if(dynlen(off_channels)>0){
 //    DebugTN("alert");
 //    DebugTN(ilist);
     if(ilist!=ilist_new){
     DebugTN(ilist);         
  //     mudcsDebug(dps[i]);
   fwAlertConfig_createSummary(data,makeDynString("","") , ilist ,"", 
                              makeDynString(),"",exceptionInfo);  
     } // if(ilist!=ilist_new)
   }
  } // for i
  
}
//========================

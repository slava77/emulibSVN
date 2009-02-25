

emufwFsmUi_addUserPanel(string node, string obj, string project_station, string project_system, int x, int y)
{
	string panel, panel_path, sys, dev;
        
//mudcsLvDebug(obj);        
        
        DebugTN(node+">>>11"+obj+"===================================================="); 
                 DebugTN(node+">>>11"+obj+"===================================================="); 
                         DebugTN(node+">>>11"+obj+"===================================================="); 
                              DebugTN(node+">>>11"+obj+"====================================================");    
 int y_add=0; 
 int x_add=0;       
  dyn_string children;
 dyn_string dsFsm;
 dyn_int child_flags; 
 int i;
 dyn_string dp_ms;
 string obj_sys="";
 int pos;
//-------- SPECIALLY FOR MARATHON: NOT CUSTOM_LV ONLY !!!!---------------

if(!(globalExists("CSC_fwG_g_CUSTOM_LV")) || !CSC_fwG_g_CUSTOM_LV){ 
  
 y_add=0;           
if(dpExists(obj) && strpos(dpTypeName(obj),"FwWienerMarathon")>=0)y_add=70; 
if(dpExists(obj) && strpos(dpTypeName(obj),"FwWienerPS")>=0)y_add=70; 
           
if(strpos(obj,"_CR")>=0 && (strpos(obj,"_LV_MRTN")>=0 || strpos(obj,"_PL500")>=0  ) ){

// mudcsLvDebug("obj1="+obj);
 children = fwFsmUi_getAllChildren(node, obj, child_flags);
 for(i=1;i<=dynlen(children);i++){
  dsFsm=strsplit(children[i],"|");
  dp_ms=dpNames("*:"+dsFsm[dynlen(dsFsm)]);
  
  DebugTN(node+">>>"+obj+">>>>>>>>>>>>>>>>>>>"+children[i]);
  DebugTN(node+">>>"+obj+">>>>>"+dp_ms[1]);
  
  if(dpTypeName(dp_ms[1]) == "FwWienerMarathon"){
    obj=dsFsm[dynlen(dsFsm)];
    break;  
  }
  
  if(dpTypeName(dp_ms[1]) == "FwWienerPS"){
    obj=dsFsm[dynlen(dsFsm)];
    break;  
  }  
    
 } // for
 
  mudcsMrtnPnlInit(node);  
  obj_sys=mudcsLvAddSystem(obj, CSC_fwCAN2_g_SYSTEM_NAME);
   
y_add=70; 
} // if
 } // if not custom_lv
//-----------------------------------------------
if(strpos(obj,"WIENER_CRATE_SNMP")>=0){

// mudcsLvDebug("obj1="+obj);
 children = fwFsmUi_getAllChildren(node, obj, child_flags);
 for(i=1;i<=dynlen(children);i++){
  dsFsm=strsplit(children[i],"|");
  dp_ms=dpNames("*:"+dsFsm[dynlen(dsFsm)]);
  
  DebugTN(node+">>>"+obj+">>>>>>>>>>>>>>>>>>>"+children[i]);
  DebugTN(node+">>>"+obj+">>>>>"+dp_ms[1]);
  
  if(dpTypeName(dp_ms[1]) == "fwWnrCr_CSC_LV"){
    obj=dsFsm[dynlen(dsFsm)];
    break;  
  }
 } // for
} // if
//-----------------------------------------------
if(strpos(obj,"_CR")>=0 && strpos(obj,"_PSU")>=0  ){ // definition of the ob_sys for the crate logical folder which contains branches
// moved to the button as for geometry problems
} // if

//-----------------------------------------------        
	fwUi_getUserPanel(node, obj, panel);
	if(panel != "")
	{
		panel_path = fwUi_getPanelPath(panel);
//mudcsDebug(panel);
//mudcsDebug2(panel_path);
		if(panel_path != "")
		{
			dev = obj;
			if(fwFsm_isDU(node, obj))
			{
				fwUi_getDomainSys(node, sys);
				dev = fwFsm_getPhysicalDeviceName(sys+obj);
				dev = sys+dev;
			}
                        if(obj_sys != "")dev=obj_sys;
			addSymbol(myModuleName(),myPanelName(),panel_path,
				"specific",
/////				makeDynString(node, dev),
				makeDynString("$node:"+node, "$obj:"+dev,"$sDpName:"+dev, "$project_station:"+project_station, "$project_system:"+project_system), // my_corr++
				x+x_add, y+y_add, 0,1,1);
                       // mudcsLvDebug(dev+">>"+obj);
		}
	}
}

emufwFsmUi_addUserPanel(string node, string obj, string project_station, string project_system, int x, int y)
{
	string panel, panel_path, sys, dev;
        
//mudcsLvDebug(obj);        
        
        DebugTN(node+">>>"+obj+"===================================================="); 
                 DebugTN(node+">>>"+obj+"===================================================="); 
                         DebugTN(node+">>>"+obj+"===================================================="); 
                              DebugTN(node+">>>"+obj+"====================================================");    
 int y_add;       
  dyn_string children;
 dyn_string dsFsm;
 dyn_int child_flags; 
 int i;
 dyn_string dp_ms;
//-------- SPECIALLY FOR MARATHON: NOT CUSTOM_LV ONLY !!!!---------------

if(!(globalExists("CSC_fwG_g_CUSTOM_LV")) || !CSC_fwG_g_CUSTOM_LV){ 
  
 y_add=0;           
if(dpExists(obj) && strpos(dpTypeName(obj),"FwWienerMarathon")>=0)y_add=70; 
           
if(strpos(obj,"_CR")>=0 && strpos(obj,"_LV_MRTN")>=0){

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
 } // for
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
			addSymbol(myModuleName(),myPanelName(),panel_path,
				"specific",
/////				makeDynString(node, dev),
				makeDynString("$node:"+node, "$obj:"+dev,"$sDpName:"+dev, "$project_station:"+project_station, "$project_system:"+project_system), // my_corr++
				x, y+y_add, 0,1,1);
                       // mudcsLvDebug(dev+">>"+obj);
		}
	}
}

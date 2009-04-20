string CurrType, CurrTypePanel;
dyn_string CurrTypePars;

fwFsm_importSmi(string typeName, dyn_string &exceptionInfo)
{
  string filename;
  time t0;
  emu_debugFuncStart("fwFsm_importSmi", t0);
  
  filename = getPath("smi/", typeName + ".fsm");
  if (isfile(filename) == 0) {
    emu_addError("SMI file does not exist: <project_path>/smi/" + typeName + ".fsm", exceptionInfo);
    return;
  }
  fwFsm_startShowFwObjects();
  fwFsm_initialize();
  fwFsm_readSmiDomain(filename);
  
  emu_debugFuncEnd("fwFsm_importSmi", t0);
}

fwFsm_readSmiDomain(string filename)
{
file fin;
string s, type, state, color, action, obj, action_text, when_text, domain, domain_type;
dyn_string states, objs, types, action_pars, whens;
int pos, class_on, action_on, when_on, pars_on, unpad, top;
string all_chars, pars_text, panel;
int i, visi;

	all_chars = "_-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{}()!1234567890*&%$#@^";
	fin = fopen(filename,"r");
	action_on = 0;
	when_on = 0;
	pars_on = 0;
	domain = "";
	while(!feof(fin))
	{
		fgets(s,2000,fin);
		if(newClass(s, type))
		{
			terminate_when(when_on, when_text);
			terminate_action(action_on, action_text);
			terminate_class(class_on, states);
			if((strpos(type,"FwChild") == 0) || 
				 (strpos(type,"FwMode") == 0) ||
				 (strpos(type,"FwDev") == 0) ||
			   (strpos(type,"_Fw") > 0))
			{
				if((strpos(filename,"FwChild") < 0) && 
					 (strpos(filename,"FwMode") < 0) &&
					 (strpos(filename,"FwDev") < 0) )
				{ 
					class_on = 0;
					continue;
				}
			}
			class_on = 1;
			if((dynContains(types,type))||(type == domain_type))
			{
				class_on = 0;
				continue;
			}
			fwFsm_createLocalObject(type);
			CurrType = type;
			dynClear(CurrTypePars);
			CurrTypePanel = "";
DebugN("Type = "+type);
//DebugN(ObjStateNames);
			dynClear(states);
		}
		else if(newPanel(s, panel))
		{
DebugN("Panel = "+panel);
			CurrTypePanel = panel;
		}
		else if(newObjPars(s, pars_text))
		{
			pars_on = 1;
		}
		else if(newState(s, state, color))
		{
			terminate_pars(pars_on, pars_text);
			terminate_when(when_on, when_text);
			terminate_action(action_on, action_text);
			if(!class_on)
				continue;
			dynAppend(states, state);
			fwFsm_addLocalState(state, color);
			fwFsm_setLocalState(state);
DebugN("  State = "+state+", color = "+color);
			fwFsm_getLocalWhens(whens);
			for(i = 1; i <= dynlen(whens); i++)
			{
				fwFsm_removeLocalWhen(whens[i]);
			}
		}
		else if(newAction(s, action, action_pars, visi))
		{
			terminate_when(when_on, when_text);
			terminate_action(action_on, action_text);
			if(!class_on)
				continue;
			if((pos = strpos(action,"NV_")) < 0)
			{
				fwFsm_addLocalAction(action, visi, 0);
				fwFsm_setLocalAction(action);
				fwFsm_setLocalActionPars(action_pars);
				action_on = 1;
			}
			action_text = "";
DebugN("    Action = "+action+", visi = "+visi);
		}		
		else if(newWhen(s))
		{
			terminate_when(when_on, when_text);
			if(!class_on)
				continue;
			when_on = 1;
			unpad = strlen(s);
			s = strltrim(s);
			unpad -= strlen(s);
			when_text = s;
		}		
		else if(newObject(s, obj, type, top))
		{
			terminate_action(action_on, action_text);
			if(!check_object_type(type))
				continue;
//			if((strpos(type,"Fw") == 0) || (strpos(type,"_Fw") > 0))
//				continue;
			if(top)
			{
				domain = obj;
				domain_type = type;
			}
			else
			{
				dynAppend(objs, obj);
				dynAppend(types, type);
			}
DebugN("Object = "+obj+" of type "+type);
		}
		else if(newObjectSet(s, obj)) 
		{
			terminate_action(action_on, action_text);
		}
		else
		{
			if(action_on)
			{
				if(action_text == "")
				{
					unpad = strlen(s);
					s = strltrim(s);
					unpad -= strlen(s);
				}
				else
				{
					pos = strtok(s, all_chars);
					if(pos > unpad)
						pos = unpad;
					s = substr(s, pos);
				}
				action_text += s;
			}
			if(when_on)
			{
				pos = strtok(s, all_chars);
				if(pos > unpad)
					pos = unpad;
				s = substr(s, pos);
				when_text += s;
			}
			if(pars_on)
			{
				pos = strtok(s, all_chars);
				if(pos > unpad)
					pos = unpad;
				s = substr(s, pos);
				pars_text += s; 
			}
		}
	}
	terminate_when(when_on, when_text);
	terminate_action(action_on, action_text);
	terminate_class(class_on, states);
	create_domain(domain, domain_type, objs, types);
	fclose(fin);
}

int check_object_type(string type)
{
	if( (type == "FwMode") ||
			(type == "FwChildMode") ||
			(type == "FwChildrenMode") ||
			(type == "FwDevMode") )
		return 0;
	if(strpos(type,"_Fw") > 0)
		return 0;
	return 1;
}

terminate_class(int &class_on, dyn_string states)
{
	if(class_on)
	{
		fwFsm_createObjectType(CurrType);
		fwFsm_writeLocalObject(CurrType, states);
		fwFsm_writeObjectParameters(CurrType, CurrTypePars);
		fwUi_setTypeFullPanel(CurrType, CurrTypePanel);
		class_on = 0;
	}
}

terminate_action(int &action_on, string action_text)
{
	if(action_on)
	{
		fwFsm_setLocalActionText(action_text);
		action_on = 0;
	}
}

terminate_pars(int &pars_on, string &pars_text)
{
	dyn_string pars;
	int i;
	
	if(pars_on)
	{
		pars = strsplit(pars_text,",");
		for(i = 1; i <= dynlen(pars); i++)
		{
			pars[i] = strrtrim(pars[i]);
			pars[i] = strltrim(pars[i]);
		}		
		CurrTypePars = pars;
		pars_text = "";
		pars_on = 0;
	}
}

terminate_when(int &when_on, string when_text)
{
dyn_string whens;
int pos;

	if(when_on)
	{
		fwFsm_getLocalWhens(whens);
		if(!dynContains(whens, when_text))
		{
			fwFsm_addLocalWhen(when_text);
		}
		when_on = 0;
	}
}

int newClass(string &s, string &res)
{
	dyn_string items;
	int pos, index = 1;
	
	if(!get_tokens(s, items))
		return 0;
	if(items[index] == "class:")
		index++;
	else if((items[index] == "class") && (items[index+1] == ":"))
		index += 2;
	else
		return 0;
	res = items[index];
	if((pos = strpos(res,"TOP_")) == 0)
		res = substr(res,4);
	if((pos = strpos(res,"ASS_")) == 0)
		res = substr(res,4);
	if((pos = strpos(res,"$TOP$")) == 0)
		res = substr(res,5);
	if((pos = strpos(res,"$FWPART_$TOP$")) == 0)
		res = substr(res,13);
	if((pos = strpos(res,"$FWPART_$ASS_")) == 0)
		res = substr(res,13);
	if((pos = strpos(res,"_CLASS")) >= 0)
		res = substr(res,0,pos);
	return 1;
}

int newPanel(string &s, string &panel)
{
	dyn_string items;
	int pos, index = 1;
	
	if(!get_tokens(s, items))
		return 0;
	if(items[index] == "!panel:")
		index++;
	else
		return 0;
	panel = s;
	strreplace(panel,"!panel: ","");
	panel = strrtrim(panel);
	return 1;
}

int newObjPars(string &s, string &pars_text)
{
	dyn_string items;
	int i, index = 1;
	
	if(!get_tokens(s, items))
		return 0;
	if(items[index] == "parameters:")
		index++;
	else if((items[index] == "parameters") && (items[index+1] == ":"))
		index += 2;
	else
		return 0;
	for(i = index; i <= dynlen(items); i++)
		pars_text += items[i]+" ";
//	res = items[index];
	return 1;
}

int newState(string &s, string &res, string &color)
{
	dyn_string items;
	int pos, index = 1;
	
	color = "";
	if(!get_tokens(s, items))
		return 0;
	if(items[index] == "state:")
		index++;
	else if((items[index] == "state") && (items[index+1] == ":"))
		index += 2;
	else
		return 0;
	res = items[index];
	if(dynlen(items) > index)
	{
		index++;
		if(items[index] == "!color:")
		{
			index++;
			color = items[index];
		}
	}
	return 1;
}

int newAction(string &s, string &res, dyn_string &action_pars, int &visi)
{
	dyn_string items, names;
	string pars_text;
	int i, pos, pos1, index = 1;

	dynClear(action_pars);
	if(!get_tokens(s, items))
		return 0;
	if(items[index] == "action:")
		index++;
	else if((items[index] == "action") && (items[index+1] == ":"))
		index += 2;
	else
		return 0;
	if((pos = strpos(s, "(")) >= 0)
	{
		names = strsplit(items[index],"(");
		res = names[1];
		pars_text = substr(s, pos+1);
		pos1 = strpos(pars_text, ")");
		pars_text = substr(pars_text, 0, pos1);
//DebugN("pars_text", pars_text);
		action_pars = strsplit(pars_text,","); 
	}
	else
		res = items[index];
	if((index = dynContains(items,"!visible:")))
	{
		visi = items[index+1];
	}
	return 1;
}

int newWhen(string &s)
{
	dyn_string items;
	int pos, index = 1;
	
	if(!get_tokens(s, items))
		return 0;
	if(items[index] == "when")
		return 1;
	else
		return 0;
}

int newObject(string &s, string &obj, string &res, int &top)
{
	dyn_string items;
	int pos, index = 1;
	
	top = 0;
	if(!get_tokens(s, items))
		return 0;
	if(items[index] == "object:")
		index++;
	else if((items[index] == "object") && (items[index+1] == ":"))
		index += 2;
	else
		return 0;
	obj = items[index++];
	if(items[index++] == "is_of_class")
		res = items[index];
	if((pos = strpos(res,"TOP_")) == 0)
	{
		res = substr(res,4);
		top = 1;
	}
	if((pos = strpos(res,"ASS_")) == 0)
	{
		res = substr(res,4);
	}
	if((pos = strpos(res,"_CLASS")) >= 0)
		res = substr(res,0,pos);
	return 1;
}

int newObjectSet(string &s, string &res)
{
	dyn_string items;
	int pos, index = 1;
	
	if(!get_tokens(s, items))
		return 0;
	if(items[index] == "objectset:")
		index++;
	else if((items[index] == "objectset") && (items[index+1] == ":"))
		index += 2;
	else
		return 0;
	res = items[index];
	return 1;
}

int get_tokens(string s, dyn_string &items)
{
	int pos, i;

	items = strsplit(s," /\t\n");
	if(!dynlen(items))
		return 0;
	for(i = 1; dynlen(items); i++)
	{ 
		if(items[1] == "")
			dynRemove(items, 1);
		else
			break;
	}
	if(!dynlen(items))
		return 0;
	return 1;
}

create_domain(string domain, string type, dyn_string objs, dyn_string types)
{
	string obj, panel;
	dyn_string allobjs, alltypes, exInfo;
	int i, cu;
	
	if(domain == "")
		return;
	cu = 1;
	fwFsmTree_addNode("FSM", domain, type, cu);
//	_fwTree_addNode("", domain, type, cu);
DebugN("Adding "+domain);
	for(i = 1; i <= dynlen(objs); i++)
	{
		cu = 0;
		obj = objs[i];
		if(fwFsm_isAssociated(objs[i]))
		{
			obj = fwFsm_getAssociatedDomain(objs[i]);
			cu = 1;
		}
//		_fwTree_addNode(domain, obj, types[i], cu);
		strreplace(obj,":",fwDev_separator);
		strreplace(obj,"//","::");
		fwFsmTree_addNode(domain, obj, types[i], cu);
DebugN("Adding "+obj+" to "+domain);
	}
//	_fwTree_generateTreeNode(domain);
	fwFsmTree_generateTreeNode(domain);
//	_fwTree_setRedoTree();
	fwFsmTree_redoTree();
	setValue("Status","text","Domain "+domain+" Generated.");
}
/*
		fwFsm_writeObjectActionParameters($1, $2, $3, par_list);
	}
	else
		fwFsm_writeObjectParameters($1, par_list);
*/

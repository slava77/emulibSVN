dyn_string fwTreeDisplay_getTrees()
{
dyn_string trees;
int pos, i;

	trees = fwFsm_getDps("*","_FwTreeType");
	for(i = 1; i <= dynlen(trees); i++)
	{
		if((pos = strpos(trees[i],"fwTT_")) >= 0)
		{
			trees[i] = substr(trees[i],pos+5);
		}
	}
	return trees;
}

fwTreeDisplay_createTree(string name, int mode, string root, dyn_string types, dyn_string names)
{
	string dp;
	dyn_string modes, exInfo;
	string type_list, name_list;
	
	dp = "fwTT_"+name;
	dpCreate(dp,"_FwTreeType");
	dpSet(dp+".name",name);
	dynAppend(modes,"editor");
	if(mode > 1)
		dynAppend(modes,"navigator");
	dpSet(dp+".modes",modes);
	dpSet(dp+".root",root);
	type_list = types;
	strreplace(type_list," | ",",");
	dpSet(dp+".nodeTypes",type_list);
	fwTree_create(name, exInfo);
	name_list = names;
	strreplace(name_list," | ",",");
	dpSet(dp+".nodeNames",name_list);
	fwTree_create(name, exInfo);
}

string fwTreeDisplay_getNodeTypes(string tree)
{
	string types;
	
	string dp = "fwTT_"+tree;
	dpGet(dp+".nodeTypes",types);
	return types;
}

string fwTreeDisplay_getNodeNames(string tree)
{
	string names;
	
	string dp = "fwTT_"+tree;
	dpGet(dp+".nodeNames",names);
	return names;
}

string fwTreeDisplay_getRoot(string tree)
{
	string root, name, sys;
	
	string dp = "fwTT_"+tree;
	dpGet(dp+".root",root);
	dpGet(dp+".name",name);
	if((root != name) && (root != ""))
	{
		sys = fwFsm_getSystemName()+":";
		if(root != sys)
		{
			fwTreeDisplay_changeSystemName(name, root, sys);
			root = sys;
			dpSet(dp+".root",root);
		}
	}
	dpConnect("fwTreeDisplay_refreshCB",dp+".refresh");
	return root;
}

fwTreeDisplay_changeSystemName(string tree, string oldsys, string newsys)
{
	dyn_string nodes, exInfo;
	string dev, type;
	int i;

	fwTree_getAllTreeNodesWithClipboard(tree, nodes, exInfo);
	for(i = 1; i <= dynlen(nodes); i++)
	{
		fwTree_getNodeDevice(nodes[i], dev, type, exInfo);
		if(strpos(dev,oldsys) == 0)
		{
			strreplace(dev, oldsys, newsys);
			fwTree_setNodeDevice(nodes[i], dev, type, exInfo);
		}
	}
}

fwTreeDisplay_getTree()
{
int i, treeIndex;
dyn_string exInfo;
string root, types, names;
string treeIndexStr;

	while(!globalExists("FwTreeInit"))
		delay(0,100);
	while(!FwTreeInit)
		delay(0,100);
	getValue("TreeArrayIndex","text", treeIndexStr);
	treeIndex = treeIndexStr;

	if(!globalExists("CurrTreeIndex"))
	{
		addGlobal("FwTreeTops",DYN_STRING_VAR);
		addGlobal("FwTreeTypes",DYN_STRING_VAR);
		addGlobal("FwTreeNames",DYN_STRING_VAR);
		addGlobal("CurrTreeIndex",INT_VAR);
		addGlobal("PasteNode",STRING_VAR);
	}
	root = fwTreeDisplay_getRoot(FwActiveTrees[treeIndex]);
	types = fwTreeDisplay_getNodeTypes(FwActiveTrees[treeIndex]);
	names = fwTreeDisplay_getNodeNames(FwActiveTrees[treeIndex]);
	FwTreeTops[treeIndex] = root;
	FwTreeTypes[treeIndex] = types;
	FwTreeNames[treeIndex] = names;
	PasteNode = "";
	CurrTreeIndex = treeIndex;
	FwTreeRedos[treeIndex] = FwActiveTrees[treeIndex];
//	fwTreeDisplay_redoTree(CurrTree);
id=-1;// *********************** add this line my_corr++
}

fwTreeDisplay_refreshCB(string dp, int value)
{
	dyn_string items;
	string tree;

	items = strsplit(dp,":.");
	tree = items[2];
	items = strsplit(tree,"_");
	tree = items[2];
	fwTreeDisplay_setRedoTree(tree);
}

fwTreeDisplay_refreshTree(string tree)
{
	string dp = "fwTT_"+tree;
	dpSetWait(dp+".refresh",1);
}

fwTreeDisplay_setRedoTree(string tree)
{
	int i;

	for(i = 1; i <= dynlen(FwActiveTrees); i++)
	{
		if(FwActiveTrees[i] == tree)
			FwTreeRedos[i] = tree;
	}
}

fwTreeDisplay_setRedoTreeNode(string tree, string node)
{
	int i;

	for(i = 1; i <= dynlen(FwActiveTrees); i++)
	{
		if(FwActiveTrees[i] == tree)
			FwTreeRedos[i] = node;
	}
}

fwTreeDisplay_setExpandTreeNode(int treeIndex, string tree, string node)
{
	int i;

	for(i = 1; i <= dynlen(FwActiveTrees); i++)
	{
		if(i != treeIndex)
		{
			if(FwActiveTrees[i] == tree)
				FwTreeExpands[i] = node;
		}
	}
}

fwTreeDisplay_setCollapseTreeNode(int treeIndex, string tree, string node)
{
	int i;

	for(i = 1; i <= dynlen(FwActiveTrees); i++)
	{
		if(i != treeIndex)
		{
			if(FwActiveTrees[i] == tree)
				FwTreeCollapses[i] = node;
		}
	}
}

fwTreeDisplay_configNode(unsigned pos)
{
int treeIndex;
string currTree, currTreeMode, currTreeParent, currTreeNode;
string treeIndexStr;

	dyn_anytype node = fwTreeView_getNode(pos);
	
	getValue("TreeArrayIndex","text", treeIndexStr);
	treeIndex = treeIndexStr;
	CurrTreeIndex = treeIndex;
	currTree = FwActiveTrees[treeIndex];
	currTreeMode = FwTreeModes[treeIndex];
	currTreeParent = node[5];
	currTreeNode = node[1];
	fwTreeDisplay_callUser2(currTree+"_"+currTreeMode+"_entered",
			currTreeNode, currTreeParent);
id=-1;// *********************** add this line my_corr++
}

fwTreeDisplay_collapse(unsigned pos)
{
int treeIndex;
string currTree, currTreeMode, currTreeParent, currTreeNode;
string treeIndexStr;

	dyn_anytype node = fwTreeView_getNode(pos);
	
//DebugN("collapse",pos, node);
	getValue("TreeArrayIndex","text", treeIndexStr);
	treeIndex = treeIndexStr;
	CurrTreeIndex = treeIndex;
	currTree = FwActiveTrees[treeIndex];
	currTreeMode = FwTreeModes[treeIndex];
	currTreeParent = node[5];
	currTreeNode = node[1];
	fwTreeDisplay_callUser2(currTree+"_"+currTreeMode+"_collapsed",
			currTreeNode, currTreeParent);
	fwTreeView_defaultCollapse(pos);
	fwTreeDisplay_setCollapseTreeNode(treeIndex, currTree, currTreeNode);
}

fwTreeDisplay_expand(unsigned pos)
{
int treeIndex, initialPos;
string currTree, currTreeMode, currTreeParent, currTreeNode;
string treeIndexStr;

	dyn_anytype node = fwTreeView_getNode(pos);
	
//DebugN("expand",pos, node);
	initialPos = pos;
	getValue("TreeArrayIndex","text", treeIndexStr);
	treeIndex = treeIndexStr;
	CurrTreeIndex = treeIndex;
	currTree = FwActiveTrees[treeIndex];
	currTreeMode = FwTreeModes[treeIndex];
	currTreeParent = node[5];
	currTreeNode = node[1];
	fwTreeDisplay_callUser2(currTree+"_"+currTreeMode+"_expanded",
			currTreeNode, currTreeParent);
	fwTreeView_prune(pos);
	fwTreeDisplay_appendTreeNodes(treeIndex, currTreeParent, currTreeNode, node[2], pos);
	fwTreeView_defaultExpand(initialPos);
	if(FwTreeExpands[treeIndex] == "")
		fwTreeDisplay_setExpandTreeNode(treeIndex, currTree, currTreeNode);
	else
		fwTreeView_draw();
}

fwTreeDisplay_selectNode(unsigned pos)
{
int treeIndex;
string currTree, currTreeMode, currTreeParent, currTreeNode;
string treeIndexStr;

	dyn_anytype node = fwTreeView_getNode(pos);

//DebugN("sel",pos, node);
	getValue("TreeArrayIndex","text", treeIndexStr);
	treeIndex = treeIndexStr;
	CurrTreeIndex = treeIndex;
	currTree = FwActiveTrees[treeIndex];
	currTreeMode = FwTreeModes[treeIndex];
	currTreeParent = node[5];
	currTreeNode = node[1];
	fwTreeDisplay_callUser2(currTree+"_"+currTreeMode+"_selected",
			currTreeNode, currTreeParent);
id=-1;// *********************** add this line my_corr++
}

fwTreeDisplay_callUser2(string function, string node, string parent)
{
	if(isFunctionDefined(function))
	{

		execScript("main(string node, string parent) { "+
			function+"(node, parent);/* delay(20);*/}",
			makeDynString(), node, parent);

//		startThread(function, node, parent);
	}
}

fwTreeDisplay_callUser1(string function, string node)
{
	if(isFunctionDefined(function))
	{

		execScript("main(string node) { "+
			function+"(node);/* delay(20);*/}",
			makeDynString(), node);

//		startThread(function, node);
	}
}

fwTreeDisplay_expandTreeNode(int treeIndex, string node)
{
int i, n;
dyn_anytype treeNode;

	n = fwTreeView_getNodeCount();
	for(i = 1; i <= n; i++) 
	{
		treeNode = fwTreeView_getNode(i);
		if(treeNode[1] == node)
		{
			fwTreeDisplay_expand(i);
//			fwTreeView_defaultExpand(i);
//			fwTreeView_draw();
			break;
		}
	}
}

fwTreeDisplay_collapseTreeNode(int treeIndex, string node)
{
int i, n;
dyn_anytype treeNode;

	n = fwTreeView_getNodeCount();
	for(i = 1; i <= n; i++) 
	{
		treeNode = fwTreeView_getNode(i);
		if(treeNode[1] == node)
		{
			fwTreeView_defaultCollapse(i);
			fwTreeView_draw();
			break;
		}
	}
}

//int id;
/*
fwTreeDisplay_redoTree(int treeIndex)
{

	int i;
	dyn_string children, exInfo;

	for(i = fwTreeView_getNodeCount(); i > 0; i--)
	{
		fwTreeView_removeNode(i);
	}
	fwTreeDisplay_appendTreeNode(treeIndex, FwActiveTrees[treeIndex], FwActiveTrees[treeIndex], 0);
	fwTreeView_defaultExpand(1);
	fwTreeView_draw();
}

fwTreeDisplay_appendTreeNode(int treeIndex, string parent, string node, int level)
{
dyn_string children, exInfo;
string node_name;
int i, ref;


	if(fwFsm_isObjectReference(node))
		node_name = "&";
	node_name += fwTree_getNodeDisplayName(node, exInfo);

	if(node_name == FwActiveTrees[treeIndex])
	{
		if(FwTreeTops[treeIndex] != "")
		{
			fwTreeView_appendNode(FwTreeTops[treeIndex], FwActiveTrees[treeIndex], FwActiveTrees[treeIndex], level);
			level++;
		}
	}
	else if(node_name == _fwTree_makeClipboard(FwActiveTrees[treeIndex]))
	{
		if(FwTreeModes[treeIndex] != "navigator")
		{
			fwTreeView_appendNode("---Clipboard---", parent, node, level);
			level++;
		}
		else
			return;
	}
	else
	{
		fwTreeView_appendNode(node_name, parent, node, level);
		level++;
	}
	fwTree_getChildrenWithClipboard(node, children, exInfo);
	for(i = 1; i <= dynlen(children); i++)
	{
		if(children[i] != node)
			fwTreeDisplay_appendTreeNode(treeIndex, node, children[i], level);
	}

}
*/

fwTreeDisplay_redoTree(int treeIndex, string currNode)
{

	int i, n, pos = 1, level = 0, initialPos;
	dyn_string children, exInfo;
	dyn_anytype treeNode;
	string currParent;

//DebugN("redoTree",treeIndex, currNode);
/*
	for(i = fwTreeView_getNodeCount(); i > 0; i--)
	{
		fwTreeView_removeNode(i);
	}
	fwTreeDisplay_appendTreeNode(treeIndex, FwActiveTrees[treeIndex], FwActiveTrees[treeIndex], 0);
	fwTreeView_defaultExpand(1);
	fwTreeView_draw();
*/

	currParent = currNode;
	n = fwTreeView_getNodeCount();
	for(i = 1; i <= n; i++) 
	{
		treeNode = fwTreeView_getNode(i);
		if(treeNode[1] == currNode)
		{
			level = treeNode[2];
			currParent = treeNode[5];
			pos = i;
			break;
		}
	}
	if(level == 0)
	{
		currParent = currNode = FwActiveTrees[treeIndex];
	}
	if(n)
		fwTreeView_prune(pos);
	initialPos = pos;
	fwTreeDisplay_appendTreeNodes(treeIndex, currParent, currNode, level, pos);
	fwTreeView_defaultExpand(initialPos);
	fwTreeView_draw();
}

fwTreeDisplay_appendTreeNodes(int treeIndex, string parent, string node, int level, int &currPos)
{
dyn_string children, exInfo;
string node_name, child_node_name;
int i, ref, pos, n, dont;


	pos = currPos;
	if(fwFsm_isObjectReference(node))
		node_name = "&";
	node_name += fwTree_getNodeDisplayName(node, exInfo);

//	fwTree_getChildrenWithClipboard(node, children, exInfo);
//	n = 0;
//	if(dynlen(children))
//		n = 1;
	if(node_name == FwActiveTrees[treeIndex])
	{
		if(FwTreeTops[treeIndex] != "")
		{
//			fwTreeView_appendNode(FwTreeTops[treeIndex], FwActiveTrees[treeIndex], FwActiveTrees[treeIndex], level);
			fwTreeDisplay_insertNode(FwTreeTops[treeIndex], FwActiveTrees[treeIndex], 
				FwActiveTrees[treeIndex], level, pos);
			level++;
		}
	}
	else if(node_name == _fwTree_makeClipboard(FwActiveTrees[treeIndex]))
	{
		if(FwTreeModes[treeIndex] != "navigator")
		{
//			fwTreeView_appendNode("---Clipboard---", parent, node, level);
			fwTreeDisplay_insertNode("---Clipboard---", parent, 
				node, level, pos);
			level++;
		}
		else
		{
			currPos = pos;
			return;
		}
	}
	else
	{
//		fwTreeView_appendNode(node_name, parent, node, level);
		fwTreeDisplay_insertNode(node_name, parent, 
			node, level, pos);
		level++;
	}
	fwTree_getChildrenWithClipboard(node, children, exInfo);
	for(i = 1; i <= dynlen(children); i++)
	{
		if(children[i] != node)
		{
//			fwTreeDisplay_appendTreeNodes(treeIndex, node, children[i], level, pos);

			dont = 0;
			child_node_name = "";
			if(fwFsm_isObjectReference(children[i]))
				child_node_name = "&";
			child_node_name += fwTree_getNodeDisplayName(children[i], exInfo);
			if(child_node_name == _fwTree_makeClipboard(FwActiveTrees[treeIndex]))
			{
				if(FwTreeModes[treeIndex] != "navigator")
				{
					child_node_name = "---Clipboard---";
				}
				else
				{
					dont = 1;
				}
			}
			if(!dont)
			{
				fwTreeDisplay_insertNode(child_node_name, node, 
					children[i], level, pos);
			}
		}
	}
	currPos = pos;
}

fwTreeDisplay_insertNode(string node_name, string parent, string data, int level, int &pos)
{
	dyn_anytype node;
	dyn_string children, exInfo;
	int flag = 0;

	fwTree_getChildrenWithClipboard(data, children, exInfo);
	if(dynlen(children))
		flag = 1;

	node[1] = data;
	node[2] = level;
	node[3] = node_name;
	node[4] = flag;
	node[5] = parent;

//DebugN("Inserting",node_name, parent, pos, level);
	fwTreeView_insertNode(node, pos++);	
}

fwTreeDisplay_editorStd(string node, string parent)
{
dyn_string txt, exInfo;
int answer, paste_flag, redo, wait = 0;
string tree;

	redo = 0;
	if(PasteNode == "")
		paste_flag = 0;
	else
		paste_flag = 1;
	if( !fwTree_isClipboard(node, exInfo))
		dynAppend(txt,"PUSH_BUTTON, Add..., 1, 1");
	dynAppend(txt,"PUSH_BUTTON, Remove..., 2, 1");
	dynAppend(txt,"SEPARATOR");
	if((!fwTree_isRoot(node, exInfo)) && (!fwTree_isClipboard(node, exInfo)))
		dynAppend(txt,"PUSH_BUTTON, Cut, 3, 1");
	dynAppend(txt,"PUSH_BUTTON, Paste, 4, "+paste_flag);
	dynAppend(txt,"SEPARATOR");
	if((!fwTree_isRoot(node, exInfo)) && (!fwTree_isClipboard(node, exInfo)))
		dynAppend(txt,"PUSH_BUTTON, Rename, 5, 1");
	dynAppend(txt,"PUSH_BUTTON, Reorder, 6, 1");
	if((!fwTree_isRoot(node, exInfo)) && (!fwTree_isClipboard(node, exInfo)))
	{
		dynAppend(txt,"SEPARATOR");
		dynAppend(txt,"PUSH_BUTTON, Settings, 7, 1");
	}
	popupMenu(txt,answer);
	if(answer == 1)
	{
		fwTreeDisplay_askAddNodeStd(node, redo);
	}
	else if(answer == 2)
	{
		fwTreeDisplay_askRemoveNodeStd(parent, node, redo);
	}
	else if(answer == 3)
	{
		fwUi_informUserProgress("Please wait - Cutting Nodes ...", 100,60);
		PasteNode = node;
		fwTree_cutNode(parent, PasteNode, exInfo);
		fwTreeDisplay_callUser2(
			FwActiveTrees[CurrTreeIndex]+"_nodeCut",
			PasteNode, parent);
		redo = 1;
	}
	else if(answer == 4)
	{
		fwUi_informUserProgress("Please wait - Pasting Nodes ...", 100,60);
		fwTree_pasteNode(node, PasteNode, exInfo);
		fwTreeDisplay_callUser2(
			FwActiveTrees[CurrTreeIndex]+"_nodePasted",
			PasteNode, node);
		PasteNode = "";
		redo = 1;
	}
	else if(answer == 5)
	{
		fwTreeDisplay_askRenameNodeStd(node, 1, redo);
	}
	if(answer == 6)
	{
		fwTreeDisplay_askReorderNodeStd(node, redo);
	}
	if(answer == 7)
	{
		fwTreeDisplay_callUser2(
			FwActiveTrees[CurrTreeIndex]+"_nodeSettings",
			node, parent);
	}
	if(redo)
	{
		fwTreeDisplay_setRedoTree(FwActiveTrees[CurrTreeIndex]);
//DebugN("do setRedoTree",FwActiveTrees[CurrTreeIndex], parent, node);
	}	
	fwTree_getTreeName(parent, tree, exInfo);
	if(tree == "FSM")
		wait = 1;
	fwUi_uninformUserProgress(wait);
}

fwTreeDisplay_navigatorStd(string node, string parent)
{
dyn_string txt, exInfo;
int answer;

	if(!fwTree_isRoot(node, exInfo))
	{
		dynAppend(txt,"PUSH_BUTTON, View, 1, 1");
		popupMenu(txt,answer);
	}
	if(answer == 1)
	{
		fwTreeDisplay_callUser2(
			FwActiveTrees[CurrTreeIndex]+"_nodeView",
			node, parent);
	}
}

fwTreeDisplay_askAddNodeStd(string parent, int &done)
{
	dyn_string ret, exInfo;
	dyn_float res;
	string type, node, label, sys, msg, ref_parent;
	int i, cu, isnode, ref;

	done = 0;
/*
	ref_parent = parent;
	ref = fwFsm_isObjectReference(parent);
	if(ref)
	{
		ref_parent = fwFsm_getReferencedObject(parent);
		if(ref == 1)
			ref_parent = "&"+ref_parent;
	}
*/
	if(fwFsm_isObjectReference(parent))
		ref_parent = "&";
	ref_parent += fwTree_getNodeDisplayName(parent, exInfo);

	ChildPanelOnReturn("fwTreeDisplay/fwTreeAddNodeStd.pnl","Add Node",
		makeDynString(parent, ref_parent, 
		FwTreeTypes[CurrTreeIndex], FwTreeNames[CurrTreeIndex]),
		100,60, res, ret);
	if(res[1])
	{
		msg = "Please Wait - Creating Nodes ...";
		fwUi_informUserProgress(msg, 100,60);

			done = 1;
			type = ret[1];
			cu = ret[2];
			for(i = 3; i <= dynlen(ret); i+=2)
			{
				node = ret[i];
				label = ret[i+1];
/*
				isnode = fwTree_isNode(label, exInfo);
				if(isnode)
				{
					fwFsm_makeObjectReference(label, label);
//					fwUi_warnUser("Node "+label+" already exists, Not created", 
//					100,60);
//					done = 0;
//					return;
				}
				fwTree_addNode(parent, label, exInfo);
*/
				label = fwTree_createNode(parent, label, exInfo);
				if(node != "")
				{
					sys = fwFsm_getSystem(node);
					if(sys == "")
					{
						sys = fwFsm_getSystemName();
						node = sys+":"+node;
					}
				}
				fwTree_setNodeDevice(label, node, type, exInfo);
				fwTree_setNodeCU(label, cu, exInfo);
				fwTreeDisplay_callUser2(
					FwActiveTrees[CurrTreeIndex]+"_nodeAdded",
					label, parent);
			}
	}
}

fwTreeDisplay_removeNode(string parent, string node)
{
int i, cu;
dyn_string children, exInfo;

	fwTreeDisplay_callUser2(
		FwActiveTrees[CurrTreeIndex]+"_nodeRemoved",
		node, parent);
/*
	fwTree_getChildren(node, children, exInfo);
	if(dynlen(children))
	{
		for(i = 1; i <= dynlen(children); i++)
		{
			fwTree_cutNode(node, children[i], exInfo);
		}
	}
*/
	fwTree_removeNode(parent, node, exInfo);
}

fwTreeDisplay_removeSubTree(string parent, string node)
{
dyn_string children, exInfo;
int i;

	fwTree_getChildren(node, children, exInfo);
	for(i = 1; i <= dynlen(children); i++)
	{
		fwTreeDisplay_removeSubTree(node, children[i]);
	}
	fwTreeDisplay_removeNode(parent, node);
}

string fwTreeDisplay_askRemoveNodeStd(string parent, string node, int &done)
{
	dyn_string ret, exInfo;
	dyn_float res;
	string msg, ref_node, ret_parent;
	int i, tree_flag, ref;

	done = 0;
/*
	ref_node = node;
	ref = fwFsm_isObjectReference(node);
	if(ref)
	{
		ref_node = fwFsm_getReferencedObject(node);
		if(ref == 1)
			ref_node = "&"+ref_node;
	}
*/
	if(fwFsm_isObjectReference(node))
		ref_node = "&";
	ref_node += fwTree_getNodeDisplayName(node, exInfo);

	ChildPanelOnReturn("fwTreeDisplay/fwTreeRemoveNodeStd.pnl","Remove Node",
		makeDynString(node, ref_node),
		100,60, res, ret);
	if(res[1])
	{
		msg = "Please Wait - Removing Nodes ...";
		fwUi_informUserProgress(msg, 100,60);
		done = 1;
		tree_flag = ret[1];
		ret_parent = parent;
		if(tree_flag == 0)
		{
			fwTreeDisplay_removeNode(parent, node);
		}
		else if(tree_flag == 1)			
		{
			fwTreeDisplay_removeSubTree(parent,node);
		}
		else
		{
			ret_parent = node;
			for(i = 2; i <= dynlen(ret); i++)
			{
				fwTreeDisplay_removeSubTree(node, ret[i]);
			}
		}
	}
	return ret_parent;
}

string fwTreeDisplay_askRenameNodeStd(string node, int allow_duplicate, int &done)
{
	dyn_string ret, exInfo;
	dyn_float res;
	int ref;
	string ref_node, new_node;

	new_node = "";
/*
	ref_node = node;
	ref = fwFsm_isObjectReference(node);
	if(ref)
	{
		ref_node = fwFsm_getReferencedObject(node);
		if(ref == 1)
			ref_node = "&"+ref_node;
	}
*/
	if(fwFsm_isObjectReference(node))
		ref_node = "&";
	ref_node += fwTree_getNodeDisplayName(node, exInfo);

	ChildPanelOnReturn("fwTreeDisplay/fwTreeRenameNodeStd.pnl","Rename Node",
		makeDynString(ref_node),
		100,60, res, ret);
	if(res[1])
	{
		new_node = ret[1];
		if(fwTree_isNode(new_node, exInfo))
		{
/*
			if(allow_duplicate)
			{
//				fwFsm_makeObjectReference(new_node, new_node);
				new_node = fwTree_makeNode(new_node, exInfo);
			}
			else
			{
				fwUi_warnUser("This node already exists",100,60);
					return "";
			}
*/
			if(!allow_duplicate)
			{
				fwUi_warnUser("This node already exists",100,60);
					return "";
			}
		}
		done = 1;
		new_node = fwTree_renameNode(node, new_node, exInfo);
		if(!globalExists("LastRenamedNode"))
			addGlobal("LastRenamedNode",STRING_VAR);
		LastRenamedNode = new_node;
		fwTreeDisplay_callUser2(
			FwActiveTrees[CurrTreeIndex]+"_nodeRenamed",
			node, new_node);
	}
	return new_node;
}

fwTreeDisplay_askReorderNodeStd(string node, int &done)
{
	dyn_string ret, exInfo, children;
	dyn_float res;

	done = 0;
	fwTree_getChildren(node, children, exInfo);
	ChildPanelOnReturn("fwTreeDisplay/fwTreeReorderNodeStd.pnl","Re-order Children",
		makeDynString(node),
		100,60, res, ret);
	if(res[1])
	{
		if(children != ret)
		{
			done = 1;
			fwTree_reorderChildren(node, ret, exInfo);
			fwTreeDisplay_callUser1(
				FwActiveTrees[CurrTreeIndex]+"_nodeReordered",
				node);
		}

	}
}

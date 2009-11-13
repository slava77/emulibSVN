
#uses "majority_treeCache/majorityLib.ctl"
#uses "majority_treeCache/treeCache.ctl"

// ===> PLEASE CHANGE <==== 
// Include here your specific majorityUser library
#uses "CMS_CSC_majority/majorityUser.ctl"

main() {
  majority_debug4("Starting majority on system " + getSystemName());
  string topNode = "CMS_CSC_majorTest"; // set your top node here    <==== PLEASE CHANGE!!
  majority_setForceValid(true); 
  majority_start(topNode,1 // debug level
		 );

  // you may want to pass a third parameter that is the treeCache topNode if you need to use 
  //  the same treeCache but with different topNodes for different majorities
			 
  // that's all
}

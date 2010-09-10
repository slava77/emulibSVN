

main(){

while(1){
file fp;
string tmp;
int count;
string tmp1;
int pos_cr;

mudcsInit();

  system("ls -1 "+CSC_fwG_g_project_name_home+"/scripts/libs/* -1 "+CSC_fwG_g_project_name_home+"/scripts/libs/*/* > old_ver_temp;");
  fp = fopen("old_ver_temp", "r");
  if(fp){
   while(1){
   count=fgets(tmp,200,fp);
   if(!count)break;
   if(strpos(tmp,"~") >= 0) {
    if((pos_cr=strpos(tmp,"\n"))>=0){
     tmp1=substr(tmp,0,pos_cr);
    }
    else{
     tmp1=tmp;
    }

      system("rm -v "+"'"+tmp1+"'");
      DebugTN("rm -v "+tmp1+"");
   }
   
   }
//   DebugN(""+count);

  }
  delay(5,0);
/// system("/home/fast/dim/linux/dns &");
}


}

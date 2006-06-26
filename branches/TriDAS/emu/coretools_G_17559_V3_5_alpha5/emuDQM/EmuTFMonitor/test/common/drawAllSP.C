void drawAllSP(const char *filename){
	TFile f(filename);

	gStyle->SetCanvasDefH(1000);
	gStyle->SetCanvasDefW(1200);

	gStyle->SetPalette(1,0);
	gStyle->SetTitleW(0.95);
	gStyle->SetTitleH(0.085);

	TString fn(filename);
	fn = fn(0,fn.Last('.'));
	fn.Append(".plots");

	char command[128];
	sprintf(command,"mkdir -p picts/%s",fn.Data());
	gSystem->Exec(command);
	filename = fn.Data();

	TIterator *keys = f.GetListOfKeys()->MakeIterator();

	TKey *key=0;
	while( ( key=(TKey*)keys->Next() ) ){
		if( strncmp(key->GetClassName(),"TCanvas",7) ) continue;
		TObject *obj = key->ReadObj();

		TString fullPath(obj->GetName()), basename, dirname;
		Ssiz_t pos=0;
		if( (pos = fullPath.Last('/')) >=0 ){
			basename = fullPath(pos+1,fullPath.Length()-pos-1);
			dirname  = fullPath(0,pos);
			cout<<"basename: "<<basename<<" dirname: "<<dirname<<endl;
		} else basename = fullPath;

		pos=5;
		while(
			basename[pos]=='1' ||
			basename[pos]=='2' ||
			basename[pos]=='3' ||
			basename[pos]=='4' ||
			basename[pos]=='5' ||
			basename[pos]=='6' ||
			basename[pos]=='7' ||
			basename[pos]=='8' ||
			basename[pos]=='9' ||
			basename[pos]=='0' ||
			basename[pos]=='_' ) pos++;

		if( pos > 5 ){
			TString location = basename(5,pos).Data();
			Ssiz_t space = location.First('_');
			TString sSP(location(0,space));
			int SP  = atoi(sSP.Data());
			int FN  = (pos>7?atoi(location(space+1,1).Data()):-1);
			int CSC = (pos>9?atoi(location(space+3,pos-space-3).Data()):-1);
			cout<<"SP: "<<SP<<" FN: "<<FN<<" CSC: "<<CSC<<endl;
			char command[128];
			if(FN>0 && CSC>0){
				sprintf(command,"mkdir -p picts/%s/SP%d/F%d/CSC%d/%s",filename,SP,FN,CSC,dirname.Data());
				gSystem->Exec(command);
				obj->Draw();
				((TCanvas*)obj)->SetCanvasSize(1200,1000);
				char gifFile[512];
				sprintf(gifFile,"picts/%s/SP%d/F%d/CSC%d/%s/%s.png",filename,SP,FN,CSC,dirname.Data(),basename(pos,basename.Length()-pos).Data());
				((TCanvas*)obj)->Print(gifFile);
			}
			if(FN>0 && CSC<0){
				sprintf(command,"mkdir -p picts/%s/SP%d/F%d/%s",filename,SP,FN,dirname.Data());
				gSystem->Exec(command);
				obj->Draw();
				((TCanvas*)obj)->SetCanvasSize(1200,1000);
				char gifFile[512];
				sprintf(gifFile,"picts/%s/SP%d/F%d/%s/%s.png",filename,SP,FN,dirname.Data(),basename(pos,basename.Length()-pos).Data());
				((TCanvas*)obj)->Print(gifFile);
			}
			if(FN<0 && CSC<0){
				sprintf(command,"mkdir -p picts/%s/SP%d/%s",filename,SP,dirname.Data());
				gSystem->Exec(command);
				obj->Draw();
				((TCanvas*)obj)->SetCanvasSize(1200,1000);
				char gifFile[512];
				sprintf(gifFile,"picts/%s/SP%d/%s/%s.png",filename,SP,dirname.Data(),basename(pos,basename.Length()-pos).Data());
				((TCanvas*)obj)->Print(gifFile);
			}
			delete obj;
		} else {
			char command[128];
			sprintf(command,"mkdir -p picts/%s/%s",filename,dirname.Data());
			gSystem->Exec(command);
			obj->Draw();
			((TCanvas*)obj)->SetCanvasSize(1200,1000);
			char gifFile[512];
			sprintf(gifFile,"picts/%s/%s.png",filename,fullPath(5,basename.Length()-5).Data());
			((TCanvas*)obj)->Print(gifFile);
			delete obj;
		}
	}
exit(0);
}

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLegend.h"
#include <iostream>
#include <cmath>

void test_28(){

  //filled for chamber 202, july 4

  const Int_t n = 13;
 
  Double_t V[ n] = {3.20,3.25,3.30,3.35,3.40, 3.45, 3.50, 3.55, 3.60, 3.65, 3.70, 3.75, 3.8};

  Double_t ALCT[n]   = {307,771,1632,2946,3913,4293,4469,4614,4551,4531,4611,4674,4794};
  Double_t ALCT_a[n] = {325,749,1604,2942,3894,4311,4327,4581,4472,4557,4595,4744,4755};
  Double_t ALCT_b[n] = {299,678,1726,2874,3868,4317,4477,4457,4607,4635,4735,4753,4763};

  Double_t CLCT[n]   = {55,153,387,1012,2066,2904,3400,3590,3667,3674,3705,3790,3803};
  Double_t CLCT_a[n] = {53,127,361,1041,2022,2959,3306,3604,3648,3622,3683,3790,3787};
  Double_t CLCT_b[n] = {61,143,416,997,1997,2931,3438,3562,3771,3729,3839,3787,3840};

  Double_t L1A[n]    = {28,83,126,684,1552,2318,2830,3043,3104,3113,3197,3204,3289};
  Double_t L1A_a[n]  = {28,81,210,681,1489,2394,2731,3030,3110,3075,3169,3287,3297};
  Double_t L1A_b[n]  = {28,84,258,694,1519,2352,2817,2989,3174,3184,3324,3264,3321};

  Double_t LCT[n] = {1.5,1.5, 1.5, 1.5 ,1.5 ,1.5 ,30471 ,1.5 ,1.5, 1.5,1.5};
  Double_t eV[n],eALCT[n],eCLCT[n],eL1A[n],eLCT[n],mALCT[n],mCLCT[n],mL1A[n];
  /*
  for (Int_t i=0; i<n;i++){
    eV[i]=0;
    eALCT[i]=sqrt(ALCT[i]);
    eCLCT[i]=sqrt(CLCT[i]);
    eL1A[i]=sqrt(L1A[i]);
    eLCT[i]=sqrt(LCT[i]);
  }
  
  TGraphErrors* gr_ALCT= new TGraphErrors(n,V,ALCT,eV,eALCT);
  gr_ALCT->SetMarkerStyle(20);
  TGraphErrors* gr_CLCT= new TGraphErrors(n,V,CLCT,eV,eCLCT);
  gr_CLCT->SetMarkerStyle(21);
  TGraphErrors* gr_L1A= new TGraphErrors(n,V,L1A,eV,eL1A);
  gr_L1A->SetMarkerStyle(22);
  TGraphErrors* gr_LCT= new TGraphErrors(n,V,LCT,eV,eLCT);
  gr_LCT->SetMarkerStyle(23);
  */
  for (Int_t i=0; i<n;i++){
    mALCT[i]=1./3.*0.1*(ALCT[i]+ALCT_a[i]+ALCT_b[i]);
    mCLCT[i]=1./3.*0.1*(CLCT[i]+CLCT_a[i]+CLCT_b[i]);
    mL1A[i] =1./3.*0.1*(L1A[i] + L1A_a[i]+ L1A_b[i]);
  }
  for (Int_t i=0; i<n;i++){
    eV[i]=0;
    eALCT[i]=sqrt(1./2.*(pow(0.1*ALCT[i]-mALCT[i],2)+ pow(0.1*ALCT_a[i]-mALCT[i],2)+pow(0.1*ALCT_b[i]-mALCT[i],2)));
    eCLCT[i]=sqrt(1./2.*(pow(0.1*CLCT[i]-mCLCT[i],2)+ pow(0.1*CLCT_a[i]-mCLCT[i],2)+pow(0.1*CLCT_b[i]-mCLCT[i],2)));
    eL1A[i]=sqrt(1./2.*(pow(0.1*L1A[i]-mL1A[i],2)+ pow(0.1*L1A_a[i]-mL1A[i],2)+pow(0.1*L1A_b[i]-mL1A[i],2)));
    std::cout<<"V/A/eA/C/eC/L/eL "<<V[i]<<"/"<<mALCT[i]<<"/"<<eALCT[i]<<"/"<<mCLCT[i]<<"/"<<eCLCT[i]<<"/"<<mL1A[i]<<"/"<<eL1A[i]<<std::endl;
  }

  TGraphErrors* gr_ALCT= new TGraphErrors(n,V,mALCT,eV,eALCT);
  gr_ALCT->SetMarkerStyle(20);
  TGraphErrors* gr_CLCT= new TGraphErrors(n,V,mCLCT,eV,eCLCT);
  gr_CLCT->SetMarkerStyle(21);
  TGraphErrors* gr_L1A= new TGraphErrors(n,V,mL1A,eV,eL1A);
  gr_L1A->SetMarkerStyle(22);
  TGraphErrors* gr_LCT= new TGraphErrors(n,V,LCT,eV,eLCT);
  gr_LCT->SetMarkerStyle(23);

  TCanvas* can= new TCanvas("TMB_canvas", "TMB Counters, 3/4 trigger", 200,10,600,400);
  can->SetFillColor(0);
  can->SetBorderSize(0);
  can->SetBorderMode(0);
  gr_ALCT->GetXaxis()->SetTitle("V [kV]");
  gr_ALCT->GetXaxis()->CenterTitle();
  gr_ALCT->GetYaxis()->SetTitle("TMB rates [Hz]");
  gr_ALCT->GetYaxis()->CenterTitle();

  gr_ALCT->Draw("APE,same");
  gr_CLCT->Draw("PE");
  gr_L1A->Draw("PE");
  //gr_LCT->Draw("PE");

  TLegend* leg = new TLegend(0.60, 0.20,0.90, 0.35);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->AddEntry(gr_ALCT, "ALCT0 rate [Hz]", "P");
  leg->AddEntry(gr_CLCT, "CLCT0 rate [Hz]", "P");
  leg->AddEntry(gr_L1A, "L1A rate [Hz]", "P");
  leg->Draw();

}

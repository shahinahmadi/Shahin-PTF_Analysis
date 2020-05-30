// This class is used to calculate the gain of 20-in PMT
#include "PTFGainAnalysis.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"
#include "PTFAnalysis.hpp"
#include <iostream>
#include<fstream>
#include<cmath>
#include<string>
#include<vector>
#include<cmath>
#include<TStyle.h>
#include"TMath.h"
#include<TStyle.h>
using namespace std;

Double_t fitfunc1(Double_t *x, Double_t *par)
{
  Double_t arg=0;
  if(par[2]!=0) arg=(x[0]-par[1])/par[2];
  Double_t gfunc1=par[3]+par[0]*TMath::Exp(-0.5*arg*arg);
  return gfunc1;
}

Double_t fitfunc2(Double_t *x, Double_t *par)
{
  Double_t gfunc2=TMath::Exp(par[0]+par[1]*x[0]);
  return gfunc2;
}




PTFGainAnalysis::PTFGainAnalysis( TFile *outfile, PTFAnalysis *ptfanalysis, PTF::Wrapper &wrapper){
  
  
  Sum  =new TH1D("Summation","Sum over many points ",1000,0,6000);
  Sum->GetXaxis()->SetTitle("Gain of PMT");
  Sum->GetYaxis()->SetTitle("Number of events");

  vector<ScanPoint>scanpoints=ptfanalysis->get_scanpoints();
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    string name = "Gain_hist" + std::to_string(iscan);
    Gain_hist = new TH1D(name.c_str(),"Charge_distribution of some pulses on the PMT",1000,0,6000);
    Gain_hist->GetXaxis()->SetTitle("Charge distribution(ADC unit)");
    Gain_hist->GetYaxis()->SetTitle("The number of events");
    Gain_hist->GetXaxis()->CenterTitle();
    Gain_hist->GetYaxis()->CenterTitle();

   
    ScanPoint scanpoint = scanpoints[iscan ];
    long long entry = scanpoint.get_entry();
    double integ_result;
    //Loop through fit results
    //  if(iscan==16){
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
      if (fit_result.haswf){
	
	integ_result=0;
	int n=100000;
	double dx;
	double xmax=wrapper.getSampleLength();
	double xmin=0;
	dx=(xmax-xmin)/n;
    
	for (int i=0; i<n;i++){
	  double xi=xmin+i*dx;
	  double functionValue=(fit_result.amp * exp( -0.5 * pow( (xi-fit_result.mean)/(fit_result.sigma), 2.0 )));
	  double integral=functionValue*dx;
	  integ_result+=integral;
	}
	cout<<"This is integ_results"<<integ_result<<endl;
        Gain_hist->Fill(integ_result);
      }
    }
    Sum->Add(Gain_hist);
	  
	  
    TF1 *f1=new TF1("m1",fitfunc1,100.0,820.0,4);
    f1->SetParameters(13000.0,400.0,166.8,4000.0);
    // Sum ->Fit("m1","","",110.0,780.0);
    gStyle->SetOptFit(1011);
    Sum->GetListOfFunctions()->Add(f1);

    
    TF1 *f2=new TF1("m2",fitfunc2,800.0,1500.0,2);
    f2->SetParameters(12.4949,-0.00499041);
    // Sum->Fit("m2","","",780.01,1500.0);
    gStyle->SetOptFit(1011);
    Sum->GetListOfFunctions()->Add(f2);
    Sum->Draw();
    
  
    
  }
}

  
      

 
     

 

    
   
   
  

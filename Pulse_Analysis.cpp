/********SK PMT signal analyzer script*********/
/********Authors:Mia Kramer, Shahin Ahmadi******************/
/*This script takes 36 points on the PMT and for each point analyze 10(or NumSamples ). The time length of analyzing is 70 for each recieved sample(waveform)*/
#include "wrapper.hpp"
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <unordered_set>
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include <TVirtualFitter.h>
#include<TFitter.h>
#include <cmath>
#include "TMath.h"
#include <TStyle.h>
using namespace std;
string* name_sum;
//Gaussian fitting function
Double_t fitfunc(Double_t *x, Double_t *par)
{
  Double_t arg=0;
  if(par[2]!=0) arg=(x[0]-par[1])/par[2];
  Double_t gfunc=-par[0]*TMath::Exp(-0.5*arg*arg)+par[3];
  return gfunc;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "Enter the run number.";
  }
  // Opening  a root file
  TFile outFile("histograms_average_waveform.root", "RECREATE");
  
  const string run_no = argv[1];
  string root_f = "/home/shahin/ptf-analysis-2/data/out_run0" + run_no + ".root";
  string csv_f  = "/home/shahin/ptf-analysis-2/data/out_run0" + run_no + ".csv";
  
  vector<int> phidgets = {0, 1, 3};
  vector<PTF::PMTChannel> activeChannels = {
    {0, 0}
    //   {1, 4},
    //   {2, 5},
    //   {3, 6},
    //   {4, 7}, 
    //   {5, 8},
    //   {6, 9},
    //   {7, 10}
  };
  PTF::Wrapper wrapper = PTF::Wrapper(16384, 70, activeChannels, phidgets);
  
  unordered_set<int> skipLines = {};// {962,1923,2884,5240,6201,9611,10572,11533,12494,13455,15811,16771};
  
  wrapper.openFile(root_f, "scan_tree");
  //ofstream csv(csv_f);
  
  cerr << "Num entries: " << wrapper.getNumEntries() << endl << endl;
  
  //csv << "X0,Y0,Z0, Bx0,By0,Bz0, Bx1,By1,Bz1, Bx3,By3,Bz3" << endl;
  
  uint32_t lines = 0;
  const uint32_t freq = 100;
  /*************First loop on number of points on the PMT***************/
 
  for (int i = 0; i < wrapper.getNumEntries(); i++) {
    if (i % freq == 0 || i == wrapper.getNumEntries() - 1) {
      cerr << "Entry " << i << "/" << wrapper.getNumEntries() << "\u001b[34;1m (" << (((double)i)/wrapper.getNumEntries()*100) << "%)\u001b[0m\033[K";
      if (skipLines.find(i) != skipLines.end()) {
        cerr << "\u001b[31;1m Skipping...\u001b[0m\r";
        continue;
      } else {
        cerr << "\r";
      }
    }
    
    if (skipLines.find(i) != skipLines.end()) continue;
    
    lines++;
    wrapper.setCurrentEntry(i);
    
    auto location = wrapper.getDataForCurrentEntry(PTF::Gantry1);
    
    cerr << location.x << "," << location.y << "," << location.z << endl;
    
    for (auto  Channel : activeChannels)
      {
	//get number of samples
	int  numSamples= wrapper.getNumSamples();
	// Creating two arrays or vectors which store bin content and its squared
	double	arr1[70];
	double arr2[70];
	double sigma = 0;
	double mean;
        vector <TH1D*> hist;
	for(int k=0;k<70;k++){
	  arr1[k] = 0;
	  arr2[k] = 0;
	}
	/*****************Second loop; looping over the number of samples***************/
	for ( int j=0; j<10; j++) {
	   
	   
	  string name_sum = "single_pulse" + std::to_string(i)+to_string(j); // change so you have unique file name
	  auto  single_pulse=new TH1D(name_sum.c_str(), "Pulse waveform; Sample length;Collected charges",70,0, 70);// To not reapiting the data of 
	  //  Getting  pmt sample
	  hist.push_back(single_pulse);
	  double* pmtsample=wrapper.getPmtSample( Channel.pmt, j);
	  /*********************Third loop; looping over the length of sample******************/
	  for(int k=0; k<wrapper.getSampleLength(); k++){
	    //Getingt bin content for h
	    double bin_value = single_pulse->GetBinContent(k+1);
	    // Calculating new bin content
	    bin_value = bin_value + pmtsample[k];
	    cout<<bin_value << " " << pmtsample[k] <<endl;
	    //Set new bin content
	    single_pulse->SetBinContent(k+1,bin_value);
	    //Storing the numbers from histogram to arrays
	    arr1[k]+=bin_value;
	    arr2[k]+=(bin_value)*(bin_value);
	  }
	  //  h1_sum->Scale(1./numSamples).Shahin: I do not know why we should Scale to the inverse of number of samples
	  // Setting the fitting parameters
	  TF1 *f1=new TF1("m1","pol6",0,24);
	  TF1 *f2=new TF1("m2","pol6",24,48);
          TF1 *f3=new TF1("m3","pol6",48,70);
          TF1 *total = new TF1("mstotal","pol6(0)+pol6(7)+pol6(14)",0,70);
	  Double_t par[21];
	  
	  single_pulse->Fit(f1,"R");
	  single_pulse->Fit(f2,"R+");
	  single_pulse->Fit(f3,"R+");

	  // Get the parameters from the fit
	  f1->GetParameters(&par[0]);
	  f2->GetParameters(&par[7]);
	  f3->GetParameters(&par[14]);
	  gStyle->SetOptFit(1011);
	}
	/************Looping over sample length**********/ 
	for(int k=0;k<70;k++){ 
	  mean=arr1[k]/10;
	  sigma=sqrt(arr2[k]/10-(mean*mean));
	  cout<< mean << " " <<sigma<<endl;
	  //  for(int l=(i)*10;l<i*10+10;l++)
	    for(int l=0;l<hist.size();l++)
	      {
	      //  cout<<i << " " <<hist.size()<<" "<< l<<endl;
	      
	      hist.at(l)->SetBinError(k+1,sigma);
	     
	      
	      }
	   
	}
	for(int m=0; m<hist.size();m++)
	  hist.at(m)->Write();
	
      }
	
      
    
    outFile.cd();
 
    
  }
  outFile.Close();
  
  
  cout<<"Done"<<endl; 
}




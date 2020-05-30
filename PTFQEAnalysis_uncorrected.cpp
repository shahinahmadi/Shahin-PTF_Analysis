// This script calculates the uncorrected detection efficiency of 20 in PMT
#include "PTFQEAnalysis.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"
#include <iostream>
#include<fstream>

using namespace std;

PTFQEAnalysis::PTFQEAnalysis( TFile *outfile, PTFAnalysis *ptfanalysis ){

  vector< double > xbins = ptfanalysis->get_bins( 'x' );
  vector< double > ybins = ptfanalysis->get_bins( 'y' );

  //Create detection efficiency histogram
  d_efficiency = new TH2D("d_efficiency", "Detection efficiency", xbins.size()-1, &xbins[0], ybins.size()-1, &ybins[0]);
  d_efficiency->GetXaxis()->SetTitle("X position on PMT(m)");
  d_efficiency->GetYaxis()->SetTitle("Y position on PMT(m)");
  d_efficiency->GetXaxis()->CenterTitle();
  d_efficiency->GetYaxis()->CenterTitle();
  vector<ScanPoint>scanpoints=ptfanalysis->get_scanpoints();
  double average=0;
  double eff;
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    ScanPoint scanpoint = scanpoints[iscan ];
    long long entry = scanpoint.get_entry();
    
    //Loop through fit results and calculate the efficiency
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
    
      eff=  (double)fit_result.haswf/(double)scanpoint.nentries();     
      
      d_efficiency->Fill(fit_result.x, fit_result.y,eff);
      average+=eff;
     
    }
         
     
  }
  average/=scanpoints.size();

  cout<<average<<endl;
 
  // Calculation of the average of efficiency  over each 10 points of scans
  double arr1[scanpoints.size()/10];
  double efficiency;
  for (unsigned int i=0;i<scanpoints.size()/10;i++){arr1[i]=0;}
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    ScanPoint scanpoint = scanpoints[iscan ];
    long long entry = scanpoint.get_entry();
    cout<<"It works2"<<endl;
    //Loop through fit results
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
    
      efficiency=(double)fit_result.haswf/(double)scanpoint.nentries();
      int  i=iscan/10;
     
      arr1[i]+=efficiency/10;
      
    }
  }


}

 
 



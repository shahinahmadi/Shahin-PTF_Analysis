// This script calculates the detection efficiency of monitor PMT for different positions of gantry, calculates the average of detection efficiency for each 20 points and takes the avarage over those points, then calculates the average over each 18 points  and finally calculates the ratios between the total  avarage and subaverage  values. These ratios are used, then, in another script to normalize the detection efficiency of main PMT
#include "PTFQEAnalysis.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"
#include<iostream>
#include<fstream>
using namespace std;

PTFQEAnalysis::PTFQEAnalysis( TFile *outfile, PTFAnalysis *ptfanalysis ){

  vector< double > xbins = ptfanalysis->get_bins( 'x' );
  vector< double > ybins = ptfanalysis->get_bins( 'y' );

  //Create detection efficiency histogram
  d_efficiency = new TH2D("d_efficiency", "Detection efficiency(Monitor of  g1)", xbins.size()-1, &xbins[0], ybins.size()-1, &ybins[0]);
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
    //Loop through fit results
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
    
      eff=  (double)fit_result.haswf/(double)scanpoint.nentries();     
      
      d_efficiency->Fill(fit_result.x, fit_result.y,eff);
      average+=eff;
     
    }
         
    
  }
  average/=scanpoints.size();
 
 
  
  double arr1[scanpoints.size()/18];
  for (int i=0;i<scanpoints.size()/18;i++){arr1[i]=0;}
 
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    ScanPoint scanpoint = scanpoints[iscan ];
    long long entry = scanpoint.get_entry();

    //Loop through fit results
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
    
      double efficiency=(double)fit_result.haswf/(double)scanpoint.nentries();
      int index = iscan/18;
      arr1[index]+=efficiency/18;
      
     
    }
  }
 
  for (int i=0;i<scanpoints.size()/18;i++){
    ofstream myfile;
    myfile.open ("Coefficient.txt", ios::app);
   
    cout<<average/arr1[i]<<endl;
  
  
    myfile <<average/arr1[i]<<endl;
  }
}

 
 



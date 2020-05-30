// This script uses the coefficients produced by monitor PMT to normalize the detection efficiency of main 20 in PMT
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
  vector<double> numbers;
  double number;

  ifstream myfile("Coefficient.txt"); //file opening constructor

  //Operation to check if the file opened
  if ( myfile.is_open()){
    
    while(myfile >>number ){

      //putting data into the vector
      numbers.push_back(number);
          

    }
  }
  //   Conversion of vector to array 
  int n=numbers.size();
  double a[n];
  for (int i=0; i<numbers.size();i++){
    a[i]=numbers.at(i);
    cout<<a[i]<<endl;}
  //Taking the scan points and the related wavelength
  vector<ScanPoint>scanpoints=ptfanalysis->get_scanpoints();
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    ScanPoint scanpoint = scanpoints[iscan];
    long long entry = scanpoint.get_entry();
    //Loop through fit results
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
    
      double efficiency=(double)fit_result.haswf/(double)scanpoint.nentries();
      int index = iscan/18;
      //Normalization of detection efficiency using the coefficients 
      
      d_efficiency->Fill(fit_result.x, fit_result.y,a[index]*efficiency);
     
       
     
    }
    
  }
}
    
 



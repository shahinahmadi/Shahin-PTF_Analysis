// This class is used to calculate the gain of 20-in PMT at each scanned point
#include "PTFGainAnalysis.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"
#include "PTFAnalysis.hpp"
#include <iostream>
#include<fstream>
#include<cmath>
#include<string>
using namespace std;

PTFGainAnalysis::PTFGainAnalysis( TFile *outfile, PTFAnalysis *ptfanalysis, PTF::Wrapper & wrapper){
  // Binning the 2D histogram
  vector< double > xbins = ptfanalysis->get_bins( 'x' );
  vector< double > ybins = ptfanalysis->get_bins( 'y' );
  // Constructing a 2D histogram according to the defined bins
  Gain_2Dhist = new TH2D("Gain", "Gain of 20-in PMT", xbins.size()-1, &xbins[0], ybins.size()-1, &ybins[0]);
  Gain_2Dhist->GetXaxis()->SetTitle("X position of laser source on the PMT");
  Gain_2Dhist->GetYaxis()->SetTitle("Y position of laser source on the PMT");
  Gain_2Dhist->GetXaxis()->CenterTitle();
  Gain_2Dhist->GetYaxis()->CenterTitle();
 
  vector<ScanPoint>scanpoints=ptfanalysis->get_scanpoints();
  double arr[3697]={0};
  double mean;
  
  //Looping over each scan point and getting the related entries
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    ScanPoint scanpoint = scanpoints[iscan ];
    long long entry = scanpoint.get_entry();
    

    vector<double>integ;//
    //Looping over the waveforms for each scan point
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      // Getting the fit results of waveforms for each scan point and checking if it has a waveform
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
      if (fit_result.haswf){

  
	// Calculating the integration of pulses
	double	integ_result=0;
	int n=100000;
	double dx;
	double xmax=wrapper.getSampleLength();
	double xmin=0.0;
	dx=(xmax-xmin)/n;
    
	for (int i=0; i<n;i++){
	  double xi=xmin+i*dx;
	
	  double functionValue=(fit_result.amp * exp( -0.5 * pow( (xi-fit_result.mean)/(fit_result.sigma), 2.0 )));
	
	  double integral=functionValue*dx;
	  integ_result+=integral;

	}
        
	// Putting the results of integrations of pulses into the vector for each scan point and calculating the mean value for each scan point
      	integ.push_back(integ_result);

	arr[iscan]+=integ_result;
      }
       
       }
    // if (integ.size()!=0)
    mean=(double)arr[iscan]/(double)integ.size();
    // cout<<"Mean is"<<mean<<endl;
     WaveformFitResult fit_result;
      for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
       entry += iwavfm;
      // Ploting the two dimentional histogram of gain,which shows the gain for every point on the PMT 
     fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
  }
      cout<<"It works 10"<<endl;
      if((((fit_result.x-0.4)*(fit_result.x-0.4))+((fit_result.y-0.4)*(fit_result.y-0.4)))<0.053){

	cout<<"It works 11"<<endl;
      
      Gain_2Dhist->Fill(fit_result.x,fit_result.y,(double)mean); 
      cout<<"It works 12"<<endl;
      }
}
}

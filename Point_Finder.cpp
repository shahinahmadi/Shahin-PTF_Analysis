#include "PTFQEAnalysis.hpp"
#include "ScanPoint.hpp"
#include "WaveformFitResult.hpp"
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
  for(unsigned int iscan=0; iscan<scanpoints.size(); iscan++){
    ScanPoint scanpoint = scanpoints[iscan];
    long long entry = scanpoint.get_entry();

    //Loop through fit results
    for(unsigned long long iwavfm=0; iwavfm<scanpoint.nentries(); iwavfm++){
      entry += iwavfm;
      const WaveformFitResult fit_result = ptfanalysis->get_fitresult(iscan, iwavfm);
      double  efficiency1=(double)fit_result.haswf/(double)scanpoint.nentries();
      int index = iscan/20;
      d_efficiency->Fill(fit_result.x, fit_result.y,efficiency1);
    }
  }
  
  //Getting the desired scan point (Maximum efficiency corresponding point)
  double a[36];
  for (int i = 0; i <d_efficiency->GetSize(); i++){
    double eff= d_efficiency->GetBinContent(i);
    cout<<"THIS IS YOUR EFFICIENCY"<< eff<<endl;
    ofstream Myeff;
    Myeff.open ("me.txt",ios::app);
    if (eff!=0){
      Myeff<<eff<<endl;
    }    
  }
}
   
    




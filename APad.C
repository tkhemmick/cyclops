#include <iostream>

#include "APad.h"

#include "TObject.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TBox.h"
#include "cyclops.h"

#include <cmath>   //don't put this in class header.
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "TPolyLine.h"
#include "TH1D.h"
#include "TF1.h"
#include "TSpectrum.h"

using namespace std;
int APad::nextID=0;

bool APad::FastQ = true;
bool APad::UseSigma = true;
std::vector<int> APad::Raw[NumberOfPads];
std::vector<double> APad::Cal[NumberOfPads];

double APad::Pedestals[NumberOfPads] = {0};
double APad::Sigmas[NumberOfPads] = {0};
double APad::Gains[NumberOfPads] = {0};
std::vector<double> APad::CommonMode;

TH1D *APad::Pulse=0; /* @TODO Do we need this? */
TF1  *APad::blue=0; /* @TODO Do we need this? */

double APad::SigmaCut=3;
double APad::PulseCut=40;
string APad::CommonModeMethod="gauss";

APad::APad(double X1, double Y1, double X2, double Y2, double Z0)
{
  // Set the patameters define the pad plane space.
  x1 = X1;
  y1 = Y1;
  x2 = X2;
  y2 = Y2;
  z0 = Z0;
  q  = 0;

  // If there are "empty" channels, the user will
  // likely overwrite myID using SetMyID()...
  myID = nextID++;
}

void APad::Draw( double MAX )
{
  // color pad according to charge fraction of total deposited charge
  TBox* apada = new TBox(x1,y1,x2,y2);

  apada->SetLineStyle(1);
  apada->SetLineWidth(2);
  apada->SetLineColor(15);

  apada->SetFillStyle(1001);

  // draw pad contour
  apada->SetFillColor(kGray);
  apada->Draw("");

  // draw pad with color
  if ( IsHit() && color((int)(MAX+0.5)) )
    {
      apada->SetFillColor(color((int)(MAX+0.5)));
      apada->Draw("");
    }

}

int APad::color(int MaxCharge)
{ //56 to 99.
  if (q <= 0) return 0;
  
  double Percent = (double)q/(double)MaxCharge;
  int hello = (int(43*Percent+0.5+56));  //the .5 is to fix rounding.
  
  if (hello > 99) hello = 99;

  return hello;
}

void APad::WriteCalibration()
{
  ofstream file;
  file.open("/home/tpc/calib/PAD_CALIBRATIONS.TXT");
  for (int i=0; i<NumberOfPads; i++)
    {
      file << Pedestals[i] << " " << Sigmas[i] << " " << Gains[i] << endl;
      cout << Pedestals[i] << " " << Sigmas[i] << " " << Gains[i] << endl;
    }
  file.close(); int myID;
}

void APad::ReadCalibration()
{
  cout << "Reading PAD calibrations...";

  ifstream file;
  file.open("/home/tpc/calib/PAD_CALIBRATIONS.TXT", ios::in);

  for ( int i=0; i < NumberOfPads; i++ )
    {
      file >> Pedestals[i] >> Sigmas[i] >> Gains[i];
    }
  file.close();

  cout << "DONE." << endl;
}

void APad::DetermineCommonMode()
{
  //string method = CommonModeMethod;

  cyclops *Scott = cyclops::instance();

  CommonMode.clear();;

  vector<double> values;
  for (int i=0; i<Raw[0].size(); i++) //loop over slices
    {
      values.clear();
      for (int j=0; j<Scott->thePads.size(); j++) // loop over INSTRUMENTED pads
	{
	  int index = Scott->thePads[j]->MyID();
	  values.push_back(Pedestals[index] - Raw[index][i]);
	}
      sort(values.begin(), values.end());
      int mid = Scott->thePads.size()/2.0;
      double average3 = (values[mid-1]+values[mid]+values[mid+1])/3.0;
      CommonMode.push_back( average3 );
    }
}

void APad::ApplyCalibration()
{
  /* loop over all pads */
  for (int i=0; i<NumberOfPads; i++)
    {
      Cal[i].clear();
      for(int j=0; j<Raw[i].size(); j++)
	{
	  Cal[i].push_back(Gains[i]*(Pedestals[i]-Raw[i][j]));
	}
    }  

  /* loop over INSTRUMENTED pads for common mode */
  cyclops *Scott = cyclops::instance();
  for (int i=0; i<Scott->thePads.size(); i++)
    {
      int index = Scott->thePads[i]->MyID();
      for(int j=0; j<Raw[index].size(); j++)
	{
	  if (CommonMode.size() > j)
	    {
	      Cal[index][j] = Cal[index][j]-(Gains[index]*CommonMode[j]);
	    }
	}
    }  

}

void APad::DetermineQ()
{

  //  New FastQ routine.  3-30-2013
  //  Here we shall determine which three contiguous time slices
  //  have the maximum sum.  The average of these three will
  //  be "q" and the bin location will be "t".
  //                         TKH


  if (FastQ)
    {
      q=0;
      t=0;
      for (int i=1; i<Cal[myID].size()-1; i++)
	{
	  double threesum = ( Cal[myID][i-1]+Cal[myID][i]+Cal[myID][i+1] )/3.0;
	  if (threesum>q)
	    {
	      q=threesum;
	      t=i;
	    }
	}
      return;
    }


  //  After some analysis of pulser data, we have 
  //  determined a fit function that does a good job at extracting the charge.  
  //
  //  The function in question is a fermi function multipled by a gaussian.
  //  The initial trial was a fermi function multiplied by an exponential,
  //  but that choice poorly described the tail.
  //
  //                                                   MB & TKH 3-8-2013
  if (!blue)
    {
      Pulse = new TH1D("Pulse","Pulse",Cal[myID].size(),-0.5,(double)Cal[myID].size()-0.5);
      blue = new TF1("blue","[0]/(exp(-(x-[1])/[2])+1)*exp(-((x-[1])^2/[3]))",0,30);
      
      blue->FixParameter(1,7);  // Expt on 3-8-2013
      blue->FixParameter(2,0.42);  // Expt on 3-8-2013
      blue->FixParameter(3,53.7);  // Expt on 3-8-2013
    }


  //Here we are just averaging over time slices as a first approximation...
  for (int i=0; i<Cal[myID].size(); i++)
    {
      Pulse->SetBinContent(i+1,Cal[myID][i]);
    }
  double max = Pulse->GetBinContent(Pulse->GetMaximumBin());
  double t0 = Pulse->GetBinCenter(Pulse->GetMaximumBin())-2.0;

  blue->SetParameter(0,max);
  blue->SetParameter(1,t0);
  Pulse->Fit(blue,"Q0");

  q = blue->GetParameter(0);
  t = blue->GetParameter(1);
}

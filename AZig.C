#include <iostream>

#include "AZig.h"

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
#include "TPolyLine.h"
#include "TMarker3DBox.h"
#include "TH1D.h"
#include "TF1.h"
#include "TSpectrum.h"

using namespace std;
int AZig::nextID=0;

bool AZig::FastQ = true;
bool AZig::UseSigma = true;
std::vector<int> AZig::Raw[NumberOfZigs];
std::vector<double> AZig::Cal[NumberOfZigs];

double AZig::Pedestals[NumberOfZigs] = {0};
double AZig::Sigmas[NumberOfZigs] = {0};
double AZig::Gains[NumberOfZigs] = {0};
double AZig::CommonMode = 0;

TH1D *AZig::Pulse=0; /* @TODO Do we need this? */
TF1  *AZig::blue=0; /* @TODO Do we need this? */

double AZig::SigmaCut=3;
double AZig::PulseCut=40;
string AZig::CommonModeMethod="gauss";

AZig::AZig(double X1, double Z1, double X2, double Z2)
{
  // Set the patameters define the pad plane space.
  x1 = X1;
  z1 = Z1;
  x2 = X2;
  z2 = Z2;
  q  = 0;
  t  = 0;
  
  // If there are "empty" channels, the user will
  // likely overwrite myID using SetMyID()...
  myID = nextID++;
}

void AZig::Draw( double MAX )
{
  // color pad according to charge fraction of total deposited charge
  TBox* aziga = new TBox(x1,z1,x2,z2);

  aziga->SetLineStyle(1);
  aziga->SetLineWidth(2);
  aziga->SetLineColor(15);

  aziga->SetFillStyle(1001);

  // draw zig contour
  aziga->SetFillColor(kGray);
  aziga->Draw("");

  // draw zig with color
  if ( IsHit() && color((int)(MAX+0.5)) )
    {
      aziga->SetFillColor(color((int)(MAX+0.5)));
      aziga->Draw("");
    }

}

void AZig::Draw3D( double MAX )
{
  //  Display is prettiest if we "cycle" the coordinates.
  //  This means treating tyhe arguments as (z,x,y)...
  TMarker3DBox *eye = new TMarker3DBox(ZCenter()  ,XCenter()  ,  0        , // Fake!!!
				       (z2-z1)/2.2,(x2-x1)/2.2,(x2-x1)/2.0, // Fake!!!
				       0,0);
  eye->SetLineColor( color((int)(MAX+0.5)) );  
  eye->SetLineWidth(1); //pixels
  eye->Draw();

}

int AZig::color(int MaxCharge)
{ //56 to 99.
  if (q <= 0) return 0;
  
  double Percent = (double)q/(double)MaxCharge;
  int hello = (int(43*Percent+0.5+56));  //the .5 is to fix rounding.
  
  if (hello > 99) hello = 99;

  return hello;
}

void AZig::WriteCalibration()
{
  ofstream file;
  file.open("/home/tpc/calib/ZIG_CALIBRATIONS.TXT");
  for (int i=0; i<NumberOfZigs; i++)
    {
      file << Pedestals[i] << " " << Sigmas[i] << " " << Gains[i] << endl;
      cout << Pedestals[i] << " " << Sigmas[i] << " " << Gains[i] << endl;
    }
  file.close(); int myID;
}

void AZig::ReadCalibration()
{
  cout << "Reading ZIG calibrations...";

  ifstream file;
  file.open("/home/tpc/calib/ZIG_CALIBRATIONS.TXT", ios::in);

  for ( int i=0; i < NumberOfZigs; i++ )
    {
      file >> Pedestals[i] >> Sigmas[i] >> Gains[i];
    }
  file.close();

  cout << "DONE." << endl;
}

void AZig::DetermineCommonMode()
{
  string method = CommonModeMethod;
  cout << "It might be nice to implement CommonMode for the Zigs BEFORE you call it." << endl;
  CommonMode = 0.0;
}

void AZig::ApplyCalibration()
{
  /* loop over all zig types */
  for (int i=0; i<NumberOfZigs; i++)
    {
      Cal[i].clear();
      for(int j=0; j<Raw[i].size(); j++)
	{
	  //cout << " i " << i;
	  //cout << " j " << j;
	  //cout << " Gain " << Gains[i];
	  //cout << " Pedestals " << Pedestals[i];
	  //cout << " Raw[i][j] " << Raw[i][j];
	  //cout << " COmmnonMode " << CommonMode;
	  //cout << endl;
	  Cal[i].push_back(Gains[i]*(Pedestals[i]-Raw[i][j]-CommonMode));
	}
    }  
}

void AZig::DetermineQ()
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

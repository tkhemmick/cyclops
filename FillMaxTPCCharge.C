////////////////////////////////////
// Fillmaxcharge.C                    //
////////////////////////////////////

//  This makes a histogram for each HBD channel showing:
//  -Maximum TPCCharge.
//  -Time slice with maximum TPCCharge.
//  -Correlation between these two.

#include "FillMaxTPCCharge.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "AZig.h"

#include "cyclops.h"

#include <iostream>
#include <cmath>

TH1D *MaxMaxTPCCharge=0;
TH2D *MaxTPCChargeSet;

using namespace std;

void FillMaxTPCCharge()
{

  int nTime = AZig::Cal[0].size();
  double left = -0.5;
  double right = (double)nTime - 0.5;

  
  if (!MaxMaxTPCCharge)
    {
      MaxMaxTPCCharge = new TH1D("MaxMaxTPCCharge","MaxMaxTPCCharge",300,-300.5,3000.5);
      MaxTPCChargeSet = new TH2D("MaxTPCChargeSet","MaxTPCChargeSet",480,-0.5,479.5,300,-300.5,3000.5);      
    }

  cyclops *Scott = cyclops::instance();

  double MAX=-999;
  int MAXindex=-1;
  for (int i=0; i<479; i++)
    {
      double TPCCharge = Scott->theZigs[i]->Q();
      if (TPCCharge>MAX)
	{
	  MAX=TPCCharge;
	  MAXindex=i;
	}

      MaxTPCChargeSet->Fill(i,TPCCharge);
    }

  MaxMaxTPCCharge->Fill(MAX);
  
}

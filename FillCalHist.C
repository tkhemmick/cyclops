////////////////////////////////////
// FillCalHist.C                    //
////////////////////////////////////

//  This makes a histogram that can show the calibrated hits.

#include "FillCalHist.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "APad.h"
#include "AZig.h"
#include "TSpectrum.h"

#include "cyclops.h"

#include <iostream>
#include <cmath>

TH2D *CalHist=0;
TH1D *CalDist[NumberOfPads];

TH2D *CalZigHist=0;
TH1D *CalZigDist[NumberOfZigs];

TH1D *slice=0;

using namespace std;

void FillCalHist()
{

  int nTime = APad::Cal[0].size();
  double left = -0.5;
  double right = (double)nTime - 0.5;

  if (!CalHist)
    {
      CalHist = new TH2D("CalHist","CalHist",NumberOfPads,-0.5,(double)NumberOfPads-0.5,nTime,left,right);
      cout << "Making the histogram" << endl;

      char name[500];
      for (int i=0; i<NumberOfPads; i++)
	{
	  sprintf(name,"CalDist%03d",i);
	  CalDist[i] = new TH1D(name,name,3301,-300.5,3000.5);
	}

    }
  CalHist->Reset();

  for (int i=0; i<NumberOfPads; i++)
    {
      for (int j=0; j<APad::Cal[i].size(); j++)
	{
	  CalHist->SetBinContent(i+1,j+1,APad::Cal[i][j]);
	  CalDist[i]->Fill(APad::Cal[i][j]);
	}
    }

}


void FillCalZigHist()
{

  int nTime = AZig::Cal[0].size();
  double left = -0.5;
  double right = (double)nTime - 0.5;

  if (!CalZigHist)
    {
      CalZigHist = new TH2D("CalZigHist","CalZigHist",NumberOfZigs,-0.5,(double)NumberOfZigs-0.5,nTime,left,right);
      cout << "Making the histogram" << endl;

      char name[500];
      for (int i=0; i<NumberOfZigs; i++)
	{
	  sprintf(name,"CalZigDist%03d",i);
	  CalZigDist[i] = new TH1D(name,name,3301,-300.5,3000.5);
	}

    }
  CalZigHist->Reset();

  for (int i=0; i<NumberOfZigs; i++)
    {
      for (int j=0; j<AZig::Cal[i].size(); j++)
	{
	  CalZigHist->SetBinContent(i+1,j+1,AZig::Cal[i][j]);
	  CalZigDist[i]->Fill(AZig::Cal[i][j]);
	}
    }

}

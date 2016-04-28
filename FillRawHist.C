////////////////////////////////////
// FillRawHist.C                    //
////////////////////////////////////

//  This makes a histogram that can show the raw hits.

#include "FillRawHist.h"
#include "TF1.h"
#include "TH2D.h"
#include "APad.h"
#include "AZig.h"

#include <iostream>

using namespace std;

TH2D *RawHist=0;
TH1D *RawDist[NumberOfPads];

TH2D *RawZigHist=0;
TH1D *RawZigDist[NumberOfZigs];


void FillRawHist()
{

  int nTime = APad::Raw[0].size();
  double left = -0.5;
  double right = (double)nTime - 0.5;


  //  NOTE:  Raw is the histogram into which we place the raw data.
  //  but APad::Raw is the raw data itself.


  if (!RawHist)
    {
      RawHist = new TH2D("RawHist","RawHist",NumberOfPads,-0.5,(double)NumberOfPads-0.5,nTime,left,right);
      cout << "Making the Raw histogram" << endl;

      char name[500];
      for (int i=0; i<NumberOfPads; i++)
	{
	  sprintf(name,"RawDist%03d",i);
	  RawDist[i] = new TH1D(name,name,2301,1499.5,3800.5);
	}

    }
  RawHist->Reset();


  for (int i=0; i<NumberOfPads; i++)
    {
      for (int j=0; j<APad::Raw[i].size(); j++)
	{
	  RawHist->SetBinContent(i+1,j+1,APad::Raw[i][j]);
	  RawDist[i]->Fill(APad::Raw[i][j]);
	}
    }

}

void FitPedestals()
{
  for (int i=0; i<NumberOfPads; i++)
    {
      RawDist[i]->Fit("gaus","Q");
      APad::Pedestals[i] = RawDist[i]->GetFunction("gaus")->GetParameter(1);
      APad::Sigmas[i] = RawDist[i]->GetFunction("gaus")->GetParameter(2);
      APad::Gains[i] = 1.0;
    }




}
void FillRawZigHist()
{

  int nTime = AZig::Raw[0].size();
  double left = -0.5;
  double right = (double)nTime - 0.5;


  //  NOTE:  RawZig is the histogram into which we place the RawZig data.
  //  but AZig::Raw is the raw data itself.


  if (!RawZigHist)
    {
      RawZigHist = new TH2D("RawZigHist","RawZigHist",NumberOfZigs,-0.5,(double)NumberOfZigs-0.5,nTime,left,right);
      cout << "Making the RawZig histogram" << endl;

      char name[500];
      for (int i=0; i<NumberOfZigs; i++)
	{
	  sprintf(name,"RawZigDist%03d",i);
	  RawZigDist[i] = new TH1D(name,name,2301,1499.5,3800.5);
	}

    }
  RawZigHist->Reset();


  for (int i=0; i<NumberOfZigs; i++)
    {
      for (int j=0; j<AZig::Raw[i].size(); j++)
	{
	  RawZigHist->SetBinContent(i+1,j+1,AZig::Raw[i][j]);
	  RawZigDist[i]->Fill(AZig::Raw[i][j]);
	}
    }

}

void FitZigPedestals()
{
  for (int i=0; i<NumberOfZigs; i++)
    {
      RawZigDist[i]->Fit("gaus","Q");
      AZig::Pedestals[i] = RawZigDist[i]->GetFunction("gaus")->GetParameter(1);
      AZig::Sigmas[i] = RawZigDist[i]->GetFunction("gaus")->GetParameter(2);
      AZig::Gains[i] = 1.0;
    }




}

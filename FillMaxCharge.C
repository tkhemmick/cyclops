////////////////////////////////////
// FillMaxCharge.C                    //
////////////////////////////////////

//  This makes a histogram for each HBD channel showing:
//  -Maximum Charge.
//  -Time slice with maximum charge.
//  -Correlation between these two.

#include "FillMaxCharge.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "APad.h"
#include "PID.h"

#include "cyclops.h"

#include <iostream>
#include <cmath>

TH2D *TPCEventDisplay=0;

TH2D *HBDEventDisplay=0;
TH2D *HBDBeamSpot=0;

TH2D *Psych;
TH1D *NumberInTime=0;
TH1D *OneChargeInTime=0;
TH1D *TotalChargeInTime=0;
TH1D *TotalChargeInTime_Hit=0;
TH1D *TotalChargeInTime_Miss=0;
TH1D *MaxMaxCharge=0;
TH2D *MaxMaxChargeVsTime=0;
TH2D *MaxMaxChargeVsNeighbor=0;
TH1D *MaxCharge[9];
TH1D *MaxChargeTime[9];
TH2D *MaxChargeVsTime[9];

using namespace std;

void FillMaxCharge()
{

  if (!IsBeam()) return;

  int nTime = APad::Cal[0].size();
  double left = -0.5;
  double right = (double)nTime - 0.5;

  
  if (!MaxMaxCharge)
    {
      NumberInTime = new TH1D("NumberInTime","NumberInTime",9,-0.5,8.5);
      TotalChargeInTime = new TH1D("TotalChargeInTime","TotalChargeInTime",300,-300.5,3000.5);
      TotalChargeInTime_Hit  = new TH1D("TotalChargeInTime_Hit","TotalChargeInTime_Hit",300,-300.5,3000.5);
      TotalChargeInTime_Miss = new TH1D("TotalChargeInTime_Miss","TotalChargeInTime_Miss",300,-300.5,3000.5);
      OneChargeInTime = new TH1D("OneChargeInTime","OneChargeInTime",300,-300.5,3000.5);
      MaxMaxCharge = new TH1D("MaxMaxCharge","MaxMaxCharge",300,-300.5,3000.5);
      MaxMaxChargeVsTime = new TH2D("MaxMaxChargeVsTime","MaxMaxChargeVsTime",300,-300.5,3000.5,nTime,left,right);
      MaxMaxChargeVsNeighbor = new TH2D("MaxMaxChargeVsNeighbor","MaxMaxChargeVsNeighbor",300,-300.5,3000.5,20,0,1);
      
      Psych = new TH2D("Psych","Psych",128,-0.5,127.5,300,-300,3000);

      TPCEventDisplay = new TH2D("TPCEventDisplay","TPCEventDisplay",48,-5,5,10,-5,5);

      HBDEventDisplay = new TH2D("HBDEventDisplay","HBDEventDisplay",3,-5,5,3,-5,5);
      HBDBeamSpot     = new TH2D("HBDBeamSpot"    ,"HBDBeamSpot",    3,-5,5,3,-5,5);
      
      char name[500];
      for (int i=0; i<9; i++)
	{
	  sprintf(name,"MaxCharge%03d",i);
	  MaxCharge[i] = new TH1D(name,name,300,-300.5,3000.5);

	  sprintf(name,"MaxChargeTime%03d",i);
	  MaxChargeTime[i] = new TH1D(name,name,nTime,left,right);

	  sprintf(name,"MaxChargeVsTime%03d",i);
	  MaxChargeVsTime[i] = new TH2D(name,name,300,-300.5,3000.5,nTime,left,right);
	}
    }

  cyclops *Scott = cyclops::instance();

  double MAX=-999;
  double TIME=-9;
  int MAXindex=-1;
  for (int i=0; i<9; i++)
    {
      double charge = Scott->thePads[i]->Q();
      double time   = Scott->thePads[i]->T();
      if (charge>MAX)
	{
	  MAX=charge;
	  TIME=time;
	  MAXindex=i;
	}

      MaxCharge[i]->Fill(charge);
      MaxChargeTime[i]->Fill(time);
      MaxChargeVsTime[i]->Fill(charge,time);
    }

  MaxMaxCharge->Fill(MAX);
  MaxMaxChargeVsTime->Fill(MAX,TIME);
  
  TPCEventDisplay->Reset();
  for (int i=0; i<Scott->theZigs.size(); i++)
    {
      double charge = Scott->theZigs[i]->Q();

      TPCEventDisplay->Fill(Scott->theZigs[i]->XCenter(),Scott->theZigs[i]->ZCenter(),Scott->theZigs[i]->Q());
    }

  double MAX2=-999;
  double TIME2=-9;
  double MAXindex2=-1;
  HBDEventDisplay->Reset();
  for (int i=0; i<9; i++)
    {
      double charge = Scott->thePads[i]->Q();
      double time   = Scott->thePads[i]->T();
      if (i!=MAXindex && charge>MAX2)
	{
	  MAX2=charge;
	  TIME2=time;
	  MAXindex2=i;
	}

      HBDEventDisplay->Fill(Scott->thePads[i]->XCenter(),Scott->thePads[i]->YCenter(),Scott->thePads[i]->Q());
      HBDBeamSpot->    Fill(Scott->thePads[i]->XCenter(),Scott->thePads[i]->YCenter(),Scott->thePads[i]->Q());
    }




  MaxMaxChargeVsNeighbor->Fill(MAX,MAX2/MAX);

  double TOTAL=0;
  int NUMBER=0;
  for (int i=0; i<9; i++)
    {
      double charge = Scott->thePads[i]->Q();
      double time   = Scott->thePads[i]->T();
      if (time>3 && time<7)
	{
	  TOTAL += charge;
	  NUMBER++;
	}
    }
  TotalChargeInTime->Fill(TOTAL);
  if (ChrHit() ) TotalChargeInTime_Hit ->Fill(TOTAL);
  if (ChrMiss()) TotalChargeInTime_Miss->Fill(TOTAL);
  NumberInTime->Fill(NUMBER);

  if (NUMBER==1) OneChargeInTime->Fill(TOTAL);


  for (int i=0; i<128; i++)
    {
      double Peak=0;
      for (int j=4; j<7; j++)
	{
	  Peak += APad::Cal[i][j];
	}
      Psych->Fill(i,Peak/3.0);
    }

}

#include "ATrace.h"  //always include class header
#include "TF1.h"
#include "TH1D.h"
#include <iostream>
#include <algorithm>

using namespace std;

int ATrace::numtraces=0;
bool ATrace::FastEdgeFinder=true;

ATrace::ATrace()
{
  voltage.clear();
  height=0;
  time=0;

  sprintf (fcn_name,"fermi%d",numtraces);
  fermi = new TF1(fcn_name,"[0]/(exp(-(x-[1])/[2])+1)+[3]",0,1024);

  sprintf (hst_name,"trace%d",numtraces);
  trace = new TH1D(hst_name,hst_name,1024,-0.5,1023.5);

  numtraces++;		   
}

ATrace::~ATrace()
{
  fermi->Delete();
  trace->Delete();
}

double ATrace::FindMaximum(int n)
{

  double MaxInBin=9999;
  int MaxI=-9999;

  //Assumes negative traces.
  for(int i=0; i<voltage.size(); i++)
    {
      if(voltage[i]<MaxInBin)
	{
	  MaxInBin = voltage[i];
	  MaxI=i;
	}
    }  

  double Sum=0;
  double Num=0;

  //  Careful with the results of the previous search...
  if (MaxI>=n && MaxI+n<voltage.size())
    {
      for(int a=MaxI-n; a<(MaxI+n); a++)
	{
	  if(a<0) continue;
	  
	  if(a>(voltage.size()-1)) break;
	  
	  Sum += voltage[a];
	  Num++;
	}
    }
  else if (MaxI>=0 && MaxI<voltage.size())
    {
      Sum = voltage[MaxI];
      Num = 1;
    }
  else
    {
      Sum = -9999;
      Num = 1;
    }

  return Sum/Num;
}

void ATrace::FitLeftEdge(int inset)
{
  RestoreBaseline();
  
  //  Assumes negative traces and flips them to be positive histograms.
  trace->Reset();
  for(int i=inset; i<voltage.size()-inset; i++)
    {
      trace->SetBinContent(i+1,-voltage[i]);
    }  

  //  Fast Edge Finder uses the maximum bin and "looks left"
  //  until we drop to 1/2 height...
  if (FastEdgeFinder)
    {
      int MaxBin = trace->GetMaximumBin();
      height = trace->GetBinContent(MaxBin);
      while (MaxBin>1 && trace->GetBinContent(MaxBin) > height/2.0)
	{
	  MaxBin--;
	}
      time = MaxBin;
    }
  else
    {
      double X = trace->GetMaximumBin();
      double h = trace->GetBinContent(X);
      fermi->SetParameter(0,h);
      fermi->SetParameter(1,X-25);
      fermi->SetParameter(2,5);
      fermi->SetParameter(3,2);
      trace->Fit(fermi,"WWMQ0","",0,X+10);
      //  cout << "here 1" << endl;
      double A = trace->GetFunction(fcn_name)->GetParameter(2);
      //  cout << "here 2" << endl;
      double B = trace->GetFunction(fcn_name)->GetParameter(1);
      //  cout << "here 3" << endl;
      trace->Fit(fermi,"WWMQ0","",0,6.5*A+B);
      //  cout << "here 4" << endl;
      height = trace->GetFunction(fcn_name)->GetParameter(0);
      // cout << "here 5" << endl;
      time = trace->GetFunction(fcn_name)->GetParameter(1);
      //  cout << "here 6" << endl;
    }

}

void ATrace::RestoreBaseline()
{
  //  This routine assumes that the pulses ride on a
  //  significant DC offset.  We assume that the majority
  //  of the digitization window is fee of the pulse and
  //  thereby use a median to restore the baseline.


  vector<double> copy = voltage;
  sort(copy.begin(), copy.end() );
  int middle = copy.size()/2.0;
  double median = copy[middle];
  
  for(int i=0; i<voltage.size(); i++)
    {
      voltage[i] = voltage[i] - median;
    }  

}

int ATrace::PeakCount(double low, double high)
{
  //  This routine assumes that the baseline of the trace
  //  has already been restored.  Thus, it must be called AFTER the
  //  FitLeftEdge() routine.
  //
  //  This inter-linkage should be resolved more elegantly in the future...

  int count = 0;
  bool inside = false;

  for(int i=0; i<voltage.size(); i++)
    {
      if (!inside && voltage[i] < -high)
	{
	  count ++;
	  inside=true;
	}
      if (inside && voltage[i] > -low)
	{
	  inside=false;
	}
    }  

  return count;
}

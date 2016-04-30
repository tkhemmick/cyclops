////////////////////////////////////
// FillCommonMode.C               //
////////////////////////////////////

//  This makes a histogram that can show the raw hits.

#include "FillCommonMode.h"
#include "cyclops.h"
#include "TF1.h"
#include "TH2D.h"
#include "APad.h"

#include <iostream>

using namespace std;

TH2D *CommonMode[8]={0};

void FillCommonMode()
{

  int nTime = APad::Raw[0].size();
  double left = -0.5;
  double right = (double)nTime - 0.5;


  //  NOTE:  Raw is the histogram into which we place the raw data.
  //  but APad::Raw is the raw data itself.


  if (!CommonMode[0])
    {
      cout << "Making the CommonMode histograms" << endl;
      char name[500];
      for (int i=0; i<8; i++)
	{
	  sprintf(name,"CommonMode_%d",i+1);
	  CommonMode[i] = new TH2D(name,name,401,-200.5,200.5,401,-200.5,200.5);
	}
    }

  cyclops *Scott = cyclops::instance();
  int Ref=Scott->thePads[0]->MyID();
  for (int i=0; i<8; i++)
    {
      int Oth=Scott->thePads[i+1]->MyID();
      for (int j=0; j<APad::Raw[Ref].size(); j++)
	{
	  int RefSignal = APad::Pedestals[Ref]-APad::Raw[Ref][j];
	  int OthSignal = APad::Pedestals[Oth]-APad::Raw[Oth][j];
	  CommonMode[i]->Fill(RefSignal,OthSignal);
	}
    }

}

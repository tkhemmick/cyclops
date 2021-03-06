//////////////////////////////////////////////
//      Histogram output.        	    //
//         Cherenkov Detector Software	    //
//		   Marie Blatnik            //
//                 Thomas Videbaek	    //
//	       Dr. Thomas Hemmick	    //
//                                          //
//      An APad Readout.	            //
//		   Version 2.0              //
//		Thursday, 2-28-2013	    //
//////////////////////////////////////////////

#include <iostream>
#include <cmath>
#include <vector>

#include "cyclops.h"
#include "APad.h"
#include "AZig.h"

#include "TCanvas.h"
#include "TView3D.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "TMarker3DBox.h"

using namespace std;

void DisplayEvent(double MIN, double MAX, bool drawregions=false)
{
  // Make a pretty Canvas and 3D viewing area...
  static TCanvas *PadCanvas = new TCanvas("ThePadCanvas", "TPC HBD EVent Display", 0,0, 1000, 1000);/// pixels
  PadCanvas->cd(); //Hey, root--put stuff on THIS canvas.  
  PadCanvas->Clear();
  TView3D *view = (TView3D*) TView::CreateView(1);
  double Scale = 32.0/10.4;
  view->SetRange(-1,-5.2*Scale,-5.2*Scale,31,5.2*Scale,5.2*Scale);

  cyclops *Scott = cyclops::instance();
  //  Display the HBD hits in 3D
  for (int i=0; i<Scott->thePads.size(); i++)
    {
      if (Scott->thePads[i]->IsHit()) Scott->thePads[i]->Draw3D(MAX);
    }
  
  //  Display the TPC hits in 3D
  for (int i=0; i<Scott->theZigs.size(); i++)
    {
      if (Scott->theZigs[i]->IsHit()) Scott->theZigs[i]->Draw3D(MAX);
    }
  
  
  // Create a PolyLine3D that will "surround" the active area with a 1 cm buffer
  TPolyLine3D *enter = new TPolyLine3D(5);
  enter->SetPoint(0, -1, -5.1,  -5.1);
  enter->SetPoint(1, -1,  5.1,  -5.1);
  enter->SetPoint(2, -1,  5.1,   5.1);
  enter->SetPoint(3, -1, -5.1,   5.1);
  enter->SetPoint(4, -1, -5.1,  -5.1);

  TPolyLine3D *exit = new TPolyLine3D(5);
  exit->SetPoint(0, 31, -5.1,  -5.1);
  exit->SetPoint(1, 31,  5.1,  -5.1);
  exit->SetPoint(2, 31,  5.1,   5.1);
  exit->SetPoint(3, 31, -5.1,   5.1);
  exit->SetPoint(4, 31, -5.1,  -5.1);

  TPolyLine3D *top = new TPolyLine3D(5);
  top->SetPoint(0,  -1, -5.1,   5.1);
  top->SetPoint(1,  31, -5.1,   5.1);
  top->SetPoint(2,  31,  5.1,   5.1);
  top->SetPoint(3,  -1,  5.1,   5.1);
  top->SetPoint(4,  -1, -5.1,   5.1);

  TPolyLine3D *bot = new TPolyLine3D(5);
  bot->SetPoint(0,  -1, -5.1,  -5.1);
  bot->SetPoint(1,  31, -5.1,  -5.1);
  bot->SetPoint(2,  31,  5.1,  -5.1);
  bot->SetPoint(3,  -1,  5.1,  -5.1);
  bot->SetPoint(4,  -1, -5.1,  -5.1);

  // Draw the perimeter...
  enter->Draw();
  exit->Draw();
  top->Draw();
  bot->Draw();
  
}

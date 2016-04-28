#include "cyclops.h"

//Important padplane headers
#include "ATrace.h"
#include "APad.h"

#include <iostream>

using namespace std;
cyclops *cyclops::__instance=0;

cyclops::cyclops()
{
  cout << "Creating a new cyclops";
  cout << endl;
  
  for(int i=0; i<8; i++)
    {
      theTraces.push_back(new ATrace());
    }

  CreateHBD();         //  This is an internal function of cyclops
  CreateTPC();         //  This is an internal function of cyclops
  ClearTheDetector();  //  Just to be sure that hexes are "born clear".
  
  APad::ReadCalibration();
  AZig::ReadCalibration();
}


cyclops::~cyclops()
{
  // here we clean up the hexes since cyclops is the owner...
  for(unsigned int i=0; i<theTraces.size(); i++)
    {
      delete theTraces[i];
    }
  
  for(unsigned int i=0; i<thePads.size(); i++)
      delete thePads[i];

  for(unsigned int i=0; i<theZigs.size(); i++)
      delete theZigs[i];

}

void cyclops::Report()
{
  cout << "The eye of cyclops sees you!" << endl;

  cout << "TRACES -------------------------------------------------------" << endl;
  for (unsigned int i=0; i< theTraces.size(); i++)
    {
      cout << "Trace " << i << endl;
      cout << " Maximum:   " << theTraces[i]->FindMaximum(1);
    }

  cout << "PADS ---------------------------------------------------------" << endl;
  for (unsigned int i=0; i< thePads.size(); i++)
    {
      cout << "Pad " << i << " ";
      thePads[i]->Report();
    }

  cout << "ZIGS ---------------------------------------------------------" << endl;
  for (unsigned int i=0; i< theZigs.size(); i++)
    {
      cout << "Zig " << i << " ";
      theZigs[i]->Report();
    }

}

void cyclops::ClearTheDetector()
{
  for (int i=0; i<theTraces.size(); i++)
    {
      theTraces[i]->voltage.clear();
    }

  //  APad static variables...
  for (int i=0; i<NumberOfPads; i++)
    {
      APad::Raw[i].clear();
      APad::Cal[i].clear();
    }
  
  //  reset (but don't destroy) each strip.
  for (unsigned int i=0; i<thePads.size(); i++)  // Same numbers...
    {
      thePads[i]->SetQ(0);
    }
    
  //  AZig static variables...
  for (int i=0; i<NumberOfZigs; i++)
    {
      AZig::Raw[i].clear();
      AZig::Cal[i].clear();
    }
  
  //  reset (but don't destroy) each strip.
  for (unsigned int i=0; i<theZigs.size(); i++)  // Same numbers...
    {
      theZigs[i]->SetQ(0);
    }
    
}

void cyclops::CreateHBD()
{ 
  // Hard-coded channel map.
  int ChannelIndex[9]={14,12,1,  11,4,10,  3,9,2};
  
  // hard-coded z position of the HBD
  double z0 = 0.;

  double delta = 10.0/3.0;  // 10 cm/3 pads
  double x0=-5;
  double y0=delta/2.0;  
  for (int i=0; i<9; i++)
    {
      APad *cell = new APad(x0,y0,x0+delta,y0+delta,z0);
      cell->SetMyID(ChannelIndex[i]);
      thePads.push_back(cell);
      x0 += delta;
      if (x0>4)
	{
	  x0 = -5.0;
	  y0 -= delta;
	}
    }

}

void cyclops::CreateTPC()
{ 
  MakeTPCMaps();

  double DeltaX = 10.0/48.0; // 48 zigs in 10 cm...
  double DeltaZ = 10.0/10.0; // 10 zigs in 10 cm...
  
  for (int i=0; i<NumberOfZigs; i++)
    {
      if (xmap[i]>=0 && ymap[i]>=0)
	{
	  double x0 = xmap[i]*DeltaX - 5.0;
	  double z0 = ymap[i]*DeltaZ - 5.0;
	  AZig *cell = new AZig(x0,z0,x0+DeltaX,z0+DeltaZ);
	  cell->SetMyID(i);
	  theZigs.push_back(cell);
	}
    }
  cout << "Cyclops sees " << theZigs.size() << " Zigs in the TPC." << endl;
}


void cyclops::MakeTPCMaps()
{  
  //===================TPC Prototype Chevron Board===================
  //Pad Map (Each Chevron Pads gets assigned an x and y value/coordinate)

  //X,Y Coord. map:  Strip# --> (x,y)
  
  //int xmap[NumberOfZigs];
  //int ymap[NumberOfZigs];
  int xmap2[NumberOfZigs];
  int ymap2[NumberOfZigs];


  int xmap_template_1[10] = {1,1,1,1,2,1,2,2,2,2};  //TPC prototype Chevron board (all one pattern) [Side with white lettering: "WOODY PATTERN"]
  //int ymap_template_1[10] = {3,2,5,4,2,1,4,3,1,5};  //Minidrift 4-quad chevron board
  int ymap_template_1[10] = {2,1,4,3,1,5,3,2,5,4};  //TPC prototype Chevron board (all one pattern) [Opposite side from above]
  //int ymap_template_1[10] = {3,4,1,2,4,5,2,3,5,1};  //changed position of origin 

  
  int xmap_template_2[10] = {1,0,1,1,1,1,2,2,2,2};
  int ymap_template_2[10] = {5,1,3,4,1,2,4,5,2,3};  //TPC prototype Chevron board (all one pattern)
  

  int round = -1;
  int i;

  //APV Ch. Map:   APV0: 0-127,   APV1: 128-255,   APV2: 256-383,   APV3: 384-511, 

  //APV0:
  for(i=0; i<128; i++)
    {
      if (i<3 || i>122)
	{
	  xmap[i] = -100;
	  ymap[i] = -100;
	}
      else
	{
	  if ( (i+7) % 10 == 0) round++; 	
	 
	  xmap[i] = xmap_template_1[(i+7) % 10] + 2*(round);
	  ymap[i] = ymap_template_1[(i+7) % 10];
	}
      
    }
  
  //APV1:
  for (i=128; i<256; i++)
    {
      if (i<131 || i>250)
	{
	  xmap[i] = -100;
	  ymap[i] = -100;
	}
      else
	{
	  xmap[i] = xmap[i-128]+24;  
	  ymap[i] = ymap[i-128];
	}
    }

  //================================
  //Must account for different pin-pad pattern for APV 2 & 3:

  round = -1;

  for(i=0; i<128; i++)
    {
      if (i<3 || i==4 || i>123)
	{
	  xmap[i] = -100;
	  ymap[i] = -100;
	}
      else
	{
	  if ( (i+7) % 10 == 0) round++; 	
	  
	  xmap2[i] = xmap_template_2[(i+7) % 10] + 2*(round);
	  ymap2[i] = ymap_template_2[(i+7) % 10];
	}
      
    }


  //for(i=0; i<128; i++)
  //{
  //  cout<<"APV_ch: "<<i<<", Xmap2: "<<xmap2[i]<<", Ymap2: "<<ymap2[i]<<endl;
  //}



  //APV2:
  for (i=256; i<384; i++)
    {
      if (i<259 || i==260 || i>379)
	{
	  xmap[i] = -100;
	  ymap[i] = -100;
	}
      else
	{
	  xmap[i] = 49-xmap2[i-256];  
	  ymap[i] = 11-ymap2[i-256];
	}
    }
  
  //APV3:
  for (i=384; i<NumberOfZigs; i++)
    {
      if (i<387 ||i==388 || i>507)
	{
	  xmap[i] = -100;
	  ymap[i] = -100;
	}
      else
	{
	  xmap[i] = 25-xmap2[i-384];  
	  ymap[i] = 11-ymap2[i-384];
	}
    }
  


  for(i=0; i<NumberOfZigs; i++)
  {
    //cout<<"APV_ch: "<<i<<", X: "<<xmap[i]<<", Y: "<<ymap[i]<<endl;
  }


}

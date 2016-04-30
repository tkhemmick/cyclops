#include <iostream>
#include <cmath>
#include <pmonitor.h>
#include "MyFavoriteMartin.h"

#include "cyclops.h"
#include "APad.h"
#include "ATrace.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TFile.h"

#include "prdfoStream.h"

#include "FillRawHist.h"
#include "FillCalHist.h"
#include "FillCommonMode.h"
#include "FillMaxCharge.h"
#include "FillMaxTPCCharge.h"

#include "caen_correction.h"

prdfoStream *os = 0;
int init_done = 0;

using namespace std;

//  Scope stuff...
TH1F *h0, *h1, *h2, *h3, *h4, *h5, *h6, *h7; // evt-by-evt
TH1F *h0cc, *h1cc, *h2cc, *h3cc, *h4cc, *h5cc; // evt-by-evt

TH1F *S1;
TH1F *S2;
TH1F *S3;
TH1F *C1;
TH1F *C2i;
TH1F *C2o;
TH2F *C2iVsC2o;
TH2F *C2iVsC2oTimed;
TH1F *phase1;
TH1F *phase2;

TH1F *S1t;
TH1F *S2t;
TH1F *S3t;
TH1F *C1t;
TH1F *C2it;
TH1F *C2ot;
TH2F *C1tVsC2ot;
TH2F *C1tVsC1;
TH1F *phase1t;
TH1F *phase2t;

TH1F *S1Count;
TH1F *S2Count;
TH1F *S3Count;
TH1F *C1Count;
TH1F *C2iCount;
TH1F *C2oCount;

//  SRS stuff...
TH1F *h1a; // The waveform...
TH1F *h1b; 
TH1F *h1c; 
TH1F *h1d; 
TH1F *h1e; 

TH1F *ha; //  Distribution of wavedform heights (for fine slewing studies...)
TH1F *hb; 
TH1F *hc; 
TH1F *hd; 
TH1F *he; 

TH1F *edgea;
TH1F *edgeb;
TH1F *edgec;
TH1F *edged;
TH1F *edgee;

caen_correction *cc;

int pinit()
{
  // Here we will book all the histograms, but NONE of the 
  // Ntuples.  The reason is that we only want to make the NTuples
  // when we've made an output file.

  // Generate the First cyclops and initialize the Hexes.
  cyclops *Scott = cyclops::instance();
  cc = new caen_correction ( "/home/tpc/calib/chip0_1GS.dat", "/home/tpc/calib/chip1_1GS.dat", "/home/tpc/calib/chip2_1GS.dat", "/home/tpc/calib/chip3_1GS.dat");

  //********************SCOPE******************************
  //  We have 8 "scope" variables:
  //  S1, S2, S3, C1, C2i, C2o, phase1 phase2
  h0 = new TH1F ( "h0","Scintillator S1", 1024, -0.5, 1023.5); 
  h1 = new TH1F ( "h1","Scintillator S2", 1024, -0.5, 1023.5); 
  h2 = new TH1F ( "h2","Scintillator S3", 1024, -0.5, 1023.5); 
  h3 = new TH1F ( "h3","Cherenkov 1", 1024, -0.5, 1023.5); 
  h4 = new TH1F ( "h4","Cherenkov 2 inner", 1024, -0.5, 1023.5); 
  h5 = new TH1F ( "h5","Cherenkov 2 outer", 1024, -0.5, 1023.5); 
  h6 = new TH1F ( "h6","Phase-Tag 1", 1024, -0.5, 1023.5); 
  h7 = new TH1F ( "h7","Phase-Tag 2", 1024, -0.5, 1023.5); 

  h0->SetLineColor(kYellow+2);
  h1->SetLineColor(kBlue);
  h2->SetLineColor(kViolet);
  h3->SetLineColor(kGreen);
  h4->SetLineColor(kYellow+2);
  h5->SetLineColor(kBlue);
  h6->SetLineColor(kViolet);
  h7->SetLineColor(kGreen);

  h0cc = new TH1F ( "h0cc","Scintillator S1", 1024, -0.5, 1023.5); 
  h1cc = new TH1F ( "h1cc","Scintillator S2", 1024, -0.5, 1023.5); 
  h2cc = new TH1F ( "h2cc","Scintillator S3", 1024, -0.5, 1023.5); 
  h3cc = new TH1F ( "h3cc","Cherenkov 1", 1024, -0.5, 1023.5); 
  h4cc = new TH1F ( "h4cc","Cherenkov 2 inner", 1024, -0.5, 1023.5); 
  h5cc = new TH1F ( "h5cc","Cherenkov 2 outer", 1024, -0.5, 1023.5); 

  h0cc->SetLineColor(kYellow+2);
  h1cc->SetLineColor(kBlue);
  h2cc->SetLineColor(kViolet);
  h3cc->SetLineColor(kGreen);
  h4cc->SetLineColor(kYellow+2);
  h5cc->SetLineColor(kBlue);

  //  These histograms will have pulse heights interpreted from the
  //  waveform using the ATrace class. (not relevant for phase signals).
  S1  = new TH1F ( "S1","; S1 pulse height [mV];# entries",  100, -0.5, 4095.5); 
  S2  = new TH1F ( "S2","; S2 pulse height [mV];# entries",  100, -0.5, 4095.5); 
  S3  = new TH1F ( "S3","; S3 pulse height [mV];# entries",  100, -0.5, 4095.5); 
  C1  = new TH1F ( "C1","; C1 pulse height [mV];# entries",  4096, -0.5, 4095.5); 
  C2i = new TH1F ( "C2i","; C2-inner height [mV];# entries", 4096, -0.5, 4095.5); 
  C2o = new TH1F ( "C2o","; C2-outer height [mV];# entries", 4096, -0.5, 4095.5); 
  C2iVsC2o = new TH2F ( "C2iVsC2o","C2iVsC2o", 4096, -0.5, 4095.5, 4096, -0.5, 4095.5);
  C2iVsC2oTimed = new TH2F ( "C2iVsC2oTimed","C2iVsC2oTimed", 4096, -0.5, 4095.5, 4096, -0.5, 4095.5); 

  //  These histograms have time information interpreted from the
  //  waveform using the ATrace class.
  S1t  = new TH1F ( "S1t","; S1 time [bins];# entries",        1024, -0.5, 1023.5); 
  S2t  = new TH1F ( "S2t","; S2 time time [bins];# entries",   1024, -0.5, 1023.5); 
  S3t  = new TH1F ( "S3t","; S3 time time [bins];# entries",   1024, -0.5, 1023.5); 
  C1t  = new TH1F ( "C1t","; C1 time [bins];# entries",        1024, -0.5, 1023.5); 
  C2it = new TH1F ( "C2it","; C2-inner time [bins];# entries", 1024, -0.5, 1023.5); 
  C2ot = new TH1F ( "C2ot","; C2-outer time [bins];# entries", 1024, -0.5, 1023.5); 

  //  Here we look for "extra" particles in the event and reject them...
  S1Count  = new TH1F ( "S1Count","S1Count",10, -0.5, 9.5); 
  S2Count  = new TH1F ( "S2Count","S2Count",10, -0.5, 9.5); 
  S3Count  = new TH1F ( "S3Count","S3Count",10, -0.5, 9.5); 
  C1Count  = new TH1F ( "C1Count","C1Count",10, -0.5, 9.5); 
  C2iCount  = new TH1F ( "C2iCount","C2iCount",10, -0.5, 9.5); 
  C2oCount  = new TH1F ( "C2oCount","C2oCount",10, -0.5, 9.5); 

  //  These histograms have time information interpreted from the
  //  waveform using the ATrace class.
  C1tVsC2ot  = new TH2F ( "C1tVsC2ot","; C1 time [bins]; C2o time [bins]", 12,493.5,506.5,16,429.5,445.5); 
  C1tVsC1    = new TH2F ( "C1tVsC1"  ,"; C1 time; C1", 100,-0.5,4096.5,16,429.5,445.5); 

  //********************SRS********************************
  h1a = new TH1F( "h1a", "Waveform TPC Card a", 4500, -0.5, 4499.5);
  h1b = new TH1F( "h1b", "Waveform TPC card b", 4500, -0.5, 4499.5);
  h1c = new TH1F( "h1c", "Waveform TPC Card c", 4500, -0.5, 4499.5);
  h1d = new TH1F( "h1d", "Waveform TPC Card d", 4500, -0.5, 4499.5);
  h1e = new TH1F( "h1e", "Waveform HBD", 4500, -0.5, 4499.5);

  ha = new TH1F( "ha", "Waveform Value Distribuition TPC Card a", 4001, -0.5, 4000.5);
  hb = new TH1F( "hb", "Waveform Value Distribuition TPC Card b", 4001, -0.5, 4000.5);
  hc = new TH1F( "hc", "Waveform Value Distribuition TPC Card c", 4001, -0.5, 4000.5);
  hd = new TH1F( "hd", "Waveform Value Distribuition TPC Card d", 4001, -0.5, 4000.5);
  he = new TH1F( "he", "Waveform Value Distribuition HBD Card", 4001, -0.5, 4000.5);

  edgea = new TH1F( "edgea", "TPC Waveform Starting Bin: Card a", 512, -0.5, 255.5);
  edgeb = new TH1F( "edgeb", "TPC Waveform Starting Bin: Card b", 512, -0.5, 255.5);
  edgec = new TH1F( "edgec", "TPC Waveform Starting Bin: Card c", 512, -0.5, 255.5);
  edged = new TH1F( "edged", "TPC Waveform Starting Bin: Card d", 512, -0.5, 255.5);
  edgee = new TH1F( "edgee", "HBD Waveform Starting Bin",         512, -0.5, 255.5);

  return 0;

}


int process_event (Event * e)
{  
  
  static int entries=0;
  entries++;
  if (entries%100==0) cout << "Processed " << entries << endl;
  
  static int nelec=0;
  static int BeamEnergy;
  
  h0->Reset();
  h1->Reset();
  h2->Reset();
  h3->Reset();
  h4->Reset();
  h5->Reset();
  h6->Reset();
  h7->Reset();

  h0cc->Reset();
  h1cc->Reset();
  h2cc->Reset();
  h3cc->Reset();
  h4cc->Reset();
  h5cc->Reset();
  
  h1a->Reset();
  h1b->Reset();
  h1c->Reset();
  h1d->Reset();
  h1e->Reset();
  
  cyclops *Scott = cyclops::instance();
  Scott->ClearTheDetector();
  
  //  Dump the Beam Info Packet to the SCREEN!!!
  Packet *p3 = e->getPacket(1020);
  if (p3)
    {
      p3->dump();
      delete p3;
    }

  //  Save the beam info to see if it ever changes...
  Packet *p4 = e->getPacket(921);
  if (p4)
    {
      BeamEnergy = p4->iValue(0,0);
      cout << "Initial Beam Energy: " << BeamEnergy << endl;
      delete p4;
    }

  //  Check the beam energy for changes...
  Packet *p5 = e->getPacket(1021);
  if (p5)
    {
      double CurrentBeamEnergy = p5->iValue(0,0);
      if (CurrentBeamEnergy!=BeamEnergy)
	{
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "Craig changed the beam energy from " << BeamEnergy;
	  cout << " to " << CurrentBeamEnergy << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	  cout << "========STOP THE ANALYSIS=============" << endl;
	}
      delete p5;
    }

  if ( os && e->getEvtType() >7 ) os->addEvent(e);
  
  // Reading the CAEN "oscilloscope"...
  Packet *p1 = e->getPacket(1005);
  if (p1)
    {
      cc->init(p1);

      int samples = p1->iValue(0, "SAMPLES"); 
      //      for ( int i = 0; i < samples; i++)
      for ( int i = 1; i < samples-1; i++)
	{
	  // If noisy break glass...
	  //h0->Fill ( i, (p1->iValue(i-1,0)+p1->iValue(i,0)+p1->iValue(i+1,0))/3.0 );
	  //h1->Fill ( i, (p1->iValue(i-1,1)+p1->iValue(i,1)+p1->iValue(i+1,1))/3.0 );
	  //h2->Fill ( i, (p1->iValue(i-1,2)+p1->iValue(i,2)+p1->iValue(i+1,2))/3.0 );
	  //h3->Fill ( i, (p1->iValue(i-1,3)+p1->iValue(i,3)+p1->iValue(i+1,3))/3.0 );
	  //h4->Fill ( i, (p1->iValue(i-1,4)+p1->iValue(i,4)+p1->iValue(i+1,4))/3.0 );
	  //h5->Fill ( i, (p1->iValue(i-1,5)+p1->iValue(i,5)+p1->iValue(i+1,5))/3.0 );

	  h0->Fill ( i, p1->iValue(i,0) );
	  h1->Fill ( i, p1->iValue(i,1) );
	  h2->Fill ( i, p1->iValue(i,2) );
	  h3->Fill ( i, p1->iValue(i,3) );
	  h4->Fill ( i, p1->iValue(i,4) );
	  h5->Fill ( i, p1->iValue(i,5) );

	  h0cc->Fill ( i, cc->caen_corrected(i,0) );
	  h1cc->Fill ( i, cc->caen_corrected(i,1) );
	  h2cc->Fill ( i, cc->caen_corrected(i,2) );
	  h3cc->Fill ( i, cc->caen_corrected(i,3) );
	  h4cc->Fill ( i, cc->caen_corrected(i,4) );
	  h5cc->Fill ( i, cc->caen_corrected(i,5) );
	  
	  Scott->theTraces[0]->voltage.push_back(cc->caen_corrected(i,0));
	  Scott->theTraces[1]->voltage.push_back(cc->caen_corrected(i,1));
	  Scott->theTraces[2]->voltage.push_back(cc->caen_corrected(i,2));
	  Scott->theTraces[3]->voltage.push_back(cc->caen_corrected(i,3));
	  Scott->theTraces[4]->voltage.push_back(cc->caen_corrected(i,4));
	  Scott->theTraces[5]->voltage.push_back(cc->caen_corrected(i,5));

	}

      //  Remove to speed up the analysis...
      Scott->theTraces[0]->FitLeftEdge(3);
      Scott->theTraces[1]->FitLeftEdge(3);
      Scott->theTraces[2]->FitLeftEdge(3);
      Scott->theTraces[3]->FitLeftEdge(3);
      Scott->theTraces[4]->FitLeftEdge(3);
      Scott->theTraces[5]->FitLeftEdge(3);
      
      S1->Fill( Scott->theTraces[0]->PulseHeight());
      S2->Fill( Scott->theTraces[1]->PulseHeight());
      S3->Fill( Scott->theTraces[2]->PulseHeight());
      C1->Fill( Scott->theTraces[3]->PulseHeight());
      C2i->Fill(Scott->theTraces[4]->PulseHeight());
      C2o->Fill(Scott->theTraces[5]->PulseHeight());
      C2iVsC2o->Fill(Scott->theTraces[5]->PulseHeight(), Scott->theTraces[4]->PulseHeight());
      
      double RefTime = Scott->theTraces[2]->PulseTime()-600.0;
      S1t->Fill( Scott->theTraces[0]->PulseTime()-RefTime);
      S2t->Fill( Scott->theTraces[1]->PulseTime()-RefTime);
      S3t->Fill( Scott->theTraces[2]->PulseTime()-RefTime);
      C1t->Fill( Scott->theTraces[3]->PulseTime()-RefTime);
      C2it->Fill(Scott->theTraces[4]->PulseTime()-RefTime);
      C2ot->Fill(Scott->theTraces[5]->PulseTime()-RefTime);
      double C2it = Scott->theTraces[4]->PulseTime()-RefTime;
      double C2ot = Scott->theTraces[5]->PulseTime()-RefTime;
      if (C2ot>497 && C2ot<504 )
	{
	  C2iVsC2oTimed->Fill(Scott->theTraces[5]->PulseHeight(), Scott->theTraces[4]->PulseHeight());
	}

      C1tVsC2ot->Fill(Scott->theTraces[5]->PulseTime()-RefTime, Scott->theTraces[3]->PulseTime()-RefTime);
      C1tVsC1->Fill  (Scott->theTraces[3]->PulseHeight(),       Scott->theTraces[3]->PulseTime()-RefTime);

      S1Count->Fill(Scott->theTraces[0]->PeakCount(400,800));
      S2Count->Fill(Scott->theTraces[1]->PeakCount(250,500));
      S3Count->Fill(Scott->theTraces[2]->PeakCount(200,400));
      C1Count->Fill(Scott->theTraces[3]->PeakCount(75,150));
      C2iCount->Fill(Scott->theTraces[4]->PeakCount(75,150));
      C2oCount->Fill(Scott->theTraces[5]->PeakCount(400,800));

      delete p1;
      
    }
  
  // Reading the PSI "oscilloscope"...
  Packet *p2 = e->getPacket(1001);
  if (p2)
    {
      int samples = p2->iValue(0, "SAMPLES"); 
      for ( int i = 0; i < samples; i++)
	{
	  
	  h6->Fill ( i, p2->rValue(i,0) );
	  h7->Fill ( i, p2->rValue(i,1) );

	  //Scott->theTraces[6]->voltage.push_back(p2->rValue(i,0));
	  //Scott->theTraces[7]->voltage.push_back(p2->rValue(i,1));
	  
	}
      
      //Scott->theTraces[6]->FitLeftEdge();
      //Scott->theTraces[7]->FitLeftEdge();
      
      //phase1->Fill( Scott->theTraces[6]->PulseTime());
      //phase2->Fill( Scott->theTraces[7]->PulseTime());
      
      delete p2;
      
    }
  
  //  Reading the SRS system...
  Packet *p = e->getPacket(1010);
  if (p)
    {
      //p->dump();
      static bool FirstEvent = true;
      if (FirstEvent)
	{
	  int nhb = p->iValue(0,"NHYBRIDS");
	  cout << "Reports for " << nhb << " Hybrids:" << endl;
	  for (int i=0; i<nhb; i++)
	    {
	      cout << i << ": " << p->iValue(i,"NSAMPLES") << endl;
	    }
	  
	  
	  FirstEvent = false;
	}
      
      int nhb = p->iValue(0,"NHYBRIDS");
      if (nhb>4)
	{
	  
	  
	  int idigi;
	  for (idigi = 0; idigi< 4500; idigi++)
	    {
	      h1a->Fill( idigi, p->iValue(idigi, 0, "RAWSAMPLES"));
	      h1b->Fill( idigi, p->iValue(idigi, 2, "RAWSAMPLES"));
	      h1c->Fill( idigi, p->iValue(idigi, 4, "RAWSAMPLES"));
	      h1d->Fill( idigi, p->iValue(idigi, 6, "RAWSAMPLES"));
	      //h1e->Fill( idigi, p->iValue(idigi, 8, "RAWSAMPLES"));
	      
	      if (idigi>3500)
		{
		  ha->Fill( p->iValue(idigi, 0, "RAWSAMPLES"));
		  hb->Fill( p->iValue(idigi, 2, "RAWSAMPLES"));
		  hc->Fill( p->iValue(idigi, 4, "RAWSAMPLES"));
		  hd->Fill( p->iValue(idigi, 6, "RAWSAMPLES"));
		  //he->Fill( p->iValue(idigi, 8, "RAWSAMPLES"));
		}
	    }
	  
	  for (int ibin = 2; ibin<254; ibin++)
	    {
	      if ( (h1a->GetBinContent(ibin-1) < 1220) &
		   (h1a->GetBinContent(ibin) < 1220) &
		   (h1a->GetBinContent(ibin+1) < 1220) )
		{
		  edgea->Fill(ibin);
		  break;
		}
	    }
	  
	  for (int ibin = 1; ibin<255; ibin++)
	    {
	      if ( (h1b->GetBinContent(ibin-1) < 1220) &
		   (h1b->GetBinContent(ibin) < 1220) &
		   (h1b->GetBinContent(ibin+1) < 1220) )
		{
		  edgeb->Fill(ibin);
		  break;
		}
	    }
	  
	  for (int ibin = 1; ibin<255; ibin++)
	    {
	      if ( (h1c->GetBinContent(ibin-1) < 1220) &
		   (h1c->GetBinContent(ibin) < 1220) &
		   (h1c->GetBinContent(ibin+1) < 1220) )
		{
		  edgec->Fill(ibin);
		  break;
		}
	    }
	  
	  for (int ibin = 1; ibin<255; ibin++)
	    {
	      if ( (h1d->GetBinContent(ibin-1) < 1220) &
		   (h1d->GetBinContent(ibin) < 1220) &
		   (h1d->GetBinContent(ibin+1) < 1220) )
		{
		  edged->Fill(ibin);
		  break;
		}
	    }

	  for (int ibin = 1; ibin<255; ibin++)
	    {
	      if ( (h1e->GetBinContent(ibin-1) < 1220) &
		   (h1e->GetBinContent(ibin) < 1220) &
		   (h1e->GetBinContent(ibin+1) < 1220) )
		{
		  edgee->Fill(ibin);
		  break;
		}
	    }

	  //  TPC Hybrids
	  for (int JINDEX=0; JINDEX<4; JINDEX++)
	    {
	      for (int j=0; j<128; j++)
		{
		  for (int i=0; i<(p->iValue(JINDEX,"NSAMPLES")); i++)
		    {
		      AZig::Raw[j+JINDEX*128].push_back(p->iValue(j,i,JINDEX*2));  // NOTE weird 2X indexing...
		    }
		}
	    }

	  //  For now we'll begin with hybrid #4 which is the HBD...
	  int JINDEX=4;
	  for (int j=0; j<128; j++)
	    {
	      for (int i=0; i<(p->iValue(JINDEX,"NSAMPLES")); i++)
		{
		  APad::Raw[j].push_back(p->iValue(j,i,JINDEX*2));  // NOTE weird 2X indexing...
		}
	    }
	  
	  /* process HBD data */
	  AZig::ApplyCalibration();
	  for(int i=0; i<Scott->theZigs.size(); i++)
	    Scott->theZigs[i]->DetermineQ();	  	  
	  
	  APad::DetermineCommonMode();
	  APad::ApplyCalibration();
	  for(int i=0; i<Scott->thePads.size(); i++)
	    Scott->thePads[i]->DetermineQ();

	  FillCommonMode();

	  FillRawHist();
	  FillRawZigHist();

	  FillCalHist();
	  FillCalZigHist();

	  FillMaxCharge();
	  FillMaxTPCCharge();
	}      
      else
	{
	  cout << "ERROR " << nhb << " Hybrids Found:" << endl;
	  for (int i=0; i<nhb; i++)
	    {
	      cout << i << ": " << p->iValue(i,"NSAMPLES") << endl;
	    }	  
	}

      delete p;
    }
  
  return 0;
  
}



int prdfopen ( const char *filename)
{
  if ( os) 
    {
      cout << "file is already open " << endl;
      return -1;
    }

  os = new prdfoStream (filename);
  if ( os->is_defunct() )
    {
      delete os;
      os = 0;
      return -1;
    }

  return 0;
}

int prdfclose ()
{
  if ( !os) 
    {
      cout << "no file open " << endl;
      return -1;
    }
  delete os;
  os = 0;
  return 0;
}

////////////////////////////////////
// PID.C               //
////////////////////////////////////

//  This makes a histogram that can show the raw hits.

#include "PID.h"
#include "cyclops.h"
#include "ATrace.h"

#include <iostream>

using namespace std;

bool IsBeam()
{

  cyclops *Scott = cyclops::instance();
  double S1  = Scott->theTraces[0]->PulseHeight();
  double S2  = Scott->theTraces[1]->PulseHeight();
  double S3  = Scott->theTraces[2]->PulseHeight();

  double RefTime = Scott->theTraces[2]->PulseTime()-600.0;
  double  S1t = Scott->theTraces[0]->PulseTime()-RefTime;
  double  S2t = Scott->theTraces[1]->PulseTime()-RefTime;
  double  S3t = Scott->theTraces[2]->PulseTime()-RefTime;

  int  S1Count= Scott->theTraces[0]->PeakCount(400,800);
  int  S2Count= Scott->theTraces[1]->PeakCount(250,500);
  int  S3Count= Scott->theTraces[2]->PeakCount(200,400);

  if (S1Count != 1) return false;
  if (S2Count != 1) return false;
  if (S3Count != 1) return false;
  if (S1 < 1000   ) return false;
  if (S2 < 600    ) return false;
  if (S3 < 500    ) return false;
  if (S1t<552 || S1t>557) return false;
  if (S2t<556 || S2t>564) return false;

  return true;
}

bool ChrHit()
{

  cyclops *Scott = cyclops::instance();
  double C1  = Scott->theTraces[3]->PulseHeight();
  double C2i = Scott->theTraces[4]->PulseHeight();
  double C2o = Scott->theTraces[5]->PulseHeight();

  double RefTime = Scott->theTraces[2]->PulseTime()-600.0;
  double  C1t = Scott->theTraces[3]->PulseTime()-RefTime;
  double C2it = Scott->theTraces[4]->PulseTime()-RefTime;
  double C2ot = Scott->theTraces[5]->PulseTime()-RefTime;

  int  C1Count= Scott->theTraces[3]->PeakCount(75,150);
  int C2iCount= Scott->theTraces[4]->PeakCount(75,150);
  int C2oCount= Scott->theTraces[5]->PeakCount(400,800);

  if (C1Count  != 1) return false;
  if (C2oCount != 1) return false;

  if (C1<300 || C1>1100) return false;
  if (C2o<2100 || C2o>2200) return false;

  if (C1t<432  || C1t>441 ) return false;
  if (C2ot<497 || C2ot>504) return false;

  return true;
}

bool ChrMiss()
{

  cyclops *Scott = cyclops::instance();
  double C1  = Scott->theTraces[3]->PulseHeight();
  double C2i = Scott->theTraces[4]->PulseHeight();
  double C2o = Scott->theTraces[5]->PulseHeight();

  if (C1  > 22) return false;
  if (C2i > 25) return false;
  if (C2o > 22) return false;

  return true;
}

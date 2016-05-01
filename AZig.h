#ifndef __AZIG_H__
#define __AZIG_H__

//
//  Hello AZig Fans:
//
//  The Zigs are charge collection regions on the TPC.  These are define 
//  by their corners, but have an internal hard-coded "ziggy" shape.  Please
//  note that our TPC drifts the charge downward.  This means that the
//  "corners" are in x-z space.
//
//  Methods of the Zig class will return both the X center and Z center of each Zig.
//  They also carry some charge and a time.
//
//  The result of the analysis of a Zig is that you will be able to determine
//  a so-called "SpacePoint" that will later be used to reconstruct tracks by
//  combining the (x,z) from the Zig's coordinate with the y that is determined
//  from the time information.
//
//
//                                                TKH & BA
//                                                4-12-2016
//

#include <vector>
#include <string>

#include <iostream>

#define NumberOfZigs 512  // Actually # READOUTS, noting that some readouts don't have Zigs attached.

class TPolyLine;
class TH1D;
class TF1;

class AZig
{
public:
  AZig(double X1, double Z1, double X2, double Z2);
  virtual ~AZig() {}

  void Clear() {q=0;}

  // sets...
  void SetQ(double Q) {q = Q;}
  void SetT(double T) {t = T;} /* @TODO Do we need this? */
  void AddQ(double Q) {q += Q;}

  // gets...
  double const XCenter()  {return (x1+x2)/2.0;}
  double const ZCenter()  {return (z1+z2)/2.0;}
  double Q            ()  {return q;}
  double T            ()  {return t;} /* @TODO Do we need this? */
  int MyID            ()  {return myID;}
  void SetMyID(int ID)    {myID = ID;}
  static int NCreated ()  {return nextID;}

  // Graphics:
  virtual void Draw(double);
  virtual void Draw3D(double);
  int color(int);

  double NumSigma() {return q/Gains[myID]/Sigmas[myID];}
  bool IsHit() 
  {
    if (UseSigma) return (NumSigma()>SigmaCut);
    else return (q>PulseCut);
  }

  void DetermineQ();

  void Report()
  {
    std::cout << " id: "     << myID;
    std::cout << " q: "      << q;
    std::cout << " t: "      << t;
    std::cout << std::endl;
  }

  //  OK..we're going for it...
  //  We shall be using the very same sytle of manipulations
  //  for the Zigs as we developed for the Hexes.  This means
  //  That we shall make a set of static member variables that
  //  treat the common data that all Hexes need to access.
  //
  //  This includes calibrations (one copy for all), cuts for
  //  hit definitions, Raw and Cal "Ziged" data.

  //  Flags for calibration operation...
  static bool FastQ;
  static bool UseSigma;
  static double SigmaCut;
  static double PulseCut;
  static std::string CommonModeMethod;

  //  Collective Data...
  static std::vector<int>    Raw[NumberOfZigs];
  static std::vector<double> Cal[NumberOfZigs];  //array OF vectors

  //  Calibration storage...
  static double Pedestals[NumberOfZigs];
  static double Sigmas[NumberOfZigs];
  static double Gains[NumberOfZigs];
  static double CommonMode;

  //  Methods for applying calibrations...
  static void WriteCalibration();
  static void ReadCalibration();
  static void DetermineCommonMode();
  static void ApplyCalibration();

  static TH1D *Pulse; /* @TODO Do we need this? */
  static TF1  *blue; /* @TODO Do we need this? */

protected:
  double x1;
  double z1;
  double x2;
  double z2;
  double z0;
  double t;  /* @TODO Do we need this? */
  double q;

  static int nextID;
  int myID;

};

#endif /* __AZIG_H__ */

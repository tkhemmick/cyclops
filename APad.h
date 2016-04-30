#ifndef __APAD_H__
#define __APAD_H__

//
//  Hello Apad Fans:
//
//  The pads are charge collection regions.  These are assumed to be 
//  simple rectangles that can be defined by their corners.  Methods
//  of the pad class will return both the X center and Y center.
//  They also carry some charge.
//
//
//                                                HG & TKH
//                                                11-16-2011
//

#include <vector>
#include <string>

#include <iostream>

#define NumberOfPads 128  // Actually # READOUTS, it is possible that some readouts don't have pads attached.

class TPolyLine;
class TH1D;
class TF1;

class APad
{
public:
  APad(double, double, double, double, double);
  virtual ~APad() {}

  void Clear() {q=0;}

  // sets...
  void SetQ(double Q) {q = Q;}
  void SetT(double T) {t = T;} /* @TODO Do we need this? */
  void AddQ(double Q) {q += Q;}

  // gets...
  double const XCenter()  {return (x1+x2)/2.0;}
  double const YCenter()  {return (y1+y2)/2.0;}
  double const ZCenter()  {return z0;}
  double Q            ()  {return q;}
  double T            ()  {return t;} /* @TODO Do we need this? */
  int MyID            ()  {return myID;}
  void SetMyID(int ID)    {myID = ID;}
  static int NCreated ()  {return nextID;}

  // Graphics:
  virtual void Draw(double);
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
  //  for the pads as we developed for the Hexes.  This means
  //  That we shall make a set of static member variables that
  //  treat the common data that all Hexes need to access.
  //
  //  This includes calibrations (one copy for all), cuts for
  //  hit definitions, Raw and Cal "paded" data.

  //  Flags for calibration operation...
  static bool FastQ;
  static bool UseSigma;
  static double SigmaCut;
  static double PulseCut;
  static std::string CommonModeMethod;

  //  Collective Data...
  static std::vector<int>    Raw[NumberOfPads];
  static std::vector<double> Cal[NumberOfPads];  //array OF vectors

  //  Calibration storage...
  static double Pedestals[NumberOfPads];
  static double Sigmas[NumberOfPads];
  static double Gains[NumberOfPads];
  static std::vector<double> CommonMode;  // There is one common mode value per TIME SLICE.

  //  Methods for applying calibrations...
  static void WriteCalibration();
  static void ReadCalibration();
  static void DetermineCommonMode();
  static void ApplyCalibration();

  static TH1D *Pulse; /* @TODO Do we need this? */
  static TF1  *blue; /* @TODO Do we need this? */

protected:
  double x1;
  double y1;
  double x2;
  double y2;
  double z0;
  double t;  /* @TODO Do we need this? */
  double q;

  static int nextID;
  int myID;

};

#endif /* __APAD_H__ */

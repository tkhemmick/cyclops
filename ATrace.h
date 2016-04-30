#ifndef __ATRACE_H__
#define __ATRACE_H__

//
//  Hello ATrace Fans,
//       
//      This class will represent a trace from the (ADC) Oscilloscope. 
//  In the initial implementation, it will hold the timebins of the ADC.
//  It will have the methods of finding its maximum (averaged over +/- n
//  channels). 
//                                                      ~TKH and MAB

#include <vector>

class TF1;
class TH1D;

class ATrace
{
 public:
  
  ATrace();
  ~ATrace();

  std::vector<double> voltage;

  double FindMaximum(int n);  
  void   FitLeftEdge(int inset=0);
  double PulseHeight(){return height;}
  double PulseTime(){return time;}
  int    PeakCount(double low, double high);

  TH1D *trace;

  static bool FastEdgeFinder;

 protected:
  
  static int numtraces;
  int mytrace;

  double height;
  double time;
  TF1 *fermi;

  char fcn_name[500];
  char hst_name[500];

  void RestoreBaseline();
};

#endif /* __ATRACE_H__ */

#ifndef __cyclops_H__
#define __cyclops_H__


//
//  Hello EIC Fans:
//
//    Welcome to cyclops.  cyclops is a one eyed monster.
//
//  The code package cyclops is used to perform pattern recognition
//  on the cherenkov test beam experiment from Spring 2016.  
//  It supports ONE tpc detector and one 9-pad HBD...the cyclops.
//
//    If this code were written for PHENIX, data objects 
//  (Blobs, Clusters, etc) would be <vector> located in a PHENIX "Node Tree" 
//  (linked list).  Since EIC is not PHENIX, we have  TEMPORARILY(!!!) located 
//  these objects inside cyclops awaiting an EIC standard for handling such details.
//
//    This file principally declares vectors of object types that are essential to the 
//  pattern recognition itself.
//
//
//                                                BA, & TKH
//                                                4-9-2016
//

#include <vector>
#include "TRandom.h"
#include "AZig.h"

// Forward declarations...
class ATrace;
class APad;

class cyclops
{
 public:
  static cyclops *instance()
  {
    if (__instance) return __instance;
    __instance = new cyclops();
    return __instance;
  }
  virtual ~cyclops();

  /* Beamline Counter */
  std::vector<ATrace*> theTraces; // 6 beam line counters and 2 phase indicators.

  /* HBD */
  std::vector<APad*> thePads;

  /* TPC */  
  std::vector<AZig*> theZigs;

  void Report();
  void ClearTheDetector();

  TRandom Randy;

protected:
  cyclops();
  static cyclops *__instance;

  void CreateHBD();
  void CreateTPC();

  // Bob's code that knows where the pads are located...
  void MakeTPCMaps();
  int xmap[NumberOfZigs];  // Output from Bob's code.
  int ymap[NumberOfZigs];  // Output from Bob's code.

};

#endif /* __cyclops_H__ */

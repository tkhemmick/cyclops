#ifndef __PID_H__
#define __PID_H__

bool IsBeam();  // Decides whether this event contains a valid beam particle...
bool ChrHit();  // Decides that this particle fires the beamline cherenkov...
bool ChrMiss(); // Decides that this particle did not fire the beamline cherenkov...

//  NOTE:  Particles whose cherenkov response is ambiguouis will
//         not be characterized as hit nor as miss.

#endif /*__PID_H__*/

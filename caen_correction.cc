#include <iostream>
#include <iomanip>
#include <fstream>

#include "caen_correction.h"
#include "packet.h"

using namespace std;

caen_correction::caen_correction ( const char *chip0, const char *chip1, const char *chip2, const char *chip3) 
{ 

  int nsample[9];

  _broken = 0;

  int index;
  ifstream IN[4];

  int adccorr[9];
  float timecorr;

  IN[0].open(chip0,ios_base::in);
  IN[1].open(chip1,ios_base::in);
  IN[2].open(chip2,ios_base::in);
  IN[3].open(chip3,ios_base::in);

  int i;
  for ( i = 0; i < 4; i++)
    {
      if ( ! IN[i].is_open())
	{
	  _broken = 1;
	}
      else 
	{
	  
	  while ( IN[i] >> index
		  >> adccorr[0]
		  >> adccorr[1]
		  >> adccorr[2]
		  >> adccorr[3]
		  >> adccorr[4]
		  >> adccorr[5]
		  >> adccorr[6]
		  >> adccorr[7]
		  >> adccorr[8]
		  >> timecorr
		  >> nsample[0]
		  >> nsample[1]
		  >> nsample[2]
		  >> nsample[3]
		  >> nsample[4]
		  >> nsample[5]
		  >> nsample[6]
		  >> nsample[7]
		  >> nsample[8]
		  )
	    {
	      //	      cout << i << "  " <<  index << endl; 
	      
	      base [index][i*9 + 0]= adccorr[0];
	      base [index][i*9 + 1]= adccorr[1];
	      base [index][i*9 + 2]= adccorr[2];
	      base [index][i*9 + 3]= adccorr[3];
	      base [index][i*9 + 4]= adccorr[4];
	      base [index][i*9 + 5]= adccorr[5];
	      base [index][i*9 + 6]= adccorr[6];
	      base [index][i*9 + 7]= adccorr[7];
	      base [index][i*9 + 8]= adccorr[8];
	      timevec[index][i] = timecorr;
	    }
	  IN[i].close();
	}
    }
}

int caen_correction::init (Packet *p)
{
  int chip,c,i,idx;

  for ( chip = 0; chip < 4; chip++)
    {

      int cell = p->iValue(chip,"INDEXCELL");

      //correct time for each chip
      idx = cell;
      for ( i = 0; i < 1024; i++)
	{
	  current_time[i][chip] = timevec[idx][chip];
	  if (idx >=1024) idx=0;
	}

      // the adc samples
      for ( c = 0; c < 8; c++)  
	{
	  idx = cell;
	  for ( i = 0; i < 1024; i++)
	    {
	      current_wave[i][chip*8+c] = p->iValue(i,chip*8+c) - base[idx][chip*9+c];
	      idx++;
	      if (idx >=1024) idx=0;
	    }
	}

      //      // the TR cells
      // idx = cell;
      //for ( i = 0; i < 1024; i++)
      //	{
      //	  if (chip < 2)	  current_wave[i][chip*8+8] = p->iValue(i,"TR0") - base[idx][chip*9+8];
      //	  else  current_wave[i][chip*9+8] = p->iValue(i,"TR1") - base[idx][chip*9+8];
      //	  idx++;
      //	  if (idx >=1024) idx=0;
      //	}
    }
  return 0;
}
	  
float caen_correction::caen_corrected(const int sample, const int channel) const
{
  if ( sample < 0 || sample >1023 || channel < 0 || channel > 31) return 0;
  return current_wave[sample][channel];
}

float caen_correction::caen_time(const int sample, const int channel) const
{
  if ( sample < 0 || sample >1023 || channel < 0 || channel > 31) return 0;
  return current_time[sample][channel/8];
}

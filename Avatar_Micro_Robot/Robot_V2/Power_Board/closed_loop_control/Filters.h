/*==============================================================================
File: Filters.h
 
Description: This module encapsulates several digital signal processing
  algorithms.  It is still very much a work in progress...
  
Responsible Engineer: Stellios Leventis (sleventis@robotex.com)
==============================================================================*/
#ifndef FILTERS_H
#define FILTERS_H
/*---------------------------Dependencies-------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#define MAX_N_FILTERS 16

/*---------------------------Public Functions---------------------------------*/

/*******************************************************************************
Function: IIRFilter
Description: Passes the input through an Infinite-Impulse-Response filter
  characterized by the parameter alpha.
Paramters:
  const uint8_t i,          the filter index
        const float x,            the current sample to be filtered
        const float alpha,        the knob on how much to filter, [0,1)
                                  alpha = t / (t + dT)
                            where t = the low-pass filter's time-constant
                                 dT = the sample rate
  const bool should_reset,  whether to clear the history
*******************************************************************************/
float IIRFilter(uint8_t i, float x, float alpha, bool should_reset);

#endif

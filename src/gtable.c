#include "gtable.h"

#include <stdlib.h>
#include <math.h>

GTABLE* new_gtable(unsigned long length)
{
    unsigned long i;
    GTABLE* gtable = NULL;
    if(length == 0) goto error;
    
    gtable = (GTABLE* ) malloc(sizeof(GTABLE));
    if(gtable == NULL) goto error;

    gtable->table = (double*) malloc((length + 1) * sizeof(double));
    if(gtable->table == NULL){
        free(gtable);
        goto error;
    }

    gtable->length = length;
    for(i=0; i <= length; i++)
        gtable->table[i] = 0.0;
    
    return gtable;

error:
    return NULL;
}

void free_gtable(GTABLE** gtable)
{
    // pointer to GTABLE is not NULL 
    // AND GTABLE is not null 
    // AND table array ptr is not NULL 
    if (gtable && *gtable && (*gtable)->table) {
        free((*gtable)->table);
        free(*gtable);
        *gtable = NULL;
    }
}

void norm_gtable(GTABLE* gtable)
{
    unsigned long i;
    double val, maxamp = 0.0;

    for(i=0; i < gtable->length; i++){
        val = fabs(gtable->table[i]);
        if(maxamp < val)
            maxamp = val;
    }
    
    maxamp = 1.0 / maxamp;
    for(i=0; i < gtable->length; i++)
        gtable->table[i] *= maxamp;
    gtable->table[i] = gtable->table[0];
}

// WAVES

// GTABLE* new_sine() {

// }

GTABLE* new_triangle(unsigned long length, unsigned long nharms) {
    unsigned long i,j;
    double step, amp;
    GTABLE* gtable;
    int harmonic = 1;

    if (length == 0 || nharms == 0 || nharms >= length/2) {
        return NULL;
    }

    gtable = new_gtable(length);
    if (gtable == NULL) {
        return NULL;
    }

    // truncated fourier series
    step = TWOPI / length;
    for (i=0; i <nharms; i++) {
        amp = 1.0 / (harmonic*harmonic); // normalise signal power, Ps = (nharms)^2
        for (j=0; j<length; j++)
            gtable->table[j] += amp * cos(step*harmonic*j);
        harmonic += 2;
    }

    norm_gtable(gtable);
    return gtable;
}

GTABLE* new_square(unsigned long length,unsigned long nharms)
{
    unsigned long i,j;
    double step,amp;
    GTABLE* gtable;
    int harmonic = 1;
    
    if(length == 0 || nharms == 0 || nharms >= length/2)
        return NULL;

    gtable = new_gtable(length);
    if(gtable == NULL)
        return NULL;
    
    step = TWOPI / length ;
    for(i=0; i < nharms; i++){
        amp = 1.0 / harmonic;
        for(j=0;j < gtable->length;j++)
            gtable->table[j] += amp * sin(step*harmonic * j);
        harmonic+=2;
    }
    
    norm_gtable(gtable);
    return gtable;
}

GTABLE* new_saw(unsigned long length,unsigned long nharms, int up)
{
    unsigned long i,j;
    double step,val,amp = 1.0;
    GTABLE* gtable;
    int harmonic = 1;
    if(length == 0 || nharms == 0 || nharms >= length/2)
        return NULL;
    
    gtable = new_gtable(length);
    if(gtable == NULL)
        return NULL;

    step = TWOPI / length ;
    if(up)
        amp = -1;
    
    for(i=0; i < nharms; i++){
        val = amp / harmonic;
        for(j=0;j < gtable->length;j++)
            gtable->table[j] += val * sin(step*harmonic * j);
        harmonic++;
    }

    norm_gtable(gtable);
    return gtable;
}

// OSCILLATOR TABLE MODIFIERS
// destructor is not required due to no internal memory

OSCILT* new_oscilt(double srate, const GTABLE* gtable, double phase) {
    OSCILT* p_osc;
    /* check gtable valid */
    if (gtable == NULL || gtable->table == NULL || gtable->length == 0)
        return NULL;

    p_osc = (OSCILT*)malloc(sizeof(OSCILT));
    if (p_osc == NULL)
        return NULL;

    // init oscillator
    p_osc->osc.curfreq = 0.0;
    p_osc->osc.curphase = gtable->length * phase;
    p_osc->osc.incr = 0.0;

    // init gtable
    p_osc->gtable = gtable;
    p_osc->dtablen = gtable->length;
    p_osc->sizeovrsr = p_osc->dtablen / (double) srate;

    return p_osc;
}

// Interpolating tick functions

double tabtick(OSCILT* p_osc, double freq)
{
    int index = (int) (p_osc->osc.curphase);
    double val;
    double dtablen = p_osc->dtablen, curphase = p_osc->osc.curphase;
    double* table = p_osc->gtable->table;

    // update frequency
    if(p_osc->osc.curfreq != freq){
        p_osc->osc.curfreq = freq;
        p_osc->osc.incr = p_osc->sizeovrsr * p_osc->osc.curfreq;
    }

    // wrap around phase
    curphase += p_osc->osc.incr;
    while(curphase >= dtablen)
        curphase -= dtablen;
    while(curphase < 0.0)
        curphase += dtablen;

    p_osc->osc.curphase = curphase;
    return table[index];
}

double tabitick(OSCILT* p_osc, double freq) {
    // identify samples phase lies between, int cast always rounds down.
    int base_index = (int) p_osc->osc.curphase;
    unsigned long next_index = base_index + 1;
    
    double frac,slope,val;
    double dtablen = p_osc->dtablen, curphase = p_osc->osc.curphase;
    double* table = p_osc->gtable->table;

    // update frequency if necessary
    if(p_osc->osc.curfreq != freq){
        p_osc->osc.curfreq = freq;
        p_osc->osc.incr = p_osc->sizeovrsr * p_osc->osc.curfreq;
    }

    // linear interpolation
    frac = curphase - base_index;
    val = table[base_index];
    slope = table[next_index] - val;
    val += (frac * slope);
    
    // wrap around phase
    curphase += p_osc->osc.incr;
    while(curphase >= dtablen)
        curphase -= dtablen;
    while(curphase < 0.0)
        curphase += dtablen;

    p_osc->osc.curphase = curphase;
    return val;
}


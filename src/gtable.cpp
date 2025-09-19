#include "gtable.h"

#include <stdlib.h>
#include <math.h>

GTable::GTable(unsigned long length, Wave wave = ZERO, unsigned long nharms=1) {
    _table = std::vector<double>(length+1);
    _length = length;
    
    switch (wave) {
        case SINE:
            new_sine(length);
            break;
        case TRIANGLE:
            new_triangle(length, nharms);
            break;
        case SQUARE:
            new_square(length, nharms);
            break;
        case SAW_UP:
            new_saw(length, nharms, 0);
            break;
        case SAW_DOWN:
            new_saw(length, nharms, 1);
            break;
        default:
            new_zeros(length);
    }
}

inline void GTable::resize(unsigned long length) {
    if (length!=_length) {
        _length = length;
        _table.resize(_length+1, 0.0);
    }
}

void GTable::norm() {
    unsigned long i;
    double val, maxamp = 0.0;

    for(i=0; i < _length; i++){
        val = fabs(_table[i]);
        if(maxamp < val)
            maxamp = val;
    }
    
    maxamp = 1.0 / maxamp;
    for(i=0; i < _length; i++)
        _table[i] *= maxamp;
    _table[i] = _table[0];
}

void GTable::new_zeros(unsigned long length) {
    resize(length);

    for(unsigned long i=0; i <= _length; i++)
        _table[i] = 0.0;
}

void GTable::new_sine(unsigned long length) {
    unsigned long i;
    double step;

    resize(length);

    step = TWOPI /_length;
    for (i=0; i<_length; i++) {
        _table[i] = sin(step*i);
    }
}

void GTable::new_triangle(unsigned long length, unsigned long nharms) {
    unsigned long i,j;
    double step, amp;
    int harmonic = 1;

    // if (length == 0 || nharms == 0 || nharms >= length/2) {
    //     return NULL;
    // }

    resize(length);

    // truncated fourier series
    step = TWOPI / _length;
    for (i=0; i <nharms; i++) {
        amp = 1.0 / (harmonic*harmonic); // normalise signal power, Ps = (nharms)^2
        for (j=0; j<_length; j++)
            _table[j] += amp * cos(step*harmonic*j);
        harmonic += 2;
    }

    norm();
}

void GTable::new_square(unsigned long length, unsigned long nharms = 5)
{
    unsigned long i,j;
    double step,amp;
    int harmonic = 1;
    
    // if(length == 0 || nharms == 0 || nharms >= length/2)
    //     return NULL;

    resize(length);
    
    step = TWOPI / _length;
    for(i=0; i < nharms; i++){
        amp = 1.0 / harmonic;
        for(j=0;j <_length;j++)
            _table[j] += amp * sin(step*harmonic * j);
        harmonic+=2;
    }
    
    norm();
}

void GTable::new_saw(unsigned long length,unsigned long nharms = 5, int up = 0)
{
    unsigned long i,j;
    double step,val,amp = 1.0;
    int harmonic = 1;

    // if(length == 0 || nharms == 0 || nharms >= length/2)
    //     return NULL;
    
    resize(length);

    step = TWOPI / _length;
    if(up) amp = -1;
    
    for(i=0; i < nharms; i++){
        val = amp / harmonic;
        for(j=0;j < _length;j++)
            _table[j] += val * sin(step*harmonic * j);
        harmonic++;
    }

    norm();
}

unsigned long GTable::length() {
    return _table.size();
}

double GTable::get(unsigned long index) {
    return _table[index];
}

void GTable::set(unsigned long index, double val) {
    _table[index] = val;
}


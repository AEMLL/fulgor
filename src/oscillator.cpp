#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "oscillator.h"

Oscillator::Oscillator(unsigned long srate, double phase)
{
	_twopiovrsr = TWOPI / (double) srate;
	_curfreq = 0.0;
	_incr = 0.0;

	/* make sure the inputted phase doesn't exceed the range 
	by taking the fractional part of the phase value */ 
	if (phase > 1.0)
		phase = phase - (int)phase;
	if (phase < 0.0)
		phase = (phase + (int)phase) * -1.0;
	/* phase offset is from 0 to 2*PI */
	_curphase = TWOPI*phase;
}

inline void Oscillator::updateFrequency(double freq) {
	if (_curfreq!=freq) {
		_incr = _twopiovrsr*freq;
	}
}

/* tick function for a sine waveform */
double Oscillator::sinetick(double freq)
{
	double val;
	
	val = sin(_curphase);
	updateFrequency(freq);
	_curphase += _incr;
	if (_curphase >= TWOPI)
		_curphase -= TWOPI;
	if (_curphase < 0.0)
		_curphase += TWOPI;
	
	return val;
}

/* tick function for a square waveform */
double Oscillator::sqtick(double freq)
{
	double val;
	 	
	updateFrequency(freq);
	val = (_curphase <= PI)?1.0:-1.0;	
	_curphase += _incr;
	if (_curphase >= TWOPI)
		_curphase -= TWOPI;
	if (_curphase < 0.0)
		_curphase += TWOPI;

	return val;
}

/* tick function for a square waveform
   with dynamic pulse wave modulation */
double Oscillator::pwmtick(double freq, double pwmod)
{
	double val;

	if (pwmod < 1.0) /* if pwmod < 1%, set minimum frequency by a factor of 0.02 */
		freq *= 0.02;	
	else if (pwmod > 99.0) /* if pwmod > 99%, set maximum frequency by a factor of 1.98 */
		freq *= 1.98;
	else
		freq *= pwmod/50.0; /* normal square wave is 50% */

	updateFrequency(freq);
	val = (_curphase <= PI)?1.0:-1.0;	
	_curphase += _incr;

	if (_curphase >= TWOPI)
		_curphase -= TWOPI;
	if (_curphase < 0.0)
		_curphase += TWOPI;

	return val;
}

/* tick function for a downward sawtooth waveform */
double Oscillator::sawdtick(double freq)
{
	double val;

	updateFrequency(freq);	
	val = 1.0 - 2.0 * (_curphase * (1.0 / TWOPI) ); 
	_curphase += _incr;
	if (_curphase >= TWOPI)
		_curphase -= TWOPI;
	if (_curphase < 0.0)
		_curphase += TWOPI;

	return val;
}

/* tick function for an upward sawtooth waveform */
double Oscillator::sawutick(double freq)
{
	double val;

	updateFrequency(freq);	
	val = (2.0 * (_curphase * (1.0 / TWOPI) )) - 1.0;	
	_curphase += _incr;
	if (_curphase >= TWOPI)
		_curphase -= TWOPI;
	if (_curphase < 0.0)
		_curphase += TWOPI;

	return val; 
}

/* tick function for a triangle waveform */
double Oscillator::tritick(double freq)
{
	double val;

	updateFrequency(freq);	
	/* rectified sawtooth */
	val = (2.0 * (_curphase * (1.0 / TWOPI) )) - 1.0;
	if (val < 0.0)
		val = -val;
	val = 2.0 * (val - 0.5);
	_curphase += _incr;
	if (_curphase >= TWOPI)
		_curphase -= TWOPI;
	if (_curphase < 0.0)
		_curphase += TWOPI;

	return val;
}

// OSCILLATOR TABLE MODIFIERS
// destructor is not required due to no internal memory

GTableOscillator::GTableOscillator(double srate, GTable *gtable, double phase)
: Oscillator(srate, phase), _gtable(gtable) 
{

    // if (gtable == NULL || gtable->_table == NULL || gtable->length == 0)
    //     return NULL;

    // init oscillator
    _curfreq = 0.0;
    _curphase = gtable->length() * phase;
    _incr = 0.0;

    // init gtable
    _dtablen = _gtable->length();
    _sizeovrsr = _dtablen / (double) srate;
}

// Interpolating tick functions

double GTableOscillator::tabtick(double freq)
{
    int index = (int) (_curphase);
    double val;

    // update frequency
    if(_curfreq != freq){
        _curfreq = freq;
        _incr = _sizeovrsr * _curfreq;
    }

    // wrap around phase
    _curphase += _incr;
    while(_curphase >= _dtablen)
        _curphase -= _dtablen;
    while(_curphase < 0.0)
        _curphase += _dtablen;

    return _gtable->get(index);
}

double GTableOscillator::tabitick(double freq) {
    // identify samples phase lies between, int cast always rounds down.
    int base_index = (int) _curphase;
    unsigned long next_index = base_index + 1;
    
    double frac,slope,val;

    // update frequency if necessary
    if(_curfreq != freq){
        _curfreq = freq;
        _incr = _sizeovrsr * _curfreq;
    }

    // linear interpolation
    frac = _curphase - base_index;
    val = _gtable->get(base_index);
    slope = _gtable->get(next_index) - val;
    val += (frac * slope);
    
    // wrap around phase
    _curphase += _incr;
    while(_curphase >= _dtablen)
        _curphase -= _dtablen;
    while(_curphase < 0.0)
        _curphase += _dtablen;
	
    return val;
}
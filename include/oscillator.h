#include "gtable.h"

/* Oscillator class adapted from The Programming Book. Functions calculate waveform realtime. */
class Oscillator {
public:
    // phase offset (0 <= phase <= 1)
    Oscillator(unsigned long srate, double phase);

    /* tick function for a sine waveform */
    double sinetick(double freq);

    /* tick function for a square waveform */
    double sqtick(double freq);

    /* tick function for a square waveform
    with dynamic pulse wave modulation */
    double pwmtick(double freq, double pwmod);

    /* tick function for a downward sawtooth waveform */
    double sawdtick(double freq);

    /* tick function for an upward sawtooth waveform */
    double sawutick(double freq);

    /* tick function for a triangle waveform */
    double tritick(double freq);

protected:
    double _twopiovrsr;
    double _curfreq;
    double _curphase;
    double _incr;

    Oscillator() = default;

    void updateFrequency(double freq);
};

class GTableOscillator: Oscillator {
public:
    GTableOscillator(double srate, GTable *gtable, double phase);

    double tabtick(double freq);
    double tabitick(double freq);
private:
    double _dtablen;
    double _sizeovrsr;
    GTable* _gtable;
};
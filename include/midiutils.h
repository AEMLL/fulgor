#include <math.h>

namespace MIDIUtils {

static double midi_to_hz(unsigned long midi_note) {
    return 440 * pow(2,(double)(midi_note - 69) / 12);
}

}
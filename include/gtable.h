#include "wave.h"

typedef struct t_gtable {
    double *table;
    unsigned long length;
} GTABLE;

typedef struct t_tab_oscil {
    OSCIL osc;
    const GTABLE *gtable;
    double dtablen;
    double sizeovrsr;
} OSCILT;

enum {SAW_DOWN, SAW_UP};

GTABLE* new_gtable(unsigned long length);
void free_gtable(GTABLE** gtable);
void norm_gtable(GTABLE* gtable);
GTABLE* new_triangle(unsigned long length, unsigned long nharms);
GTABLE* new_square(unsigned long length,unsigned long nharms);
GTABLE* new_saw(unsigned long length,unsigned long nharms, int up);

OSCILT* new_oscilt(double srate, const GTABLE* gtable, double phase);
double tabtick(OSCILT* p_osc, double freq);
double tabitick(OSCILT* p_osc, double freq);
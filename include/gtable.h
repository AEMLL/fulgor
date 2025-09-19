#include <memory>
#include <vector>

constexpr double PI = (3.1415926535897932);
constexpr double TWOPI = (2.0 * PI);

class GTable {
public:
    typedef enum t_wave {ZERO, SINE, TRIANGLE, SQUARE, SAW_UP, SAW_DOWN} Wave;

    GTable(unsigned long length, Wave wave, unsigned long nharms);

    void new_zeros(unsigned long length);
    void new_sine(unsigned long length);
    void new_triangle(unsigned long length, unsigned long nharms);
    void new_square(unsigned long length, unsigned long nharms);
    void new_saw(unsigned long length, unsigned long nharms, int up);
    void norm();
    void resize(unsigned long length);

    unsigned long length();
    double get(unsigned long index);
    void set(unsigned long index, double val);

private:
    std::vector<double> _table;
    unsigned long _length;
};

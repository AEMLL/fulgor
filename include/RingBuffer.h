#include <vector>

class RingBuffer {
private:
    std::vector<float> buffer_;
    unsigned long capacity_;
    unsigned long front_;

public:
    RingBuffer();

    RingBuffer(unsigned long capacity);

    void resize(unsigned long capacity);

    unsigned long getCapacity();
    unsigned long getFront();


    void push(float input);
    
    float& operator[](int index);

    ~RingBuffer();
};
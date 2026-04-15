#include "RingBuffer.h"

RingBuffer::RingBuffer(): RingBuffer(1) {}

RingBuffer::RingBuffer(unsigned long capacity) {
    capacity_ = capacity;
    buffer_ = std::vector<float>(capacity_);
    front_ = 0;
}

unsigned long RingBuffer::getCapacity() {
    return buffer_.size();
}

void RingBuffer::resize(unsigned long capacity) {
    capacity_ = capacity;
    buffer_.resize(capacity_);
}

float& RingBuffer::operator[](int index) {
    // index corresponds to amount of delay
    index = front_ - index;

    // wrap-around
    while (index < 0) {
        index = index + capacity_;
    }

    return buffer_[index];
}

void RingBuffer::push(float input) {
    front_ = (front_+1)%capacity_;
    buffer_[front_] = input;
}

unsigned long RingBuffer::getFront() {
    return front_;
}

RingBuffer::~RingBuffer() {
    // nothing to do here
}
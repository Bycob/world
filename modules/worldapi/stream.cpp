//
// Created by louis on 30/04/17.
//

#include "stream.h"

buf_ostream::buf_ostream(char *buf, int size) : _buf(buf), _size(size) {

}


bin_ostream::bin_ostream(std::ostream &stream) : _stream(stream) {

}

bin_ostream& bin_ostream::operator<<(const double &value) {
    _stream.write((char*) &value, sizeof(double));
    return *this;
}

bin_ostream& bin_ostream::operator<<(const float &value) {
    _stream.write((char*) &value, sizeof(float));
    return *this;
}


bin_istream::bin_istream(std::istream &stream) : _stream(stream) {

}
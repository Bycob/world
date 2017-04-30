//
// Created by louis on 30/04/17.
//

#ifndef WORLD_BINARYSTREAM_H
#define WORLD_BINARYSTREAM_H

#include <iostream>

class buf_ostream {
public:
    buf_ostream(char* buf, int size);
private:
    char* _buf;
    int _size;
};

class bin_ostream {
public:
    bin_ostream(std::ostream & stream);

    bin_ostream & operator<<(const double & value);
    bin_ostream & operator<<(const float & value);
private:
    std::ostream & _stream;
};

class bin_istream {
public:
    bin_istream(std::istream & stream);

private:
    std::istream & _stream;
};


#endif //WORLD_BINARYSTREAM_H

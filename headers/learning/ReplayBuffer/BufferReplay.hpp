//
// Created by ERANHER on 12.3.2020.
//

#ifndef TRACK_RACING_BUFFERREPLAY_HPP
#define TRACK_RACING_BUFFERREPLAY_HPP
class BufferReplay{

    short BtachSizeSample;
    unsigned int maxSize;

    virtual void sample()const=0;
    virtual void add()const=0;

};
#endif //TRACK_RACING_BUFFERREPLAY_HPP

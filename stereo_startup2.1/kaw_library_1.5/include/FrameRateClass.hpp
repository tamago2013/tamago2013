#ifndef FRAME_RATE_CLASS_HPP
#define FRAME_RATE_CLASS_HPP

class FrameRate{
    double previous,present;
    double result;
    bool isFirst;

public:
    FrameRate();
    double getFrameRate(void);

};

#endif // TIMECONTROL_HPP

#ifndef GAITS_H
#define GAITS_H

#include <vector>
using std::vector;

// Gait types
enum GaitType
{
    TRI,    // 0
    RIPPLE, // 1
    WAVE,   // 2
    QUAD,   // 3
    BI,     // 4
    HOP     // 5
};

// Single Gait struct definition
struct Gait
{
    GaitType gaitType;
    vector<float> cycleOffsetPercentages;
    float pushFraction;
    float gaitSpeedMult;
    float strideLengthMult;
    float liftHeight;
    float maxStrideLength;

    Gait(GaitType gt, vector<float> co, float pf, float gsm, float slm, float lh, float msl)
        : gaitType(gt), cycleOffsetPercentages(co), pushFraction(pf), gaitSpeedMult(gsm), strideLengthMult(slm), liftHeight(lh), maxStrideLength(msl) {}
};

// The actual objects are defined in Gaits.cpp
extern Gait triGait;
extern Gait rippleGait; 
extern Gait waveGait;
extern Gait quadGait;
extern Gait biGait;
extern Gait hopGait;

Gait getGait(GaitType gaitType);

#endif // GAITS_H

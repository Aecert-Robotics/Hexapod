#include "Gaits.h" // Include the header file

Gait triGait = Gait(
    TRI, // gaitType
    {
        0.0f, // leg 1 offset
        0.5f, // leg 2 offset
        0.0f, // leg 3 offset
        0.5f, // leg 4 offset
        0.0f, // leg 5 offset
        0.5f  // leg 6 offset
    },
    0.55f,  // pushFraction 
    1.1f,   // gaitSpeedMult
    1.1f,   // strideLengthMult
    100.0f, // liftHeight
    200.0f  // maxStrideLength
);

Gait rippleGait = Gait(
    RIPPLE, // gaitType
    {
        //042513
        0.0f,  // leg 1 offset
        0.67f, // leg 2 offset
        0.33f, // leg 3 offset
        0.83f, // leg 4 offset
        0.17f, // leg 5 offset
        0.5f   // leg 6 offset
    },
    0.6f,   // pushFraction
    1.1f,   // gaitSpeedMult
    1.1f,   // strideLengthMult
    100.0f,  // liftHeight
    200.0f  // maxStrideLength
);

Gait waveGait = Gait(
    WAVE, // gaitType
    {
        0.0f, // leg 1 offset
        0.17f, // leg 2 offset
        0.33f, // leg 3 offset
        0.83f, // leg 4 offset
        0.67f, // leg 5 offset
        0.5f   // leg 6 offset
    },
    0.82f,  // pushFraction 
    0.50f,  // gaitSpeedMult
    1.4f,   // strideLengthMult
    130.0f, // liftHeight
    200.0f  // maxStrideLength
);

Gait quadGait = Gait(
    QUAD, // gaitType
    {
        0.0f, // leg 1 offset
        0.33f, // leg 2 offset
        0.66f, // leg 3 offset
        0.0f, // leg 4 offset
        0.33f, // leg 5 offset
        0.66f  // leg 6 offset
    },
    0.68f,  // pushFraction (example value)
    1.0f,   // gaitSpeedMult
    1.2f,   // strideLengthMult
    100.0f, // liftHeight
    200.0f  // maxStrideLength
);

Gait biGait = Gait(
    BI, // gaitType
    {
        0.0f, // leg 1 offset
        0.33f, // leg 2 offset
        0.66f, // leg 3 offset
        0.0f, // leg 4 offset
        0.33f, // leg 5 offset
        0.66f  // leg 6 offset
    },
    0.35f,  // pushFraction (example value)
    2.0f,   // gaitSpeedMult
    1.1f,   // strideLengthMult
    160.0f, // liftHeight
    200.0f  // maxStrideLength
);

Gait hopGait = Gait(
    HOP, // gaitType
    {
        0.0f, // leg 1 offset
        0.0f, // leg 2 offset
        0.0f, // leg 3 offset
        0.0f, // leg 4 offset
        0.0f, // leg 5 offset
        0.0f  // leg 6 offset
    },
    0.55f,  // pushFraction (example value)
    1.1f,   // gaitSpeedMult
    1.4f,   // strideLengthMult
    80.0f, // liftHeight
    225.0f  // maxStrideLength
);


Gait getGait(GaitType gaitType)
{
    switch (gaitType)
    {
    case TRI:
        return triGait;
    case RIPPLE:
        return rippleGait;
    case WAVE:
        return waveGait;
    case QUAD:
        return quadGait;
    case BI:
        return biGait;
    case HOP:
        return hopGait;
    default:
        return triGait; // Default to TRI if no match found
    }
}
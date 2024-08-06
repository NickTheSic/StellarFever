// Copyright Quarantine Games 2020


#include "Shake_Engineer.h"

UShake_Engineer::UShake_Engineer()
{
    
}

void UShake_Engineer::Init()
{
    OscillationDuration = fOscillationDuration;//.25
    OscillationBlendInTime = fOscillationBlendIn;//.2
    OscillationBlendOutTime = fOscillationBlendOut;//.2

    RotOscillation.Pitch.Amplitude = fPitchAmplitude;//20
    RotOscillation.Pitch.Frequency = fPitchFrequency;//1
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
    RotOscillation.Pitch.Waveform = bPitchSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise; //perlin

    RotOscillation.Yaw.Amplitude = fYawAmplitude;
    RotOscillation.Yaw.Frequency = fYawFrequency;
    RotOscillation.Yaw.InitialOffset = bYawRandom ? EInitialOscillatorOffset::EOO_OffsetZero : EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Yaw.Waveform = bYawSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise;

    RotOscillation.Roll.Amplitude = fRollAmplitude;
    RotOscillation.Roll.Frequency = fYawFrequency;
    RotOscillation.Roll.InitialOffset = bRollRandom ? EInitialOscillatorOffset::EOO_OffsetZero : EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Roll.Waveform = bRollSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise;

    bSingleInstance = bIsSingleInstance;
}
// Copyright Quarantine Games 2020


#include "Shake_Gunner.h"

UShake_Gunner::UShake_Gunner()
{
    
}

void UShake_Gunner::Init()
{
    OscillationDuration = fOscillationDuration; //.25
    OscillationBlendInTime = fOscillationBlendIn;//.3
    OscillationBlendOutTime = fOscillationBlendOut;//.15

    RotOscillation.Pitch.Amplitude = fPitchAmplitude;//20.0
    RotOscillation.Pitch.Frequency = fPitchFrequency;//.15
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
    RotOscillation.Pitch.Waveform = bPitchSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise; //sine

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
// Copyright Quarantine Games 2020


#include "Shake_GunnerMelee.h"

UShake_GunnerMelee::UShake_GunnerMelee()
{
    
}

void UShake_GunnerMelee::Init()
{
    OscillationDuration = fOscillationDuration;
    OscillationBlendInTime = fOscillationBlendIn;
    OscillationBlendOutTime = fOscillationBlendOut;

    RotOscillation.Pitch.Amplitude = fPitchAmplitude;
    RotOscillation.Pitch.Frequency = fPitchFrequency;
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
    RotOscillation.Pitch.Waveform = bPitchSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise;

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
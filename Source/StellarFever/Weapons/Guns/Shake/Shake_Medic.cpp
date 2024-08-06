// Copyright Quarantine Games 2020


#include "Shake_Medic.h"

UShake_Medic::UShake_Medic()
{
    
}

void UShake_Medic::Init()
{
    OscillationDuration = fOscillationDuration; //.25
    OscillationBlendInTime = fOscillationBlendIn;//.25
    OscillationBlendOutTime = fOscillationBlendOut;//.15

    RotOscillation.Pitch.Amplitude = fPitchAmplitude;//15.0
    RotOscillation.Pitch.Frequency = fPitchFrequency;//.3
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
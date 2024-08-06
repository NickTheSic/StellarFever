// Copyright Quarantine Games 2020


#include "Shake_MedicGrenade.h"

UShake_MedicGrenade::UShake_MedicGrenade()
{
    
}

void UShake_MedicGrenade::Init()
{
    OscillationDuration = fOscillationDuration;//.25
    OscillationBlendInTime = fOscillationBlendIn;//.1
    OscillationBlendOutTime = fOscillationBlendOut;//.1

    RotOscillation.Pitch.Amplitude = fPitchAmplitude;//100
    RotOscillation.Pitch.Frequency = fPitchFrequency;//5.0
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
    RotOscillation.Pitch.Waveform = bPitchSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise;

    RotOscillation.Yaw.Amplitude = fYawAmplitude;//50.0
    RotOscillation.Yaw.Frequency = fYawFrequency;//5.0
    RotOscillation.Yaw.InitialOffset = bYawRandom ? EInitialOscillatorOffset::EOO_OffsetZero : EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Yaw.Waveform = bYawSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise;

    RotOscillation.Roll.Amplitude = fRollAmplitude;
    RotOscillation.Roll.Frequency = fYawFrequency;
    RotOscillation.Roll.InitialOffset = bRollRandom ? EInitialOscillatorOffset::EOO_OffsetZero : EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Roll.Waveform = bRollSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise;

    bSingleInstance = bIsSingleInstance;
}
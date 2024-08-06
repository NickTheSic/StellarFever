// Copyright Quarantine Games 2020


#include "Shake_EngineerBullet.h"

UShake_EngineerBullet::UShake_EngineerBullet()
{
    
}

void UShake_EngineerBullet::Init()
{
    OscillationDuration = fOscillationDuration; //.25
    OscillationBlendInTime = fOscillationBlendIn;//.1
    OscillationBlendOutTime = fOscillationBlendOut;//.1

    RotOscillation.Pitch.Amplitude = fPitchAmplitude;//100.0
    RotOscillation.Pitch.Frequency = fPitchFrequency;//5.0
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
    RotOscillation.Pitch.Waveform = bPitchSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise; //perlin

    RotOscillation.Yaw.Amplitude = fYawAmplitude; //50.0
    RotOscillation.Yaw.Frequency = fYawFrequency; //5.0
    RotOscillation.Yaw.InitialOffset = bYawRandom ? EInitialOscillatorOffset::EOO_OffsetZero : EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Yaw.Waveform = bYawSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise; //perlin

    RotOscillation.Roll.Amplitude = fRollAmplitude;
    RotOscillation.Roll.Frequency = fYawFrequency;
    RotOscillation.Roll.InitialOffset = bRollRandom ? EInitialOscillatorOffset::EOO_OffsetZero : EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Roll.Waveform = bRollSine ? EOscillatorWaveform::SineWave : EOscillatorWaveform::PerlinNoise;

    bSingleInstance = bIsSingleInstance;
}
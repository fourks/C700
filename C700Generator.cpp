/*
 *  C700Generator.cpp
 *  C700
 *
 *  Created by osoumen on 06/09/06.
 *  Copyright 2006 Vermicelli Magic. All rights reserved.
 *
 */

#include "C700defines.h"
#include "C700Generator.h"
#include <math.h>

const float onepi = 3.14159265358979;

#define ANALOG_PORTAMENTO 0

static const int	VOLUME_CURB[128]
= {
	0x000, 0x001, 0x001, 0x001, 0x002, 0x003, 0x005, 0x006, 0x008, 0x00a, 0x00d, 0x00f, 0x012, 0x015, 0x019, 0x01d, 
	0x020, 0x025, 0x029, 0x02e, 0x033, 0x038, 0x03d, 0x043, 0x049, 0x04f, 0x056, 0x05d, 0x064, 0x06b, 0x072, 0x07a, 
	0x082, 0x08a, 0x093, 0x09b, 0x0a4, 0x0ae, 0x0b7, 0x0c1, 0x0cb, 0x0d5, 0x0e0, 0x0eb, 0x0f6, 0x101, 0x10d, 0x118, 
	0x124, 0x131, 0x13d, 0x14a, 0x157, 0x165, 0x172, 0x180, 0x18e, 0x19c, 0x1ab, 0x1ba, 0x1c9, 0x1d8, 0x1e8, 0x1f8, 
	0x208, 0x218, 0x229, 0x23a, 0x24b, 0x25c, 0x26e, 0x280, 0x292, 0x2a4, 0x2b7, 0x2ca, 0x2dd, 0x2f0, 0x304, 0x318, 
	0x32c, 0x341, 0x355, 0x36a, 0x380, 0x395, 0x3ab, 0x3c1, 0x3d7, 0x3ed, 0x404, 0x41b, 0x432, 0x44a, 0x461, 0x479, 
	0x492, 0x4aa, 0x4c3, 0x4dc, 0x4f5, 0x50f, 0x528, 0x542, 0x55d, 0x577, 0x592, 0x5ad, 0x5c8, 0x5e4, 0x600, 0x61c, 
	0x638, 0x655, 0x671, 0x68e, 0x6ac, 0x6c9, 0x6e7, 0x705, 0x724, 0x742, 0x761, 0x780, 0x79f, 0x7bf, 0x7df, 0x7ff
};

static const int PAN_CURB[129]={
    511, 511, 510, 510, 510, 510, 510, 509, 509, 508, 507, 507, 506, 505, 504, 503,
    502, 500, 499, 498, 496, 495, 493, 491, 490, 488, 486, 484, 482, 480, 477, 475,
    473, 470, 468, 465, 463, 460, 457, 454, 451, 448, 445, 442, 439, 436, 432, 429,
    425, 422, 418, 414, 411, 407, 403, 399, 395, 391, 387, 383, 378, 374, 370, 365,
    361, 356, 352, 347, 342, 338, 333, 328, 323, 318, 313, 308, 303, 298, 293, 287,
    282, 277, 271, 266, 260, 255, 249, 244, 238, 233, 227, 221, 215, 210, 204, 198,
    192, 186, 180, 174, 168, 162, 156, 150, 144, 138, 132, 126, 119, 113, 107, 101,
    95, 88, 82, 76, 69, 63, 57, 50, 44, 38, 31, 25, 19, 12, 6, 0, 0
};

static const int sinctable[] = {
	-8,31,0,-462,1911,-4438,7057,32767,7057,-4438,1911,-462,0,31,-8,0,
	0,18,46,-535,1858,-3768,4443,32510,9863,-4978,1871,-347,-57,45,-9,0,
	0,7,80,-572,1726,-3016,2076,31745,12796,-5345,1725,-193,-125,61,-10,0,
	0,0,104,-574,1532,-2226,0,30497,15785,-5492,1464,0,-200,78,-11,0,
	0,-5,117,-548,1294,-1437,-1755,28803,18752,-5378,1087,227,-279,93,-11,0,
	0,-9,120,-500,1029,-685,-3169,26715,21615,-4969,596,481,-360,107,-11,0,
	0,-11,116,-435,753,0,-4239,24296,24296,-4239,0,753,-435,116,-11,0,
	0,-11,107,-360,481,596,-4969,21615,26715,-3169,-685,1029,-500,120,-9,0,
	0,-11,93,-279,227,1087,-5378,18752,28803,-1755,-1437,1294,-548,117,-5,0,
	0,-11,78,-200,0,1464,-5492,15785,30497,0,-2226,1532,-574,104,0,0,
	0,-10,61,-125,-193,1725,-5345,12796,31745,2076,-3016,1726,-572,80,7,0,
	0,-9,45,-57,-347,1871,-4978,9863,32510,4443,-3768,1858,-535,46,18,0,
	0,-8,31,0,-462,1911,-4438,7057,32767,7057,-4438,1911,-462,0,31,-8
};

//-----------------------------------------------------------------------------
void C700Generator::VoiceStatus::Reset()
{
	pb = 0;
	reg_pmod = 0;
	vibdepth = 0;
	vibPhase = 0.0f;
    portaPitch = .0f;
    pan = 64;
	
	loopPoint = 0;
	loop = false;
	pitch = 0;
}

//-----------------------------------------------------------------------------
C700Generator::C700Generator()
: mSampleRate(44100.0),
  mVelocityMode( kVelocityMode_Square ),
  mVPset(NULL)
{
	for ( int i=0; i<NUM_BANKS; i++ ) {
		mDrumMode[i] = false;
	}
	for ( int bnk=0; bnk<NUM_BANKS; bnk++ ) {
		for ( int i=0; i<128; i++ ) {
			mKeyMap[bnk][i] = 0;
		}
	}
	//Initialize
    mVibfreq = 0.00137445;
	mVibdepth = 0.5;

    mEventDelayClocks = 8192;   // 8ms
    mEventDelaySamples = calcEventDelaySamples();
    
	for (int i=0; i<16; i++) {
        mChStat[i].changeFlg = 0;
        mChStat[i].prog = 0;
        mChStat[i].pitchBend = 0;
        mChStat[i].vibDepth = 0;
        mChStat[i].pbRange = static_cast<float>(DEFAULT_PBRANGE);
        //mChStat[i].portaOn = false;
        mChStat[i].portaTc = 1.0f;
        mChStat[i].portaStartPitch = 0;
        mChStat[i].volume = VOLUME_DEFAULT;
        mChStat[i].expression = EXPRESSION_DEFAULT;
        mChStat[i].pan = 64;
        //mChStat[i].releasePriority = 0;
        mChStat[i].damper = false;
        
        mChStat[i].lastNote = 0;
	}
    mDVoice.Initialize(8);
	Reset();
}

//-----------------------------------------------------------------------------
void C700Generator::Reset()
{
	for (int i=0; i<16; i++) {
		mProcessbuf[0][i]=0;
		mProcessbuf[1][i]=0;
	}
	mProcessFrac=0;
	mProcessbufPtr=0;
    mPortamentCount=0;
	
    mDVoice.Reset();
    
    AllSoundOff();
}

//-----------------------------------------------------------------------------
void C700Generator::NoteOn( int ch, int note, int velo, unsigned int uniqueID, int inFrame )
{
	MIDIEvt			evt;
	evt.type = NOTE_ON;
	evt.ch = ch;
	evt.note = note;
	evt.velo = velo;
	evt.uniqueID = uniqueID;
	evt.remain_samples = inFrame;
	mMIDIEvt.push_back( evt );
}

//-----------------------------------------------------------------------------
void C700Generator::NoteOff( int ch, int note, int velo, unsigned int uniqueID, int inFrame )
{
	MIDIEvt			evt;
	evt.type = NOTE_OFF;
	evt.ch = ch;
	evt.note = note;
	evt.velo = velo;
	evt.uniqueID = uniqueID;
	evt.remain_samples = inFrame;
	mMIDIEvt.push_back( evt );
}


//-----------------------------------------------------------------------------
void C700Generator::ControlChange( int ch, int controlNum, int value, int inFrame )
{
    MIDIEvt			evt;
	evt.type = CONTROL_CHANGE;
	evt.ch = ch;
	evt.note = controlNum;
	evt.velo = value;
	evt.uniqueID = 0;
	evt.remain_samples = inFrame;
	mMIDIEvt.push_back( evt );
}

//-----------------------------------------------------------------------------
void C700Generator::AllNotesOff()
{
    // TODO: mPlayVo, mWaitVo���Ȃ��N���A���Ă��Ȃ��̂����ׂ�
    //mClearEvent = true;
	for ( int i=0; i<kMaximumVoices; i++ ) {
		mDSP.ResetVoice(i);
        mVoiceStat[i].Reset();
	}
    mDVoice.Reset();
}

//-----------------------------------------------------------------------------
void C700Generator::AllSoundOff()
{
    AllNotesOff();
	mDSP.ResetEcho();
}

//-----------------------------------------------------------------------------
void C700Generator::ResetAllControllers()
{
	for (int i=0; i<16; i++) {
        mChStat[i].changeFlg = 0;
        mChStat[i].changedVP = mVPset[mChStat[i].prog];
        mChStat[i].pitchBend = 0;
        mChStat[i].vibDepth = 0;
        mChStat[i].pbRange = static_cast<float>(DEFAULT_PBRANGE);
        mChStat[i].expression = EXPRESSION_DEFAULT;
        //mChStat[i].pan = 64;
        //mChStat[i].portaOn = false;
        //mChStat[i].portaTc = 1.0f;
        mChStat[i].portaStartPitch = 0;
        //mChStat[i].priority = 64;
        //mChStat[i].limit = 127;
        //mChStat[i].releasePriority = 0;
        mChStat[i].damper = false;
        //mChStat[i].monoMode = false;
	}
}

//-----------------------------------------------------------------------------
void C700Generator::ProgramChange( int ch, int value, int inFrame )
{
    MIDIEvt			evt;
	evt.type = PROGRAM_CHANGE;
	evt.ch = ch;
	evt.note = value;
	evt.velo = 0;
	evt.uniqueID = 0;
	evt.remain_samples = inFrame;
	mMIDIEvt.push_back( evt );
}
//-----------------------------------------------------------------------------
void C700Generator::doProgramChange( int ch, int value )
{
	mChStat[ch].prog = value;
    if (mVPset) {
        mChStat[ch].changeFlg = 0;
        mChStat[ch].changedVP = mVPset[mChStat[ch].prog];
        
        SetPortamentTime(ch, value);
    }
}

//-----------------------------------------------------------------------------
int C700Generator::CalcPBValue( int ch, float pitchBend, int basePitch )
{
	return (int)((pow(2., (pitchBend * mChStat[ch].pbRange) / 12.) - 1.0)*basePitch);
}

//-----------------------------------------------------------------------------
void C700Generator::PitchBend( int ch, int value1, int value2, int inFrame )
{
    MIDIEvt			evt;
	evt.type = PITCH_BEND;
	evt.ch = ch;
	evt.note = value1;
	evt.velo = value2;
	evt.uniqueID = 0;
	evt.remain_samples = inFrame;
	mMIDIEvt.push_back( evt );
}

//-----------------------------------------------------------------------------
void C700Generator::doPitchBend( int ch, int value1, int value2 )
{
    int pitchBend = ((value2 << 7) | value1) - 8192;
	float pb_value = pitchBend / 8192.0;
	
    mChStat[ch].pitchBend = pb_value;
	for ( int i=0; i<kMaximumVoices; i++ ) {
		if ( mDVoice.GetVoiceMidiCh(i) == ch ) {
			mVoiceStat[i].pb = CalcPBValue( ch, pb_value, mVoiceStat[i].pitch );
		}
	}
}

//-----------------------------------------------------------------------------
void C700Generator::ModWheel( int ch, int value )
{
    mChStat[ch].vibDepth = value;
	for ( int i=0; i<kMaximumVoices; i++ ) {
		if ( mDVoice.GetVoiceMidiCh(i) == ch ) {
			mVoiceStat[i].vibdepth = value;
			mVoiceStat[i].reg_pmod = value > 0 ? true:false;
		}
	}
}

//-----------------------------------------------------------------------------
void C700Generator::Damper( int ch, bool on )
{
    mChStat[ch].damper = on;
}

//-----------------------------------------------------------------------------
void C700Generator::SetVoiceLimit( int value )
{
	mDVoice.ChangeVoiceLimit(value);
    mDSP.SetVoiceLimit(value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetPBRange( float value )
{
    for (int i=0; i<16; i++) {
        mChStat[i].pbRange = value;
    }
}
//-----------------------------------------------------------------------------
void C700Generator::SetPBRange( int ch, float value )
{
	mChStat[ch].pbRange = value;
}

//-----------------------------------------------------------------------------
void C700Generator::SetADPCMMode( bool value )
{
    mDSP.SetNewADPCM(value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetMultiMode( int bank, bool value )
{
	mDrumMode[bank] = value;
}

//-----------------------------------------------------------------------------
bool C700Generator::GetMultiMode( int bank ) const
{
	return mDrumMode[bank];
}

//-----------------------------------------------------------------------------
void C700Generator::SetVelocityMode( velocity_mode value )
{
	mVelocityMode = value;
}

//-----------------------------------------------------------------------------
void C700Generator::SetVibFreq( int ch, float value )
{
	mVibfreq = value*((onepi*2)/INTERNAL_CLOCK);
}

//-----------------------------------------------------------------------------
void C700Generator::SetVibDepth( int ch, float value )
{
	mVibdepth = value / 2;
}

//-----------------------------------------------------------------------------
void C700Generator::SetMainVol_L( int value )
{
    mDSP.SetMainVolumeL(value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetMainVol_R( int value )
{
    mDSP.SetMainVolumeR(value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetEchoVol_L( int value )
{
    mDSP.SetEchoVol_L(value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetEchoVol_R( int value )
{
    mDSP.SetEchoVol_R(value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetFeedBackLevel( int value )
{
    mDSP.SetFeedBackLevel(value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetDelayTime( int value )
{
    mDSP.SetDelayTime(value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetFIRTap( int tap, int value )
{
    mDSP.SetFIRTap(tap, value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetPortamentOn( int ch, bool on )
{
    //mChStat[ch].portaOn = on;
    mChStat[ch].changedVP.portamentoOn = on;
    mChStat[ch].changeFlg |= HAS_PORTAMENTOON;
}

//-----------------------------------------------------------------------------
float C700Generator::calcGM2PortamentCurve(int value)
{
    float   logCentPerMilis;
    if (value < 16) {
        logCentPerMilis = (value * value)/192.0f - (3.0f/16.0f)*value + 3.0f;
    }
    else if (value < 112) {
        logCentPerMilis = -value/48.0f + 5.0f/3.0f;
    }
    else {
        logCentPerMilis = -(value * value)/256.0f + (41.0f/48.0f)*value - 142.0f/3.0f;
    }
    return powf(10.0f, logCentPerMilis);
}

//-----------------------------------------------------------------------------
void C700Generator::SetPortamentTime( int ch, int value )
{
#if ANALOG_PORTAMENTO
    float secs = value / 100.0f;
    mChStat[ch].portaTc = expf(-2.0f / ((INTERNAL_CLOCK / PORTAMENT_CYCLE_SAMPLES) * secs));
#else
    float centPerMilis = calcGM2PortamentCurve(value);
    centPerMilis *= 1000.0f / (static_cast<float>(INTERNAL_CLOCK) / PORTAMENT_CYCLE_SAMPLES);
    mChStat[ch].portaTc = powf(2.0f, centPerMilis / 1200);
#endif
}

//-----------------------------------------------------------------------------
void C700Generator::UpdatePortamentoTime( int prog )
{
    if (mVPset == NULL) {
        return;
    }
    
    for (int i=0; i<16; i++) {
        if (mChStat[i].prog == prog) {
            SetPortamentTime(i, mVPset[prog].portamentoRate);
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::SetPortamentControl( int ch, int note )
{
    InstParams		vp = getChannelVP(ch, mChStat[ch].lastNote);
    mChStat[ch].portaStartPitch = pow(2., (note - vp.basekey) / 12.)/INTERNAL_CLOCK*vp.rate*4096 + 0.5;
}

//-----------------------------------------------------------------------------
void C700Generator::SetChPriority( int ch, int value )
{
    //mChStat[ch].priority = value;
    mChStat[ch].changedVP.noteOnPriority = value;
    mChStat[ch].changeFlg |= HAS_NOTEONPRIORITY;
}

//-----------------------------------------------------------------------------
void C700Generator::SetChLimit( int ch, int value )
{
    mDVoice.SetChLimit(ch, value);
}

//-----------------------------------------------------------------------------
void C700Generator::SetReleasePriority( int ch, int value )
{
    //mChStat[ch].releasePriority = value;
    mChStat[ch].changedVP.releasePriority = value;
    mChStat[ch].changeFlg |= HAS_RELEASEPRIORITY;
}

//-----------------------------------------------------------------------------
void C700Generator::SetMonoMode( int ch, bool on )
{
    //mChStat[ch].monoMode = on;
    mChStat[ch].changedVP.monoMode = on;
    mChStat[ch].changeFlg |= HAS_MONOMODE;
}

//-----------------------------------------------------------------------------
InstParams C700Generator::getChannelVP(int ch, int note)
{
    InstParams  *pgVP = &mVPset[mChStat[ch].prog];
    if (mDrumMode[pgVP->bank]) {
        return *(getMappedVP(pgVP->bank, note));
    }
    else {
        InstParams  mergedVP;
        mergedVP = *pgVP;
        //if (mChStat[ch].changeFlg & HAS_PGNAME) mergedVP.pgname = mChStat[ch].changedVP.pgname;
        if (mChStat[ch].changeFlg & HAS_RATE) mergedVP.rate = mChStat[ch].changedVP.rate;
        if (mChStat[ch].changeFlg & HAS_BASEKEY) mergedVP.basekey = mChStat[ch].changedVP.basekey;
        if (mChStat[ch].changeFlg & HAS_LOWKEY) mergedVP.lowkey = mChStat[ch].changedVP.lowkey;
        if (mChStat[ch].changeFlg & HAS_HIGHKEY) mergedVP.highkey = mChStat[ch].changedVP.highkey;
        if (mChStat[ch].changeFlg & HAS_AR) mergedVP.ar = mChStat[ch].changedVP.ar;
        if (mChStat[ch].changeFlg & HAS_DR) mergedVP.dr = mChStat[ch].changedVP.dr;
        if (mChStat[ch].changeFlg & HAS_SL) mergedVP.sl = mChStat[ch].changedVP.sl;
        if (mChStat[ch].changeFlg & HAS_SR) mergedVP.sr = mChStat[ch].changedVP.sr;
        if (mChStat[ch].changeFlg & HAS_VOLL) mergedVP.volL = mChStat[ch].changedVP.volL;
        if (mChStat[ch].changeFlg & HAS_VOLR) mergedVP.volR = mChStat[ch].changedVP.volR;
        if (mChStat[ch].changeFlg & HAS_ECHO) mergedVP.echo = mChStat[ch].changedVP.echo;
        if (mChStat[ch].changeFlg & HAS_BANK) mergedVP.bank = mChStat[ch].changedVP.bank;
//        if (mChStat[ch].changeFlg & HAS_ISEMPHASIZED) mergedVP.isEmphasized = mChStat[ch].changedVP.isEmphasized;
//        if (mChStat[ch].changeFlg & HAS_SOURCEFILE) mergedVP.sourceFile = mChStat[ch].changedVP.sourceFile;
        if (mChStat[ch].changeFlg & HAS_SUSTAINMODE) mergedVP.sustainMode = mChStat[ch].changedVP.sustainMode;
        if (mChStat[ch].changeFlg & HAS_MONOMODE) mergedVP.monoMode = mChStat[ch].changedVP.monoMode;
        if (mChStat[ch].changeFlg & HAS_PORTAMENTOON) mergedVP.portamentoOn = mChStat[ch].changedVP.portamentoOn;
        if (mChStat[ch].changeFlg & HAS_PORTAMENTORATE) mergedVP.portamentoRate = mChStat[ch].changedVP.portamentoRate;
        if (mChStat[ch].changeFlg & HAS_NOTEONPRIORITY) mergedVP.noteOnPriority = mChStat[ch].changedVP.noteOnPriority;
        if (mChStat[ch].changeFlg & HAS_RELEASEPRIORITY) mergedVP.releasePriority = mChStat[ch].changedVP.releasePriority;
        return mergedVP;
    }
}

//-----------------------------------------------------------------------------
void C700Generator::Volume( int ch, int value )
{
    mChStat[ch].volume = value & 0x7f;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].volume = mChStat[ch].volume;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::Expression( int ch, int value )
{
    mChStat[ch].expression = value & 0x7f;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].expression = mChStat[ch].expression;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::Panpot( int ch, int value )
{
    mChStat[ch].pan = value & 0x7f;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].pan = mChStat[ch].pan;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChRate(int ch, double rate)
{
    mChStat[ch].changedVP.rate = rate;
    mChStat[ch].changeFlg |= HAS_RATE;
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChBasekey(int ch, int basekey)
{
    mChStat[ch].changedVP.basekey = basekey;
    mChStat[ch].changeFlg |= HAS_BASEKEY;
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChLowkey(int ch, int lowkey)
{
    mChStat[ch].changedVP.lowkey = lowkey;
    mChStat[ch].changeFlg |= HAS_LOWKEY;
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChHighkey(int ch, int highkey)
{
    mChStat[ch].changedVP.highkey = highkey;
    mChStat[ch].changeFlg |= HAS_HIGHKEY;
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChAR(int ch, int ar)
{
    mChStat[ch].changedVP.ar = ar & 0x0f;
    mChStat[ch].changeFlg |= HAS_AR;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].ar = mChStat[ch].changedVP.ar;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChDR(int ch, int dr)
{
    mChStat[ch].changedVP.dr = dr & 0x07;
    mChStat[ch].changeFlg |= HAS_DR;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].dr = mChStat[ch].changedVP.dr;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChSL(int ch, int sl)
{
    mChStat[ch].changedVP.sl = sl & 0x07;
    mChStat[ch].changeFlg |= HAS_SL;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].sl = mChStat[ch].changedVP.sl;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChSR(int ch, int sr)
{
    mChStat[ch].changedVP.sr = sr & 0x1f;
    mChStat[ch].changeFlg |= HAS_SR;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].sr = mChStat[ch].changedVP.sr;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChVolL(int ch, int voll)
{
    mChStat[ch].changedVP.volL = voll;
    mChStat[ch].changeFlg |= HAS_VOLL;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].vol_l = mChStat[ch].changedVP.volL;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChVolR(int ch, int volr)
{
    mChStat[ch].changedVP.volR = volr;
    mChStat[ch].changeFlg |= HAS_VOLR;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].vol_r = mChStat[ch].changedVP.volR;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChEcho(int ch, int echo)
{
    mChStat[ch].changedVP.echo = echo ? true:false;
    mChStat[ch].changeFlg |= HAS_ECHO;
    // �������̃{�C�X�ɔ��f
    for (int i=0; i<kMaximumVoices; i++) {
        if (mDVoice.GetVoiceMidiCh(i) == ch) {
            mVoiceStat[i].echoOn = mChStat[ch].changedVP.echo;
        }
    }
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChBank(int ch, int bank)
{
    mChStat[ch].changedVP.bank = bank & 0x03;
    mChStat[ch].changeFlg |= HAS_BANK;
}

//-----------------------------------------------------------------------------
void C700Generator::ChangeChSustainMode(int ch, int sustainMode)
{
    mChStat[ch].changedVP.sustainMode = sustainMode ? true:false;
    mChStat[ch].changeFlg |= HAS_SUSTAINMODE;
}

//-----------------------------------------------------------------------------
float C700Generator::VibratoWave(float phase)
{
	float x2=phase*phase;
	float vibwave = 7.61e-03f;
	vibwave *= x2;
	vibwave -= 1.6605e-01f;
	vibwave *= x2;
	vibwave += 1.0f;
	vibwave *= phase;
	return vibwave;
}
//-----------------------------------------------------------------------------
void C700Generator::processPortament(int vo)
{
    float   newPitch;
#if ANALOG_PORTAMENTO
    float   tc = mChStat[ mVoice[vo].midi_ch ].portaTc;
    float   tcInv = 1.0f - tc;
    newPitch = mVoice[vo].pitch * tcInv + mVoice[vo].portaPitch * tc;
    mVoice[vo].portaPitch = newPitch;
#else
    if ( mVoiceStat[vo].pitch > mVoiceStat[vo].portaPitch) {
        newPitch = mVoiceStat[vo].portaPitch * mChStat[ mDVoice.GetVoiceMidiCh(vo) ].portaTc;
        if (newPitch > mVoiceStat[vo].pitch) {
            newPitch = mVoiceStat[vo].pitch;
        }
        mVoiceStat[vo].portaPitch = newPitch;
    }
    else if ( mVoiceStat[vo].pitch < mVoiceStat[vo].portaPitch) {
        newPitch = mVoiceStat[vo].portaPitch / mChStat[ mDVoice.GetVoiceMidiCh(vo) ].portaTc;
        if (newPitch < mVoiceStat[vo].pitch) {
            newPitch = mVoiceStat[vo].pitch;
        }
        mVoiceStat[vo].portaPitch = newPitch;
    }
#endif
    mChStat[ mDVoice.GetVoiceMidiCh(vo) ].portaStartPitch = mVoiceStat[vo].portaPitch;
}

//-----------------------------------------------------------------------------
bool C700Generator::doNoteOn1( MIDIEvt dEvt )
{
    bool legato = false;
    dEvt.remain_samples = mEventDelaySamples;
    
    if (dEvt.type == NOTE_ON) {
        //�{�C�X���m�ۂ��čĐ�������Ԃɂ���
        InstParams		vp = getChannelVP(dEvt.ch, dEvt.note);
        int	v = -1;

        int releasedCh;
        v = mDVoice.AllocVoice(vp.noteOnPriority, dEvt.ch, dEvt.uniqueID,
                               vp.monoMode, &releasedCh, &legato);
        if (legato) {
            dEvt.note |= 0x80;  // ���K�[�g�t���O
        }
        
        if (v != -1) {
            if (legato == false) {
                mDSP.KeyOffVoice(v);
            }
            // ���4bit�Ƀ{�C�X�ԍ�������
            dEvt.ch += v << 4;
        }
    }
    mDelayedEvt.push_back(dEvt);
    return true;
}

//-----------------------------------------------------------------------------
void C700Generator::doNoteOn2(const MIDIEvt *evt)
{
    int     midiCh = evt->ch & 0x0f;
    int     note = evt->note & 0x7f;
    
	InstParams		vp = getChannelVP(midiCh, note);
	
	//�g�`�f�[�^�����݂��Ȃ��ꍇ�́A�����Œ��f
	if (vp.brr.data == NULL) {
		return;
	}
	
	//�{�C�X���擾
    int v = (evt->ch >> 4) & 0x0f;
    
    //mPlayVo �� v ���܂܂�Ă��Ȃ�������炳�Ȃ�
    if (
        (mDVoice.IsPlayingVoice(v) == false) ||     // doNoteOn1���o�Ă��Ȃ�
        (mDVoice.GetVoiceUniqueID(v) != evt->uniqueID) //|| // �����O�ɂ���������ď㏑�����ꂽ
        ) {
        return;
    }
	
	// ���S���g���̎Z�o
	mVoiceStat[v].pitch = pow(2., (note - vp.basekey) / 12.)/INTERNAL_CLOCK*vp.rate*4096 + 0.5;
    
    if (vp.portamentoOn) {
        if (mChStat[midiCh].portaStartPitch == 0) {
            mChStat[midiCh].portaStartPitch = mVoiceStat[v].pitch;
        }
        mVoiceStat[v].portaPitch = mChStat[midiCh].portaStartPitch;
    }
    else {
        mVoiceStat[v].portaPitch = mVoiceStat[v].pitch;
    }
    
    mDVoice.SetKeyOn(v);
    
    if ((evt->note & 0x80) == 0) {
        //�x���V�e�B�̎擾
        if ( mVelocityMode == kVelocityMode_Square ) {
            mVoiceStat[v].velo = VOLUME_CURB[evt->velo];
        }
        else if ( mVelocityMode == kVelocityMode_Linear ) {
            mVoiceStat[v].velo = evt->velo << 4;
        }
        else {
            mVoiceStat[v].velo=VOLUME_CURB[127];
        }
        
        mVoiceStat[v].volume = mChStat[midiCh].volume;
        mVoiceStat[v].expression = mChStat[midiCh].expression;
        mVoiceStat[v].pan = mChStat[midiCh].pan;
        
        mVoiceStat[v].brrdata = vp.brr.data;
        mVoiceStat[v].loopPoint = vp.lp;
        mVoiceStat[v].loop = vp.loop;
        mVoiceStat[v].echoOn = vp.echo;
        
        mVoiceStat[v].pb = CalcPBValue( midiCh, mChStat[midiCh].pitchBend, mVoiceStat[v].pitch );
        mVoiceStat[v].vibdepth = mChStat[midiCh].vibDepth;
        
        mVoiceStat[v].reg_pmod = mVoiceStat[v].vibdepth>0 ? true:false;
        mVoiceStat[v].vibPhase = 0.0f;
        
        mVoiceStat[v].vol_l=vp.volL;
        mVoiceStat[v].vol_r=vp.volR;
        mVoiceStat[v].ar=vp.ar;
        mVoiceStat[v].dr=vp.dr;
        mVoiceStat[v].sl=vp.sl;
        if (vp.sustainMode) {
            mVoiceStat[v].sr=0;		//�m�[�g�I�t���ɐݒ�l�ɂȂ�
        }
        else {
            mVoiceStat[v].sr=vp.sr;
        }
        
        // TODO: �m�[�g�I���ɕK�v�ȃp�����[�^��DSP�ɔ��f
        
        // �L�[�I��
        mDSP.KeyOnVoice(v);
        
        // �Ō�ɔ��������m�[�g�ԍ���ۑ�
        mChStat[midiCh].lastNote = note;
    }
}


//-----------------------------------------------------------------------------
int C700Generator::doNoteOff( const MIDIEvt *evt )
{
	int	stops=0;
    
    // �Đ����Ȃ��~����
    // �Đ����ł����Đ��ł����������Ŏ~�߂���͂�
    InstParams		vp = getChannelVP(evt->ch, evt->note);
    int             vo=-1;
    stops = mDVoice.ReleaseVoice(vp.releasePriority, evt->ch, evt->uniqueID, &vo);
    if (stops > 0) {
        if (vp.sustainMode) {
            //�L�[�I�t��������sr��ύX����
            mDSP.SetDR(vo, 7);
            mDSP.SetSR(vo, vp.sr);
        }
        else {
            // �L�[�I�t
            mDSP.KeyOffVoice(vo);
        }
    }
	return stops;
}

//-----------------------------------------------------------------------------
bool C700Generator::doEvents1( const MIDIEvt *evt )
{
    bool    handled = true;
    
    if (evt->type == NOTE_OFF) {
        if (mChStat[evt->ch].damper) {
            handled = false;
        }
        else {
            doNoteOff( evt );
            //mChStat[evt->ch].noteOns -= stops;
        }
    }
    else {
        // �m�[�g�I�t�ȊO�̃C�x���g�͑S�Ēx�����s����
        doNoteOn1(*evt);
    }
    
    return handled;
}

//-----------------------------------------------------------------------------
bool C700Generator::doEvents2( const MIDIEvt *evt )
{
    bool    handled = true;
    
    switch (evt->type) {
        case NOTE_ON:
            doNoteOn2( evt );
            break;
            
        case NOTE_OFF:
            //doNoteOff( evt );
            break;
            
        case PROGRAM_CHANGE:
            doProgramChange(evt->ch, evt->note);
            break;
            
        case PITCH_BEND:
            doPitchBend(evt->ch, evt->note, evt->velo);
            break;
            
        case CONTROL_CHANGE:
            doControlChange(evt->ch, evt->note, evt->velo);
            break;
            
        default:
            //handled = false;
            break;
    }
    return handled;
}

//-----------------------------------------------------------------------------
void C700Generator::doControlChange( int ch, int controlNum, int value )
{
    switch (controlNum) {
        case 1:
            // ���W�����[�V�����z�C�[��
            ModWheel(ch, value);
            break;
            
        case 5:
            // �|���^�����g�^�C��
            SetPortamentTime(ch, value);
            mChStat[ch].changedVP.portamentoRate = value;
            mChStat[ch].changeFlg |= HAS_PORTAMENTORATE;
            break;
            
        case 7:
            // �{�����[��
            Volume(ch, value);
            break;
            
        case 10:
            // �p��
            Panpot(ch, value);
            break;
            
        case 11:
            // �G�N�X�v���b�V����
            Expression(ch, value);
            break;
            
        case 55:
            // �`�����l�� ���~�b�g
            SetChLimit(ch, value);
            break;
            
        case 56:
            // �`�����l�� �v���C�I���e�B
            SetChPriority(ch, value);
            break;
            
        case 57:
            // �����[�X �v���C�I���e�B
            SetReleasePriority(ch, value);
            break;
            
        case 64:
            // �z�[���h�P�i�_���p�[�j
            Damper(ch, (value < 64)?false:true);
            break;
            
        case 65:
            // �|���^�����g�E�I���E�I�t
            SetPortamentOn(ch, (value < 64)?false:true);
            break;
            
        case 72:
            // SR
            ChangeChSR(ch, value >> 2);
            break;
            
        case 73:
            // AR
            ChangeChAR(ch, value >> 3);
            break;
            
        case 80:
            // SL
            ChangeChSL(ch, value >> 4);
            break;
            
        case 75:
            // DR
            ChangeChDR(ch, value >> 4);
            break;
            
        case 76:
            // �r�u���[�g�E���[�g
            SetVibFreq(ch, (35.0f * value) / 127);
            break;
            
        case 77:
            // �r�u���[�g�E�f�v�X
            SetVibDepth(ch, (15.0f * value) / 127);
            break;
            
        case 84:
            // �|���^�����g�E�R���g���[��
            SetPortamentControl(ch, value);
            break;
            
        case 91:
            // ECEN ON/OFF
            ChangeChEcho(ch, (value < 64)?0:127);
            break;
            
        case 126:
            // Mono Mode
			SetMonoMode(ch, (value < 64)?false:true);
            break;
            
        case 127:
            // Poly Mode
            SetMonoMode(ch, (value < 64)?true:false);
            break;
            
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
void C700Generator::Process( unsigned int frames, float *output[2] )
{
	int		outx;
	int		pitch;
	int		procstep = (INTERNAL_CLOCK*CYCLES_PER_SAMPLE) / mSampleRate;    // CYCLES_PER_SAMPLE=1.0 �Ƃ����Œ菬��
	
	//���C������
    /*
    if (mClearEvent) {
        mMIDIEvt.clear();
        mDelayedEvt.clear();
        mClearEvent = false;
    }
    */
	for (unsigned int frame=0; frame<frames; ++frame) {
		//�C�x���g����
		if ( mMIDIEvt.size() != 0 ) {
			std::list<MIDIEvt>::iterator	it = mMIDIEvt.begin();
			while ( it != mMIDIEvt.end() ) {
				if ( it->remain_samples >= 0 ) {
					it->remain_samples--;
                }
                if ( it->remain_samples < 0 ) {
                    if (doEvents1(&(*it))) {
                        it = mMIDIEvt.erase( it );
                        continue;
                    }
                }
				it++;
			}
		}
        if ( mDelayedEvt.size() != 0 ) {
			std::list<MIDIEvt>::iterator	it = mDelayedEvt.begin();
			while ( it != mDelayedEvt.end() ) {
				if ( it->remain_samples >= 0 ) {
					it->remain_samples--;
                }
                if ( it->remain_samples < 0 ) {
                    if (doEvents2(&(*it))) {
                        it = mDelayedEvt.erase( it );
                        continue;
                    }
                }
				it++;
			}
		}
        
        while (mPortamentCount >= 0) {
            // �|���^�����g����
#if 0
            std::list<int>::iterator	it = mPlayVo.begin();
            while (it != mPlayVo.end()) {
                int	vo = *it;
                if (mDVoice.IsKeyOn(vo)) {
                    processPortament(vo);
                }
                it++;
            }
#else
            for (int i=0; i<mDVoice.GetVoiceLimit(); i++) {
                if (mDVoice.IsKeyOn(i)) {
                    processPortament(i);
                }
            }
#endif
            mPortamentCount -= PORTAMENT_CYCLE_SAMPLES;
        }
		
		for ( ; mProcessFrac >= 0; mProcessFrac -= CYCLES_PER_SAMPLE ) {
			int outl=0,outr=0;
			for ( int v=0; v<kMaximumVoices; v++ ) {
				outx = 0;
				//--
				{
					switch( mVoice[v].envstate ) {
						case ATTACK:
							if ( mVoice[v].ar == 15 ) {
								mVoice[v].envx += 0x400;
							}
							else {
								mVoice[v].envcnt -= ENVCNT[ ( mVoice[v].ar << 1 ) + 1 ];
								if ( mVoice[v].envcnt > 0 ) {
									break;
								}
								mVoice[v].envx += 0x20;       /* 0x020 / 0x800 = 1/64         */
								mVoice[v].envcnt = CNT_INIT;
							}
							
							if ( mVoice[v].envx > 0x7FF ) {
								mVoice[v].envx = 0x7FF;
								mVoice[v].envstate = DECAY;
							}
							break;
							
						case DECAY:
							mVoice[v].envcnt -= ENVCNT[ mVoice[v].dr*2 + 0x10 ];
							if ( mVoice[v].envcnt <= 0 ) {
								mVoice[v].envcnt = CNT_INIT;
								mVoice[v].envx -= ( ( mVoice[v].envx - 1 ) >> 8 ) + 1;
							}
							
							if ( mVoice[v].envx <= 0x100 * ( mVoice[v].sl + 1 ) ) {
								mVoice[v].envstate = SUSTAIN;
							}
							break;
							
						case SUSTAIN:
							mVoice[v].envcnt -= ENVCNT[ mVoice[v].sr ];
							if ( mVoice[v].envcnt > 0 ) {
								break;
							}
							mVoice[v].envcnt = CNT_INIT;
							mVoice[v].envx -= ( ( mVoice[v].envx - 1 ) >> 8 ) + 1;
							break;
							
						case RELEASE:
							mVoice[v].envx -= 0x8;
							if ( mVoice[v].envx <= 0 ) {
								mVoice[v].envx = -1;
							}
							break;
					}
				}
				
				if ( mVoice[v].envx < 0 ) {
					outx = 0;
					continue;
				}
				
				//�s�b�`�̎Z�o
                int voicePitch = static_cast<int>(mVoice[v].portaPitch + 0.5f);

				pitch = (voicePitch + mVoice[v].pb) & 0x3fff;
				
				if (mVoice[v].reg_pmod) {
					mVoice[v].vibPhase += mVibfreq;
					if (mVoice[v].vibPhase > onepi) {
						mVoice[v].vibPhase -= onepi*2;
					}
					
					float vibwave = VibratoWave(mVoice[v].vibPhase);
					int pitchRatio = (vibwave*mVibdepth)*VOLUME_CURB[mVoice[v].vibdepth];
					
					pitch = ( pitch * ( pitchRatio + 32768 ) ) >> 15;
					if (pitch <= 0) {
						pitch=1;
					}
				}
				
				for( ; mVoice[v].mixfrac >= 0; mVoice[v].mixfrac -= 4096 ) {
					if( !mVoice[v].headerCnt ) {	//�u���b�N�̎n�܂�
						if( mVoice[v].end & 1 ) {	//�f�[�^�I���t���O����
							if( mVoice[v].loop ) {
								mVoice[v].memPtr = mVoice[v].loopPoint;	//�ǂݏo���ʒu�����[�v�|�C���g�܂Ŗ߂�
							}
							else {	//���[�v�Ȃ�
								mVoice[v].envx = 0;
								while( mVoice[v].mixfrac >= 0 ) {
									mVoice[v].sampbuf[mVoice[v].sampptr] = 0;
									outx = 0;
									mVoice[v].sampptr  = ( mVoice[v].sampptr + 1 ) & 3;
									mVoice[v].mixfrac -= 4096;
								}
								break;
							}
						}
						
						//�J�n�o�C�g�̏����擾
						mVoice[v].headerCnt = 8;
						int headbyte = ( unsigned char )mVoice[v].brrdata[mVoice[v].memPtr++];
						mVoice[v].range = headbyte >> 4;
						mVoice[v].end = headbyte & 3;
						mVoice[v].filter = ( headbyte & 12 ) >> 2;
					}
					
					if ( mVoice[v].half == 0 ) {
						mVoice[v].half = 1;
						outx = ( ( signed char )mVoice[v].brrdata[ mVoice[v].memPtr ] ) >> 4;
					}
					else {
						mVoice[v].half = 0;
						outx = ( signed char )( mVoice[v].brrdata[ mVoice[v].memPtr++ ] << 4 );
						outx >>= 4;
						mVoice[v].headerCnt--;
					}
					//outx:4bit�f�[�^
					
					if ( mVoice[v].range <= 0xC ) {
						outx = ( outx << mVoice[v].range ) >> 1;
					}
					else {
						outx &= ~0x7FF;
					}
					//outx:4bit�f�[�^*Range
					
					switch( mVoice[v].filter ) {
						case 0:
							break;
							
						case 1:
							outx += filter1(mVoice[v].smp1);
							break;
							
						case 2:
							outx += filter2(mVoice[v].smp1,mVoice[v].smp2);
							break;
							
						case 3:
							outx += filter3(mVoice[v].smp1,mVoice[v].smp2);
							break;
					}
					
                    // �t�B���^��ɃN���b�v
					if ( outx < -32768 ) {
						outx = -32768;
					}
					else if ( outx > 32767 ) {
						outx = 32767;
					}
                    // y[-1]�֑���ۂɂQ�{���ꂽ��N�����v
					if (mNewADPCM) {
						mVoice[v].smp2 = mVoice[v].smp1;
						mVoice[v].smp1 = ( signed short )( outx << 1 );
					}
					else {
                        // �Â��G�~�����[�^�̈ꕔ�ɂ̓N�����v���s��Ȃ����̂�����
                        // ���͎��@�ƈقȂ�
						mVoice[v].smp2 = mVoice[v].smp1;
						mVoice[v].smp1 = outx << 1;
					}
                    mVoice[v].sampbuf[mVoice[v].sampptr] = mVoice[v].smp1;
					mVoice[v].sampptr = ( mVoice[v].sampptr + 1 ) & 3;
				}
				
				int fracPos = mVoice[v].mixfrac >> 4;
				int smpl = ( ( G4[ -fracPos - 1 ] * mVoice[v].sampbuf[ mVoice[v].sampptr ] ) >> 11 ) & ~1;
				smpl += ( ( G3[ -fracPos ]
						 * mVoice[v].sampbuf[ ( mVoice[v].sampptr + 1 ) & 3 ] ) >> 11 ) & ~1;
				smpl += ( ( G2[ fracPos ]
						 * mVoice[v].sampbuf[ ( mVoice[v].sampptr + 2 ) & 3 ] ) >> 11 ) & ~1;
				// openspc�ł͂Ȃ��������ł��N�����v�����Ă���
                // �����������Ǝ��@�ƈ���Ă��܂�
				if (mNewADPCM) {
					smpl = ( signed short )smpl;
				}
				smpl += ( ( G1[ fracPos ]
						 * mVoice[v].sampbuf[ ( mVoice[v].sampptr + 3 ) & 3 ] ) >> 11 ) & ~1;
				
                // �K�E�X��Ԍ�ɃN���b�v
				if ( smpl > 32767 ) {
					smpl = 32767;
				}
				else if ( smpl < -32768 ) {
					smpl = -32768;
				}
				outx = smpl;
				
				mVoice[v].mixfrac += pitch;
				
				outx = ( ( outx * mVoice[v].envx ) >> 11 ) & ~1;
				outx = ( outx * mVoice[v].velo ) >> 11;
				
                //�p���̃{�����[���l�ւ̔��f
                int volL = mVoice[v].vol_l;
                int volR = mVoice[v].vol_r;
                calcPanVolume(mVoice[v].pan, &volL, &volR);
                
				//�{�����[���l�̔��f
				volL = ( volL * VOLUME_CURB[ mVoice[v].volume ] ) / 0x7ff;
				volR = ( volR * VOLUME_CURB[ mVoice[v].volume ] ) / 0x7ff;
                
                // �G�N�X�v���b�V�����̔��f
				volL = ( volL * VOLUME_CURB[ mVoice[v].expression ] ) / 0x7ff;
				volR = ( volR * VOLUME_CURB[ mVoice[v].expression ] ) / 0x7ff;
				
				//�Q�C���l�̔��f
				int vl = ( volL * outx ) >> 7;
				int vr = ( volR * outx ) >> 7;
                
				//�G�R�[����
				if ( mVoice[v].echoOn ) {
					mEcho[0].Input(vl);
					mEcho[1].Input(vr);
				}
				//���C���{�����[���̔��f
				outl += ( vl * mMainVolume_L ) >> 7;
				outr += ( vr * mMainVolume_R ) >> 7;
			}
			outl += mEcho[0].GetFxOut();
			outr += mEcho[1].GetFxOut();
			mProcessbuf[0][mProcessbufPtr] = outl;
			mProcessbuf[1][mProcessbufPtr] = outr;
			mProcessbufPtr=(mProcessbufPtr+1)&0x0f;
            
            mPortamentCount++;
		}
		//--
		//16pointSinc���
		for ( int ch=0; ch<2; ch++ ) {
			int inputFrac = mProcessFrac+CYCLES_PER_SAMPLE;
			int tabidx1 = ( inputFrac/1764 ) << 4;
			int tabidx2 = tabidx1 + 16;
			int a1 = 0, a2 = 0;
			for (int i=0; i<4; i++) {
				a1 += sinctable[tabidx1++] * mProcessbuf[ch][mProcessbufPtr] >> 15;
				a2 += sinctable[tabidx2++] * mProcessbuf[ch][mProcessbufPtr] >> 15;
				mProcessbufPtr=(mProcessbufPtr+1)&0x0f;
				a1 += sinctable[tabidx1++] * mProcessbuf[ch][mProcessbufPtr] >> 15;
				a2 += sinctable[tabidx2++] * mProcessbuf[ch][mProcessbufPtr] >> 15;
				mProcessbufPtr=(mProcessbufPtr+1)&0x0f;
				a1 += sinctable[tabidx1++] * mProcessbuf[ch][mProcessbufPtr] >> 15;
				a2 += sinctable[tabidx2++] * mProcessbuf[ch][mProcessbufPtr] >> 15;
				mProcessbufPtr=(mProcessbufPtr+1)&0x0f;
				a1 += sinctable[tabidx1++] * mProcessbuf[ch][mProcessbufPtr] >> 15;
				a2 += sinctable[tabidx2++] * mProcessbuf[ch][mProcessbufPtr] >> 15;
				mProcessbufPtr=(mProcessbufPtr+1)&0x0f;
			}
			if ( output[ch] ) {
				output[ch][frame] += ( a1 + ( (( a2 - a1 ) * ( inputFrac % 1764 )) / 1764 ) ) / 32768.0f;
			}
		}
		//--
		
		mProcessFrac += procstep;
	}
}

//-----------------------------------------------------------------------------
void C700Generator::calcPanVolume(int value, int *volL, int *volR)
{
#if 0
    int     absL = (*volL > 0)?(*volL):-(*volL);
    int     absR = (*volR > 0)?(*volR):-(*volR);
    int     center =(absR + absL) / 2;
    int     diffRL = absR - absL;
    diffRL += value * 2 - 128;
    absL = center - diffRL;
    absR = center + diffRL;
    if (absL > 127) {
        absL = 127;
    }
    if (absL < 0) {
        absL = 0;
    }
    if (absR > 127) {
        absR = 127;
    }
    if (absR < 0) {
        absR = 0;
    }
    *volL = (*volL)>0 ? absL:-absL;
    *volR = (*volR)>0 ? absR:-absR;
#else
    *volL = (*volL * PAN_CURB[value]) / 511;
    *volR = (*volR * PAN_CURB[128-value]) / 511;
#endif
}

//-----------------------------------------------------------------------------
void C700Generator::RefreshKeyMap(void)
{
	if ( mVPset ) {
		bool	initialized[NUM_BANKS];
		for (int i=0; i<NUM_BANKS; i++ ) {
			initialized[i] = false;
		}
		
		for (int prg=0; prg<128; prg++) {
			if (mVPset[prg].brr.data) {
				if ( !initialized[mVPset[prg].bank] ) {
					// ��ԍŏ��̃v���O�����ŏ��������邱�ƂŁA���g�p�p�b�`��0�ɂȂ�Ȃ��悤�ɂ���
					for (int i=0; i<128; i++) {
						mKeyMap[mVPset[prg].bank][i]=prg;
					}
					initialized[mVPset[prg].bank] = true;
				}
				for (int i=mVPset[prg].lowkey; i<=mVPset[prg].highkey; i++) {
					mKeyMap[mVPset[prg].bank][i]=prg;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
void C700Generator::SetVPSet( InstParams *vp )
{
    mVPset = vp;
    for (int i=0; i<16; i++) {
        mChStat[i].changeFlg = 0;
        mChStat[i].changedVP = vp[0];
    }
}

//-----------------------------------------------------------------------------
double C700Generator::GetProcessDelayTime()
{
    return ((mEventDelayClocks / CLOCKS_PER_SAMPLE) + 8) / static_cast<double>(INTERNAL_CLOCK);    // 8ms + resample
}
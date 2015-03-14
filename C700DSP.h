//
//  C700DSP.h
//  C700
//
//  Created by osoumen on 2014/11/30.
//
//

#ifndef __C700__C700DSP__
#define __C700__C700DSP__

#include "EchoKernel.h"
#include "C700defines.h"
#include <iostream>

//-----------------------------------------------------------------------------
typedef enum
{
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    //	FASTRELEASE
} env_state_t32;

class C700DSP {
public:
    C700DSP();
    ~C700DSP();
    
    void ResetVoice(int voice);
    void ResetEcho();
    
    void SetVoiceLimit(int value);
    void SetNewADPCM(bool value);
    void SetMainVolumeL(int value);
    void SetMainVolumeR(int value);
    void SetEchoVol_L( int value );
    void SetEchoVol_R( int value );
    void SetFeedBackLevel( int value );
    void SetDelayTime( int value );
    void SetFIRTap( int tap, int value );
    void SetAR(int v, int value);
    void SetDR(int v, int value);
    void SetSL(int v, int value);
    void SetSR(int v, int value);
    
    void KeyOffVoice(int v);
    void KeyOnVoice(int v);
    
private:
    struct DSPState {
        // 音源内部状態
		int				ar,dr,sl,sr,vol_l,vol_r;
		unsigned int	loopPoint;
		bool			loop;
		bool			echoOn;
        
		unsigned char	*brrdata;
		int				memPtr;        /* Sample data memory pointer   */
		int             end;            /* End or loop after block      */
		int             envcnt;         /* Counts to envelope update    */
		env_state_t32   envstate;       /* Current envelope state       */
		int             envx;           /* Last env height (0-0x7FFF)   */
		int             filter;         /* Last header's filter         */
		int             half;           /* Active nybble of BRR         */
		int             headerCnt;     /* Bytes before new header (0-8)*/
		int             mixfrac;        /* Fractional part of smpl pstn */	//サンプル間を4096分割した位置
		int				pitch;          /* Sample pitch (4096->32000Hz) */
		int             range;          /* Last header's range          */
		int             sampptr;        /* Where in sampbuf we are      */
		int				smp1;           /* Last sample (for BRR filter) */
		int				smp2;           /* Second-to-last sample decoded*/
		int				sampbuf[4];   /* Buffer for Gaussian interp   */
        
		void Reset();
	};
    EchoKernel		mEcho[2];
    
    DSPState		mVoice[kMaximumVoices];		//ボイスの状況
	
	int				mVoiceLimit;
	int				mMainVolume_L;
	int				mMainVolume_R;
	bool			mNewADPCM;

};

#endif /* defined(__C700__C700DSP__) */

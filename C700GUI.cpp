/*
 *  C700GUI.cpp
 *  Chip700
 *
 *  Created by osoumen on 12/10/01.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "C700GUI.h"
#include "ControlInstances.h"
#include "czt.h"
#include "brrcodec.h"
#include "cfileselector.h"

#if AU
#include "plugguieditor.h"
#else
#include "aeffguieditor.h"
#endif

static CFontDesc g_LabelFont("Helvetica Bold", 9);
CFontRef kLabelFont = &g_LabelFont;

//-----------------------------------------------------------------------------
CControl *C700GUI::makeControlFrom( const ControlInstances *desc, CFrame *frame )
{
	CControl	*cntl = NULL;
	CRect size(0, 0, desc->w , desc->h);
	size.offset(desc->x, desc->y);
	int	value = desc->value;
	int	minimum = desc->minimum;
	int	maximum = desc->maximum;
	
	switch (desc->kind_sig) {
		case 'VeMa':
			switch (desc->kind ) {
				case 'wave':
				{
					CWaveView	*waveview;
					waveview = new CWaveView(size, frame, this, desc->id);
					cntl = waveview;
					break;
				}
				default:
					goto makeDummy;
					break;
			}
			break;
			
		case 'airy':
			switch (desc->kind) {
				case 'slid':
				{
					CMySlider	*slider;
					if ( desc->w < desc->h )
					{
						slider = new CMySlider(size, this, desc->id, size.top, size.bottom - sliderHandleBitmap->getHeight(), sliderHandleBitmap, 0, CPoint(0, 0), kBottom|kVertical );
					}
					else
					{
						slider = new CMySlider(size, this, desc->id, size.left, size.right - sliderHandleBitmap->getWidth(), sliderHandleBitmap, 0, CPoint(0, 0), kLeft|kHorizontal );
					}
					cntl = slider;
					break;
				}
				case 'knob':
				{
					CMyKnob		*knob;
					knob = new CMyKnob(size, this, desc->id, bgKnob, 0);
					knob->setColorHandle( MakeCColor(67, 75, 88, 255) );
					knob->setColorShadowHandle( kTransparentCColor );
					knob->setInsetValue(1);
					cntl = knob;
					break;
				}
				case 'cbtn':
				{
					CLabelOnOffButton	*button;
					button = new CLabelOnOffButton(size, this, desc->id, onOffButton, desc->title);
					cntl = button;
					break;
				}
				case 'dtxt':
				{
					char fontName[100], unitStr[100];
					float fontSize, fontRColour, fontGColour, fontBColour, horizBorder, vertBorder, valueMultipler;
					sscanf(desc->title, "%s %f %f %f %f %f %f %f %s", fontName, &fontSize, &fontRColour, &fontGColour, &fontBColour, &horizBorder, &vertBorder, &valueMultipler, unitStr);
					
					CFontRef	fontDesc = new CFontDesc(fontName, fontSize);
					CMyParamDisplay	*paramdisp;
					paramdisp = new CMyParamDisplay(size, desc->id, valueMultipler, unitStr, 0, 0);
					paramdisp->setFont(fontDesc);
					paramdisp->setFontColor(MakeCColor(fontRColour, fontGColour, fontBColour, 255));
					paramdisp->setAntialias(true);

					fontDesc->forget();
					cntl = paramdisp;
					break;
				}
				case 'eutx':
				{
					CMyTextEdit	*textEdit;
					textEdit = new CMyTextEdit(size, this, desc->id, desc->title, desc->futureuse==2?true:false, desc->futureuse==1?true:false);
					textEdit->setFontColor(MakeCColor(180, 248, 255, 255));
					textEdit->setAntialias(true);
					
					CFontRef	fontDesc;
					int			fontsize = 10;
					if ( desc->fontsize > 0 )
					{
						fontsize = desc->fontsize;
					}
					if ( desc->fontname[0] != 0 )
					{
						fontDesc = new CFontDesc(desc->fontname, fontsize);
					}
					else
					{
						fontDesc = new CFontDesc("Monaco", fontsize);
					}
					textEdit->setFont(fontDesc);
					textEdit->setHoriAlign(desc->fontalign);
					fontDesc->forget();
					cntl = textEdit;
					
					if ( desc->futureuse==1 )
					{
						//小数型のとき最大値の制限を外す
						maximum = 0x7fffffff;
					}
					break;
				}
				case 'valp':
				{
					char		rsrcName[100];
					snprintf(rsrcName, 99, "%s.png", desc->title);
					CBitmap		*btnImage = new CBitmap(rsrcName);
					CMovieBitmap	*button;
					button = new CMovieBitmap(size, this, desc->id, btnImage );
					btnImage->forget();
					cntl = button;
					break;
				}
				case 'bttn':
				{
					char		rsrcName[100];
					snprintf(rsrcName, 99, "%s.png", desc->title);
					CBitmap		*btnImage = new CBitmap(rsrcName);
					COnOffButton	*button;
					button = new COnOffButton(size, this, desc->id, btnImage );
					btnImage->forget();
					cntl = button;
					break;
				}
				case 'push':
				{
					char		rsrcName[100];
					snprintf(rsrcName, 99, "%s.png", desc->title);
					CBitmap		*btnImage = new CBitmap(rsrcName);
					CKickButton	*button;
					button = new CKickButton(size, this, desc->id, btnImage );
					btnImage->forget();
					cntl = button;
					break;
				}
				case 'vtsw':
				{
					char		rsrcName[100];
					snprintf(rsrcName, 99, "%s.png", desc->title);
					CBitmap			*btnImage = new CBitmap(rsrcName);
					CVerticalSwitch	*button;
					button = new CVerticalSwitch(size, this, desc->id, btnImage );
					btnImage->forget();
					cntl = button;
					break;
				}
				default:
					goto makeDummy;
					break;
			}
			break;
			
		case 'appl':
			switch (desc->kind) {
				case 'stxt':
				{
					CTextLabel	*textLabel;
					size.offset(0, -2);	//位置補正
					textLabel = new CTextLabel(size, desc->title, 0, 0);
					textLabel->setFontColor(kBlackCColor);
					textLabel->setHoriAlign(desc->fontalign);
					textLabel->setTransparency(true);
					textLabel->setAntialias(true);
					textLabel->setTag(desc->id);
					
					CFontRef	fontDesc;
					int			fontsize = 9;
					if ( desc->fontsize > 0 )
					{
						fontsize = desc->fontsize;
					}
					if ( desc->fontname[0] != 0 || desc->fontsize != 0)
					{
						if ( desc->fontname[0] == 0 )
						{
							fontDesc = new CFontDesc(kLabelFont->getName(), fontsize);
						}
						else
						{
							fontDesc = new CFontDesc(desc->fontname, fontsize);
						}
						textLabel->setFont(fontDesc);
						fontDesc->forget();
					}
					else
					{
						textLabel->setFont(kLabelFont);
					}
					cntl = textLabel;
					break;
				}
				case 'larr':
				{
					CRockerSwitch *rockerSwitch;
					rockerSwitch = new CRockerSwitch(size, this, desc->id, rocker->getHeight() / 3, rocker, CPoint(0, 0), kVertical);
					cntl = rockerSwitch;
					minimum = -1;
					maximum = 1; 
					break;
				}
				case 'sepa':
				{
					CSeparatorLine	*sepa;
					sepa = new CSeparatorLine(size, this, desc->id);
					cntl = sepa;
					break;
				}
				default:
					goto makeDummy;
					break;
			}
			break;
			
		default:
			goto makeDummy;
			break;
	}
	goto setupCntl;
	
makeDummy:
	cntl = new CDummyCntl(size);
	
setupCntl:
	if ( cntl )
	{
		cntl->setMin(minimum);
		cntl->setMax(maximum);
		cntl->setValue(value);
		//cntl->setAttribute(kCViewTooltipAttribute,strlen(desc->title)+1,desc->title);
	}
	return cntl;
}

//-----------------------------------------------------------------------------
C700GUI::C700GUI(const CRect &inSize, CFrame *frame, CBitmap *pBackground)
: CViewContainer (inSize, frame, pBackground)
, mNumCntls( 0 )
, mCntl(NULL)
, efxAcc(NULL)
{
	//共通グラフィックの読み込み
	bgKnob = new CBitmap("knobBack.png");
	sliderHandleBitmap = new CBitmap("sliderThumb.png");
	onOffButton = new CBitmap("bt_check.png");
	rocker = new CBitmap("rocker_sw.png");
	
	//コントロールの個数
	mNumCntls = sizeof(sCntl) / sizeof(ControlInstances);
	
	//作成したコントロールのインスタンスへのポインタを保持しておく
	mCntl = new CControl*[mNumCntls];
	
	for ( int i=0; i<mNumCntls; i++ )
	{
		CControl	*cntl;
		cntl = makeControlFrom( &sCntl[i], frame );
		if ( cntl )
		{
			addView(cntl);
			mCntl[i] = cntl;
		}
	}
	
	bgKnob->forget();
	sliderHandleBitmap->forget();
	onOffButton->forget();
	rocker->forget();
	
	//以下テストコード
#if 0
	//--CMyKnob--------------------------------------
	CBitmap *bgKnob = new CBitmap("knobBack.png");
	
 	CRect	size(0, 0, bgKnob->getWidth(), bgKnob->getHeight());
	size.offset(140 + 15, 15);
	CPoint	point(0, 0);
	cKnob = new CMyKnob(size, this, 801, bgKnob, 0, point);
	cKnob->setColorHandle( MakeCColor(67, 75, 88, 255) );
	cKnob->setColorShadowHandle( kTransparentCColor );
	cKnob->setInsetValue(1);
	cKnob->setMax(2.0f);
	cKnob->setMin(-2.0f);
	addView(cKnob);
	bgKnob->forget();
	cKnob->setAttribute(kCViewTooltipAttribute,strlen("CMyKnob")+1,"CMyKnob");
	
	//--CMySlider--------------------------------------
	CBitmap *sliderHandleBitmap = new CBitmap("sliderThumb.png");
	
	size(0, 0, sliderHandleBitmap->getWidth(), 128);
	size.offset(280, 70);
#if 1
	point(0, 0);
	cVerticalSlider = new CMySlider(size, this, 505, size.top, size.top + 128 - sliderHandleBitmap->getHeight(), sliderHandleBitmap, 0, point, kBottom|kVertical);
//	point(0, 0);
//	cVerticalSlider->setOffsetHandle(point);
#else
	point(0, 0);
	CPoint handleOffset(0, 0);
	cVerticalSlider = new CMySlider(size, this, 505, handleOffset, 
										   size.height() - 2 * handleOffset.v, sliderHandleBitmap, 0, point, kBottom);
#endif
	//cVerticalSlider->setFreeClick(false);
	cVerticalSlider->setMax(2.0f);
	cVerticalSlider->setMin(-2.f);
	addView(cVerticalSlider);
	cVerticalSlider->setAttribute(kCViewTooltipAttribute,strlen("CMySlider")+1,"CMySlider");
	
	sliderHandleBitmap->forget();
	
	//--CLabelOnOffButton--------------------------------------
	CBitmap *onOffButton = new CBitmap("bt_check.png");
	
	size(0, 0, 80, onOffButton->getHeight() / 2);
	size.offset(20, 20);
	cCheckBox = new CLabelOnOffButton(size, this, 808, onOffButton, "Echo");
	addView(cCheckBox);
	cCheckBox->setAttribute(kCViewTooltipAttribute,strlen("CLabelOnOffButton")+1,"CLabelOnOffButton");
	
	onOffButton->forget();
	
	//--CRockerSwitch--------------------------------------
	CBitmap *rocker = new CBitmap("rocker_sw.png");
 	size(0, 0, rocker->getWidth(), rocker->getHeight() / 3);
	size.offset(9, 70 + 29);
	point(0, 0);
	cRockerSwitch = new CRockerSwitch(size, this, 707, rocker->getHeight() / 3, rocker, point, kVertical);
	addView(cRockerSwitch);
	rocker->forget();
	cRockerSwitch->setAttribute(kCViewTooltipAttribute,strlen("CRockerSwitch")+1,"CRockerSwitch");
	
	//--CWaveView--------------------------------------
 	size(0, 0, 200, 100);
	size.offset(64, 128);
	cWaveView = new CWaveView(size, frame, this, 606);
	{
		short	testWave[1024];
		float	freq = 1.0f;
		for ( int i=0; i<1024; i++ )
		{
			testWave[i] = sin( 3.14*2*freq * i / 1024 )*32767;
			cWaveView->setWave(testWave, 1024);
			cWaveView->setLooppoint(96);
			freq += 0.03f;
		}
	}
	addView(cWaveView);
	cWaveView->setAttribute(kCViewTooltipAttribute,strlen("CWaveView")+1,"CWaveView");
	
	//--CDummyCntl--------------------------------------
 	size(0, 0, 200, 100);
	size.offset(256, 256);
	cDummyTest = new CDummyCntl(size);
	addView(cDummyTest);
	cDummyTest->setAttribute(kCViewTooltipAttribute,strlen("CDummyCntl")+1,"CDummyCntl");
#endif
}

//-----------------------------------------------------------------------------
C700GUI::~C700GUI()
{
	if ( mCntl )
	{
		delete [] mCntl;
	}
	removeAll();
}

//-----------------------------------------------------------------------------
void C700GUI::valueChanged(CControl* control)
{
	//コントロールが操作された時に呼ばれる
	//エフェクター側に変化したパラメータを設定する処理を行う
	
	int		tag = control->getTag();
	float	value = control->getValue();
	const char	*text = NULL;
	
	//スライダーで設定出来る値には整数値しかないの少数以下を切り捨てる
	if ( control->isTypeOf("CMySlider") )
	{
		//エコーフィルタイコライザを除く
		if ( !(tag >= kAudioUnitCustomProperty_Band1 && tag <= kAudioUnitCustomProperty_Band5) ) {
			value = (int)value;
		}
	}
	
	//テキストボックスの場合は数値に変換する
	if ( control->isTypeOf("CMyTextEdit") )
	{
		CMyTextEdit		*textedit = static_cast<CMyTextEdit*> (control);
		text = textedit->getText();
		sscanf(text, "%f", &value);
		if ( tag == kAudioUnitCustomProperty_LoopPoint ) {
			value = ((int)value / 16) * 16;
		}
		control->setValue(value);
		control->bounceValue();		//値を範囲内に丸める
		value = control->getValue();
		if ( tag == kAudioUnitCustomProperty_LoopPoint ) {
			value = ((int)value / 16) * 9;
		}
	}
	
#if AU
	//0-2の値域に拡張する
	if ( tag == kParam_velocity )
	{
		value *= 2;
	}
#endif
	if ( tag < kAudioUnitCustomProperty_First )
	{
		efxAcc->SetParam( this, tag%1000, value );
	}
	else if ( tag < kControlCommandsFirst ) {
		int	propertyId = ((tag-kAudioUnitCustomProperty_First)%1000)+kAudioUnitCustomProperty_First;
		switch (propertyId) {
			case kAudioUnitCustomProperty_ProgramName:
				if ( text ) {
					efxAcc->SetSourceFilePath( text );
				}
				break;
			default:
				efxAcc->SetProperty( propertyId, value );
				break;
		}
	}
	else {
		switch (tag) {
			case kControlButtonCopy:
				if ( value > 0 ) {
					copyFIRParamToClipBoard();
				}
				break;
				
			case kControlButtonPreemphasis:
				break;
				
			case kControlButtonUnload:
				if ( value > 0 ) {
					BRRData		brr;
					brr.data=NULL;
					efxAcc->SetBRRData( &brr );
				}
				break;
			
			case kControlButtonLoad:
				if ( value > 0 ) {
					char	path[1024];
					bool	isSelected;
					isSelected = getLoadFile(path, 1024, "");
					if ( isSelected ) {
						loadToCurrentProgram(path);
					}
				}
				break;
				
			case kControlButtonSave:
				if ( value > 0 ) {
					saveFromCurrentProgram();
				}
				break;
				
			case kControlButtonSaveXI:
				if ( value > 0 ) {
					saveFromCurrentProgramToXI();
				}
				break;
				
			case kControlButtonAutoSampleRate:
				if ( value > 0 ) {
					autocalcCurrentProgramSampleRate();
				}
				break;
				
			case kControlButtonAutoKey:
				if ( value > 0 ) {
					autocalcCurrentProgramBaseKey();
				}
				break;
				
			case kControlButtonChangeLoopPoint:
			{
				int	loopPoint = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_LoopPoint);
				loopPoint -= value * 9;
				efxAcc->SetProperty(kAudioUnitCustomProperty_LoopPoint, loopPoint);
				break;
			}
				
			case kControlButtonChangeProgram:
			{
				int	programNo = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_EditingProgram);
				programNo -= value;
				efxAcc->SetProperty(kAudioUnitCustomProperty_EditingProgram, programNo);
				break;
			}
			
			case kControlSelectTrack16:
			case kControlSelectTrack15:
			case kControlSelectTrack14:
			case kControlSelectTrack13:
			case kControlSelectTrack12:
			case kControlSelectTrack11:
			case kControlSelectTrack10:
			case kControlSelectTrack9:
			case kControlSelectTrack8:
			case kControlSelectTrack7:
			case kControlSelectTrack6:
			case kControlSelectTrack5:
			case kControlSelectTrack4:
			case kControlSelectTrack3:
			case kControlSelectTrack2:
			case kControlSelectTrack1:
				efxAcc->SetProperty( kAudioUnitCustomProperty_EditingChannel, 15-(tag-kControlSelectTrack16) );
				break;
				
			case kControlBankDBtn:
			case kControlBankCBtn:
			case kControlBankBBtn:
			case kControlBankABtn:
				efxAcc->SetProperty( kAudioUnitCustomProperty_Bank, 3-(tag-kControlBankDBtn) );
				break;
				
			default:
				break;
		}
	}
}

//-----------------------------------------------------------------------------
bool C700GUI::attached(CView* view)
{
	return CViewContainer::attached(view);
}

//-----------------------------------------------------------------------------
bool C700GUI::removed(CView* parent)
{
	return CViewContainer::removed(parent);
}

//-----------------------------------------------------------------------------
CMessageResult C700GUI::notify(CBaseObject* sender, const char* message)
{
	return CViewContainer::notify(sender, message);
}

//-----------------------------------------------------------------------------
CControl *C700GUI::FindControlByTag( long tag )
{
	CControl	*cntl = NULL;
	if ( mCntl )
	{
		for ( int i=0; i<mNumCntls; i++ )
		{
			//単純な線形探索なのでもっと冴えた方法があるかも
			if ( mCntl[i]->getTag() == tag )
			{
				cntl = mCntl[i];
				break;
			}
		}
	}
	return cntl;
}

//-----------------------------------------------------------------------------
void C700GUI::copyFIRParamToClipBoard()
{
	// FIRパラメータをクリップボードにコピー
	CTextLabel	*textView = reinterpret_cast<CTextLabel*> (FindControlByTag(kControlXMSNESText));
	if ( textView == NULL ) return;
	
	const char	*text = textView->getText();
	
	OSStatus err = noErr;
	PasteboardRef theClipboard;
	err = PasteboardCreate( kPasteboardClipboard, &theClipboard );
	err = PasteboardClear( theClipboard );
	
	CFDataRef   data = CFDataCreate( kCFAllocatorDefault, (UInt8*)text, (strlen(text)) * sizeof(char) );
	err = PasteboardPutItemFlavor( theClipboard, (PasteboardItemID)1, kUTTypeUTF8PlainText, data, 0 );
	
	CFRelease(theClipboard);
	CFRelease( data );
}

//-----------------------------------------------------------------------------
void C700GUI::loadToCurrentProgram( const char *path )
{
	BRRFile		brrfile(path,false);
	AudioFile	audiofile(path,false);
	SPCFile		spcfile(path,false);
	
	brrfile.Load();
	if ( brrfile.IsLoaded() ) {
		loadToCurrentProgramFromBRR( &brrfile );
		goto _ret;
	}
	
	audiofile.Load();
	if ( audiofile.IsLoaded() ) {
		loadToCurrentProgramFromAudioFile( &audiofile );
		goto _ret;
	}
	
	spcfile.Load();
	if ( spcfile.IsLoaded() ) {
		loadToCurrentProgramFromSPC( &spcfile );
		goto _ret;
	}
	
_ret:
	return;
}

//-----------------------------------------------------------------------------
void C700GUI::loadToCurrentProgramFromBRR( BRRFile *file )
{
}

//-----------------------------------------------------------------------------
void C700GUI::loadToCurrentProgramFromAudioFile( AudioFile *file )
{
	InstData	inst;
	short		*wavedata;
	long		numSamples;
	BRRData		brr;
	int			looppoint;
	bool		loop;
	int			pad;
	
	//波形ファイルの情報を取得
	wavedata	= file->GetAudioData();
	numSamples	= file->GetLoadedSamples();
	file->GetInstData( &inst );
	
	if ( IsPreemphasisOn() ) {
		emphasis(wavedata, numSamples);
	}
	
	brr.data = new unsigned char[numSamples/16*9+18];
	if (inst.loop) {
		numSamples = inst.lp_end;
	}
	looppoint = (inst.lp + 15)/16*9;
	loop = inst.loop ? true:false;
	pad = 16-(numSamples % 16);
	brr.size = brrencode(wavedata, brr.data, numSamples, loop, (looppoint/9)*16, pad);
	looppoint += pad/16 * 9;
	
	//波形データを設定
	efxAcc->SetBRRData(&brr);
	efxAcc->SetProperty(kAudioUnitCustomProperty_Rate,		inst.srcSamplerate);
	efxAcc->SetProperty(kAudioUnitCustomProperty_BaseKey,	inst.basekey);
	efxAcc->SetProperty(kAudioUnitCustomProperty_LowKey,	inst.lowkey);
	efxAcc->SetProperty(kAudioUnitCustomProperty_HighKey,	inst.highkey);
	efxAcc->SetProperty(kAudioUnitCustomProperty_LoopPoint,	looppoint);
	efxAcc->SetProperty(kAudioUnitCustomProperty_Loop,		loop ? 1.0f:.0f);
	
	//元波形データの情報をセットする
	efxAcc->SetSourceFilePath( file->GetFilePath() );
	efxAcc->SetProperty(kAudioUnitCustomProperty_IsEmaphasized,	IsPreemphasisOn() ? 1.0f:.0f);
	
	//TODO : 拡張子を除いたファイル名をプログラム名に設定する
	efxAcc->SetProgramName( file->GetFilePath() );
	
	delete[] brr.data;	
}

//-----------------------------------------------------------------------------
void C700GUI::loadToCurrentProgramFromSPC( SPCFile *file )
{
	BRRData	brr;
	double	samplerate;
	int		looppoint;
	bool	loop;
	int		pitch, length;
	short	*buffer;
	int		cEditNum=0;
	
	for (int i=0; i<128; i++) {
		brr.data = file->GetSampleIndex(i, &brr.size);
		if ( brr.data == NULL ) continue;
		
		looppoint = file->GetLoopSizeIndex( i );
		loop = looppoint >= 0 ? true:false;
		
		samplerate = 32000;
		if (loop) {
			buffer = new short[(brr.size*2)/9*16];
			brrdecode(brr.data, buffer, looppoint, 2);
			length = ((brr.size-looppoint)*2)/9*16;
			pitch = estimatebasefreq(buffer+looppoint/9*16, length);
			if (pitch > 0) {
				samplerate = length/(double)pitch * 440.0*pow(2.0,-9.0/12);
			}
			delete[] buffer;
		}
		
		efxAcc->SetProperty(kAudioUnitCustomProperty_EditingProgram, cEditNum);
		efxAcc->SetBRRData(&brr);
		efxAcc->SetProperty(kAudioUnitCustomProperty_Rate, samplerate);
		efxAcc->SetProperty(kAudioUnitCustomProperty_BaseKey, 60);
		efxAcc->SetProperty(kAudioUnitCustomProperty_LowKey, 0);
		efxAcc->SetProperty(kAudioUnitCustomProperty_HighKey, 127);
		efxAcc->SetProperty(kAudioUnitCustomProperty_LoopPoint, looppoint);
		efxAcc->SetProperty(kAudioUnitCustomProperty_Loop, loop?1.0f:.0f);
		
		//TODO : ファイルネームの処理
		efxAcc->SetProgramName(file->GetFilePath());
		/*
		CFURLRef	extlesspath=CFURLCreateCopyDeletingPathExtension(NULL,path);
		CFStringRef	filename = CFURLCopyLastPathComponent(extlesspath);
		CFStringRef	dataname = CFStringCreateWithFormat(NULL,NULL,CFSTR("%@#%02x"),filename,i);
		AudioUnitSetProperty(mEditAudioUnit,kAudioUnitCustomProperty_ProgramName,kAudioUnitScope_Global,0,&dataname,sizeof(CFStringRef));
		CFRelease(dataname);
		CFRelease(filename);
		CFRelease(extlesspath);
		*/
		cEditNum++;
	}
	efxAcc->SetParam(this, kParam_clipnoise, 1);
}

//-----------------------------------------------------------------------------
bool C700GUI::getLoadFile( char *path, int maxLen, const char *title )
{
#ifdef WIN32
	CFileSelector OpenFile( ((AEffGUIEditor *)getEditor())->getEffect() );
	VstFileSelect Filedata;
	memset(&Filedata, 0, sizeof(VstFileSelect));
	Filedata.command=kVstFileLoad;
	Filedata.type= kVstFileType;
	strncpy(Filedata.title, title, 1023 );
	//Filedata.nbFileTypes=1;
	//Filedata.fileTypes=&waveType;
	Filedata.returnPath= path;
	Filedata.initialPath = 0;
	Filedata.future[0] = 1;
	if (OpenFile.run(&Filedata) > 0) {
		return true;
	}
#else
	CNewFileSelector* selector = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectFile);
	if (selector)
	{
		if ( title ) selector->setTitle(title);
		selector->runModal();
		if ( selector->getNumSelectedFiles() > 0 ) {
			const char *url = selector->getSelectedFile(0);
			strncpy(path, url, maxLen-1);
		}
		selector->forget();
		return true;
	}
#endif
	return false;
}

//-----------------------------------------------------------------------------
bool C700GUI::getSaveFile( char *path, int maxLen, const char *defaultName, const char *title )
{
#ifdef WIN32
	CFileSelector OpenFile( ((AEffGUIEditor *)getEditor())->getEffect() );
	VstFileSelect Filedata;
	memset(&Filedata, 0, sizeof(VstFileSelect));
	Filedata.command=kVstFileSave;
	Filedata.type= kVstFileType;
	strncpy(Filedata.title, title, 1023 );
	//Filedata.nbFileTypes=1;
	//Filedata.fileTypes=&waveType;
	Filedata.returnPath= path;
	Filedata.initialPath = 0;
	Filedata.future[0] = 1;
	if (OpenFile.run(&Filedata) > 0) {
		return true;
	}
#else
	CNewFileSelector* selector = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectSaveFile);
	if (selector)
	{
		if ( defaultName ) selector->setDefaultSaveName(defaultName);
		if ( title ) selector->setTitle(title);
		selector->runModal();
		if ( selector->getNumSelectedFiles() > 0 ) {
			const char *url = selector->getSelectedFile(0);
			strncpy(path, url, maxLen-1);
		}
		selector->forget();
		return true;
	}
#endif
	return false;
}

//-----------------------------------------------------------------------------
void C700GUI::saveFromCurrentProgram()
{
}

//-----------------------------------------------------------------------------
void C700GUI::saveFromCurrentProgramToXI()
{
}

//-----------------------------------------------------------------------------
void C700GUI::autocalcCurrentProgramSampleRate()
{
	// 波形のサンプリングレートを検出
	int		looppoint;
	int		key;
	double	samplerate;
	BRRData	brr;
	short	*buffer;
	int		pitch, length;
	
	int		size = sizeof(BRRData);
	efxAcc->GetBRRData( &brr, &size );
	
	if (brr.data == NULL) return;
	
	looppoint = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_LoopPoint);
	if (looppoint == brr.size) {
		looppoint = 0;
	}
	
	key = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_BaseKey);
	
	buffer = new short[brr.size/9*16];
	brrdecode(brr.data, buffer, 0, 0);
	length = (brr.size-looppoint)/9*16;
	pitch = estimatebasefreq(buffer+looppoint/9*16, length);
	if (pitch > 0) {
		samplerate = length/(double)pitch * 440.0*pow(2.0,(key-69.0)/12);
		efxAcc->SetProperty(kAudioUnitCustomProperty_Rate, samplerate);
	}
	delete[] buffer;
}

//-----------------------------------------------------------------------------
void C700GUI::autocalcCurrentProgramBaseKey()
{
	// 波形の基本ノートを検出
	int		looppoint;
	int		key;
	double	samplerate, freq;
	BRRData	brr;
	short	*buffer;
	int		pitch, length;
	
	int		size = sizeof(BRRData);
	efxAcc->GetBRRData( &brr, &size );
	
	if (brr.data == NULL) return;
	
	looppoint = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_LoopPoint);
	if (looppoint == brr.size) {
		looppoint = 0;
	}	
	
	samplerate = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_Rate);
	
	buffer = new short[brr.size/9*16];
	brrdecode(brr.data, buffer, 0, 0);
	length = (brr.size-looppoint)/9*16;
	pitch = estimatebasefreq(buffer+looppoint/9*16, length);
	if (pitch > 0) {
		freq = samplerate / (length/(double)pitch);
		key = log(freq)*17.312-35.874;
		efxAcc->SetProperty(kAudioUnitCustomProperty_BaseKey, key);
	}
	delete[] buffer;
}

//-----------------------------------------------------------------------------
bool C700GUI::IsPreemphasisOn()
{
	CControl	*cntl = FindControlByTag(kControlButtonPreemphasis);
	return cntl->getValue()>0.5f ? true:false;
}

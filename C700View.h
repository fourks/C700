/*
 *  C700View.h
 *  C700
 *
 *  Created by 開発用 on 06/11/30.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __C700View__H_
#define __C700View__H_

#include "C700Version.h"
#include "AUCarbonViewNib.h"
#include "C700defines.h"

static OSErr MyDragSendDataFunction(FlavorType theType, void *dragSendRefCon,
									DragItemRef theItemRef, DragRef theDrag);

class C700View: public CAUCarbonViewNib
{
public:
    // Construction ------------------------------
    C700View(AudioUnitCarbonView inInstance): CAUCarbonViewNib(inInstance)
	{
		mEventDisable=false;
		preemphasis=true;
		shouldload=false;
	};
	~C700View();
    // Baseclass overrides -----------------------
    CFStringRef GetBundleID() { return CFSTR("com.VeMa.audiounit.Chip700"); };  // my Bundle ID 
    void InitWindow(CFBundleRef sBundle);
	virtual void FinishWindow(CFBundleRef sBundle);
	virtual bool HandleCommand(EventRef	inEvent, HICommandExtended &cmd);
	virtual bool HandleEventForView(EventRef event, HIViewRef view);
	virtual int GetFontsize() { return 10; }
	virtual int GetTextJustification() { return teJustCenter; }
	virtual CFStringRef GetFontname() { return CFSTR("Monaco"); }
	virtual void PropertyHasChanged(AudioUnitPropertyID inPropertyID, AudioUnitScope inScope,  
                                    AudioUnitElement inElement);
	virtual void Idle();
	
	void applyEditTextProp(ControlRef control);
	void enqueueFile(FSRef *ref);
	void loadFile(FSRef *ref);
	void loadFile(CFURLRef path);
	void saveToFile(CFURLRef savefile);
	void saveToXIFile(CFURLRef savefile);
private:
	void ChangeTrackSelectorValue( int track );
	void ChangeBankSelectorValue( int bank );
	
	static pascal ControlKeyFilterResult StdKeyFilterCallback(ControlRef theControl, 
																		SInt16 *keyCode, SInt16 *charCode, 
																		EventModifiers *modifiers);
	static pascal ControlKeyFilterResult NumericKeyFilterCallback(ControlRef theControl, 
																		SInt16 *keyCode, SInt16 *charCode, 
																		EventModifiers *modifiers);
	
	void changeEditingChannel( int trk );
	void changeBank( int bank );
	
	CFStringRef CreateXMSNESText();
	
	void setBRRData(UInt8 *data, UInt32 length);
	void setLoopoint(UInt32 lp);
	
	void correctSampleRateSelected(void);
	void correctBaseKeySelected(void);
	void loadSelected(void);
	void saveSelected(void);
	void saveSelectedXI(void);
	bool dragStart(ControlRef cont, EventRecord *event);
	int getLoadFile(FSRef *ref, CFStringRef window_title = NULL);
	CFURLRef getSaveFile(CFStringRef defaultName);
	int loadSPCFile(CFURLRef path);
	
	
	HIViewRef	hiOverView,hiTailView,hiHeadView;
	bool	preemphasis;
	bool	mEventDisable;
	
	FSRef	queingfile;
	bool	shouldload;
	
	friend OSErr MyDragSendDataFunction(FlavorType theType, void *dragSendRefCon,
								  DragItemRef theItemRef, DragRef theDrag);
};

#endif
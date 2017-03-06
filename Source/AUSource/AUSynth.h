//
//  AUSynth.h
//
//  Created by Matt Hill
//  Adapted from the MiniSynth diagram from Will Pirkle
//
/*
 
 */

#include "AUInstrumentBase.h"
#include "AUSynthVersion.h"
#include "AUSynthStructures.h"
#include "synthfunctions.h"
#include "SynthParamLimits.h"

// --- for MiniSynth
#include "MiniSynthVoice.h"
#include "StereoDelayFX.h"

#define MAX_VOICES 16

// --- our main AU Synth Object, derived from AUInstrumentBase
class AUSynth : public AUInstrumentBase
{
public:
    // --- const/dest
    AUSynth(AudioUnit inComponentInstance);
	virtual	~AUSynth();
    
    // --- AUInstrumentBase Overrides
    //
    // --- One-time init
	virtual OSStatus Initialize();
    
    // --- de-allocator (not used in our synths)
	virtual void Cleanup();
    
    // --- our version number, defined in AUSynthVersion.h
	virtual OSStatus Version() {return kAUSynthVersion;}
    
    // --- restore from presets
	OSStatus RestoreState(CFPropertyListRef plist);
   	
    // --- reset(); prepareForPlay();
    virtual OSStatus Reset(AudioUnitScope inScope,
                           AudioUnitElement inElement);

    // --- !!!the most important method: synthesizes audio!!!
	virtual OSStatus Render(AudioUnitRenderActionFlags& ioActionFlags,
                            const AudioTimeStamp& inTimeStamp,
                            UInt32 inNumberFrames);

    // --- host queries for information about our Paramaters (controls)
	virtual OSStatus GetParameterInfo(AudioUnitScope inScope,
                                      AudioUnitParameterID inParameterID,
                                      AudioUnitParameterInfo& outParameterInfo);
    
    // --- host queries for string-list controls we set up in above
    virtual OSStatus GetParameterValueStrings(AudioUnitScope inScope,
                                              AudioUnitParameterID inParameterID,
                                              CFArrayRef* outStrings);

     // --- host queries for Property info like MIDI and CocoaGUI capabilities
    virtual OSStatus GetPropertyInfo(AudioUnitPropertyID inID,
                                     AudioUnitScope inScope,
                                     AudioUnitElement inElement,
                                     UInt32& outDataSize,
                                     Boolean& outWritable);
	
    // --- host queries to get the Property info like MIDI Callback and Cocoa GUI factory
    //     results are returned cloaked as void*
	virtual OSStatus GetProperty(AudioUnitPropertyID inID,
                                 AudioUnitScope inScope,
                                 AudioUnitElement inElement,
                                 void* outData);
    
    // --- host calls to set a Property like MIDI callback
	virtual OSStatus SetProperty(AudioUnitPropertyID inID,
                                 AudioUnitScope inScope,
                                 AudioUnitElement inElement,
                                 const void* inData,
                                 UInt32 inDataSize);
    
    // --- MIDI Functions
    //
    // --- MIDI Note On
	virtual OSStatus StartNote(MusicDeviceInstrumentID inInstrument,
                               MusicDeviceGroupID inGroupID,
                               NoteInstanceID* outNoteInstanceID,
                               UInt32 inOffsetSampleFrame,
                               const MusicDeviceNoteParams &inParams);
    
    // --- MIDI Note Off
	virtual OSStatus StopNote(MusicDeviceGroupID inGroupID,
                              NoteInstanceID inNoteInstanceID,
                              UInt32 inOffsetSampleFrame);
    
    // --- MIDI Pitchbend (slightly different from all other CCs)
	virtual OSStatus HandlePitchWheel(UInt8 inChannel,
                                      UInt8 inPitch1,
                                      UInt8 inPitch2,
                                      UInt32 inStartFrame);
    
    // --- all other MIDI CC messages
    virtual OSStatus HandleControlChange(UInt8 inChannel,
                                         UInt8 inController,
                                         UInt8 inValue,
                                         UInt32	inStartFrame);

    // --- for ALL other MIDI messages you can get them here
    OSStatus HandleMidiEvent(UInt8 status,
                             UInt8 channel, 
                             UInt8 data1, 
                             UInt8 data2, 
                             UInt32 inStartFrame);

     // --- helper method for setting up Parameter Info
    void setAUParameterInfo(AudioUnitParameterInfo& outParameterInfo, 
                            CFStringRef paramName, 
                            CFStringRef paramUnits,  
                            Float32 fMinValue, 
                            Float32 fMaxValue, 
                            Float32 fDefaultValue,  
                            bool bLogControl = false, 
                            bool bStringListControl = false);
    
    // --- helper method for dealing with string-list control setup
    void setAUParameterStringList(CFStringRef stringList, 
                                  CFArrayRef* outStrings);
    
    // handle presets:
    virtual ComponentResult	GetPresets(CFArrayRef* outData)	const;    
    virtual OSStatus NewFactoryPresetSet(const AUPreset& inNewFactoryPreset);
    
    // CHALLENGE: use bool SetAFactoryPresetAsCurrent (const AUPreset & inPreset);
    //            to establish factory setting; see AUBase.h
    // 
    // CHALLENGE: implement an array of presets

    // --- example of a preset; you can have an array of them
    double factoryPreset[NUMBER_OF_SYNTH_PARAMETERS];

private:
    // --- NEW  DELAY FX
	CStereoDelayFX m_DelayFX;	
    
	// our voice array
	CMiniSynthVoice* m_pVoiceArray[MAX_VOICES];
   
    // -- MmM
	CModulationMatrix m_GlobalModMatrix;
    
	// --- global params
	globalSynthParams m_GlobalSynthParams;

	// --- helper functions for note on/off/voice steal
	void incrementVoiceTimestamps();
	CMiniSynthVoice* getOldestVoice();
	CMiniSynthVoice* getOldestVoiceWithNote(UINT uMIDINote);
    
	// --- updates all voices at once
	void update();
    
	// --- for portamento
	double m_dLastNoteFrequency;
    
	// --- our receive channel
	UINT m_uMidiRxChannel;
};







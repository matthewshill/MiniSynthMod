//
//  AUSynthStructures.h
//
//  Created by Matt Hill
//  Adapted from the MiniSynth diagram from Will Pirkle
//
/*
 
 */

#ifndef MiniSynth_AUSynthStructures_h
#define MiniSynth_AUSynthStructures_h

// --- structure for holding MIDI Messages
typedef struct MIDIMessageInfoStruct 
{
	UInt8	status;
	UInt8	channel;
	UInt8	data1;
	UInt8	data2;
	UInt32	startFrame;
} MIDIMessageInfoStruct;



#endif

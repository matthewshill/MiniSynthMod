//
//  AUSynth.r // for back compatibility with Logic 9
//
//  Created by Matt Hill
//  Adapted from the MiniSynth diagram from Will Pirkle
//
/*
 
 */

#include <AudioUnit/AudioUnit.r>

#include "AUSynthVersion.h"

// Note that resource IDs must be spaced 2 apart for the 'STR ' name and description
#define kAudioUnitResID_Synth				1000

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Filter~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define RES_ID			kAudioUnitResID_Synth
#define COMP_TYPE		'aumu'
#define COMP_SUBTYPE	'MS04'
#define COMP_MANUF		'SWIL'

#define VERSION			kAUSynthVersion
#define NAME			"Matt Hill: MiniSynthMod" // NOTE this needs to be in the format "<COMP_MANUF>: <Custom Name>"
#define DESCRIPTION		"Synth AU"
#define ENTRY_POINT		"AUSynthEntry"

#include "AUResources.r"


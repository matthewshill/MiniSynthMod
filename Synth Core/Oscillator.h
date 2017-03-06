#pragma once
#include "pluginconstants.h"
#include "synthfunctions.h"
#include "ModulationMatrix.h"

#define OSC_FO_MOD_RANGE 2			//2 semitone default
#define OSC_HARD_SYNC_RATIO_RANGE 4	//
#define OSC_PITCHBEND_MOD_RANGE 12	//12 semitone default
#define OSC_FO_MIN 20				//20Hz
#define OSC_FO_MAX 20480			//20.480kHz = 10 octaves up from 20Hz
#define OSC_FO_DEFAULT 440.0		//A5
#define OSC_PULSEWIDTH_MIN 2		//2%
#define OSC_PULSEWIDTH_MAX 98		//98%
#define OSC_PULSEWIDTH_DEFAULT 50	//50%

class COscillator
{
public:
	COscillator(void);
	virtual ~COscillator(void);	

	// --- ATTRIBUTES
	// --- PUBLIC: these variables may be get/set 
	//             you may make get/set functions for them 
	//             if you like, but will add function call layer
	//
	// --- Modulation Matrix --------------------
	//
	// --- the shared modulation matrix
	CModulationMatrix* m_pModulationMatrix;	// the matrix
	
	// --- sources that we read from
	//	   indexes in m_pModulationMatrix->Sources[]
	UINT m_uModSourceFo; 
	UINT m_uModSourcePulseWidth; 
	UINT m_uModSourceAmp;

	// --- destinations that we write to
	//	   indexes in m_pModulationMatrix->Destinations[]
	UINT m_uModDestOutput1;
	UINT m_uModDestOutput2;
	// ------------------------------------------
	
	// --- Global Parameters --------------------
	//
	globalOscillatorParams* m_pGlobalOscParams;
	// ------------------------------------------
	
	// --- oscillator run flag
	bool m_bNoteOn;

	// --- user controls or MIDI 
	double m_dOscFo;		// oscillator frequency from MIDI note number
	double m_dFoRatio;	    // FM Synth Modulator OR Hard Sync ratio 
	double m_dAmplitude;	// 0->1 from GUI

	// --- modulo counter and inc for timebase
	double m_dModulo;		// modulo counter 0->1
	double m_dInc;			// phase inc = fo/fs

	// --- more pitch mods
	int m_nOctave;			// octave tweak
	int m_nSemitones;		// semitones tweak
	int m_nCents;			// cents tweak
	
	// ---  pulse width in % (sqr only) from GUI
	double m_dPulseWidthControl;	
	
	// --- for PITCHED Oscillators
	enum {SINE,SAW1,SAW2,SAW3,TRI,SQUARE,NOISE,PNOISE};
	UINT m_uWaveform;	// to store type
	
	// --- for LFOs
	enum {sine,usaw,dsaw,tri,square,expo,rsh,qrsh};

	// --- for LFOs - MODE
	enum {sync,shot,free};
	UINT m_uLFOMode;	// to store MODE
	
	// --- for hard sync or other dual-oscillator ideas
	COscillator* m_pBuddyOscillator;

	// --- flag indicating we are a master oscillator
	bool m_bMasterOsc;
	
	// --- MIDI note that is being played
	UINT m_uMIDINoteNumber;
	
protected:
	// --- PROTECTED: generally these are either basic calc variables
	//                and modulation stuff
	// --- calculation variables
	double m_dSampleRate;	// fs
	double m_dFo;			// current (actual) frequency of oscillator	
	double m_dPulseWidth;	// pulse width in % for calculation
	bool m_bSquareEdgeRising; // hysteresis for square edge
	
	// --- for noise and random sample/hold
	UINT   m_uPNRegister;	// for PN Noise sequence
	int    m_nRSHCounter;	// random sample/hold counter
	double m_dRSHValue;		// currnet rsh output
	
	// --- for DPW
	double m_dDPWSquareModulator;	// square toggle
	double m_dDPW_z1; // memory register for differentiator
	
	// --- mondulation inputs
	double m_dFoMod;			/* modulation input -1 to +1 */
	double m_dPitchBendMod;	    /* modulation input -1 to +1 */
	double m_dFoModLin;			/* FM modulation input -1 to +1 (not actually used in Yamaha FM!) */
	double m_dPhaseMod;			/* Phase modulation input -1 to +1 (used for DX synth) */
	double m_dPWMod;			/* modulation input for PWM -1 to +1 */
	double m_dAmpMod;			/* output amplitude modulation for AM 0 to +1 (not dB)*/
		
public:
	// --- FUNCTIONS: all public
	//
	// --- modulo functions for master/slave operation
	// --- increment the modulo counters
	inline void incModulo(){m_dModulo += m_dInc;}

	// --- check and wrap the modulo
	//     returns true if modulo wrapped
	inline bool checkWrapModulo()
	{	
		// --- for positive frequencies
		if(m_dInc > 0 && m_dModulo >= 1.0) 
		{
			m_dModulo -= 1.0; 
			return true;
		}
		// --- for negative frequencies
		if(m_dInc < 0 && m_dModulo <= 0.0) 
		{
			m_dModulo += 1.0; 
			return true;
		}
		return false;
	}
	
	// --- reset the modulo (required for master->slave operations)
	inline void resetModulo(double d = 0.0){m_dModulo = d;}

	// --- modulation functions - NOT needed/used if you implement the Modulation Matrix!
	//
	// --- output amplitude modulation (AM, not tremolo (dB); 0->1, NOT dB
	inline void setAmplitudeMod(double dAmp){m_dAmpMod = dAmp;}

	// --- modulation, exponential
	inline void setFoModExp(double dMod){m_dFoMod = dMod;}
	inline void setPitchBendMod(double dMod){m_dPitchBendMod = dMod;}
	
	// --- for FM only (not used in Yamaha or my DX synths!)
	inline void setFoModLin(double dMod){m_dFoModLin = dMod;}

	// --- for Yamaha and my DX Synth
	inline void setPhaseMod(double dMod){m_dPhaseMod = dMod;}

	// --- PWM for square waves only
	inline void setPWMod(double dMod){m_dPWMod = dMod;}

	// --- VIRTUAL FUNCTIONS ----------------------------------------- //
	//
	// --- PURE ABSTRACT: derived class MUST implement
	// --- start/stop control
	virtual void startOscillator() = 0;
	virtual void stopOscillator() = 0;
	
	// --- render a sample
	//		for LFO:	 pAuxOutput = QuadPhaseOutput
	//			Pitched: pAuxOutput = Right channel (return value is left Channel
	virtual double doOscillate(double* pAuxOutput = NULL) = 0; 

	// --- ABSTRACT: derived class overrides if needed
	virtual void setSampleRate(double dFs){m_dSampleRate = dFs;}
	
	// --- reset counters, etc...
	virtual void reset();

	// INLINE FUNCTIONS: these are inlined because they will be 
	//                   called every sample period
	//					 You may want to move them to the .cpp file and
	//                   enable the compiler Optimization setting for 
	//					 Inline Function Expansion: Any Suitable though
	//					 inlining here forces it.
	//
	// --- init the global params
	inline void initGlobalParameters(globalOscillatorParams* pGlobalOscParams)
	{
		m_pGlobalOscParams = pGlobalOscParams;

		m_pGlobalOscParams->dOscFo = -1.0;	
		m_pGlobalOscParams->dFoRatio = m_dFoRatio;
		m_pGlobalOscParams->dAmplitude = m_dAmplitude;
		m_pGlobalOscParams->dPulseWidthControl = m_dPulseWidthControl;
		m_pGlobalOscParams->nOctave = m_nOctave;
		m_pGlobalOscParams->nSemitones = m_nSemitones;
		m_pGlobalOscParams->nCents = m_nCents;
		m_pGlobalOscParams->uWaveform = m_uWaveform;
		m_pGlobalOscParams->uLFOMode = m_uLFOMode;
	}

	// --- update the frequency, amp mod and PWM
	inline virtual void update()
	{		
		// --- Global Parameters
		if(m_pGlobalOscParams)
		{
			// --- if -1, no osc mod (for pitched, MIDI note controlled osc)
			if(m_pGlobalOscParams->dOscFo >= 0)
				m_dOscFo = m_pGlobalOscParams->dOscFo;
			
			m_dFoRatio = m_pGlobalOscParams->dFoRatio;
			m_dAmplitude = m_pGlobalOscParams->dAmplitude;
			m_dPulseWidthControl = m_pGlobalOscParams->dPulseWidthControl;
			m_nOctave = m_pGlobalOscParams->nOctave;
			m_nSemitones = m_pGlobalOscParams->nSemitones;
			m_nCents = m_pGlobalOscParams->nCents;
			m_uWaveform = m_pGlobalOscParams->uWaveform;
			m_uLFOMode = m_pGlobalOscParams->uLFOMode;				
		}
		
		// --- ignore LFO mode for noise sources
		if(m_uWaveform == rsh || m_uWaveform == qrsh)
		   m_uLFOMode = free;

		// --- Modulation Matrix
		// 
		// --- get from matrix Sources
		if(m_pModulationMatrix)
		{
			// --- zero is norm for these
			m_dFoMod = m_pModulationMatrix->m_dDestinations[m_uModSourceFo];
			m_dPWMod = m_pModulationMatrix->m_dDestinations[m_uModSourcePulseWidth];

			// --- amp mod is 0->1
			// --- invert for oscillator output mod
			m_dAmpMod = m_pModulationMatrix->m_dDestinations[m_uModSourceAmp];
			m_dAmpMod = 1.0 - m_dAmpMod;
		}

		// --- do the  complete frequency mod
		m_dFo = m_dOscFo*m_dFoRatio*pitchShiftMultiplier(m_dFoMod + 
													     m_dPitchBendMod + 
													     m_nOctave*12.0 + 
													     m_nSemitones + 
													     m_nCents/100.0);
		// --- apply linear FM (not used in book projects)
		m_dFo += m_dFoModLin;

		// --- bound Fo (can go outside for FM/PM mod)
		//     +/- 20480 for FM/PM
		if(m_dFo > OSC_FO_MAX)
			m_dFo = OSC_FO_MAX;
		if(m_dFo < -OSC_FO_MAX)
			m_dFo = -OSC_FO_MAX;

		// --- calculate increment (a.k.a. phase a.k.a. phaseIncrement, etc...)
		m_dInc = m_dFo/m_dSampleRate;

		// --- Pulse Width Modulation --- //
		// --- limits are 2% and 98%
		m_dPulseWidth = m_dPulseWidthControl + m_dPWMod*(OSC_PULSEWIDTH_MAX - OSC_PULSEWIDTH_MIN)/OSC_PULSEWIDTH_MIN; 

		// --- bound the PWM to the range
		m_dPulseWidth = fmin(m_dPulseWidth, OSC_PULSEWIDTH_MAX);
		m_dPulseWidth = fmax(m_dPulseWidth, OSC_PULSEWIDTH_MIN);
	}
};

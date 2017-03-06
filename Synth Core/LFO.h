#pragma once
#include "oscillator.h"

class CLFO : public COscillator
{
public:
	CLFO(void);
	~CLFO(void);
	
	// virtual overrides
	virtual void reset();
	virtual void startOscillator();
	virtual void stopOscillator();

	inline virtual double doOscillate(double* pQuadPhaseOutput = NULL)
	{
		if(!m_bNoteOn)
		{
			if(pQuadPhaseOutput)
				*pQuadPhaseOutput = 0.0;

			return 0.0;
		}

		// output
		double dOut = 0.0;
		double dQPOut = 0.0;
			
		// always first
		bool bWrap =  checkWrapModulo();

		// one shot LFO? 
		if(m_uLFOMode == shot && bWrap)
		{
			m_bNoteOn = false;
					
			if(pQuadPhaseOutput)
				*pQuadPhaseOutput = 0.0;

			return 0.0;
		}

		// for QP output
		// advance modulo by 0.25 = 90 degrees
		double dQuadModulo = m_dModulo + 0.25;

		// check and wrap
		if(dQuadModulo >= 1.0)
			dQuadModulo -= 1.0;

		// decode and calculate
		switch(m_uWaveform)
		{
			case sine:
			{
				// calculate angle
				double dAngle = m_dModulo*2.0*pi - pi;

				// call the parabolicSine approximator
				dOut = parabolicSine(-dAngle);	

				// use second modulo for quad phase
                dAngle = dQuadModulo*2.0*pi - pi;
                dQPOut = parabolicSine(-dAngle);
                
				break;
			}

			case usaw:
			case dsaw:
			{	
				// --- one shot is unipolar for saw
				if(m_uLFOMode != shot)
				{
					// unipolar to bipolar
					dOut = unipolarToBipolar(m_dModulo);
					dQPOut = unipolarToBipolar(dQuadModulo);
				}
				else
				{
					dOut = m_dModulo - 1.0;
					dQPOut = dQuadModulo - 1.0;
				}

				// invert for downsaw
				if(m_uWaveform == dsaw)
				{
					dOut *= -1.0;
                    dQPOut *= -1.0;
				}

				break;
			}

			case square:
			{
				// check pulse width and output either +1 or -1
				dOut = m_dModulo > m_dPulseWidth/100.0 ? -1.0 : +1.0;
                dQPOut = dQuadModulo > m_dPulseWidth/100.0 ? -1.0 : +1.0;

				break;
			}

			case tri:
			{
				if(m_dModulo < 0.5)		// out  = 0 -> +1
					dOut = 2.0*m_dModulo;
				else					// out = +1 -> 0
					dOut = 1.0 - 2.0*(m_dModulo - 0.5);

				if(m_uLFOMode != shot)
					// unipolar to bipolar
					dOut = unipolarToBipolar(dOut);

                if(dQuadModulo < 0.5)	// out  = 0 -> +1
                    dQPOut = 2.0*dQuadModulo;
                else					// out = +1 -> 0
                    dQPOut = 1.0 - 2.0*(dQuadModulo - 0.5);

                // unipolar to bipolar
                dQPOut = unipolarToBipolar(dQPOut);

				break;
			}
			
			// --- expo is unipolar!
			case expo:
			{
				// calculate the output directly
				dOut = concaveInvertedTransform(m_dModulo);
				dQPOut = concaveInvertedTransform(dQuadModulo);

				break;
			}
			
			case rsh:
			case qrsh:
			{
				// this is the very first run
				if(m_nRSHCounter < 0)
				{
					if(m_uWaveform == rsh)
						m_dRSHValue = doWhiteNoise();
					else
						m_dRSHValue = doPNSequence(m_uPNRegister);

					m_nRSHCounter = 1.0;
				}
				// hold time exceeded?
				else if(m_nRSHCounter > m_dSampleRate/m_dFo)
				{
					m_nRSHCounter -= m_dSampleRate/m_dFo;
					
					if(m_uWaveform == rsh)
						m_dRSHValue = doWhiteNoise();
					else
						m_dRSHValue = doPNSequence(m_uPNRegister);
				}

				// inc the counter
				m_nRSHCounter += 1.0;

				// output held value
				dOut = m_dRSHValue;

				// not meaningful for this output
				dQPOut = m_dRSHValue;
				break;
			}

			default:
				break;
		}

		// ok to inc modulo now
		incModulo();

		// m_dAmplitude & m_dAmpMod is calculated in update() on base class
		if(m_pModulationMatrix)
		{
			// write our outputs into their destinations
			m_pModulationMatrix->m_dSources[m_uModDestOutput1] = dOut*m_dAmplitude*m_dAmpMod;
			
			// add quad phase/stereo output
			m_pModulationMatrix->m_dSources[m_uModDestOutput2] = dQPOut*m_dAmplitude*m_dAmpMod;
		}
        
        if(pQuadPhaseOutput)
            *pQuadPhaseOutput = dQPOut*m_dAmplitude*m_dAmpMod;

		// m_dAmplitude & m_dAmpMod is calculated in update() on base class
		return dOut*m_dAmplitude*m_dAmpMod;
	}
};

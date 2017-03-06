#include "SEMFilter.h"

CSEMFilter::CSEMFilter(void)
{
	// --- init
	m_dAlpha0 = 1.0;
	m_dAlpha = 1.0;
	m_dRho = 1.0;
	m_dAuxControl = 0.5; // for BSF this centers it

	//--- our default filter type
	m_uFilterType = LPF2;

	// --- flush registers
	reset();
}

CSEMFilter::~CSEMFilter(void)
{
}

// decode the Q value; Q on UI is 1->10
void CSEMFilter::setQControl(double dQControl)
{
	//this maps dQControl = 1->10 to Q = 0.5->25
	m_dQ = (25.0 - 0.5)*(dQControl - 1.0)/(10.0 - 1.0) + 0.5;
}

// recalc the coeffs
void CSEMFilter::update()
{
	// base class does modulation
	CFilter::update();

	// prewarp the cutoff- these are bilinear-transform filters
	double wd = 2*pi*m_dFc;
	double T  = 1/m_dSampleRate;
	double wa = (2/T)*tan(wd*T/2);
	double g  = wa*T/2;

	// note R is the traditional analog damping factor
	double R = 1.0/(2.0*m_dQ);
	if(m_dQ == 25.0) R = 0.0; // optional self-oscillatoory

	// set the coeffs
	m_dAlpha0 = 1.0/(1.0 + 2.0*R*g + g*g);
	m_dAlpha = g;
	m_dRho = 2.0*R + g;
}

// do the filter
double CSEMFilter::doFilter(double xn)
{
	// return xn if filter not supported
	if(m_uFilterType != LPF2 && m_uFilterType != HPF2 &&
	   m_uFilterType != BPF2 && m_uFilterType != BSF2)
		return xn;

	// form the HP output first
	double hpf = m_dAlpha0*(xn - m_dRho*m_dZ11 - m_dZ12);

	// BPF Out
	double bpf = m_dAlpha*hpf + m_dZ11;

	// for nonlinear proc
	if(m_uNLP == ON)
		bpf = tanh(m_dSaturation*bpf);

	// LPF Out
	double lpf = m_dAlpha*bpf + m_dZ12;

	// note R is the traditional analog damping factor
	double R = 1.0/(2.0*m_dQ);

	// BSF Out
	double bsf = xn - 2.0*R*bpf;

	// SEM BPF Output
	// using m_dAuxControl for this one-off control
	double semBSF = m_dAuxControl*hpf + (1.0 - m_dAuxControl)*lpf;

	// update memory
	m_dZ11 = m_dAlpha*hpf + bpf;
	m_dZ12 = m_dAlpha*bpf + lpf;

	// return our selected type
	if(m_uFilterType == LPF2)
		return lpf;
	else if(m_uFilterType == HPF2)
		return hpf;
	else if(m_uFilterType == BPF2)
		return bpf;
	else if(m_uFilterType == BSF2)
		return semBSF;

	// return input if filter not supported
	return xn;
}
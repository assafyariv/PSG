#include "pch.h"
#include <nvmath/Trafo.h>


namespace nvmath {
	Trafo::Trafo () {}
	void Trafo::setMatrix (const Mat44f &m) { m_matrix = m;}
	const Mat44f &Trafo::getMatrix () const { return m_matrix ;}
	Trafo &Trafo::operator= (const Trafo &tra) { m_matrix = tra.getMatrix(); return *this; }
	Trafo::Trafo (const Trafo &tra) { m_matrix = tra.getMatrix(); }
	bool Trafo::operator== (const Trafo &t) const { return (m_matrix == t.m_matrix); }
	Mat44f Trafo::getInverse () const { Mat44f ivMat = m_matrix; ivMat.invert (); return ivMat;}
	void Trafo::decompose () const {}
	Trafo  lerp(float t,Trafo const &tr1,Trafo const &tr2) 
	{
		return tr1;
	}
	 void decompose( const Matnnt<3,float> &m
                         , Quatt<float> &orientation, Vecnt<3,float> &scaling
						 , Quatt<float> &scaleOrientation ){}
}
#include "pch.h"
#include <nvsg/VertexAttributeSet.h>
#include "D3DDal.h"
#include "D3DBase.h"
using namespace nvd3d;

namespace nvsg {
	VertexAttributeSet::VertexAttributeSet ()
	{
		m_objectCode = nvsg::OC_VERTEX_ATTRIBUTE_SET;
	}
	VertexAttributeSet::~VertexAttributeSet () 
	{
	}
	VertexAttributeSet::VertexAttributeSet (VertexAttributeSet const &rh)
	{
	}
	VertexAttributeSetSharedPtr VertexAttributeSet::create ()
	{
		return VertexAttributeSetSharedPtr (VertexAttributeSetHandle::create());	
	}
	void
		VertexAttributeSet::initReflectionInfo ()
	{
	}
	void VertexAttributeSet::notifyChange (const nvutil::Subject *sub, unsigned int ui) const {}
	bool VertexAttributeSet::isEquivalent (const Object *x,bool ise,bool isb) const { return false;}
	void VertexAttributeSet::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}
	bool VertexAttributeSet::isDataShared () const
	{
		return false;
	}
	nvsg::DataID VertexAttributeSet::getDataID () const
	{
		return 0;
	}
	unsigned int VertexAttributeSet::getSizeOfVertexData (unsigned int x) const
	{
		return 0;
	}
	unsigned int VertexAttributeSet::getTypeOfVertexData (unsigned int x) const
	{
		return 0;
	}
	unsigned int VertexAttributeSet::getNumberOfVertexData (unsigned int x) const
	{
		return 0;
	}
	unsigned int VertexAttributeSet::getStrideOfVertexData (unsigned int x) const
	{
		return 0;
	}
	unsigned int VertexAttributeSet::getOffsetOfVertexData (unsigned int x) const
	{
		return 0;
	}
	void VertexAttributeSet::removeVertexData (unsigned int id)
	{
	}
	void VertexAttributeSet::swapVertexData (unsigned int id, VertexAttribute &vas)
	{
	}
	void VertexAttributeSet::reserveVertexData (unsigned int id, unsigned int idd,unsigned int s,unsigned int w)
	{
	}
	Buffer::DataReadLock VertexAttributeSet::getVertexData(unsigned int attrib) const
	{
		return Buffer::DataReadLock ();
	}
	const BufferSharedPtr & VertexAttributeSet::getVertexBuffer(unsigned int attrib) const
	{
		return BufferSharedPtr ();
	}
	bool VertexAttributeSet::isContiguousVertexData (unsigned int attrib) const
	{
		return true;
	}

	void VertexAttributeSet::setVertexData( unsigned int attrib, unsigned int size, unsigned int type
		, const void * data, unsigned int strideInBytes, unsigned int count )

	{
		D3DDalHostSharedPtr  d3dHost = nvutil::sharedPtr_cast<D3DDalHost> ( getDALHost () );
		const nvmath::Vec3f *vecarr = (const nvmath::Vec3f *)data;
		//const unsigned int cc = count;
		int vecpi = 0;
		VertexPositionColor *cubeVertices = new VertexPositionColor [ count ];
		for (size_t i=0; i<count; i++)
		{
			cubeVertices[i].pos = DirectX::XMFLOAT3(vecarr[vecpi][0],vecarr[vecpi][1],vecarr[vecpi][2]);
			vecpi ++;
			cubeVertices[i].color = DirectX::XMFLOAT3(vecarr[vecpi][0],vecarr[vecpi][1],vecarr[vecpi][2]);
			vecpi ++;
		}
		DALData *dalData  = GetDataCreator ().createVertexBuffer (/*size*/ sizeof (VertexPositionColor)*count,type,cubeVertices,strideInBytes,count);

		D3DDalHostWriteLock (d3dHost)->storeDeviceAbstractionLinkData (DALData::DT_DALDATA,dalData );
		delete [] cubeVertices;
	}
	void VertexAttributeSet::setVertexData( unsigned int attrib, unsigned int pos, unsigned int size
		, unsigned int type, const void * data, unsigned int strideInBytes
		, unsigned int count )

	{
	}
	void VertexAttributeSet::setVertexData( unsigned int attrib, unsigned int size, unsigned int type
		, const BufferSharedPtr &buffer, unsigned int offset
		, unsigned int strideInBytes, unsigned int count )
	{
	}

	void VertexAttributeSet::setVertexData( unsigned int attrib, const unsigned int * to
		, const unsigned int * from, unsigned int size
		, unsigned int type, const void * data, unsigned int stride, unsigned int count )
	{
	}
}

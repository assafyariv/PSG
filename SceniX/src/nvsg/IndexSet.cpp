#include "pch.h"
#include <nvsg/IndexSet.h>
#include "D3DDal.h"

namespace nvsg {
IndexSet::IndexSet ()
	{
	}
	IndexSet::IndexSet (const IndexSet &c)
	{
	}
	IndexSet::~IndexSet ()
	{
	}
	IndexSetSharedPtr
		IndexSet::create ()
	{
		return IndexSetSharedPtr (IndexSetHandle::create());
	}
	void
		IndexSet::initReflectionInfo ()
	{
	}
	void IndexSet::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}
	void IndexSet::notifyChange (const nvutil::Subject  *sub, unsigned int x) const
	{
	}
	void IndexSet::setData (const unsigned int   * indices, unsigned int count, unsigned int primitiveRestartIndex )
	{
		D3DDalHostSharedPtr  d3dHost = nvutil::sharedPtr_cast<D3DDalHost> ( getDALHost () );
		DALData *dalData  = GetDataCreator ().createIndexBuffer (count,indices,D3DVertexBufferData::D3D_DATA_SIZE_UINT_32);
		D3DDalHostWriteLock (d3dHost)->storeDeviceAbstractionLinkData (DALData::DT_DALDATA,dalData);
	}
	void IndexSet::setData (const unsigned short   * indices, unsigned int count, unsigned int primitiveRestartIndex )
	{
		D3DDalHostSharedPtr  d3dHost = nvutil::sharedPtr_cast<D3DDalHost> ( getDALHost () );
		DALData *dalData  = GetDataCreator ().createIndexBuffer (count,indices,D3DVertexBufferData::D3D_DATA_SIZE_UINT_16);
		D3DDalHostWriteLock (d3dHost)->storeDeviceAbstractionLinkData (DALData::DT_DALDATA,dalData );
	}
}
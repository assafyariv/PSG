#pragma once
#include <nvsg/DAL.h>
namespace nvsg{
	class D3DDalHost;
	CORE_TYPES (D3DDalHost,DALHost);
	class D3DDataCreator;
	D3DDataCreator  &GetDataCreator () ;
}
namespace nvutil {
	OBJECT_TRAITS( nvsg::D3DDalHost, nvsg::DALHost );
}
namespace nvsg {

	class D3DVertexBufferData : public DALData
	{
	public:
		enum D3DDataSize {
			D3D_DATA_SIZE_UINT_16 = 0,
			D3D_DATA_SIZE_UINT_32 = 1
		};
		bool CreateVertexBuffer (unsigned int size, unsigned int type
                                         , const void * data, unsigned int strideInBytes, unsigned int count)
		;
		bool CreateIndexBuffer (unsigned int size, const void * data, const D3DDataSize &)
		;
		static bool test_func (D3DVertexBufferData *data) ;
		ID3D11Buffer *GetBuffer () { return m_d3dBuffer; }
		unsigned int GetCount () { return m_count; }
		unsigned int GetFormat () 
		{
			switch (m_type)
			{
				case D3D_DATA_SIZE_UINT_16:
								return DXGI_FORMAT_R16_UINT;
				case D3D_DATA_SIZE_UINT_32:
								return DXGI_FORMAT_R32_UINT;
			}
			return 0;
		}
	protected:
		friend class D3DDataCreator;
		D3DVertexBufferData (DALDataCreator *x) : DALData (x) {};
		ID3D11Buffer *m_d3dBuffer;
		unsigned int m_count;
		D3DDataSize  m_type;
	};

class D3DDalHost : public DALHost
	{
		friend  nvutil::Holder<D3DDalHost>;
	protected:
		D3DDalHost ();
		~D3DDalHost () ;
	public:
		static D3DDalHostSharedPtr create ();
	};


	class D3DDataCreator : public DALDataCreator
	{
	public:
		DALData * createVertexBuffer (unsigned int size, unsigned int type
			, const void * data, unsigned int strideInBytes, unsigned int count)
		{
			D3DVertexBufferData * ret = new D3DVertexBufferData (this);
			ret->CreateVertexBuffer (size,type,data,strideInBytes,count);	
			return (DALData*)ret;
		}
		DALData * createIndexBuffer (unsigned int size,
			const void * data, const D3DVertexBufferData::D3DDataSize &type)
		{
			D3DVertexBufferData * ret = new D3DVertexBufferData (this);
			ret->CreateIndexBuffer (size,data,type);	
			return (DALData*)ret;
		}
	};
}


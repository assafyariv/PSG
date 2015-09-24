// dllmain.h : Declaration of module class.

class CAvocadoHandlersModule : public ATL::CAtlDllModuleT< CAvocadoHandlersModule >
{
public :
	DECLARE_LIBID(LIBID_AvocadoHandlersLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_AVOCADOHANDLERS, "{3E250C57-CF3E-4292-AD50-5B1BCE6C6EE6}")
};

extern class CAvocadoHandlersModule _AtlModule;

#include "pch.h"
#include "nvsg/DAL.h"

namespace nvsg {
	
	DALServer::DALServer ()
	{
		m_hDALNext = 0;
	}
	DALServer::~DALServer ()
	{
	}
	HDAL
		DALServer::storeDALData (HDAL hDAL, unsigned int dataID, DALData* data)
	{
		m_lock.lockExclusive ();
		if (hDAL == HDAL_INVALID)
		{
			std::multimap<unsigned int, DALData *> map;
			std::pair<unsigned int, DALData *> pair (dataID,data);
			map.insert (pair);
			std::pair<HDAL,std::multimap<unsigned int,DALData *>> np (m_hDALNext,map);
			m_DALMap.insert (np);
			nvsg::HDAL ret = m_hDALNext;
			m_hDALNext ++;
			return ret;
		}
		std::map<HDAL, std::multimap<unsigned int , DALData *>>::iterator it = m_DALMap.find (hDAL);
		if (it == m_DALMap.end())
		{
			// assert here
		}
		std::multimap<unsigned int,DALData*> mulmap =(*it).second;
		mulmap.insert (std::pair<unsigned int, DALData*>(dataID,data));
		m_lock.unlockExclusive ();
		return hDAL;
	}

	bool
		DALServer::getDALData (HDAL hDAL, unsigned int dataID,std::vector<DALData*>&  data) const
	{
		m_lock.lockShared ();
		if (hDAL == HDAL_INVALID)
		{
			return false;
		}
		std::map<HDAL, std::multimap<unsigned int , DALData *>>::const_iterator it= m_DALMap.find(hDAL); // = m_DALMap.find (hDAL);
		if (it == m_DALMap.end())
		{
			return false;
		}

		std::multimap<unsigned int,DALData*> mulmap =(*it).second;
		std::multimap<unsigned int,DALData*>::const_iterator ittt = mulmap.find (dataID);
		if (ittt == mulmap.end())
		{
			return false;
		}
		data.push_back (ittt->second);
		m_lock.unlockShared ();
		return true;
	}
	
	void DALServer::releaseDALData(HDAL hDAL, DALDataCreator * creator, unsigned int dataID) {}
 
	void DALServer::deleteDALData(DALData * data) {}
	const HDAL HDAL_INVALID = 0xfffffff;
}
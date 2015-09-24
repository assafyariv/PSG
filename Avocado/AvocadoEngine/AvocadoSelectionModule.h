#pragma once
#include "AvocadoModuleInterface.h"
#include "AvocadoEngineObject.h"

namespace avocado 
{
    /*class AvocadoEngineDocFileElement: public AvocadoEngineDocElement
	{
	public:
		AvocadoEngineDocFileElement(int docId,string ownerModuleName , string fileName);
		~AvocadoEngineDocFileElement();
		void	ResetScene();
		virtual void createScene(SceneSharedPtr &scene);
	private:
		std::string					m_fileName;
		SceneSharedPtr				m_scene;
		int							m_docId;
	};*/

	class SelectionItem
	{
	public:
		SelectionItem(int eid,int vid, string owner) {m_owner = owner;m_elementId=eid;m_viewId=vid;};
		~SelectionItem() {};

		std::string m_owner;
		int m_elementId;
		int m_viewId;
	};

	class AvocadoSelection : public AvocadoDocModule 
	{
	public:
		AvocadoSelection ();
		virtual ~AvocadoSelection ();
		virtual AvocadoDocModule* create ();

		bool OnUnload ();
		virtual bool OnRegister ();
		////int AddDocFileElement(std::string filename);
	//	bool DeleteDocFileElement(int elemId);
		//void ClearDocFileElements();
		//std::vector<AvocadoEngineDocElement *>::iterator GetDocFileElemById(int elemId);

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint){ return false; }		// unused
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId,const std::string &paramStr, bool &needRepaint){ return false; }	// unused
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId,const std::string &paramStr, bool &needRepaint) ;

		bool InSelectionList (int eid,int vid)
		{
			for (size_t i=0;i < m_selectionList.size(); i++)
			{
				if (m_selectionList[i].m_elementId==eid && m_selectionList[i].m_viewId==vid)
					return true;
			}
			return false;
		}
		void AddToSelectionList (int eid,int vid,std::string owner)
		{
			m_selectionList.push_back(SelectionItem(eid,vid,owner));
		}
		void RemoveFromSelectionList(int eid,int vid)
		{
			std::vector<SelectionItem>::iterator it = m_selectionList.begin();
			for (;it!=m_selectionList.end();it++)
				if ((*it).m_elementId == eid && (*it).m_viewId == vid)
				{
					m_selectionList.erase (it);
					break;
				}
		}
		std::string GetEidOwner(int eid)
		{
			std::vector<SelectionItem>::iterator it = m_selectionList.begin();
			for (;it!=m_selectionList.end();it++)
				if ((*it).m_elementId == eid /*&& (*it).m_viewId == vid*/)
				{
					return (*it).m_owner;
					break;
				}
			return "";
		}
		void ClearSelectionList(int vid)
		{
			bool try_again = true;
			while (try_again)
			{
				std::vector<SelectionItem>::iterator it = m_selectionList.begin();
				try_again = false;
				for (;it!=m_selectionList.end();it++)
				{
					if ((*it).m_viewId == vid)
					{
						m_selectionList.erase (it);
						try_again = true;
						break;
					}
				}
		   }
		}
		int SelectionCountPerView (int vid) 
		{
				int ret = 0;
				std::vector<SelectionItem>::iterator it = m_selectionList.begin();
				for (;it!=m_selectionList.end();it++)
				if ((*it).m_viewId == vid)
				{
				 ret++;
				}
				return ret;
		}
		int SubSelectionCountPerView (int vid) 
		{
				int ret = 0;
				std::vector<std::pair<nvsg::DrawableSharedPtr,int>>::iterator it = m_subSelectionList.begin();
				for (;it!=m_subSelectionList.end();it++)
				if ((*it).second == vid)
				{
				 ret++;
				}
				return ret;
		}
		int GetFirstSelectionInView(int vid)
		{
				int ret = -1;
				std::vector<SelectionItem>::iterator it = m_selectionList.begin();
				for (;it!=m_selectionList.end();it++)
				if ((*it).m_viewId == vid)
				{
					ret = (*it).m_elementId;
				}
				return ret;
		}
		int GetIndexSelectionInView(int vid,int idx)
		{
				int ret = -1;
				int cnt = 0;
				std::vector<SelectionItem>::iterator it = m_selectionList.begin();
				for (;it!=m_selectionList.end();it++)
				if ((*it).m_viewId == vid)
				{
					if (cnt == idx)
					{
						ret = (*it).m_elementId;
						break;
					}
					cnt++;
				}
				return ret;
		}
		void ClearSubSelectionList(int vid)
		{
			std::vector<std::pair<nvsg::DrawableSharedPtr,int>>::iterator it = m_subSelectionList.begin();
			for (;it!=m_subSelectionList.end();it++)
				if ((*it).second == vid)
				{
					m_subSelectionList.erase (it);
					break;
				}
		}
		nvsg::DrawableSharedPtr GetViewFirstSubSelection(int vid)
		{
			    nvsg::DrawableSharedPtr ret = 0;
				std::vector<std::pair< nvsg::DrawableSharedPtr,int>>::iterator it = m_subSelectionList.begin();
				for (;it!=m_subSelectionList.end();it++)
				if ((*it).second == vid)
				{
					ret = (*it).first;
				}
				return ret;
		}
		void AddToSubSelectionList (nvsg::DrawableSharedPtr sub,int vid)
		{
			m_subSelectionList.push_back(std::pair<nvsg::DrawableSharedPtr,int>(sub,vid));
		}
		//std::vector<AvocadoEngineDocElement *>	m_docFileElements;
		std::vector<SelectionItem>							m_selectionList;
		//std::vector<std::pair<std::string,unsigned int>>					m_owners;
		//std::vector<std::pair<int,nvsg::DrawableSharedPtr>> m_subSelectionList;
	    std::vector<std::pair<nvsg::DrawableSharedPtr,int>> m_subSelectionList;

		nvsg::DrawableSharedPtr					m_currentSubSlection;
		int		m_elwithSubID;
	};
}
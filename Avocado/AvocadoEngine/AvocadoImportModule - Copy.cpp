#pragma once
#include "AvocadoImportModule.h"
#include "SceneFunctions.h"

#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include <nvtraverser\SearchTraverser.h>
#include <nvsg/DepthAttribute.h>
#include <nvtraverser/UnifyTraverser.h>

namespace avocado
{
	AvocadoEngineDocFileElement::AvocadoEngineDocFileElement(int docId,string ownerModuleName, string fileName)
		:AvocadoEngineDocElement (docId, ownerModuleName)
	{
		m_fileName = fileName;
	}

	AvocadoEngineDocFileElement::~AvocadoEngineDocFileElement()
	{
		
	}
	string AvocadoEngineDocFileElement::serialize()
	{
		//string res = "filename=" + m_fileName + ";";
		ParamListSharedPtr ppl = ParamList::createNew();
		ppl->PushString ("fileName",m_fileName);
		ppl->PushInt ("MetaCount",this->m_intr.metaData.size ());
		for (size_t i = 0;i < m_intr.metaData.size ();i++)
		{
				char cc[30];
					itoa(i,cc,10);
			ppl->PushString ("metaVarName" + string (cc),m_intr.metaData[i].first);
			ppl->PushString ("metaVarData" + string (cc),m_intr.metaData[i].second);
		}
		return (ppl->SerializeList ());
		//return (m_fileName);
	}

	void AvocadoEngineDocFileElement::removeFromScene(SceneSharedPtr &scene)
	{
		
	}
	void AvocadoEngineDocFileElement::createFromElement (SceneSharedPtr &scene, AvocadoEngineDocFileElement *el)
	{
		//SceneSharedPtr tempScene ;
		//nvutil::loadScene (m_fileName,tempScene);
		// add the new scene root under the current view root node
		NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
		
			DrawableSharedPtr orgchild =  el->m_subDrawable;
			ObjectWeakPtr obj = DrawableReadLock(orgchild)->getOwner (DrawableReadLock(orgchild)->ownersBegin ());
			GeoNodeSharedPtr orgParent ((GeoNodeWeakPtr)obj);

			ObjectWeakPtr obj2 = GeoNodeReadLock(orgParent)->getOwner (GeoNodeReadLock(orgParent)->ownersBegin ());
			GroupSharedPtr orgRoot ((GroupWeakPtr)obj2);

			//NodeSharedPtr orgRoot = NodeReadLock (orgParent)->getOwner (NodeReadLock (orgParent)->ownersBegin ());
			//GeoNodeSharedPtr child = orgParent;
			GroupSharedPtr child = Group::create();
			GroupWriteLock (child)->addChild (orgParent);
		
		TransformSharedPtr elementRoot = Transform::create();
		nvmath::Mat44f idmat = nvmath::Mat44f(true);
		TransformWriteLock (elementRoot)->setMatrix(idmat);

		/**/
		/* This part will add a transform node , between element root and the file root , to adjust the 
		/* Center of rotation to the center of the bounding box */
		TransformSharedPtr chi = Transform::create();
		TransformWriteLock (chi)->addChild(child);
		TransformWriteLock (chi)->setTranslation (-GroupReadLock(child)->getBoundingBox().getCenter());
		/**/


		TransformWriteLock (elementRoot)->addChild(chi);
		//TransformWriteLock (elementRoot)->setCenter(GroupReadLock(child)->getBoundingBox().getCenter());
		//Scale to scene
		float sc = TransformWriteLock (elementRoot)->getBoundingSphere().getRadius();
		nvmath::Vec3f center = NodeWriteLock (child)->getBoundingBox().getCenter();//getCenter();
			sc = 100.0f / sc;
			TransformWriteLock (elementRoot)->setScaling(nvmath::Vec3f(sc,sc,sc));
		//	TransformWriteLock (elementRoot)->setTranslation(-center);

		// end scale to scene

		TransformWriteLock (elementRoot)->setUserData ((void*)this);
		TransformWriteLock (elementRoot)->setName ("AvocadoElement");
		TransformWriteLock (root)->addChild(elementRoot);
		m_elementRoot = elementRoot;
		
		GroupWriteLock(orgRoot)->removeChild (child);
#if 0
		/* TODO : MOVE THIS TO PIPELINE MODULE !!! */
		TransformWriteLock(elementRoot)->setTraversalMask (1);
		nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class nvsg::Drawable");
				st->setBaseClassSearch(true);
				st->apply( child );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator it;
				for(it=searchResults.begin(); it!=searchResults.end(); it++)
				{
					//DrawableWeakPtr node = static_cast<GeoNodeWeakPtr>(*it);
					DrawableSharedPtr node(dynamic_cast<DrawableWeakPtr>(*it));
				    int mask = 1;
					for (int viewId=0;viewId<8;viewId++)
					     mask |= (int(pow(2.0,2*viewId+4)));// >> 1);
					nvsg::DrawableWriteLock(node)->setTraversalMask (mask);

					{
						// Set apperance.
					/*	Drawable::OwnerIterator it = nvsg::DrawableWriteLock(node)->ownersBegin();
						ObjectWeakPtr obj = nvsg::DrawableWriteLock(node)->getOwner(it);
						GeoNodeSharedPtr geo ((GeoNodeWeakPtr)obj);
						GeoNode::StateSetIterator ss = GeoNodeWriteLock (geo)->findStateSet(DrawableSharedPtr(node));*/
					/*	DepthAttributeSharedPtr fs = DepthAttribute::create();
						DepthAttributeWriteLock (fs)->setDepthWritingEnabled(true);
						DepthAttributeWriteLock (fs)->setDepthTestingEnabled(true);
						DepthAttributeWriteLock (fs)->setDepthFunction(DF_LESS_OR_EQUAL);*/

					//	StateSetWriteLock (*ss)->addAttribute(fs);

//						StatePass::AttributeIterator itt = StateSetWriteLock (*ss)->beginAttributes();
					//	while (itt !=StateSetWriteLock (*ss)->endAttributes())
						{
						//	StateAttributeSharedPtr attr = *itt;
						//	itt.operator++();


						}


					}
					//node->setTraversalMask();
				}
#endif
	}
	void AvocadoEngineDocFileElement::createScene(SceneSharedPtr &scene) 
	{
		SceneSharedPtr tempScene ;
		nvutil::loadScene (m_fileName,tempScene);
		// add the new scene root under the current view root node
		NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
		NodeSharedPtr child = SceneWriteLock(tempScene)->getRootNode();
		

		TransformSharedPtr elementRoot = Transform::create();
		nvmath::Mat44f idmat = nvmath::Mat44f(true);
		TransformWriteLock (elementRoot)->setMatrix(idmat);

		/**/
		/* This part will add a transform node , between element root and the file root , to adjust the 
		/* Center of rotation to the center of the bounding box */
		TransformSharedPtr chi = Transform::create();
		TransformWriteLock (chi)->addChild(child);
		TransformWriteLock (chi)->setTranslation (-GroupReadLock(child)->getBoundingBox().getCenter());
		/**/


		TransformWriteLock (elementRoot)->addChild(chi);
		//TransformWriteLock (elementRoot)->setCenter(GroupReadLock(child)->getBoundingBox().getCenter());
		//Scale to scene
		float sc = TransformWriteLock (elementRoot)->getBoundingSphere().getRadius();
		nvmath::Vec3f center = NodeWriteLock (child)->getBoundingBox().getCenter();//getCenter();
			sc = 100.0f / sc;
			TransformWriteLock (elementRoot)->setScaling(nvmath::Vec3f(sc,sc,sc));
		//	TransformWriteLock (elementRoot)->setTranslation(-center);

		// end scale to scene

		TransformWriteLock (elementRoot)->setUserData ((void*)this);
		TransformWriteLock (elementRoot)->setName ("AvocadoElement");
		TransformWriteLock (root)->addChild(elementRoot);
		m_elementRoot = elementRoot;
		
		/* TODO : MOVE THIS TO PIPELINE MODULE !!! */
		TransformWriteLock(elementRoot)->setTraversalMask (1);
		nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class nvsg::Drawable");
				st->setBaseClassSearch(true);
				st->apply( child );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator it;
				for(it=searchResults.begin(); it!=searchResults.end(); it++)
				{
					//DrawableWeakPtr node = static_cast<GeoNodeWeakPtr>(*it);
					DrawableSharedPtr node(dynamic_cast<DrawableWeakPtr>(*it));
				    int mask = 1;
					for (int viewId=0;viewId<8;viewId++)
					     mask |= (int(pow(2.0,2*viewId+4)));// >> 1);
					nvsg::DrawableWriteLock(node)->setTraversalMask (mask);

					{
						// Set apperance.
					/*	Drawable::OwnerIterator it = nvsg::DrawableWriteLock(node)->ownersBegin();
						ObjectWeakPtr obj = nvsg::DrawableWriteLock(node)->getOwner(it);
						GeoNodeSharedPtr geo ((GeoNodeWeakPtr)obj);
						GeoNode::StateSetIterator ss = GeoNodeWriteLock (geo)->findStateSet(DrawableSharedPtr(node));*/
					/*	DepthAttributeSharedPtr fs = DepthAttribute::create();
						DepthAttributeWriteLock (fs)->setDepthWritingEnabled(true);
						DepthAttributeWriteLock (fs)->setDepthTestingEnabled(true);
						DepthAttributeWriteLock (fs)->setDepthFunction(DF_LESS_OR_EQUAL);*/

					//	StateSetWriteLock (*ss)->addAttribute(fs);

//						StatePass::AttributeIterator itt = StateSetWriteLock (*ss)->beginAttributes();
					//	while (itt !=StateSetWriteLock (*ss)->endAttributes())
						{
						//	StateAttributeSharedPtr attr = *itt;
						//	itt.operator++();


						}


					}
					//node->setTraversalMask();
				}

		
	}

	AvocadoImport::AvocadoImport (): AvocadoDocModule ("ImportModule")
	{
		
	}

	AvocadoImport::~AvocadoImport ()
	{
		ClearDocFileElements();
	}

	AvocadoDocModule* AvocadoImport::create ()
	{
		 return ( (AvocadoDocModule *) new AvocadoImport);
	}

	bool AvocadoImport::OnUnload()
	{
		return true;
	}

	bool AvocadoImport::OnRegister()
	{
		return true;
	}

	void AvocadoImport::ClearDocFileElements()
	{
		for (size_t i=0;i<m_docFileElements.size();i++)
		{
			delete m_docFileElements[i];
			m_docFileElements[i] = NULL;
		}
		m_docFileElements.clear ();
	}

	std::vector<AvocadoEngineDocFileElement *>::iterator AvocadoImport::GetDocFileElemById (int elemId)
	{
		std::vector<AvocadoEngineDocFileElement *>::iterator it = m_docFileElements.begin ();
		while (it != m_docFileElements.end ())
		{
			if ((*it)->GetID () == elemId)
			{
				return it;
			}
			it++;
		}
		return m_docFileElements.end ();
	}
	int AvocadoImport::AddDocInstancedElement(int eid,  AvocadoElementInterface::MetaDataList *metaData)
	{
		AvocadoEngineDocFileElement *docElem = new AvocadoEngineDocFileElement(m_docId,m_name,"");
		int id = (int)(m_smartIDGen->RequestUniqueID());
		std::stringstream idStr;
		idStr << id;
		std::string name = "docFileElem_" + idStr.str();
		docElem->SetID(id);
		docElem->SetName (name);
		if (metaData != NULL)
		{
			docElem->m_intr.metaData = (*metaData);
		}

		m_docFileElements.push_back (docElem);

		return id;
	}
	int AvocadoImport::AddDocFileElement(std::string filename,  AvocadoElementInterface::MetaDataList *metaData)
	{
#if 0
		int id = 0;
		AvocadoEngineDocFileElement *docElem = new AvocadoEngineDocFileElement(m_docId,m_name,filename);
		if (!m_docFileElements.empty())
		{
			std::vector<AvocadoEngineDocFileElement *>::iterator it = m_docFileElements.begin ();
			while (it != m_docFileElements.end ())
			{
				if ((*it)->GetID () == id)
				{
					id++;
					it = m_docFileElements.begin ();
					continue;
				}
				it++;
			}
		}
#endif
		AvocadoEngineDocFileElement *docElem = new AvocadoEngineDocFileElement(m_docId,m_name,filename);
		int id = (int)(m_smartIDGen->RequestUniqueID());
		std::stringstream idStr;
		idStr << id;
		std::string name = "docFileElem_" + idStr.str();
		docElem->SetID(id);
		docElem->SetName (name);
		if (metaData != NULL)
		{
			docElem->m_intr.metaData = (*metaData);
		}

		m_docFileElements.push_back (docElem);

		return id;
	}

	bool AvocadoImport::DeleteDocFileElement(int elemId)
	{
		std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById (elemId);

		delete *it;
		m_docFileElements.erase (it);
		return true;
	}
	void AvocadoImport::OnElementSelected (int eid,int vid,bool sub)
	{
		std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(eid);
		if (it == m_docFileElements.end())
			return;
		if (!sub)
			(*it)->m_subDrawable = 0;
		string msg = AvocadoEngineDocElement::GetParamsFromElement(*it);
	    OnSendAvocadoGeneralStringMessage("HightlightElement", vid, msg);
	}
	
	void AvocadoImport::OnElementUnSelected (int eid,int vid)
	{
		std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(eid);
		if (it == m_docFileElements.end())
			return;
		string msg = AvocadoEngineDocElement::GetParamsFromElement(*it);
	    OnSendAvocadoGeneralStringMessage("UnHightlightElement", vid, msg);
	}
	void AvocadoImport::OnElementPreSelected (int eid,int vid)
	{
		std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(eid);
		if (it == m_docFileElements.end())
			return;
		string msg = AvocadoEngineDocElement::GetParamsFromElement(*it);
	    OnSendAvocadoGeneralStringMessage("PreHightlightElement", vid, msg);
	}
	bool AvocadoImport::HandleAvocadoDocGeneralStringMessage (std::string msg, int docId, std::string paramStr, bool &needRepaint)
	{
		bool ret = false;

		if (msg == "AddDocFileElement")
		{
			vector <pair<string,string>> t_meta;
			ParamListSharedPtr i_ppl = ParamList::createFromString (paramStr);
			string fileName;
			StringParam *prm = (StringParam*)i_ppl->GetParam ("fileName");
			prm->GetValue (fileName);
			IntParam *irm = (IntParam*)i_ppl->GetParam ("MetaCount");
			int mc = 0;
			irm->GetValue (mc);
			if (mc > 0)
			{
				for (int j=0;j<mc;j++)
				{
					char cc[30];
					itoa(j,cc,10);
					StringParam *irm_vn = (StringParam*)i_ppl->GetParam ("metaVarName" + string (cc));
					StringParam *irm_vd = (StringParam*)i_ppl->GetParam ("metaVarData" + string (cc));
					string varName,varData;
					irm_vn->GetValue (varName);
					irm_vd->GetValue (varData);
					pair <string , string> newMeta (varName,varData);
					t_meta.push_back(newMeta);
				}
			}
			
			int elemId = AddDocFileElement(fileName/*paramStr*/,&t_meta);
			std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(elemId);
			AvocadoEngineDocFileElement *docElem = *it;
			docElem->createScene(m_scene);
			ParamListSharedPtr ppl = ParamList::createNew();
			//ppl->PushInt("eid",elemId);
			ParamListWriteLock(ppl)->PushPtr("elem",(void *)docElem);
			ret = OnSendAvocadoDocGeneralStringMessage("AddDocElement",docId,ParamListWriteLock(ppl)->SerializeList());
			
			needRepaint = true;
		}
		else if (msg == "AddDocInstancedElement")
		{
			vector <pair<string,string>> t_meta;
			ParamListSharedPtr i_ppl = ParamList::createFromString (paramStr);
			int eid;
			IntParam *prm = (IntParam*)i_ppl->GetParam ("eid");
			prm->GetValue (eid);

			IntParam *irm = (IntParam*)i_ppl->GetParam ("MetaCount");
			int mc = 0;
			irm->GetValue (mc);
			if (mc > 0)
			{
				for (int j=0;j<mc;j++)
				{
					char cc[30];
					itoa(j,cc,10);
					StringParam *irm_vn = (StringParam*)i_ppl->GetParam ("metaVarName" + string (cc));
					StringParam *irm_vd = (StringParam*)i_ppl->GetParam ("metaVarData" + string (cc));
					string varName,varData;
					irm_vn->GetValue (varName);
					irm_vd->GetValue (varData);
					pair <string , string> newMeta (varName,varData);
					t_meta.push_back(newMeta);
				}
			}
			bool updateUI;
			BoolParam *urm = (BoolParam*)i_ppl->GetParam ("UpdateDocUI");
			urm->GetValue (updateUI);
			
			int elemId = AddDocInstancedElement(eid/*paramStr*/,&t_meta);
			std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(elemId);
			AvocadoEngineDocFileElement *docElem = *it;

			std::vector<AvocadoEngineDocFileElement *>::iterator iti = GetDocFileElemById(eid);
			AvocadoEngineDocFileElement *idocElem = *iti;

			docElem->createFromElement(m_scene,idocElem);
			ParamListSharedPtr ppl = ParamList::createNew();
			//ppl->PushInt("eid",elemId);
			ParamListWriteLock(ppl)->PushPtr("elem",(void *)docElem);
			ret = OnSendAvocadoDocGeneralStringMessage("AddDocElement",docId,ParamListWriteLock(ppl)->SerializeList());
			if (updateUI)
			{
				NotifyElementsChanged ();
			}
			needRepaint = true;
		}
		else 
		if (msg == "DeleteDocFileElement")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;
			ParamListWriteLock(pl)->PopInt(eid);
			DeleteDocFileElement(eid);
			// currently unused - needs to be implemented !!!
			ret = OnSendAvocadoDocGeneralStringMessage("DeleteDocElement",docId,paramStr/*ppl->SerializeList()*/);
		} 
		else if (msg == "ViewSelectionChanged")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			//std::vector <int> added;
			//std::vector <int> removed;
			int vid;
			size_t i;
			ParamListWriteLock(pl)->PopInt (vid);
			i = 0;
			while (i< ParamListWriteLock(pl)->GetParamCount())
			{
				string name;
				int val;
				Param *p =ParamListWriteLock(pl)->GetParam(i);
				p->GetName(name);
				p->GetValue(val);
				if (name == "added")
				{
					OnElementSelected (val,vid,false);
					//added.push_back(val);
				}
				else if (name == "secondary")
				{
					OnElementSelected (val,vid,true);
				}
				else 
				{
					OnElementUnSelected (val,vid);
					//removed.push_back(val);
				}
				i++;
			}

		}
		else if (msg == "MouseOverElement")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int vid,eid;
		
			ParamListWriteLock(pl)->PopInt (vid);
			ParamListWriteLock(pl)->PopInt (eid);
			OnElementPreSelected (eid,vid);
		}
		else if (msg == "NotifyDocElementMove")
		{
			needRepaint = true;
		}
		return ret;
	}
}
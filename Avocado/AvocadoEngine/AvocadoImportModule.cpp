/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoImportModule.h"
#include "SceneFunctions.h"
#include "MeshGenerator.h"
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include <nvtraverser\SearchTraverser.h>
#include <nvsg/DepthAttribute.h>
#include <nvsg/TextureAttribute.h>
#include <nvsg/BlendAttribute.h>
#include <nvtraverser/UnifyTraverser.h>
#include <nvutil\Trace.h>
#include <nvutil/Tools.h>
#include "AvocadoMaterials.h"
#include <sstream>
#include <iostream>
using namespace nvmath;
using namespace nvsg;
namespace avocado
{



	AvocadoEngineDocFileElement::AvocadoEngineDocFileElement(int docId,string ownerModuleName, string fileName)
		:AvocadoEngineDocElement (docId, ownerModuleName)
	{
		m_isGroup = false;
		m_fileName = fileName;
		m_intr.materialID = -1;
		m_intr.color[0] = -1;
		m_intr.color[1] = -1;
		m_intr.color[2] = -1;
		m_groupType = string ("Kit");
	    avocado::MaterialLibraryInitDefault (m_intr.materialData);
	}

	AvocadoEngineDocFileElement::~AvocadoEngineDocFileElement()
	{
		m_children.clear();
	}

	bool AvocadoImport::deserializeMaterial (ParamListSharedPtr &ppl,AvocadoMaterialInterface *materialData)
	{
			// Note that deserialize is  done in the module  whlile serialization is performed in the element class.. 
		// not the smartest stuff.. should be optimized. its all about time.
		//OUCH !!! this hurts but its kind of a must.. of course grouping and optimizations is always welcome..

		bool res = AvocadoMaterialInterfaceFromString (ppl,materialData);
		return res;
	}

	void AvocadoEngineDocFileElement::serializeMaterial (ParamListSharedPtr &ppl)
	{
		avocado::AvocadoMaterialInterfaceToString (ppl, &(this->m_intr.materialData));
	}
	void AvocadoEngineDocFileElement::setChildren (std::vector<AvocadoEngineDocFileElement*>	&children)
	{
		m_children = children;

		if (m_children.size())
			m_isGroup = true;
		else
			m_isGroup = false;
	}
	bool AvocadoEngineDocFileElement::ApplyStateSet (StateSetSharedPtr newStateSet,bool cache,bool overwrite,bool convertFFP)
	{
		// CALL BASE CLASS FOR NOW.
#if 0
		bool result = AvocadoEngineDocElement::ApplyStateSet ( newStateSet,cache);
		if (result && convertFFP)
		{
			nvutil::convertFFPToCGFXNode (nvutil::sharedPtr_cast<Node>(m_elementRoot));
		}
#endif
		int level = 1;
		bool result = ApplyStateSetRecursive ( level,newStateSet,cache,overwrite,convertFFP);

		return (result);
	}
	bool AvocadoEngineDocFileElement::ApplyStateSetRecursive (int &level,StateSetSharedPtr newStateSet,bool cache, bool overwrite,bool convertFFP)
	{
		if ( (m_intr.materialData.inherit_from_parent || (overwrite) || level == 1))
		{
			if (overwrite && level > 1)
			{
				m_intr.materialData.inherit_from_parent = true;
				m_intr.materialID = -1;
			}			
			if (level == 1)
				m_cachedStateSet = newStateSet;

			if (!m_isGroup)
			{
				bool result = AvocadoEngineDocElement::ApplyStateSet ( newStateSet,(cache || level == 1),false);
				if (result && convertFFP)
				{
					nvutil::convertFFPToCGFXNode (nvutil::sharedPtr_cast<Node>(m_elementRoot));
				}
				return (result);
				//elmtPtr->ApplyStateSet (newStateSet,(level == 1),convertFFP);
			}
			else
			{
				for (size_t ch=0;ch< getChildrenCount ();ch++)
				{
					//apply_recursive_stateset (getChild (ch),newStateSet,++level,overwrite);
					getChild(ch)->ApplyStateSetRecursive (++level,newStateSet,cache,overwrite,convertFFP);
					
				}
				return true;
			}
		}
		return true;
	}

#if 0
	static void apply_recursive_stateset (AvocadoEngineDocFileElement* elmtPtr, StateSetSharedPtr newStateSet, int &level, bool overwrite, bool convertFFP = false)
	{
		if ( (elmtPtr->m_intr.materialData.inherit_from_parent || (overwrite) || level == 1))
		{
			if (overwrite && level > 1)
			{
				elmtPtr->m_intr.materialData.inherit_from_parent = true;
				elmtPtr->m_intr.materialID = -1;
			}			
			if (level == 1)
				(elmtPtr)->m_cachedStateSet = newStateSet;

			if (!elmtPtr->m_isGroup)
			{
				elmtPtr->ApplyStateSet (newStateSet,(level == 1),convertFFP);
#if 0
		        NodeSharedPtr root = (elmtPtr)->m_elementRoot;
				
				nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class nvsg::GeoNode");
				st->setBaseClassSearch(true);
				st->apply( root );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator its;
				for(its=searchResults.begin(); its!=searchResults.end(); its++)
				{
					GeoNodeSharedPtr node(dynamic_cast<GeoNodeWeakPtr>(*its));
					GeoNodeWriteLock (node)->replaceAllStateSets (newStateSet);			
				}
#endif
			}
			else
			{
				for (size_t ch=0;ch<elmtPtr->getChildrenCount ();ch++)
				{
					apply_recursive_stateset (elmtPtr->getChild (ch),newStateSet,++level,overwrite);
				}
			}
		}
	}
#endif
	string AvocadoEngineDocFileElement::serialize()
	{
		//string res = "filename=" + m_fileName + ";";

		ParamListSharedPtr ppl = ParamList::createNew();
		ppl->PushString ("OwnerModule",m_ownerModule);
		ppl->PushBool ("isGroup",m_isGroup);
		ppl->PushBool ("isRef",m_isRef);
		if (!m_isGroup)
		{
			if (m_isRef)
			{
				ppl->PushInt ("eid",m_refEID);
				ppl->PushInt ("drawableIdx",m_refDrawableIdx);
				ppl->PushString ("geoName",m_refGeoNode);
			}
			else
			{
				ppl->PushString ("fileName",m_fileName);			
			}
		} else
		{
			// group element.
			ppl->PushString  ("GroupType",m_groupType);
			ppl->PushInt ("GroupChildrenCount",int(m_children.size()));
			for (int csk=0;csk<int (m_children.size());csk++)
			{
				if (m_children[csk]!=NULL)
				{
					char cskis[10];
					itoa (csk,cskis,10);
					std::string cskparam = "GroupChildID" + string (cskis);
					ppl->PushInt (cskparam,m_children[csk]->GetID());
				}
			}
		}
		/* start element name and ID*/
		ppl->PushInt ("elementID",GetID ());
		ppl->PushString ("elementName",GetName ());
		/* end element name and ID*/

		ppl->PushInt ("MetaCount",int (this->m_intr.metaData.size ()));
		for (size_t i = 0;i < m_intr.metaData.size ();i++)
		{
				char cc[30];
					itoa(int(i),cc,10);
			ppl->PushString ("metaVarName" + string (cc),m_intr.metaData[i].first);
			ppl->PushString ("metaVarData" + string (cc),m_intr.metaData[i].second);
		}
		ppl->PushBool ("Visibility",m_intr.isVisible);
		ppl->PushInt ("MaterialID",this->m_intr.materialID);
		ppl->PushInt ("ColorRed",this->m_intr.color[0]);
		ppl->PushInt ("ColorGreen",this->m_intr.color[1]);
		ppl->PushInt ("ColorBlue",this->m_intr.color[2]);
		serializeMaterial (ppl);
		// write removed geo nodes list.
		if (!m_isRef)
		{
			ppl->PushInt ("removedGeoNodesCount",int (m_removedGeoNodes.size()));
			for (size_t i = 0;i < m_removedGeoNodes.size ();i++)
			{
					char cc[30];
						itoa(int(i),cc,10);
				ppl->PushString ("RemovedGeoNode" + string (cc),m_removedGeoNodes[i]);
			}
		}
		// end write removed geo node list.

				
		if (m_elementRoot ) 
		{
			Mat44f mat = TransformReadLock (m_elementRoot)->getMatrix ();
			const float * matPtr = mat.getPtr();
			/*
			for (int i=0;i<16;i++)
			{
				char cc[30];
				itoa(i,cc,10);
			
				ppl->PushFloat ("Location"+string(cc),matPtr[i]);		
			}*/
			ppl->PushFloat16 ("Location",matPtr);
			m_lastSavedLocation = mat;
		}
		
		return (ppl->SerializeList ());
		//return (m_fileName);
	}
		bool AvocadoEngineDocFileElement::addToGroup (SceneSharedPtr &scene,std::vector<AvocadoEngineDocFileElement*>	&children)
		{

			for( size_t kc=0;kc<children.size();kc++)
			{
			//assuming valid input. child should not be there..

			m_children.push_back ( children[kc] );
			}


			NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
			Transform::ChildrenIterator secondChilld = TransformWriteLock (m_elementRoot)->beginChildren();
			TransformSharedPtr newGroup (nvutil::sharedPtr_cast<Transform>(*secondChilld));
			//TransformSharedPtr newGroup (secondhildGroup);//= 0;
	
	//	newGroup = secondChilld;
	//	 newGroup = Transform::create();		
	//	 TransformWriteLock (newGroup)->setMatrix (Mat44f (true));
		

		int mask = 1;
			for (int viewId=0;viewId<8;viewId++)
					mask |= (int(pow(2.0,2*viewId+4)));// >> 1);
		
			Vec3f oldcentt = TransformReadLock(newGroup)->getBoundingBox().getCenter();

		for (size_t childX=0;childX<children.size();childX++)
		{
			if (children[childX]->m_elementRoot)
			{
				children[childX]->m_parentGroupID = GetID();
				TransformWriteLock (children[childX]->m_elementRoot)->setName("AvocadoSupressedElement");
				TransformWriteLock (newGroup)->addChild (children[childX]->m_elementRoot);
				GroupWriteLock (root)->removeChild  (children[childX]->m_elementRoot);
				TransformWriteLock(children[childX]->m_elementRoot)->setTraversalMask (~0);
			}
		}

		/**/
		/* This part will add a transform node , between element root and the file root , to adjust the 
		/* Center of rotation to the center of the bounding box */
	Vec3f centt2 = GroupWriteLock (newGroup)->getBoundingBox().getCenter();
		
		TransformSharedPtr NewGroupTop = nvutil::sharedPtr_cast <Transform>(m_elementRoot);//Transform::create();
		Vec3f centt = TransformWriteLock (NewGroupTop)->getTranslation();//getBoundingBox().getCenter();
		Vec3f centtdelta = centt - oldcentt;
		//TransformWriteLock (NewGroupTop)->addChild(newGroup);
		Mat44f groupmat = TransformWriteLock (NewGroupTop)->getMatrix ();
		Trafo centtTrafo;
		centtTrafo.setIdentity ();
		centtTrafo.setTranslation (centt);
		Mat44f centtmat = centtTrafo.getMatrix ();

		TransformWriteLock (newGroup)->setTranslation (  oldcentt -centt2 );

		TransformWriteLock (NewGroupTop)->setTranslation ( -oldcentt+ centt2 );
		
		/**/

		/*TransformWriteLock(NewGroupTop)->setTraversalMask (1);
		TransformWriteLock (NewGroupTop)->setUserData ((void*)this);
		TransformWriteLock (NewGroupTop)->setName ("AvocadoElement");*/
		/*GroupWriteLock (root)->addChild(NewGroupTop);*/
	//	m_elementRoot = NewGroupTop;
		
	/*	NodeSharedPtr newRoot = NewGroupTop;
		nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
		NVSG_ASSERT(st);
		st->setClassName("class Drawable");
		st->setBaseClassSearch(true);
		st->apply ( newRoot );
				
		const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
		std::vector<ObjectWeakPtr>::const_iterator it;
		for(it=searchResults.begin(); it!=searchResults.end(); it++)
		{
			//DrawableWeakPtr node = static_cast<GeoNodeWeakPtr>(*it);
			DrawableSharedPtr node(dynamic_cast<DrawableWeakPtr>(*it));
			
			DrawableWriteLock(node)->setTraversalMask (mask);
		}*/

		return true;
		}
		bool AvocadoEngineDocFileElement::splitGroup (SceneSharedPtr &scene,string sessionFolder)
		{
			NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
			GroupSharedPtr thisGroup = m_elementRoot;
			Group::ChildrenIterator secondChilld = TransformWriteLock (m_elementRoot)->beginChildren();
			Mat44f secondMat = TransformWriteLock (nvutil::sharedPtr_cast<Transform>(*secondChilld))->getMatrix ();
			
			Vec3f centt = GroupReadLock(m_elementRoot)->getBoundingBox().getCenter();
			Trafo inverseCenter;
			inverseCenter.setIdentity ();
			inverseCenter.setTranslation (centt);
			Mat44f grpMatrix = TransformWriteLock(m_elementRoot)->getMatrix();
			GroupWriteLock (root)->removeChild (thisGroup);
			int mask = 1;
				for (int viewId=0;viewId<8;viewId++)
						mask |= (int(pow(2.0,2*viewId+4)));// >> 1);

			for (size_t childX=0;childX<m_children.size();childX++)
			{
				if (m_children[childX]->m_elementRoot)
				{
					m_children[childX]->m_parentGroupID = -1;
					//grpMatrix.invert();
					//TransformWriteLock(m_children[childX]->m_elementRoot)->setMatrix(TransformWriteLock(m_children[childX]->m_elementRoot)->getMatrix() *grpMatrix);
					TransformWriteLock(m_children[childX]->m_elementRoot)->setMatrix( TransformWriteLock(m_children[childX]->m_elementRoot)->getMatrix() * secondMat * grpMatrix  );
					
					TransformWriteLock (m_children[childX]->m_elementRoot)->setName("AvocadoElement");
					TransformWriteLock (root)->addChild (m_children[childX]->m_elementRoot);
					TransformWriteLock(m_children[childX]->m_elementRoot)->setTraversalMask (1);
					
					// apply original state set.. (the material for each element is magically saved before element was grouped). here we restore it back.
					//MaterialBase2 mb2 (&(m_children[childX]->m_intr.materialData));
					if (m_children[childX]->m_intr.materialData.inherit_from_parent || m_groupType == "Weld")
					{
						if (m_intr.materialID > -1)
						{
							m_children[childX]->m_intr.materialData = m_intr.materialData;
							m_children[childX]->m_intr.materialID = m_intr.materialID;
						}
						if (m_intr.color[0] > -1 && m_intr.color[1] > -1 && m_intr.color[2] > -1 )
						{
							m_children[childX]->m_intr.color[0] = m_intr.color[0];
							m_children[childX]->m_intr.color[1] = m_intr.color[1];
							m_children[childX]->m_intr.color[2] = m_intr.color[2];
						}
					}
					MaterialBase2 mb2 (&(m_children[childX]->m_intr.materialData),sessionFolder);
					StateSetSharedPtr newStateSet = mb2.CreateShaderLogic ( m_children[childX]->m_shaderFileName);

				//	int lev = 1;
				 //   apply_recursive_stateset (m_children[childX],newStateSet,lev,true,false);
					m_children[childX]->ApplyStateSet (newStateSet,false,true,false);
				}
			}
			m_children.clear();
			return true;
		}
	bool AvocadoEngineDocFileElement::createGroup (SceneSharedPtr &scene)
	{
		NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();


		TransformSharedPtr newGroup = 0;
	
		
		 newGroup = Transform::create();		
		 TransformWriteLock (newGroup)->setMatrix (Mat44f (true));
		

		int mask = 1;
			for (int viewId=0;viewId<8;viewId++)
					mask |= (int(pow(2.0,2*viewId+4)));// >> 1);
		
		for (size_t childX=0;childX<m_children.size();childX++)
		{
			if (m_children[childX]->m_elementRoot)
			{
				m_children[childX]->m_parentGroupID = GetID();
				TransformWriteLock (m_children[childX]->m_elementRoot)->setName("AvocadoSupressedElement");
				TransformWriteLock (newGroup)->addChild (m_children[childX]->m_elementRoot);
				GroupWriteLock (root)->removeChild  (m_children[childX]->m_elementRoot);
				TransformWriteLock(m_children[childX]->m_elementRoot)->setTraversalMask (~0);
			}
		}

		/**/
		/* This part will add a transform node , between element root and the file root , to adjust the 
		/* Center of rotation to the center of the bounding box */
		Vec3f centt = GroupReadLock(newGroup)->getBoundingBox().getCenter();

		TransformSharedPtr NewGroupTop = Transform::create();
		TransformWriteLock (NewGroupTop)->addChild(newGroup);
		TransformWriteLock (newGroup)->setTranslation (-centt);
		TransformWriteLock (NewGroupTop)->setTranslation (centt);
	
		/*for (size_t childX=0;childX<m_children.size();childX++)
		{
			if (m_children[childX]->m_elementRoot)
			{

			}
		}*/
		/**/

		TransformWriteLock(NewGroupTop)->setTraversalMask (1);
		TransformWriteLock (NewGroupTop)->setUserData ((void*)this);
		TransformWriteLock (NewGroupTop)->setName ("AvocadoElement");
		GroupWriteLock (root)->addChild(NewGroupTop);
		m_elementRoot = NewGroupTop;
		
		NodeSharedPtr newRoot = NewGroupTop;
		nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
		NVSG_ASSERT(st);
		st->setClassName("class nvsg::Drawable");
		st->setBaseClassSearch(true);
		st->apply ( newRoot );
				
		const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
		std::vector<ObjectWeakPtr>::const_iterator it;
		for(it=searchResults.begin(); it!=searchResults.end(); it++)
		{
			//DrawableWeakPtr node = static_cast<GeoNodeWeakPtr>(*it);
			DrawableSharedPtr node(dynamic_cast<DrawableWeakPtr>(*it));
			
			DrawableWriteLock(node)->setTraversalMask (mask);
		}

		return true;
		//GroupWriteLock (root)->addChild (newGroup);
	}

	bool AvocadoEngineDocFileElement::DrinkMaterials (nvsg::StateSetSharedPtr originalStateSet)
	{
		
				float originalAmbient[3];
				float originalDiffuse[3];
				float originalSpecular[3];
				float originalShininess = 0.0;
				float originalOpacity = 0.0;
				string originalTexture = "";
				if (!originalStateSet)
					return false;
				if (nvutil::DrinkMaterialsFromStateSet (originalStateSet,originalAmbient,originalDiffuse,originalSpecular,originalShininess,originalOpacity,originalTexture))
				{
				m_intr.color[0] = int (255.0f * originalDiffuse[0]);
				m_intr.color[1] = int (255.0f * originalDiffuse[1]);
				m_intr.color[2] = int (255.0f * originalDiffuse[2]);
								
				m_intr.materialData.diffuse = 0.0f;
				m_intr.materialData.ambient = (originalAmbient[0] + originalAmbient[1] + originalAmbient[2]) / 3.0f;
				m_intr.materialData.specular = (originalSpecular[0] + originalSpecular[1] + originalSpecular[2]) / 3.0f;
				m_intr.materialData.baseColor[0] = originalDiffuse[0];
				m_intr.materialData.baseColor[1] = originalDiffuse[1];
				m_intr.materialData.baseColor[2] = originalDiffuse[2];
				m_intr.materialData.opacity = originalOpacity;
				m_intr.materialData.shininess = originalShininess;
				if (originalTexture != "")
				m_intr.materialData.textureFilename = originalTexture;
				return true;
	     }
	return false;
	}
	bool AvocadoEngineDocFileElement::createFromElement (SceneSharedPtr &scene, AvocadoEngineDocFileElement *el, std::string geoName,bool &hasColor)
	{
		// A Request to the nvidia magic fairy - 
		// I wish that all geonodes in the scene will have a uniqe name , and that all drawables will be loaded constant
		// predictable order. 
		// Also , I wish that all export and imports will keep the naming consistent.
		if (el->m_cachedDrawable == 0 && geoName == string ("new"))
			return false;
		if (el->m_isRef)
			return false;
		DrawableSharedPtr orgchild =  el->m_cachedDrawable;
		NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
		NodeSharedPtr searchRoot;
		if (el->m_elementRoot)
				//root = nvutil::sharedPtr_cast<nvsg::Node>(el->m_elementRoot);
					searchRoot = el->m_elementRoot;
				else
				{
					searchRoot = root;
					// shoult not reach here..assert or something..
				}
		//if (geoName != string ("new"))
		{
			// search the original item. we are here when loading from file.
				nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class nvsg::GeoNode");
				st->setBaseClassSearch(true);

				if (geoName != string ("new"))
					st->setObjectName (geoName);
				else
				{
					DrawableReadLock (orgchild)->ownersBegin ();
					Drawable::OwnerIterator ixt = DrawableWriteLock(orgchild)->ownersBegin();
					ObjectWeakPtr objxx = DrawableWriteLock(orgchild)->getOwner(ixt);
					st->setObjectName (GeoNodeReadLock(objxx)->getName ());
				}
				st->setTraversalMask (~0);
				st->apply( searchRoot );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator it;
				for(it=searchResults.begin(); it!=searchResults.end(); it++)
				{
					//DrawableWeakPtr node = static_cast<GeoNodeWeakPtr>(*it);
					GeoNodeSharedPtr resnode(dynamic_cast<GeoNodeWeakPtr>(*it));
					

					if (GeoNodeReadLock (resnode)->getNumberOfStateSets ())
					{
						if (geoName != string ("new"))
						{
							int lc = 0;
							DrawableSharedPtr drawe = 0 ;
							//el->m_cachedDrawable = drawe;
							GeoNode::StateSetConstIterator	stateSetITT = GeoNodeReadLock (resnode)->beginStateSets();
							while (stateSetITT != GeoNodeReadLock (resnode)->endStateSets())
							{
								GeoNode::DrawableConstIterator itppo = GeoNodeReadLock(resnode)->beginDrawables (stateSetITT);
								GeoNode::DrawableConstIterator itend = GeoNodeReadLock(resnode)->endDrawables (stateSetITT);
								while (itppo!=itend )
								{
									if (lc == m_refDrawableIdx)
									{
										drawe = *itppo;
										break;
									}
									++itppo;
									lc++;
								}
								if (drawe)
								{
									orgchild =drawe; 
									break;
								}
								++stateSetITT;
							}
						  	if (stateSetITT != GeoNodeReadLock (resnode)->endStateSets())
							{
								hasColor = DrinkMaterials (*stateSetITT);
							}
						}
						else
						{
							bool foundDrawableIdx = false;
							m_refDrawableIdx = 0;
							GeoNode::StateSetConstIterator	stateSetITT = GeoNodeReadLock (resnode)->beginStateSets();
							while (stateSetITT != GeoNodeReadLock (resnode)->endStateSets())
							{
							
								int lc = 0;
								GeoNode::DrawableConstIterator itpp = GeoNodeReadLock(resnode)->beginDrawables (stateSetITT);
								int dc =  GeoNodeReadLock(resnode)->getNumberOfDrawables(stateSetITT);
								
								while (lc < dc && (DrawableReadLock (*itpp)->getName() != DrawableReadLock (orgchild)->getName())  )
								{
									++itpp ;
									m_refDrawableIdx++;
									lc++;
								}
								if (lc < dc)
								{
									foundDrawableIdx = true;
									//m_refDrawableIdx =lc; 
									break;
								}
								++stateSetITT;
							}	
							if (stateSetITT != GeoNodeReadLock (resnode)->endStateSets())
							{
								hasColor = DrinkMaterials (*stateSetITT);
							}
							if (!foundDrawableIdx)
							{
								//NVSG_TRACE_OUT ("DRAWABLE NOT FOUND.\n")
								m_refDrawableIdx = 0;
							}
						}
						break;
						
					}

				}
		}
		if (!orgchild)
			return false;
		ObjectWeakPtr obj = DrawableReadLock(orgchild)->getOwner (DrawableReadLock(orgchild)->ownersBegin ());
		GeoNodeSharedPtr orgParent ((GeoNodeWeakPtr)obj);
		m_isRef = true;
		m_refGeoNode = GeoNodeReadLock (orgParent)->getName ();
		m_refEID = el->GetID ();
			
		// Get the original martix.
		Mat44f ResultMatrix (true);
		std::vector <Mat44f> mat_col;

		bool reachTop = false;
		ObjectWeakPtr parentGroupObj = GeoNodeReadLock(orgParent)->getOwner (GeoNodeReadLock(orgParent)->ownersBegin());
		Vec3f parentGroupCenter = Vec3f(0.0f,0.0f,0.0f);
		if (parentGroupObj)
		{
			GroupWeakPtr parentggwcenter = dynamic_cast< GroupWeakPtr >( parentGroupObj );
			parentGroupCenter = GroupReadLock(parentggwcenter)->getBoundingBox().getCenter();
		}
		while (parentGroupObj && !reachTop)
		{
			GroupWeakPtr parentggw = dynamic_cast< GroupWeakPtr >( parentGroupObj );
			if (parentggw)
			{
				TransformWeakPtr parentTTF = dynamic_cast< TransformWeakPtr >( parentGroupObj );
				if (parentTTF)
				{
					TransformSharedPtr myttf (parentTTF);
					mat_col.push_back( TransformReadLock (myttf)->getMatrix ());
				}
			};
			GroupSharedPtr parentGroup ((GroupWeakPtr)parentGroupObj);
			if (GroupReadLock(parentGroup)->getNumberOfOwners())
				parentGroupObj = GroupReadLock(parentGroup)->getOwner (GroupReadLock(parentGroup)->ownersBegin());
			else
				parentGroupObj = 0;
		}

		// First two transforms from the root
		// acually belongs to us (avocado) and used
		// to mange the element.
		if (mat_col.size() > 2)
		{

			//ResultMatrix =mat_col[0];
			size_t xr = 0;
			while (xr < mat_col.size()-2)
			{
				ResultMatrix *= mat_col[xr];// * ResultMatrix;
				xr++;
			}
		}
			
		TransformSharedPtr newTrans = Transform::create ();
		TransformWriteLock (newTrans)->setMatrix (ResultMatrix*mat_col[mat_col.size()-2]/*mat_col[0]*/);	

		// End get original matrix.
		
		GeoNode::StateSetIterator newSS = (GeoNodeWriteLock (orgParent))->findStateSet (orgchild);
		StateSetSharedPtr xxxss = newSS->get ();
		
		GeoNodeSharedPtr newGeo = GeoNode::create();
			
		GeoNodeWriteLock (newGeo)->addDrawable (xxxss,orgchild);
		
		TransformSharedPtr elementRoot = Transform::create();
		Mat44f idmat = Mat44f(true);
		//idmat = ResultMatrix;
		TransformWriteLock (elementRoot)->setMatrix(idmat);

		TransformWriteLock (newTrans)->addChild (newGeo);
		/**/
		/* This part will add a transform node , between element root and the file root , to adjust the 
		/* Center of rotation to the center of the bounding box */

		TransformSharedPtr chi = Transform::create();
		TransformWriteLock (chi)->setMatrix (idmat);
		
		TransformWriteLock (chi)->addChild(newTrans/*newGeo*/);

		// it wasnt easy task. but this code below sorts out the rotation center of the sub element.
		Mat44f mat;
		Trafo inverx;
		inverx.setIdentity ();
		Vec3f centt = TransformWriteLock(chi)->getBoundingBox().getCenter();
		TransformWriteLock (elementRoot)->setMatrix (TransformReadLock (el->m_elementRoot)->getMatrix ());
		inverx.setTranslation (/*TransformWriteLock (elementRoot)->getTranslation()+*/ centt);
		TransformWriteLock (chi)->setTranslation (-centt);
		TransformWriteLock (elementRoot)->addChild(chi);
		TransformWriteLock (elementRoot)->setMatrix (	inverx.getMatrix () * TransformReadLock (el->m_elementRoot)->getMatrix () );
		// YEAHHH !!!! - > WORKS LIKE A CHARM. PERFRECT CENTER OF ROTATION AND SUB ELEMENT POSITION.


		//Scale to scene - currently disabled , should be a preff
		float sc = TransformWriteLock (elementRoot)->getBoundingSphere().getRadius();
		Vec3f center = GeoNodeWriteLock (newGeo)->getBoundingBox().getCenter();//getCenter();
		float orgcenter = DrawableWriteLock (orgchild)->getBoundingSphere().getRadius();
		sc = 2.0;//orgcenter / sc;
		//	TransformWriteLock (elementRoot)->setScaling(Vec3f(sc,sc,sc));
		//	TransformWriteLock (elementRoot)->setTranslation(-center);

		// end scale to scene
			GeoNodeWriteLock (orgParent)->removeDrawable (orgchild);
			if (GeoNodeReadLock(orgParent)->getNumberOfDrawables () == 0)
			{
				// geo node is empty.. we must emove it or optix goes crazy.. thank the good lord for catching this bug :).
				// and dont blame nvidia , shit happens.
				    bool reachTop2 = false;
					ObjectWeakPtr parentGroupObj2 = GeoNodeReadLock(orgParent)->getOwner (GeoNodeReadLock(orgParent)->ownersBegin());
					while (parentGroupObj2 && !reachTop2)
					{
						GroupWeakPtr parentggw2 = dynamic_cast< GroupWeakPtr >( parentGroupObj2 );
						GroupSharedPtr GroupSS (parentggw2);
						GroupWriteLock (GroupSS)->removeChild (orgParent);
						if (GroupReadLock (GroupSS)->getNumberOfChildren () == 0)
							parentGroupObj2 = GroupReadLock(GroupSS)->getOwner (GroupReadLock(GroupSS)->ownersBegin());
						else
							reachTop2 = true;
					}
			}
	// should we remove the geo node if its empty ? perhaps for performance reasons?
		TransformWriteLock (elementRoot)->setUserData ((void*)this);
		TransformWriteLock (elementRoot)->setName ("AvocadoElement");
		TransformWriteLock (root)->addChild(elementRoot);
		m_elementRoot = elementRoot;
		TransformWriteLock(elementRoot)->setTraversalMask (1);
		return true;
	}
	void AvocadoEngineDocFileElement::setLocation (float *mat,bool updateLastSaved)
	{
		Mat44f mmat (true);
		for (int j=0;j<4;j++)
		for (int i=0;i<4;i++)
			mmat[j][i] = mat[i+4*j];
		if (updateLastSaved)
			m_lastSavedLocation = mmat;
		TransformWriteLock (m_elementRoot)->setMatrix (mmat );//* TransformWriteLock (m_elementRoot)->getMatrix ());
	}
	void AvocadoEngineDocFileElement::removeGeoNodes (vector<string> geonodes)
	{
		for (size_t i_geo = 0;i_geo < geonodes.size ();i_geo++)
		{
				NodeSharedPtr root = this->m_elementRoot;
				nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class nvsg::GeoNode");
				st->setBaseClassSearch(true);
				st->setObjectName (geonodes[i_geo]);
				st->apply( root );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator it;
				for(it=searchResults.begin(); it!=searchResults.end(); it++)
				{
					//DrawableWeakPtr node = static_cast<GeoNodeWeakPtr>(*it);
					GeoNodeSharedPtr resnode(dynamic_cast<GeoNodeWeakPtr>(*it));
					GroupWeakPtr gg = GeoNodeReadLock(resnode)->getOwner (GeoNodeReadLock(resnode)->ownersBegin());
					GroupSharedPtr pargroup (dynamic_cast<GroupWeakPtr>(gg));
					GroupWriteLock (pargroup)->removeChild (resnode);
				}
		}
	}
	void AvocadoEngineDocFileElement::setMaterial (int materialID)
	{
		m_intr.materialID = materialID;
	}
	void AvocadoEngineDocFileElement::setColor (int color[])
	{
		m_intr.color[0] = color[0];
		m_intr.color[1] = color[1];
		m_intr.color[2] = color[2];
	}
	void AvocadoEngineDocFileElement::removeFromScene(SceneSharedPtr &scene) 
	{
		NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
		
		GroupWriteLock (root)->removeChild (m_elementRoot);
	}
	void AvocadoEngineDocFileElement::createScene(SceneSharedPtr &scene,std::string sessionFolder,bool skip_optimization ) 
	{
		SceneSharedPtr tempScene ;
		std::vector<std::string> searchPaths;
		searchPaths.push_back (sessionFolder + "\\models\\");
		searchPaths.push_back (sessionFolder + "\\textures\\");
		searchPaths.push_back (sessionFolder);
		nvutil::loadScene (m_fileName,tempScene,searchPaths);
		//lets optimize
		if (!skip_optimization)
		{
			nvutil::optimizeForRaytracing (tempScene);
			string ext;
			nvutil::GetFileExtFromPath( m_fileName, ext );
			//bool flipYZ = (ext == string( ".3ds" ) || ext == string (".obj")) ? true : false;
			// Dont flip for now. 
			bool flipYZ = false;
			bool fl;
			if (avocado::GetEngineOptionBool ("flip_yz_on_import", &fl))
			{
				flipYZ = fl;
			}

			//bool flipYZ ;
			nvutil::convertFFPToCGFX (tempScene, flipYZ);
	 		//nvutil::optimizeUnifyVertices (tempScene);
		}
		// add the new scene root under the current view root node
		NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
		NodeSharedPtr child = SceneWriteLock(tempScene)->getRootNode();
		

		TransformSharedPtr elementRoot = Transform::create();
		Mat44f idmat = Mat44f(true);
		TransformWriteLock (elementRoot)->setMatrix(idmat);

		/**/
		/* This part will add a transform node , between element root and the file root , to adjust the 
		/* Center of rotation to the center of the bounding box */
		TransformSharedPtr chi = Transform::create();
		TransformWriteLock (chi)->addChild(child);
		TransformWriteLock (chi)->setTranslation (-GroupReadLock(child)->getBoundingBox().getCenter());
		/**/


		TransformWriteLock (elementRoot)->addChild(chi);
		//Scale to scene
		float sc = TransformWriteLock (elementRoot)->getBoundingSphere().getRadius();
		Vec3f center = NodeWriteLock (child)->getBoundingBox().getCenter();//getCenter();
			sc = 100.0f / sc;
			//TransformWriteLock (elementRoot)->setScaling(Vec3f(sc,sc,sc));
		// end scale to scene

		TransformWriteLock (elementRoot)->setUserData ((void*)this);
		TransformWriteLock (elementRoot)->setName ("AvocadoElement");
		TransformWriteLock (root)->addChild(elementRoot);
		m_elementRoot = elementRoot;
		//int geoIDcount = 1;
		std::vector <GeoNodeSharedPtr> GeoNodesList;
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
			DrawableWriteLock(node)->setTraversalMask (mask);

			{
				// Set apperance. name?
				Drawable::OwnerIterator ixt = DrawableWriteLock(node)->ownersBegin();
				ObjectWeakPtr objxx = DrawableWriteLock(node)->getOwner(ixt);
				if (!objxx) 
					continue;
				GeoNodeSharedPtr geox ((GeoNodeWeakPtr)objxx);
				GeoNodesList.push_back (geox);
			}
					
		}

		for (size_t gi = 0; gi < GeoNodesList.size ();gi++)
		{
			std::stringstream pStr;
			pStr << gi;
			
			std::string geoNameInOrigin = (GeoNodeWriteLock (GeoNodesList[gi]))->getName () ;
			if (geoNameInOrigin == "")	
			{
				//NVSG_TRACE_OUT ("NAMELESS GEONODE FOUND.\n")
				//We now got a geo node without a name.. very problematic.
				// Lets check if this geo node has a parent group that has a name.
				// if so we will set the name of the geonode according to its parent.
				ObjectWeakPtr objgxx  = GeoNodeReadLock(GeoNodesList[gi])->getOwner ( (GeoNodeReadLock(GeoNodesList[gi]))->ownersBegin());
				if (!objgxx) 
					continue;
				GroupSharedPtr groux ((GroupWeakPtr)objgxx);
				std::string parentGroupName = GroupReadLock (groux)->getName ();		
				if (parentGroupName != "")
					GeoNodeWriteLock (GeoNodesList[gi])->setName ("AvoGeoNode" + parentGroupName);
				else
					// This is absolutly the worst case. :(
				GeoNodeWriteLock (GeoNodesList[gi])->setName ("AvoGeoNode" + pStr.str());


			}

			///
			// Here we provide default names for each drawable.. realying on the same load order.
				int refDrawableIdx = 0;
				GeoNode::StateSetConstIterator	stateSetITT = GeoNodeReadLock (GeoNodesList[gi])->beginStateSets();
				while (stateSetITT != GeoNodeReadLock (GeoNodesList[gi])->endStateSets())
				{
					GeoNode::DrawableConstIterator itpp = GeoNodeReadLock(GeoNodesList[gi])->beginDrawables (stateSetITT);
					//int dc =  GeoNodeReadLock(GeoNodesList[gi])->getNumberOfDrawables(stateSetITT);			
					while (itpp != GeoNodeReadLock(GeoNodesList[gi])->endDrawables (stateSetITT))
					{	
						char idxstr [30];
						itoa (refDrawableIdx,idxstr,10);
						DrawableWriteLock ((*itpp))->setName (string("Drawable") + string (idxstr));
						
						refDrawableIdx++;
						++itpp ;
						//lc++;
					}
					++stateSetITT;
				}	
				///
		}
	}

	AvocadoImport::AvocadoImport (): AvocadoDocModule ("ImportModule")
	{
		m_lastbackimage = "crosscube.hdr";
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
		FreeGlobalMaterialCache();

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
		m_elementHash.clear();
	}

	std::vector<AvocadoEngineDocFileElement *>::iterator AvocadoImport::GetDocFileElemById (int elemId)
	{
		// first check cache
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
	AvocadoEngineDocFileElement * AvocadoImport::GetDocFileElemPtrById (int elemId)
	{
		AvocadoEngineDocFileElement * ret = 0;
		// first check cache
		DocFileElementHasIterator hit = m_elementHash.find (elemId);
		if (hit != m_elementHash.end ())
		{
			// great
			return hit->second;
		}
		else
		{
			std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById (elemId);
			if (it != m_docFileElements.end())
			{
				ret = *it;
			//	m_cachedElementID = elemId;
				//m_cachedElement = ret;
				m_elementHash.insert (std::pair<int,AvocadoEngineDocFileElement *>(elemId,ret));
			}
		}
		return ret;
	}
	AvocadoEngineDocFileElement* AvocadoImport::AddDocInstancedElement(int eid,  AvocadoElementInterface::MetaDataList *metaData,int inputID,string inputName )
	{
		AvocadoEngineDocFileElement *docElem = new AvocadoEngineDocFileElement(m_docId,m_name,"");
		int id = inputID;
		if (id == -1)
			id = (int)(m_smartIDGen->RequestUniqueID());
		std::stringstream idStr;
		idStr << id;
		std::string name = inputName;
		if (name == "")
			name =  "Derived Element " + idStr.str();
		docElem->SetID(id);
		docElem->SetName (name);
		docElem->m_intr.isVisible = true;
		if (metaData != NULL)
		{
			docElem->m_intr.metaData = (*metaData);
		}

		m_docFileElements.push_back (docElem);

		return docElem;
	}
	AvocadoEngineDocFileElement *AvocadoImport::AddDocFileElement(std::string filename,  AvocadoElementInterface::MetaDataList *metaData, float *mat, int inputID,string inputName,bool isGroup , string groupType   )
	{
		AvocadoEngineDocFileElement *docElem = new AvocadoEngineDocFileElement(m_docId,m_name,filename);
		int id = inputID;
		if (id == -1)
			id = (int)(m_smartIDGen->RequestUniqueID());
		std::stringstream idStr;
		idStr << id;
		std::string name = inputName;
		if (name == "")
			name = (isGroup ?  "Group  " + idStr.str() + " (" + groupType+")" : "Basic Element " + idStr.str());
		docElem->SetID(id);
		docElem->SetName (name);
		docElem->m_isGroup = isGroup;
		docElem->m_intr.isVisible = true;
		if (metaData != NULL)
		{
			docElem->m_intr.metaData = (*metaData);
		}

		m_docFileElements.push_back (docElem);

		return docElem;
	}
	bool AvocadoImport::DeleteElementChildren (int elemId,int docId)
	{
		std::vector<AvocadoEngineDocFileElement *>::iterator it=m_docFileElements.begin();
		vector<int> temp_list;
		// Collect..
		while (it!=m_docFileElements.end())
		{
			if ((*it)->m_isRef)
			{
				int eid = (*it)->GetID();
				int parentId = (*it)->m_refEID;
				if (parentId == elemId)
				{
					
					temp_list.push_back (eid);
				}
				
			}
			it++;
		}
		// Delete for real..
		for (size_t k=0;k<temp_list.size();k++)
		{
				ParamListSharedPtr newPL = ParamList::createNew ();
				newPL->PushInt ("eid",temp_list[k]);
				// currently unused - needs to be implemented !!!
				OnSendAvocadoDocGeneralStringMessage("DeleteDocElement",docId,newPL->SerializeList());
				std::vector<AvocadoEngineDocFileElement *>::iterator parent_it = GetDocFileElemById (temp_list[k]);
				if (parent_it != m_docFileElements.end())
				DeleteDocFileElement(parent_it);
		}
		return true;
	}

	bool AvocadoImport::DeleteDocFileElement(std::vector<AvocadoEngineDocFileElement *>::iterator &it )
	{
		//std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById (elemId);
		
		if ((*it)->m_isRef)
		{
			// ??should not reach here in thoery because we deleted the children already..
			// Add the geo node name to the removed geo nodes list
			// inside the parent element.
			int parentId = (*it)->m_refEID;
			AvocadoEngineDocFileElement * parent_it = GetDocFileElemPtrById (parentId);
		
			std::string refNode = (*it)->m_refGeoNode;
			// Parent might be deleted already...
			if (parent_it != 0)//this->m_docFileElements.end())
				(parent_it)->m_removedGeoNodes.push_back (refNode);
		}

		DocFileElementHasIterator hit= m_elementHash.find ((*it)->GetID());

		if (hit !=m_elementHash.end())
			m_elementHash.erase (hit);
		(*it)->removeFromScene (m_scene);
		delete *it;
		m_docFileElements.erase (it);
		return true;
	}
	
	void AvocadoImport::OnElementSelected (int eid,int vid,bool sub)
	{
		AvocadoEngineDocFileElement *it = GetDocFileElemPtrById(eid);
		if (it == 0/*m_docFileElements.end()*/)
			return;

		AvocadoEngineDocFileElement *itt = 0;

		if ((it)->m_parentGroupID > -1)
		{
			 itt = GetDocFileElemPtrById((it)->m_parentGroupID);
		}
		
		if (itt && (itt->m_groupType == string ("Weld") || itt->m_parentGroupID > -1))
		{
		// the  selected element is part of a welded group. OR a group inside a group.
		//lets select the parent group instead. 
			   std::stringstream pStr;
					pStr <<   "string owner=ImportModule"
						 <<  ",bool prePick=0" 
						 <<  ",int eid=" << (it)->m_parentGroupID
						 <<  ",int vid=" << vid
						 << ",bool multi=0;";
				
				avocado::OnSendAvocadoDocGeneralStringMessage("OnPick",(it)->m_docId,pStr.str());
				
				return;
		}

		if (sub && (it)->m_isGroup && (it)->m_subDrawable  != 0 && (it)->m_groupType != string ("Weld"))
		{
			// get sub drawable parent id.
			Drawable::OwnerIterator geop (DrawableReadLock((it)->m_subDrawable)->ownersBegin ());
			//GeoNodeSharedPtr grop (*geop);
			GeoNodeWeakPtr grop = dynamic_cast< GeoNodeWeakPtr >(*geop);
			GeoNode::OwnerIterator gw (GeoNodeWriteLock (grop)->ownersBegin ());

			
			GroupWeakPtr parent = dynamic_cast< GroupWeakPtr >(*gw);
			GroupWeakPtr last_parent;
			while (parent)
			{
				std::string name = GroupReadLock (parent)->getName();
				if (name == "AvocadoSupressedElement") 
				{
					last_parent = parent;
				}
				Group::OwnerIterator git = GroupReadLock(parent)->ownersBegin ();
				if (git != GroupReadLock(parent)->ownersEnd ())
					parent = dynamic_cast< GroupWeakPtr >(*(git));
				else
					parent = 0;
			}
			if (last_parent)
			{
					AvocadoEngineObject *el = (AvocadoEngineDocElement*)(GroupReadLock (last_parent)->getUserData());
					if (el)
					{
						if (el->GetID () != (it)->GetID())
						{
							 std::stringstream pStr;
							pStr <<   "string owner=ImportModule"
							 <<  ",bool prePick=0" 
							 <<  ",int eid=" << (el)->GetID()
						 <<  ",int vid=" << vid
						 << ",bool multi=0;";
				
						avocado::OnSendAvocadoDocGeneralStringMessage("OnPick",(it)->m_docId,pStr.str());
				
						return;
						}
					}
					
				} 
			
			// if the current sub-drawable has a parent id and its not me..select his parent.

		}
		// dont highlight non-visible elements..
		if ((it)->m_intr.isVisible == false)
			return;

		if (!sub)
		{
			(it)->m_subDrawable = 0;
			(it)->m_cachedDrawable = 0;
		}
		else
		{
			(it)->m_cachedDrawable = (it)->m_subDrawable ;
		}
		
		if (itt ) //(it)->m_parentGroupID > -1)
		{
			//Drill -down selection in a group..
			//AvocadoEngineDocFileElement *itt = GetDocFileElemPtrById(it->m_parentGroupID);
			string msg = AvocadoEngineDocElement::GetParamsFromElement(GetDocFileElemPtrById(it->m_parentGroupID));
			int mask = int(pow(2.0,2*vid+4));
			GroupWriteLock(itt->m_elementRoot)->setTraversalMask (GroupWriteLock(itt->m_elementRoot)->getTraversalMask () | (mask));
			for (size_t ch = 0; ch < itt->getChildrenCount ();ch++)
			{
				if (itt->getChild(ch)->GetID () != it->GetID ())
				  GroupWriteLock( (itt->getChild (ch))->m_elementRoot )->setTraversalMask (GroupWriteLock((itt->getChild(ch)->m_elementRoot))->getTraversalMask () & ~(mask));
			}
			//itt->SetSingleChildTraversal (it->GetID (), mask);
		}

		string msg = AvocadoEngineDocElement::GetParamsFromElement(it);
	    OnSendAvocadoGeneralStringMessage("HightlightElement", vid, msg);

		
	}
	
	void AvocadoImport::OnElementUnSelected (int eid,int vid)
	{
		AvocadoEngineDocFileElement * it = GetDocFileElemPtrById(eid);
		if (it == 0)
			return;

		if ((it)->m_parentGroupID > -1)
		{
			AvocadoEngineDocFileElement * itt = GetDocFileElemPtrById((it)->m_parentGroupID );
			if (itt)
			{
				for (size_t ch = 0; ch < itt->getChildrenCount ();ch++)
					{
						GroupWriteLock (itt->getChild(ch)->m_elementRoot)->setTraversalMask (~0);
				    }
				int mask = int(pow(2.0,2*vid+4));
				GroupWriteLock(itt->m_elementRoot)->setTraversalMask (GroupWriteLock(itt->m_elementRoot)->getTraversalMask () &~ (mask));
			}
			
		}
		string msg = AvocadoEngineDocElement::GetParamsFromElement(it);
	    OnSendAvocadoGeneralStringMessage("UnHightlightElement", vid, msg);
	}

	void AvocadoImport::SetElementMaterial (AvocadoEngineDocFileElement* elmtPtr, int matID, bool overwriteGroup)
	{
		
		/*
		std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(eid);
		if (it == m_docFileElements.end())
			return;*/
		//(*it)->setColor (col);
		(elmtPtr)->setMaterial (matID);
		int r = (elmtPtr)->m_intr.color[0];
		int g = (elmtPtr)->m_intr.color[1];
		int b = (elmtPtr)->m_intr.color[2];
		NormalizeRGB (r,g,b,(elmtPtr)->m_intr.materialData);

		if (matID != AVOCADO_MATERIAL_CUSTOM)
		{
			avocado::MaterialLibraryFetch3 (matID , r,g,b,(elmtPtr)->m_intr.materialData);	
			if (m_lastbackimage != "" && (elmtPtr)->m_intr.materialData.environmentMap == "")
							(elmtPtr)->m_intr.materialData.environmentMap = m_lastbackimage;
		}
		MaterialBase2 mb2 (&((elmtPtr)->m_intr.materialData),m_sessionFolder);
		StateSetSharedPtr newStateSet = mb2.CreateShaderLogic ( (elmtPtr)->m_shaderFileName);
		//(elmtPtr)->ApplyStateSet (newStateSet);
		if (newStateSet !=0 && newStateSet != StateSetSharedPtr())
		{

			(elmtPtr)->m_cachedStateSet = newStateSet;
//			if ((elmtPtr)->m_isGroup)
#if 0
			{
		        NodeSharedPtr root = (elmtPtr)->m_elementRoot;
				nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class nvsg::GeoNode");
				st->setBaseClassSearch(true);
				st->apply( root );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator its;
				for(its=searchResults.begin(); its!=searchResults.end(); its++)
				{
					GeoNodeSharedPtr node(dynamic_cast<GeoNodeWeakPtr>(*its));
					GeoNodeWriteLock (node)->replaceAllStateSets (newStateSet);			
				}
			} 
#endif
//			else
//			{
			 //   int level = 1;
				
			//	apply_recursive_stateset (elmtPtr,newStateSet,level,overwriteGroup);
				elmtPtr->ApplyStateSet (newStateSet,false,overwriteGroup,false);
			//}

			}
	} 
	void AvocadoImport::SetElementMaterialPropAll (int eid, ParamListSharedPtr &pl, bool applyBaseColor)
	{
		AvocadoMaterialInterface mt;
		avocado::AvocadoMaterialInterfaceFromString (pl,&mt);
			AvocadoEngineDocFileElement * it = GetDocFileElemPtrById(eid);
		if (it == 0)//m_docFileElements.end())
			return;
		int rgb[3];
		if (applyBaseColor)
		{
			rgb[0] = int (mt.baseColor[0] * 255.0f);
			rgb[1] = int (mt.baseColor[1] * 255.0f);
			rgb[2] = int (mt.baseColor[2] * 255.0f);
			(it)->setColor (rgb);
		}
		(it)->m_intr.materialData = mt;
		(it)->setMaterial (AVOCADO_MATERIAL_CUSTOM);
		// Set the current HDR environment.
		if (m_lastbackimage != "" && (it)->m_intr.materialData.environmentMap != "")
						(it)->m_intr.materialData.environmentMap = m_lastbackimage;

		MaterialBase2 mb2 (&((it)->m_intr.materialData),m_sessionFolder);
		StateSetSharedPtr newStateSet = mb2.CreateShaderLogic ( (it)->m_shaderFileName);
		if (newStateSet && newStateSet != StateSetSharedPtr())

		{
		//	int lev = 1;
		//	apply_recursive_stateset (it,newStateSet,lev,true,false);
			(it)->ApplyStateSet (newStateSet,false,true,false);
			//(it)->ApplyStateSet (newStateSet,true);
		}
		/*
				(it)->m_cachedStateSet = newStateSet;
				NodeSharedPtr root = (it)->m_elementRoot;
				nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class GeoNode");
				st->setBaseClassSearch(true);
				st->apply( root );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator its;
				for(its=searchResults.begin(); its!=searchResults.end(); its++)
				{
					GeoNodeSharedPtr node(dynamic_cast<GeoNodeWeakPtr>(*its));
					GeoNodeWriteLock (node)->replaceAllStateSets (newStateSet);			
				}
			
		}*/
	}
	void AvocadoImport::SetElementMaterialProp (int eid, float matID,string prop)
	{
		unsigned int updateMode = 0;
		bool sh_ref_changed = false;
		bool sh_wood_changed = false;
		bool sh_opacity_changed = false;
		AvocadoEngineDocFileElement *it = GetDocFileElemPtrById(eid);
		if (it == 0)
			return;
		//(*it)->setColor (col);
		(it)->setMaterial (AVOCADO_MATERIAL_CUSTOM);
		int r = (it)->m_intr.color[0];
		int g = (it)->m_intr.color[1];
		int b = (it)->m_intr.color[2];
		if (prop=="diffuse" )
		{
			if (  (it)->m_intr.materialData.diffuse == matID)
				return;
		   (it)->m_intr.materialData.diffuse = matID;
		} 
		else if (prop=="ambient")
		{
			if (  (it)->m_intr.materialData.ambient == matID)
				return;

			(it)->m_intr.materialData.ambient = matID;
		}
		else if (prop== "specular")
		{
			if (  (it)->m_intr.materialData.specular == matID)
				return;

			(it)->m_intr.materialData.specular = matID;
		}
		else if (prop=="shininess")
		{
			if (  (it)->m_intr.materialData.shininess == matID)
				return;

			(it)->m_intr.materialData.shininess = matID;
		}
		else if (prop=="ior")
		{
			if (  (it)->m_intr.materialData.ior == matID)
				return;

			(it)->m_intr.materialData.ior = matID;
		}
		else if (prop== "exposure")
		{
			updateMode = 1;
			if (  (it)->m_intr.materialData.exposure == matID)
				return;

			(it)->m_intr.materialData.exposure = matID;
		}
		else if (prop=="opacity")
		{
			if (  (it)->m_intr.materialData.opacity == matID)
				return;
			if ( (it)->m_intr.materialData.opacity == 1.0 && matID < 1.0 ||  (it)->m_intr.materialData.opacity < 1.0 && matID == 1.0)
				sh_opacity_changed = true;
			
			(it)->m_intr.materialData.opacity = matID;
		}
		else if (prop== "roughness")
		{
		    if (  (it)->m_intr.materialData.roughness == matID)
				return;

			(it)->m_intr.materialData.roughness = matID;
			updateMode = 1;
		}
		else if (prop=="polishness")
		{
		   if (  (it)->m_intr.materialData.polished == matID)
				return;

			(it)->m_intr.materialData.polished = matID;
			updateMode = 1;
		}
		else if (prop=="texturescaleu")
		{
			if (  (it)->m_intr.materialData.texScaleU == matID)
				return;

		   (it)->m_intr.materialData.texScaleU = matID;
			updateMode = 1;
		}
		else if (prop=="texturescalev")
		{
			if (  (it)->m_intr.materialData.texScaleV == matID)
				return;

			(it)->m_intr.materialData.texScaleV = matID;
			updateMode = 1;
		}
		else if (prop=="textureoffsetu")
		{
			if (  (it)->m_intr.materialData.texOffsetU == matID)
				return;

			(it)->m_intr.materialData.texOffsetU = matID;
			updateMode = 1;
		}
		else if (prop=="textureoffsetv")
		{
			if (  (it)->m_intr.materialData.texOffsetV == matID)
				return;

			(it)->m_intr.materialData.texOffsetV = matID;
			updateMode = 1;
		}
		else if (prop=="textureangle")
		{
			if (  (it)->m_intr.materialData.texAngle== matID)
				return;

			(it)->m_intr.materialData.texAngle = matID;
			updateMode = 1;
		}
		else if (prop=="bumpheight")
		{
		   if (  (it)->m_intr.materialData.bumpHeight == matID)
				return;

			(it)->m_intr.materialData.bumpHeight = matID;
			updateMode = 1;
		}
		else if (prop=="bumpscale")
		{
		   if (  (it)->m_intr.materialData.bumpScale == matID)
				return;

			(it)->m_intr.materialData.bumpScale= matID;
			updateMode = 1;
		}
		else if (prop== "fresnel")
		{
			updateMode = 1;
			if (  (it)->m_intr.materialData.frenselExp == matID)
				return;

			(it)->m_intr.materialData.frenselExp = matID;
		}
		else if (prop=="minrefl")
		{
			updateMode = 1;
			if (  (it)->m_intr.materialData.reflectionMin == matID)
				return;

			(it)->m_intr.materialData.reflectionMin = matID;
		}
		else if (prop== "maxrefl")
		{
			updateMode = 1;
        	if (  (it)->m_intr.materialData.reflectionMax == matID)
				return;
			
			(it)->m_intr.materialData.reflectionMax = matID;
		}
		else if (prop== "woodscale")
		{
			updateMode = 1;
			if (  (it)->m_intr.materialData.woodScale == matID)
				return;
			
			(it)->m_intr.materialData.woodScale = matID;
		}else if (prop== "wooble")
		{
			updateMode = 1;
			if (  (it)->m_intr.materialData.wooble == matID)
				return;
			
			(it)->m_intr.materialData.wooble = matID;
		}
		else if (prop== "ringscale")
		{
			updateMode = 1;
			if (  (it)->m_intr.materialData.ringScale == matID)
				return;
			
			(it)->m_intr.materialData.ringScale = matID;
		}
		else if (prop== "woodintensity")
		{
			updateMode = 1;
			if (  (it)->m_intr.materialData.woodColor[3] == matID)
				return;
			
			(it)->m_intr.materialData.woodColor[3] = matID;
		} 
		else if (prop== "reflectivity")
		{
			if (  (it)->m_intr.materialData.reflectivity == matID)
				return;
	
			if (  ( (it)->m_intr.materialData.reflectivity > 0 && matID == 0)
				||  ( (it)->m_intr.materialData.reflectivity == 0 && matID > 0))
				sh_ref_changed = true;
			if (matID > 0)
			{
				(it)->m_intr.materialData.environmentMap = m_lastbackimage;

			}else
				(it)->m_intr.materialData.environmentMap = "";

			(it)->m_intr.materialData.reflectivity = matID;

		}

		else if (prop== "iswood")
		{
			if (  (it)->m_intr.materialData.isWood == bool(matID == 1.0f))
				return;
	
		//	if (  ( (*it)->m_intr.materialData.reflectivity > 0 && matID == 0)
			//	||  ( (*it)->m_intr.materialData.reflectivity == 0 && matID > 0))
				sh_wood_changed = true;
			if (matID > 0)
			{
				(it)->m_intr.materialData.bumpTexture = "mybump3.dds";
				//(*it)->m_intr.materialData.environmentMap = "";

			}else
				(it)->m_intr.materialData.bumpTexture = "";

			(it)->m_intr.materialData.isWood = bool(matID == 1.0f);
		} else  if (prop== "isnormal")
		{
			if (  (it)->m_intr.materialData.bumpIsNormalMap == bool(matID == 1.0f))
				return;
			updateMode = 1;
		//	if (  ( (*it)->m_intr.materialData.reflectivity > 0 && matID == 0)
			//	||  ( (*it)->m_intr.materialData.reflectivity == 0 && matID > 0))
			//	sh_wood_changed = true;
			(it)->m_intr.materialData.bumpIsNormalMap = bool(matID == 1.0f);
		}
			else  if (prop== "enablebump")
		{
			if (  (it)->m_intr.materialData.bumpEnabled == bool(matID == 1.0f))
				return;
			if ((it)->m_intr.materialData.bumpTexture == "")
			{
				(it)->m_intr.materialData.bumpEnabled = bool(matID == 1.0f);
				return;
			}
			updateMode = 2;
		//	if (  ( (*it)->m_intr.materialData.reflectivity > 0 && matID == 0)
			//	||  ( (*it)->m_intr.materialData.reflectivity == 0 && matID > 0))
			//	sh_wood_changed = true;
			(it)->m_intr.materialData.bumpEnabled = bool(matID == 1.0f);
		}
		else  if (prop== "enabletexture")
		{
			if (  (it)->m_intr.materialData.texEnabled == bool(matID == 1.0f))
				return;
			if ((it)->m_intr.materialData.textureFilename == "")
			{
				(it)->m_intr.materialData.texEnabled = bool(matID == 1.0f);
				return;
			}
			updateMode = 2;
		//	if (  ( (*it)->m_intr.materialData.reflectivity > 0 && matID == 0)
			//	||  ( (*it)->m_intr.materialData.reflectivity == 0 && matID > 0))
			//	sh_wood_changed = true;
			(it)->m_intr.materialData.texEnabled = bool(matID == 1.0f);
		}
		else  if (prop== "enableskin")
		{
			if (  (it)->m_intr.materialData.isSkin == bool(matID == 1.0f))
				return;
			updateMode = 2;
		//	if (  ( (*it)->m_intr.materialData.reflectivity > 0 && matID == 0)
			//	||  ( (*it)->m_intr.materialData.reflectivity == 0 && matID > 0))
			//	sh_wood_changed = true;
			(it)->m_intr.materialData.isSkin = bool(matID == 1.0f);
		}
		else  if (prop== "enablehair")
		{
			if (  (it)->m_intr.materialData.isHair == bool(matID == 1.0f))
				return;
			updateMode = 2;
		//	if (  ( (*it)->m_intr.materialData.reflectivity > 0 && matID == 0)
			//	||  ( (*it)->m_intr.materialData.reflectivity == 0 && matID > 0))
			//	sh_wood_changed = true;
			(it)->m_intr.materialData.isHair = bool(matID == 1.0f);
		}
		else  if (prop== "hairlength")
		{
			if (  (it)->m_intr.materialData.hairLengh== matID)
				return;
			updateMode = 1;
		//	if (  ( (*it)->m_intr.materialData.reflectivity > 0 && matID == 0)
			//	||  ( (*it)->m_intr.materialData.reflectivity == 0 && matID > 0))
			//	sh_wood_changed = true;
			(it)->m_intr.materialData.hairLengh =  matID ;
		}
			else  if (prop== "hairdensity")
		{
			if (  (it)->m_intr.materialData.hairDens == matID )
				return;
			updateMode = 1;
		//	if (  ( (*it)->m_intr.materialData.reflectivity > 0 && matID == 0)
			//	||  ( (*it)->m_intr.materialData.reflectivity == 0 && matID > 0))
			//	sh_wood_changed = true;
			(it)->m_intr.materialData.hairDens = matID ;
		}
		MaterialBase2 mb2 (&((it)->m_intr.materialData),m_sessionFolder);
		if (sh_wood_changed || (sh_ref_changed && !((it)->m_intr.materialData.isWood)))
			updateMode = 2;
		if (sh_opacity_changed)
			if ( (it)->m_intr.materialData.reflectivity > 0)
				updateMode = 2;
		if (updateMode == 0)
		{
			mb2.UpdateSimpleMaterial ((it)->m_cachedStateSet);

			if  (prop == "reflectivity") //(it)->m_intr.materialData.reflectivity > 0)
			{
				mb2.UpdateSimpleUniform ((it)->m_cachedStateSet,prop);
			}
			if  (prop == "ior") //(it)->m_intr.materialData.reflectivity > 0)
			{
				// still no shader support for IOR
				mb2.UpdateSimpleUniform ((it)->m_cachedStateSet,prop);
			}
		}
		else if (updateMode == 1)
		{
			mb2.UpdateSimpleUniform ((it)->m_cachedStateSet,prop);
		}
		else
		{
				StateSetSharedPtr newStateSet = mb2.CreateShaderLogic ( (it)->m_shaderFileName);
				if (newStateSet && newStateSet !=StateSetSharedPtr())
				{
					//int lev = 1;
					//apply_recursive_stateset (it,newStateSet,lev,false,false);
					(it)->ApplyStateSet (newStateSet,false,false,false);
					(it)->m_cachedStateSet = newStateSet;
#if 0
					(it)->m_cachedStateSet = newStateSet;
					NodeSharedPtr root = (it)->m_elementRoot;
					nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
					NVSG_ASSERT(st);
					st->setClassName("class nvsg::GeoNode");
					st->setBaseClassSearch(true);
					st->apply( root );

					const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
					std::vector<ObjectWeakPtr>::const_iterator its;
					for(its=searchResults.begin(); its!=searchResults.end(); its++)
					{
						GeoNodeSharedPtr node(dynamic_cast<GeoNodeWeakPtr>(*its));
						GeoNodeWriteLock (node)->replaceAllStateSets (newStateSet);			
					}
#endif
				}
		}
	} 
	
	void AvocadoImport::SetElementMaterialProp3Float (int eid, string prop,float f0,float f1,float f2,float f3)
	{
		unsigned int updateMode = 0;
		bool sh_ref_changed = false;
		bool sh_wood_changed = false;
		bool sh_opacity_changed = false;
		AvocadoEngineDocFileElement * it = GetDocFileElemPtrById(eid);
		if (it == 0)//m_docFileElements.end())
			return;
		//(*it)->setColor (col);
		(it)->setMaterial (AVOCADO_MATERIAL_CUSTOM);
		int r = (it)->m_intr.color[0];
		int g = (it)->m_intr.color[1];
		int b = (it)->m_intr.color[2];
		if (prop=="woodcolor" )
		{
			updateMode = 1;
			if (f0==(it)->m_intr.materialData.woodColor[0] && f1==(it)->m_intr.materialData.woodColor[1]
			 && f2==(it)->m_intr.materialData.woodColor[2] && f3==(it)->m_intr.materialData.woodColor[3])
				 return;
			 (it)->m_intr.materialData.woodColor[0] = f0;
			 (it)->m_intr.materialData.woodColor[1] = f1;
			 (it)->m_intr.materialData.woodColor[2] = f2;
			 //(*it)->m_intr.materialData.woodColor[3] = f3;
		}
		MaterialBase2 mb2 (&((it)->m_intr.materialData),m_sessionFolder);
		if (sh_wood_changed || (sh_ref_changed && !((it)->m_intr.materialData.isWood)))
			updateMode = 2;
		if (sh_opacity_changed)
			if ( (it)->m_intr.materialData.reflectivity > 0)
				updateMode = 2;
		if (updateMode == 0)
			mb2.UpdateSimpleMaterial ((it)->m_cachedStateSet);
		else if (updateMode == 1)
		{
			mb2.UpdateSimpleUniform ((it)->m_cachedStateSet,prop);
		}
		else
		{
				StateSetSharedPtr newStateSet = mb2.CreateShaderLogic ( (it)->m_shaderFileName);
				//(it)->m_cachedStateSet = newStateSet;
				//int lev = 1;
				//apply_recursive_stateset (it,newStateSet,lev,false,false);
				(it)->ApplyStateSet (newStateSet,false,false,false);
				/*
				(it)->m_cachedStateSet = newStateSet;
				NodeSharedPtr root = (it)->m_elementRoot;
				nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class GeoNode");
				st->setBaseClassSearch(true);
				st->apply( root );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator its;
				for(its=searchResults.begin(); its!=searchResults.end(); its++)
				{
					GeoNodeSharedPtr node(dynamic_cast<GeoNodeWeakPtr>(*its));
					GeoNodeWriteLock (node)->replaceAllStateSets (newStateSet);			
				}
				*/
		}
	
	}
	void AvocadoImport::SetElementMaterialPropString (int eid, string matID,string prop)
	{
		unsigned int updateMode = 0;
		bool sh_ref_changed = false;
		bool sh_wood_changed = false;
		bool sh_opacity_changed = false;
		

		
		AvocadoEngineDocFileElement* elmtPtr= GetDocFileElemPtrById(eid);
		if (elmtPtr == 0)
			return;
		//(*it)->setColor (col);
		if (elmtPtr)
		{
		(elmtPtr)->setMaterial (AVOCADO_MATERIAL_CUSTOM);
		int r = (elmtPtr)->m_intr.color[0];
		int g = (elmtPtr)->m_intr.color[1];
		int b = (elmtPtr)->m_intr.color[2];
		if (prop=="bumptexture" )
		{
			if (  (elmtPtr)->m_intr.materialData.bumpTexture == matID)
				return;
			if (((matID == "empty" || matID == "") && (elmtPtr)->m_intr.materialData.bumpTexture  != "") || 
				(((matID != "empty" && matID != "" ) && (elmtPtr)->m_intr.materialData.bumpTexture  == "") ))
				updateMode = 2; 
			else
		   updateMode = 1;
			if (matID !="empty" && matID != "")
			{
					//(*it)->m_intr.materialData.bumpEnabled = true;
					(elmtPtr)->m_intr.materialData.bumpTexture = matID;
			}
			else
				(elmtPtr)->m_intr.materialData.bumpTexture = "";
				if (updateMode == 1 && ((elmtPtr)->m_intr.materialData.bumpEnabled == false))
					return;
		} else 
		if (prop=="texturefilename" )
		{
			if (  (elmtPtr)->m_intr.materialData.textureFilename == matID)
				return;
			if (((matID == "empty" || matID == "") && (elmtPtr)->m_intr.materialData.textureFilename  != "") || 
				(((matID != "empty" && matID != "" ) && (elmtPtr)->m_intr.materialData.textureFilename  == "") ))
				updateMode = 2; else
				updateMode = 1;

			if (matID !="empty" && matID != "")
			{
			//	(*it)->m_intr.materialData.texEnabled = true;
				(elmtPtr)->m_intr.materialData.textureFilename = matID;
			}
			else
				(elmtPtr)->m_intr.materialData.textureFilename = "";

			if (updateMode == 1 && ((elmtPtr)->m_intr.materialData.texEnabled == false))
					return;
		} 
		if (prop=="skintexturefilename" )
		{
			if (  (elmtPtr)->m_intr.materialData.skinTex == matID)
				return;
		/*	if (((matID == "empty" || matID == "") && (*it)->m_intr.materialData.textureFilename  != "") || 
				(((matID != "empty" && matID != "" ) && (*it)->m_intr.materialData.textureFilename  == "") ))
				updateMode = 2; else
				updateMode = 1;*/
			updateMode = 1;
			if (matID !="empty" && matID != "")
			{
			//	(*it)->m_intr.materialData.texEnabled = true;
				(elmtPtr)->m_intr.materialData.skinTex = matID;
			}
			else
				(elmtPtr)->m_intr.materialData.skinTex = "";

			if (updateMode == 1 && ((elmtPtr)->m_intr.materialData.isSkin == false))
					return;
		} 
		if (prop=="hairtexturefilename" )
		{
			if (  (elmtPtr)->m_intr.materialData.hairTex == matID)
				return;
		/*	if (((matID == "empty" || matID == "") && (*it)->m_intr.materialData.textureFilename  != "") || 
				(((matID != "empty" && matID != "" ) && (*it)->m_intr.materialData.textureFilename  == "") ))
				updateMode = 2; else
				updateMode = 1;*/
			updateMode = 1;
			if (matID !="empty" && matID != "")
			{
			//	(*it)->m_intr.materialData.texEnabled = true;
				(elmtPtr)->m_intr.materialData.hairTex = matID;
			}
			else
				(elmtPtr)->m_intr.materialData.hairTex = "";

			if (updateMode == 1 && ((elmtPtr)->m_intr.materialData.isHair == false))
					return;
		} 
		
		MaterialBase2 mb2 (&((elmtPtr)->m_intr.materialData),m_sessionFolder);
		if (sh_wood_changed || (sh_ref_changed && !((elmtPtr)->m_intr.materialData.isWood)))
			updateMode = 2;
		if (sh_opacity_changed)
			if ( (elmtPtr)->m_intr.materialData.reflectivity > 0)
				updateMode = 2;
		if (updateMode == 0)
			mb2.UpdateSimpleMaterial ((elmtPtr)->m_cachedStateSet);
		else if (updateMode == 1)
		{
			mb2.UpdateSimpleUniform ((elmtPtr)->m_cachedStateSet,prop);
		}
		else
		{
				StateSetSharedPtr newStateSet = mb2.CreateShaderLogic ( (elmtPtr)->m_shaderFileName);
				//(elmtPtr)->m_cachedStateSet = newStateSet;
				///int lev = 1;
				//apply_recursive_stateset (elmtPtr,newStateSet,lev,false,false);
				(elmtPtr)->ApplyStateSet (newStateSet,false,false,false);
				/*
				NodeSharedPtr root = (elmtPtr)->m_elementRoot;
				nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class GeoNode");
				st->setBaseClassSearch(true);
				st->apply( root );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator its;
				for(its=searchResults.begin(); its!=searchResults.end(); its++)
				{
					GeoNodeSharedPtr node(dynamic_cast<GeoNodeWeakPtr>(*its));
					GeoNodeWriteLock (node)->replaceAllStateSets (newStateSet);			
				}
				*/
		}
		}
	
	} 
	void AvocadoImport::SetElementColor (AvocadoEngineDocFileElement* elmtPtr, int r, int g, int b, bool applyShader , bool overwrite)
	{
		int col[3];

		
		col[0] = r;
		col[1] = g;
		col[2] = b;
	//	std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(eid);
	//	if (it == m_docFileElements.end())
	//		return;
		if (elmtPtr)
		{
			elmtPtr->setColor (col);
			if (applyShader)
			{
				int matID  = (elmtPtr)->m_intr.materialID;
				NormalizeRGB (r,g,b,(elmtPtr)->m_intr.materialData);
				//StateSetSharedPtr newStateSet = avocado::MaterialLibraryFetch (matID , r,g,b );
				//if (matID > 0)
				//{
					if (matID != -1 && matID != AVOCADO_MATERIAL_CUSTOM)
					{
						avocado::MaterialLibraryFetch3 (matID , r,g,b,(elmtPtr)->m_intr.materialData);	
						if (m_lastbackimage != "" && (elmtPtr)->m_intr.materialData.environmentMap == "")
							(elmtPtr)->m_intr.materialData.environmentMap = m_lastbackimage;
					}
					MaterialBase2 mb2 (&((elmtPtr)->m_intr.materialData),m_sessionFolder);

					if (matID > -1 )
					{
						StateSetSharedPtr newStateSet = mb2.CreateShaderLogic ( (elmtPtr)->m_shaderFileName);
					  //  (elmtPtr)->ApplyStateSet (newStateSet);
						//int level = 1;
						//apply_recursive_stateset (elmtPtr,newStateSet,level,overwrite);
						elmtPtr->ApplyStateSet (newStateSet,false,overwrite,false);
					}
					else
					{
						StateSetSharedPtr newStateSet = mb2.CreateFFPShader ();
					    //(elmtPtr)->ApplyStateSet (newStateSet,true);
						//int level = 1;
						//apply_recursive_stateset (elmtPtr,newStateSet,level,overwrite,true);
						elmtPtr->ApplyStateSet (newStateSet,false,overwrite,true);
					}
				//} 
#if 0
				(elmtPtr)->m_cachedStateSet = newStateSet;
						NodeSharedPtr root = (elmtPtr)->m_elementRoot;
						nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
						NVSG_ASSERT(st);
						st->setClassName("class nvsg::GeoNode");
						st->setBaseClassSearch(true);
						st->apply( root );

						const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
						std::vector<ObjectWeakPtr>::const_iterator its;
						for(its=searchResults.begin(); its!=searchResults.end(); its++)
						{
							//DrawableWeakPtr node = static_cast<GeoNodeWeakPtr>(*it);
							GeoNodeSharedPtr node(dynamic_cast<GeoNodeWeakPtr>(*its));
							//GeoNode::StateSetIterator itss = (GeoNodeWriteLock (node))->beginStateSets ();
							//StateSetSharedPtr newStateSet = avocado::MaterialLibraryFetch (matID , r,g,b );

							GeoNodeWriteLock (node)->replaceAllStateSets (newStateSet);
							//StateStateSetWriteLock (itss)->beginAttributes ();
								// Set apperance.
							/*	Drawable::OwnerIterator it = DrawableWriteLock(node)->ownersBegin();
								ObjectWeakPtr obj = DrawableWriteLock(node)->getOwner(it);
								GeoNodeSharedPtr geo ((GeoNodeWeakPtr)obj);
								GeoNode::StateSetIterator ss = GeoNodeWriteLock (geo)->findStateSet(DrawableSharedPtr(node));*/
							/*	DepthAttributeSharedPtr fs = DepthAttribute::create();
								DepthAttributeWriteLock (fs)->setDepthWritingEnabled(true);
								DepthAttributeWriteLock (fs)->setDepthTestingEnabled(true);
								DepthAttributeWriteLock (fs)->setDepthFunction(DF_LESS_OR_EQUAL);*/

							//	StateSetWriteLock (*ss)->addAttribute(fs);

		//						StatePass::AttributeIterator itt = StateSetWriteLock (*ss)->beginAttributes();
							//	while (itt !=StateSetWriteLock (*ss)->endAttributes())
				

							}
#endif
			}
		}
	}
	
	void AvocadoImport::OnElementPreSelected (int eid,int vid)
	{
		AvocadoEngineDocFileElement *it = GetDocFileElemPtrById(eid);
		if (it == 0)
			return;
		string msg = AvocadoEngineDocElement::GetParamsFromElement(it);
	    OnSendAvocadoGeneralStringMessage("PreHightlightElement", vid, msg);
	}
	void AvocadoImport::HideElement (int eid)
	{
		AvocadoEngineDocFileElement *it = GetDocFileElemPtrById(eid);
		if (it == 0)
			return;
		//(it)->m_intr.isVisible = false;
		//NodeWriteLock ((it)->m_elementRoot)->setTraversalMask (0);
		(it)->SetVisibility (false);
	}
	bool AvocadoImport::HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId,const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string ("Module: " + m_name + string(" - ") + msg + string(" (")+paramStr+ string(")\n")).c_str());

		bool ret = false;

	    if (msg == "AddToGroup")
		{
			ret = true;
			// Add to group
			ParamListSharedPtr i_ppl = ParamList::createFromString (paramStr);
			vector<int> eid_list;
			vector<AvocadoEngineDocFileElement*> l_children;
			while (i_ppl->GetParam("eid"))
			{
				int eid;
				i_ppl->PopInt (eid);
				eid_list.push_back(eid);
			}
			// Find the one and only group element.
			AvocadoEngineDocFileElement *grpPtr =0;
			int groupCount = 0;
			for (size_t kx = 0;kx <eid_list.size();kx++)
			{
				AvocadoEngineDocFileElement *idocChildElem = GetDocFileElemPtrById(eid_list[kx]);;//*itiChild;
				
				if (idocChildElem->m_isGroup )
				{
					groupCount++;
					grpPtr = idocChildElem;
				} else
				{
					l_children.push_back (idocChildElem);
				}
			}
			// update children list.
			if (groupCount == 1)
			{
				
				//if (updateUI)
				{
					ParamListSharedPtr pl2 = ParamList::createNew();
					ParamListWriteLock(pl2)->PushBool ("prepick",false);
					ParamListWriteLock(pl2)->PushInt ("vid",0);
					OnSendAvocadoDocGeneralStringMessage("OnPickNothing", m_docId, ParamListWriteLock(pl2)->SerializeList());
				
				}
				grpPtr->addToGroup (m_scene,l_children);
				needRepaint = true;
				
			}
			// recalc group.
		}
		else if (msg == "AddDocFileElement")
		{
			string fileName (""),geoName ("");
			vector <pair<string,string>> t_meta;
			vector <string> t_removed_geo_nodes;
			vector<AvocadoEngineDocFileElement*> l_children;
			ParamListSharedPtr i_ppl = ParamList::createFromString (paramStr);
			bool isRef = false;
			bool isGroup = false;
			string lgrouptype = "";
			bool updateUI = false;
			BoolParam *isrefPrm = (BoolParam*)i_ppl->GetParam ("isRef");
			isrefPrm->GetValue (isRef);
			isrefPrm->GetValue (isRef);
			if (i_ppl->GetParam ("isGroup"))
			{
				BoolParam *isgroupPrm = (BoolParam*)i_ppl->GetParam ("isGroup");
				isgroupPrm->GetValue (isGroup);
			}
		
			if (!isGroup)
			{
				if (!isRef)
				{
					StringParam *prm = (StringParam*)i_ppl->GetParam ("fileName");
					prm->GetValue (fileName);

				} else
				{
					// Should not reach here...
					StringParam *prm = (StringParam*)i_ppl->GetParam ("refGeoName");
					prm->GetValue (geoName);
				}
			}
			else
			{
				if (i_ppl->GetParam ("UpdateDocUI"))
				{
					BoolParam *upadurm = (BoolParam*)i_ppl->GetParam ("UpdateDocUI");
					upadurm->GetValue (updateUI);
				}
				if (i_ppl->GetParam ("GroupType"))
				{
					StringParam *grouptypePrm = (StringParam*)i_ppl->GetParam ("GroupType");
					grouptypePrm->GetValue (lgrouptype);
				}
				if (i_ppl->GetParam ("GroupChildrenCount"))
				{
					IntParam *groupcountPrm = (IntParam*)i_ppl->GetParam ("GroupChildrenCount");
					int groupChildrenCount = 0;
					groupcountPrm->GetValue (groupChildrenCount);
					for (int cskix=0;cskix < groupChildrenCount ;cskix ++)
					{
						char cskis[10];
						itoa (cskix,cskis,10);
						if (i_ppl->GetParam ("GroupChildID" + string (cskis)))
						{
							IntParam *groupChildIDPrm = (IntParam*)i_ppl->GetParam ("GroupChildID" + string (cskis));
							int groupchildID = -1;
							groupChildIDPrm->GetValue(groupchildID);
							if (groupchildID>-1)
							{
								// go get the child..
								//std::vector<AvocadoEngineDocFileElement *>::iterator itiChild = GetDocFileElemById(groupchildID);
								AvocadoEngineDocFileElement *idocChildElem = GetDocFileElemPtrById(groupchildID);;//*itiChild;
								l_children.push_back (idocChildElem);
							}
						}
					}
				}

			}
			/* start element name and ID*/
			string elementNameInput("");
			int elementIDInput = -1;
			if (i_ppl->GetParam ("elementID"))
			{
				IntParam *ielemID = (IntParam*)i_ppl->GetParam ("elementID");
				ielemID->GetValue  (elementIDInput);
			}
			if (i_ppl->GetParam ("elementName"))
			{
				StringParam *selemName = (StringParam*)i_ppl->GetParam ("elementName");
				selemName->GetValue (elementNameInput);
			}
			/* end element name and ID*/

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
			// start read visibility
			bool isVisible = true;
			if (i_ppl->GetParam ("Visibility"))
			{
				BoolParam *brv = (BoolParam*)i_ppl->GetParam ("Visibility");
				brv->GetValue (isVisible);
			}
			// end read visibility
			// start read material override
			int materialID = -1;
			if (i_ppl->GetParam ("MaterialID"))
			{
				IntParam *irmat = (IntParam*)i_ppl->GetParam ("MaterialID");
				irmat->GetValue (materialID);
			}
			int color[3 ];
			color[0] = -1;
			color[1] = -1;
			color[2] = -1;
			if (i_ppl->GetParam ("ColorRed")&&
				i_ppl->GetParam ("ColorGreen")&&
				i_ppl->GetParam ("ColorBlue"))
			{
				IntParam *irmatcolr = (IntParam*)i_ppl->GetParam ("ColorRed");
				irmatcolr->GetValue (color[0]);
				IntParam *irmatcolg = (IntParam*)i_ppl->GetParam ("ColorGreen");
				irmatcolg->GetValue (color[1]);
				IntParam *irmatcolb = (IntParam*)i_ppl->GetParam ("ColorBlue");
				irmatcolb->GetValue (color[2]);
			}
			AvocadoMaterialInterface mat_intr;
			bool hasMaterialData = deserializeMaterial (i_ppl,&mat_intr);
			//end read material override
			// start read removed geo nodes
			if (i_ppl->GetParam ("removedGeoNodesCount"))
			{
				IntParam *irmgn = (IntParam*)i_ppl->GetParam ("removedGeoNodesCount");

				int mc3 = 0;
				irmgn->GetValue (mc3);
				if (mc3 > 0)
				{
					for (int j=0;j<mc3;j++)
					{
						char cc[30];
						itoa(j,cc,10);
						StringParam *irm_gn = (StringParam*)i_ppl->GetParam ("RemovedGeoNode" + string (cc));
						string varName;
						irm_gn->GetValue (varName);
						t_removed_geo_nodes.push_back (varName);
					}
				}
				
			}
			// end read removed geo nodes

			// start read locations
			float mat[16];
			bool hasLocation = false;
			hasLocation = i_ppl->GetFloat16ValueByName ("Location",mat);
			/*
			for (int j=0;j<16;j++)
			{
				char cc[30];
				itoa(j,cc,10);
				mat[j] = 1.0f;
				if (i_ppl->GetParam ("Location" + string (cc)))
				{
					FloatParam *irm_vn = (FloatParam*)i_ppl->GetParam ("Location" + string (cc));
					if (irm_vn)
					{
						irm_vn->GetValue (mat[j]);
						hasLocation = true;
					}
				}
			}
			*/

			// end read locations
			
			//
			//add to files list..
			ParamListSharedPtr fileppl = ParamList::createNew ();
			fileppl->PushBool ("embed",0);
			fileppl->PushString ("filename",fileName);
			fileppl->PushInt ("type",0);
			ret = OnSendAvocadoDocGeneralStringMessage("InsertFileLink",docId,ParamListWriteLock(fileppl)->SerializeList());

			//

		//	int elemId = AddDocFileElement(fileName/*paramStr*/,&t_meta,mat,elementIDInput,elementNameInput,isGroup);
			AvocadoEngineDocFileElement *docElem  = AddDocFileElement(fileName/*paramStr*/,&t_meta,mat,elementIDInput,elementNameInput,isGroup,lgrouptype);
			int elemId = (docElem)->GetID();
			//	GetDocFileElemById(elemId);
			//AvocadoEngineDocFileElement *docElem = *it;
			
			docElem->m_isRef = isRef;
			docElem->m_refGeoNode = geoName;
			if (lgrouptype != "")
				docElem->m_groupType = lgrouptype;
			if (isGroup)
			{
				docElem->setChildren (l_children);
			}
			if (!isRef && !isGroup)
				docElem->createScene(m_scene,m_sessionFolder);
			else if (isGroup)
			{
				if (updateUI)
				{
					ParamListSharedPtr pl2 = ParamList::createNew();
					ParamListWriteLock(pl2)->PushBool ("prepick",false);
					ParamListWriteLock(pl2)->PushInt ("vid",0);
					OnSendAvocadoDocGeneralStringMessage("OnPickNothing", docElem->m_docId, ParamListWriteLock(pl2)->SerializeList());
				
				}

				docElem->createGroup (m_scene);
			}

			if (hasLocation)
				docElem->setLocation (mat,true);
			
			//if (materialID != -1)
			if (color[0] != -1 && color[1] != -1 && color[2] != -1)
				SetElementColor (docElem,color[0],color[1],color[2], (materialID == -1 ? true : false),false);
			if (hasMaterialData)
				docElem->m_intr.materialData = mat_intr;
			if (materialID != -1)
			//docElem->setMaterial (materialID);
				SetElementMaterial (docElem,materialID);
		
			if (!isVisible)
				HideElement (docElem->GetID ());

			if (t_removed_geo_nodes.size ())
			{
				docElem->removeGeoNodes (t_removed_geo_nodes);
				docElem->m_removedGeoNodes = t_removed_geo_nodes;
			}
			ParamListSharedPtr ppl = ParamList::createNew();
			//ppl->PushInt("eid",elemId);
			ParamListWriteLock(ppl)->PushPtr("elem",(void *)docElem);
			ret = OnSendAvocadoDocGeneralStringMessage("AddDocElement",docId,ParamListWriteLock(ppl)->SerializeList());
			if (updateUI)
			{
				NotifyElementsChanged();
							std::stringstream pStr;
					pStr <<   "string owner=ImportModule"
						 <<  ",bool prePick=0" 
						 <<  ",int eid=" << docElem->GetID()
						 <<  ",int vid=0"// << viewID 
						 << ",bool multi=0;";
				
				avocado::OnSendAvocadoDocGeneralStringMessage("OnPick",docElem->m_docId,pStr.str());
			}
			needRepaint = true;
		}
		else if (msg == "AddDocInstancedElement")
		{
			vector <pair<string,string>> t_meta;
			ParamListSharedPtr i_ppl = ParamList::createFromString (paramStr);
			int eid;
			string geoName;
			int drawableIdx= 0;
			IntParam *prm = (IntParam*)i_ppl->GetParam ("eid");
			// handle group case..
			prm->GetValue (eid);
			//std::vector<AvocadoEngineDocFileElement *>::iterator itgroup = GetDocFileElemPtrById(eid);
			AvocadoEngineDocFileElement *parentDocFileElem =  GetDocFileElemPtrById(eid);
			//if ( parentDocFileElem != 0)//m_docFileElements.end ())
			//	parentDocFileElem = *itgroup;
			bool updateUI;
			BoolParam *urm = (BoolParam*)i_ppl->GetParam ("UpdateDocUI");
			urm->GetValue (updateUI);

			if ((parentDocFileElem)->m_isGroup)
			{
				(parentDocFileElem)->splitGroup (m_scene,m_sessionFolder);
				ParamListSharedPtr pl = ParamList::createNew ();
				pl->PushInt ("eid",eid);
				pl->PushBool ("unhighlight",updateUI);//(GetDocInterface())->m_isAvailable);
				HandleAvocadoDocGeneralStringMessage ("DeleteDocCommonElement",m_docId, pl->SerializeList (),needRepaint);
				return true;
			} else
			if ((parentDocFileElem)->m_parentGroupID > -1)
			{
				// impossible..
				return true;
			}
			// end group case..
		
			StringParam *geonameprm = (StringParam*)i_ppl->GetParam ("geoName");
			geonameprm->GetValue (geoName);

			i_ppl->GetIntValueByName ("drawableIdx",drawableIdx);

			/*if (i_ppl->GetParam("drawableIdx"))
			{
				IntParam *drawableIdxParam = (IntParam*)i_ppl->GetParam ("drawableIdx");
				drawableIdxParam->GetValue (drawableIdx);
			}*/

			/* start element name and ID*/
			string elementNameInput("");
			int elementIDInput = -1;
			if (i_ppl->GetParam ("elementID"))
			{
				IntParam *ielemID = (IntParam*)i_ppl->GetParam ("elementID");
				ielemID->GetValue  (elementIDInput);
			}
			if (i_ppl->GetParam ("elementID"))
			{
				StringParam *selemName = (StringParam*)i_ppl->GetParam ("elementName");
				selemName->GetValue (elementNameInput);
			}
			/* end element name and ID*/
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

	
			
				// start read locations
			float mat[16];
			bool hasLocation = false;
			hasLocation = i_ppl->GetFloat16ValueByName("Location",mat);
			/*
			for (int j=0;j<16;j++)
			{
				char cc[30];
				itoa(j,cc,10);
				mat[j] = 1.0f;
				if (i_ppl->GetParam ("Location" + string (cc)))
				{
					FloatParam *irm_vn = (FloatParam*)i_ppl->GetParam ("Location" + string (cc));
					if (irm_vn)
					{
						irm_vn->GetValue (mat[j]);
						hasLocation = true;
					}
				}
			}
			*/

			// end read locations

			// start read visibility
			bool isVisible = true;
			i_ppl->GetBoolValueByName ("Visibility",isVisible);
			/*if (i_ppl->GetParam ("Visibility"))
			{
				BoolParam *brv = (BoolParam*)i_ppl->GetParam ("Visibility");
				brv->GetValue (isVisible);
			}*/
			//end read visibility
			// start read material override
			int materialID = -1;
			i_ppl->GetIntValueByName ("MaterialID",materialID);
			/*if (i_ppl->GetParam ("MaterialID"))
			{
				IntParam *irmat = (IntParam*)i_ppl->GetParam ("MaterialID");
				irmat->GetValue (materialID);
			}*/
			int color[3 ];
			color[0] = -1;
			color[1] = -1;
			color[2] = -1;
			/*
			if (i_ppl->GetParam ("ColorRed")&&
				i_ppl->GetParam ("ColorGreen")&&
				i_ppl->GetParam ("ColorBlue"))
			{
				IntParam *irmatcolr = (IntParam*)i_ppl->GetParam ("ColorRed");
				irmatcolr->GetValue (color[0]);
				IntParam *irmatcolg = (IntParam*)i_ppl->GetParam ("ColorGreen");
				irmatcolg->GetValue (color[1]);
				IntParam *irmatcolb = (IntParam*)i_ppl->GetParam ("ColorBlue");
				irmatcolb->GetValue (color[2]);
			}
			*/
			i_ppl->GetIntValueByName ("ColorRed",color[0]);
			i_ppl->GetIntValueByName ("ColorGreen",color[1]);
			i_ppl->GetIntValueByName ("ColorBlue",color[2]);


			AvocadoMaterialInterface mat_intr;
			bool hasMaterialData = deserializeMaterial (i_ppl,&mat_intr);
			//end read material override

			
			//int elemId = AddDocInstancedElement(eid/*paramStr*/,&t_meta,elementIDInput,elementNameInput);
			//std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(elemId);
			AvocadoEngineDocFileElement *docElem = AddDocInstancedElement(eid/*paramStr*/,&t_meta,elementIDInput,elementNameInput);;
			int elemId = docElem->GetID();

			//std::vector<AvocadoEngineDocFileElement *>::iterator iti = itgroup;//GetDocFileElemById(eid);
			AvocadoEngineDocFileElement *idocElem = parentDocFileElem;//*iti;
			bool originalWasRemoved = false;
			if (idocElem == 0)
				originalWasRemoved = true;
			docElem->m_refDrawableIdx =  drawableIdx;
			if  ((idocElem->m_cachedDrawable || geoName != string ("new")))
			{
				bool foundOriginalColor = false;
				if (!docElem->createFromElement(m_scene,idocElem,geoName,foundOriginalColor))
					return true;
				if (geoName == string ("new") && !originalWasRemoved)
				{
					// new splitted element .. lets copy apperance from original parent.
					
					docElem->m_intr.materialID = idocElem->m_intr.materialID; 
					if (idocElem->m_intr.materialID > - 1 || (!foundOriginalColor))
					{
						docElem->m_intr.color[0] = idocElem->m_intr.color[0];
						docElem->m_intr.color[1] = idocElem->m_intr.color[1];
						docElem->m_intr.color[2] = idocElem->m_intr.color[2];
						docElem->m_intr.materialData = idocElem->m_intr.materialData;
					}
				}
				if (hasMaterialData)
					docElem->m_intr.materialData = mat_intr;
				if (hasLocation)
					docElem->setLocation (mat,true);
				if (color[0] != -1 && color[1] != -1 && color[2] != -1)
					SetElementColor (docElem,color[0],color[1],color[2],(materialID == -1 ? true : false),false);
				if (materialID != -1)
					SetElementMaterial (docElem,materialID);
				if (!isVisible)
					HideElement (docElem->GetID());

				ParamListSharedPtr ppl = ParamList::createNew();
				//ppl->PushInt("eid",elemId);
				ParamListWriteLock(ppl)->PushPtr("elem",(void *)docElem);
				ret = OnSendAvocadoDocGeneralStringMessage("AddDocElement",docId,ParamListWriteLock(ppl)->SerializeList());
				if (updateUI)
				{
					NotifyElementsChanged ();
						
					ParamListSharedPtr pl2 = ParamList::createNew();
					ParamListWriteLock(pl2)->PushBool ("prepick",false);
					ParamListWriteLock(pl2)->PushInt ("vid",0);
					OnSendAvocadoDocGeneralStringMessage("OnPickNothing", docElem->m_docId, ParamListWriteLock(pl2)->SerializeList());
					std::stringstream pStr;
					pStr <<   "string owner=ImportModule"
						 <<  ",bool prePick=0" 
						 <<  ",int eid=" << docElem->GetID()
						 <<  ",int vid=0"// << viewID 
						 << ",bool multi=0;";
					avocado::OnSendAvocadoDocGeneralStringMessage("OnPick",docElem->m_docId,pStr.str());
		
				}
				needRepaint = true;
			}
		}
		else 
		if (msg == "DeleteDocCommonElement")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;
			bool updateUI;

			ParamListWriteLock(pl)->PopBool(updateUI);
			ParamListWriteLock(pl)->PopInt(eid);
			std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById (eid);
			if (it == m_docFileElements.end ())
				return false;
			if (updateUI)
			{
				ParamListSharedPtr pl2 = ParamList::createNew();
				ParamListWriteLock(pl2)->PushBool ("prepick",false);
				ParamListWriteLock(pl2)->PushInt ("vid",0);
			
				OnSendAvocadoDocGeneralStringMessage("OnPickNothing", docId, ParamListWriteLock(pl2)->SerializeList());
			} else
			{
				// THis is called from document destructor.
				// Usually when we exit the application
			}
			//
			// Delete the children of this element
			//
			//
				DeleteElementChildren (eid,docId);
			//
			// End delete children
			ParamListSharedPtr newPL = ParamList::createNew ();
			newPL->PushInt ("eid",eid);
			// currently unused - needs to be implemented !!!
		    OnSendAvocadoDocGeneralStringMessage("DeleteDocElement",docId,newPL->SerializeList());
			DeleteDocFileElement(it);
			if (updateUI)
			{
			NotifyElementsChanged ();
			needRepaint = true;
			}
			
			ret = false;
		} 
		else if (msg  ==  "SetDocParam")
		{
			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			if (pl->GetParam ("backimage"))
			{
				StringParam* ip = (StringParam*)pl->GetParam ("backimage");
				string val;
				ip->GetValue (val);
				if (m_lastbackimage != val)
						m_lastbackimage = val;
				
			}
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
		else if (msg == "NotifyDocElementMove")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;

			ParamListWriteLock(pl)->PopInt (eid);
			AvocadoEngineDocFileElement *docElem = GetDocFileElemPtrById(eid);
			if (docElem == 0)
				return false;
			for (size_t ki=0;ki < docElem->m_attachments.size ();ki++)
			{
				int aid = docElem->m_attachments[ki];
				ParamListSharedPtr ppls= ParamList::createNew ();
					ppls->PushInt ("eid",aid);
					ppls->PushInt ("eid",eid);

				avocado::OnSendAvocadoDocGeneralStringMessage ("NotifyAttachmentMove",m_docId,ppls->SerializeList());
			}
			// now traverse children.. this is the reason why we do this here in the module and not in the docuemnt level.
			// we can make a decision to put the children inside base doc element and move this message to doc.

			//this entire thing should be a recursive play , but I am too tired now.
			for (size_t kx=0;kx < docElem->getChildrenCount();kx++)
			{
					AvocadoEngineDocFileElement *docElemC = docElem->getChild (kx);
				
					for (size_t ki=0;ki < docElemC->m_attachments.size ();ki++)
					{
						int aid = docElemC->m_attachments[ki];
						ParamListSharedPtr ppls= ParamList::createNew ();
			 			ppls->PushInt ("eid",aid);
							ppls->PushInt ("eid",docElemC->GetID());
							
						avocado::OnSendAvocadoDocGeneralStringMessage ("NotifyAttachmentMove",m_docId,ppls->SerializeList());
					}
			}
		}
		else if (msg == "RestoreToOrigin")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;
			ParamListWriteLock(pl)->PopInt (eid);
			//std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(eid);
			AvocadoEngineDocFileElement *docElem =  GetDocFileElemPtrById(eid);
		//	TransformSharedPtr xx  = (*it)->m_elementRoot;
			TransformWriteLock ((docElem)->m_elementRoot)->setMatrix (Mat44f (true));
			
			ParamListSharedPtr pl2 = ParamList::createNew();
			ParamListWriteLock(pl2)->PushInt("eid",eid);
			OnSendAvocadoDocGeneralStringMessage("NotifyDocElementMove",this->m_docId, ParamListWriteLock(pl2)->SerializeList());
			//needRepaint =true;
		}
		else if (msg == "RestoreToDefault")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;
		
			
			ParamListWriteLock(pl)->PopInt (eid);
			//std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemPtrById(eid);
			AvocadoEngineDocFileElement *docElem =  GetDocFileElemPtrById(eid);
			//TransformSharedPtr xx  = (*it)->m_elementRoot;
			TransformWriteLock ((docElem)->m_elementRoot)->setMatrix (Mat44f ((docElem)->m_lastSavedLocation));
			
			ParamListSharedPtr pl2 = ParamList::createNew();
			ParamListWriteLock(pl2)->PushInt("eid",eid);
			OnSendAvocadoDocGeneralStringMessage("NotifyDocElementMove",this->m_docId, ParamListWriteLock(pl2)->SerializeList());

			needRepaint =true;
		}
		else if (msg == "MouseOverElement")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int vid,eid;
		
			ParamListWriteLock(pl)->PopInt (vid);
			ParamListWriteLock(pl)->PopInt (eid);
			OnElementPreSelected (eid,vid);
		}
		 else if (msg == "ChangeElementColor")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int r,g,b,eid;
		
			ParamListWriteLock(pl)->PopInt (b);
			ParamListWriteLock(pl)->PopInt (g);
			ParamListWriteLock(pl)->PopInt (r);
			ParamListWriteLock(pl)->PopInt (eid);
			//std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(eid);
			AvocadoEngineDocFileElement *docElem = GetDocFileElemPtrById(eid);
			SetElementColor (docElem,r,g,b,true,true);
			docElem->m_intr.materialData.inherit_from_parent = false;
			needRepaint = true;
			NotifyElementsChanged ();
		} else if (msg == "ChangeElementMaterial")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int matid,eid;
		
			ParamListWriteLock(pl)->PopInt (matid);
			ParamListWriteLock(pl)->PopInt (eid);

		//	std::vector<AvocadoEngineDocFileElement *>::iterator it = GetDocFileElemById(eid);
			AvocadoEngineDocFileElement *docElem = GetDocFileElemPtrById(eid);
			SetElementMaterial (docElem,matid, true);
			docElem->m_intr.materialData.inherit_from_parent = false;
			needRepaint = true;
			NotifyElementsChanged ();
		}
		else if (msg == "ChangeElementMaterialPropAll" || msg == "ChangeElementMaterialPropAllColor")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			vector<int> eids;
			while (ParamListWriteLock(pl)->GetParam("eid") != NULL)
			{
				int eid;
				ParamListWriteLock(pl)->PopInt(eid);
				eids.push_back (eid);
			}
			bool applyColor =  (msg == "ChangeElementMaterialPropAllColor");
			for (size_t k=0;k<eids.size();k++)
				SetElementMaterialPropAll (eids[k],pl, applyColor);
			needRepaint = true;
			NotifyElementsChanged ();
		}
		else if (msg == "ChangeElementMaterialProp")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;
			float matid;
			string proopname;
			ParamListWriteLock(pl)->PopString(proopname);
			ParamListWriteLock(pl)->PopFloat (matid);
			ParamListWriteLock(pl)->PopInt (eid);
			SetElementMaterialProp (eid,matid,proopname);
			needRepaint = true;
			NotifyElementsChanged ();
		}
		else if (msg == "ChangeElementMaterialPropString")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;
			string matid;
			string proopname;
			ParamListWriteLock(pl)->PopString(proopname);
			ParamListWriteLock(pl)->PopString (matid);
			ParamListWriteLock(pl)->PopInt (eid);
			SetElementMaterialPropString (eid,matid,proopname);
			needRepaint = true;
			NotifyElementsChanged ();
		}
		else if (msg == "ChangeElementMaterialProp3Float")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;
			float f0,f1,f2;// matid;
			string proopname;
			ParamListWriteLock(pl)->PopString(proopname);
			ParamListWriteLock(pl)->PopFloat (f2);
			ParamListWriteLock(pl)->PopFloat (f1);
			ParamListWriteLock(pl)->PopFloat (f0);
			ParamListWriteLock(pl)->PopInt (eid);
			SetElementMaterialProp3Float (eid,proopname,f0,f1,f2,1.0f);
			needRepaint = true;
			NotifyElementsChanged ();
		}
		else if (msg == "HideElement")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid,vid;
			ParamListWriteLock(pl)->PopInt (vid);
			ParamListWriteLock(pl)->PopInt (eid);
			AvocadoEngineDocFileElement * it = GetDocFileElemPtrById(eid);
			if (it !=0)
			{
				if (it->SetVisibility (false))
				{
	//			(it)->m_intr.isVisible = false;
//				NodeWriteLock((it)->m_elementRoot)->setTraversalMask (0);
				  needRepaint = true;
				  NotifyElementsChanged ();
				}
			}		
		}
		else if (msg == "UnHideElement")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid,vid;
			ParamListWriteLock(pl)->PopInt (vid);
			ParamListWriteLock(pl)->PopInt (eid);
			AvocadoEngineDocFileElement * it = GetDocFileElemPtrById(eid);
			if (it != 0)
			{
				if (it->SetVisibility (true))
				{
				//(it)->m_intr.isVisible = true;
			//	NodeWriteLock((it)->m_elementRoot)->setTraversalMask (~0); // or maybe ~0 or per view..
  		     		needRepaint = true;
	     			NotifyElementsChanged ();
				}
			}
		}
		else if (msg == "UnHideAllElements")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int vid;
			ParamListWriteLock(pl)->PopInt (vid);
			std::vector<AvocadoEngineDocFileElement *>::iterator it = m_docFileElements.begin();
			while (it != m_docFileElements.end())
			{
				if ((*it)->SetVisibility (true))
				{
				//if ((*it)->m_intr.isVisible == false)
			//	{
					//(*it)->m_intr.isVisible = true;
					//NodeWriteLock((*it)->m_elementRoot)->setTraversalMask (~0); // or maybe ~0 or per view..
					needRepaint = true;
				//}
				}
				it++;
			}
			if (needRepaint)
				NotifyElementsChanged ();
		}
		else if (msg == "LookAt")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid,vid;
			ParamListWriteLock(pl)->PopInt (vid);
			ParamListWriteLock(pl)->PopInt (eid);
			AvocadoEngineDocFileElement * it = GetDocFileElemPtrById(eid);
			if (it != 0)
				
			{
				//get bounding sphere
				Sphere3f bs = NodeWriteLock((it)->m_elementRoot)->getBoundingSphere();
				ParamListSharedPtr plf = ParamList::createNew ();
				plf->PushFloat ("radius",bs.getRadius());
				plf->PushFloat ("center0",bs.getCenter()[0]);
				plf->PushFloat ("center1",bs.getCenter()[1]);
				plf->PushFloat ("center2",bs.getCenter()[2]);
				OnSendAvocadoGeneralStringMessage ("OrientCamera",vid,plf->SerializeList ());
			}
		}
		return ret;
	}
}
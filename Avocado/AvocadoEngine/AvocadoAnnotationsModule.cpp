/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoAnnotationsModule.h"
#include "SceneFunctions.h"
#include "MeshGenerator.h"
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include <nvtraverser\SearchTraverser.h>
#include <nvsg/AlphaTestAttribute.h>
#include <nvsg/BlendAttribute.h>
#include <nvsg/DepthAttribute.h>
#include <nvsg/Billboard.h>
#include <nvsg/SpotLight.h>
#include <nvsg/CgFx.h>
#include <nvutil\Tools.h>
using namespace nvmath;
namespace avocado
{
	AvocadoEngineDocAnnotationElement::AvocadoEngineDocAnnotationElement(int docId,string ownerModuleName, string type)
		:AvocadoEngineDocElement (docId, ownerModuleName)
	{
		m_type = type;
		m_intr.materialID = -1;
		m_intr.materialData.environmentMap = "";
		m_intr.materialData.reflectivity = 0.0f;
		m_intr.isVisible = true;
		m_refGeoNode = m_type;
		m_targetEID = -1;
	}

	AvocadoEngineDocAnnotationElement::~AvocadoEngineDocAnnotationElement()
	{

	}
	 bool  AvocadoEngineDocAnnotationElement::SetVisibility (bool isVisible ) 
	 {
		 if (m_type == "SectionPlane")
		 {
			 if (m_plane)
			 {
			   	 ClipPlaneWriteLock (m_plane)->setEnabled (isVisible);
			  //  ClipPlaneWriteLock (m_plane)->setTraversalMask ((isVisible ? ~0 : 0));
		     }
		 }
		 return AvocadoEngineDocElement::SetVisibility (isVisible);
	 }
	string AvocadoEngineDocAnnotationElement::serialize()
	{
		// TODO : MOVE COMMON SERIALIZATION STUFF TO THE BASE ELEMENT CLASS..ITS DUPLICATE CODE, BUT HEY.. ITS ONLY 2 COPIES SO FAR.. MANAGABLE.
		ParamListSharedPtr ppl = ParamList::createNew ();
		ppl->PushString ("OwnerModule",m_ownerModule);
		ppl->PushString ("AnnotationType",m_type);
		
		ppl->PushString ("Name",m_name);
		ppl->PushInt ("AnnoID",m_id);
		ppl->PushInt ("AttachmentID",m_targetEID);
		ppl->PushInt ("AttachmentID2",m_targetEID2);
		ppl->PushBool ("isRef",0);
		ppl->PushBool ("isVisible",m_intr.isVisible);
		if (m_elementRoot ) 
		{
			nvmath::Mat44f mat = TransformReadLock (m_elementRoot)->getMatrix ();
			const float * matPtr = mat.getPtr();
			for (int i=0;i<16;i++)
			{
				char cc[30];
				itoa(i,cc,10);

				ppl->PushFloat ("Location"+string(cc),matPtr[i]);		
			}
			m_lastSavedLocation = mat;
		}
		ppl->PushInt ("AnnotationParamCount",int (m_intr.annotationData.size ()));
		for (size_t xx = 0; xx < m_intr.annotationData.size ();xx++)
		{
			string ann = m_intr.annotationData[xx].first;
			string annval = m_intr.annotationData[xx].second;
			stringstream aass;
			aass << "AnnotationParamName" << xx;
			ppl->PushString (aass.str (),ann);

			stringstream aaval;
			aaval << "AnnotationParamValue" << xx;
			ppl->PushString (aaval.str (),annval);

		}

		return (ppl->SerializeList ());
	}
	void AvocadoEngineDocAnnotationElement::RecalcAnnotation ()
	{

	}

	void AvocadoEngineDocAnnotationElement::attachmentMoved (int attID,nvsg::GroupSharedPtr &node,nvsg::SceneSharedPtr &scene) 
	{
		if (m_type == "PathLine" || m_type == "Callout")
		{
				//	SceneSharedPtr TempScene = Scene::create();
					nvmath::Box3f box = GroupReadLock(node)->getBoundingBox();
					Vec3f bs ( box.getCenter());
					

					if (attID == m_targetEID)
					{
						m_pt1Initialized = true;
						m_cachedPt1 = bs;
					}
					else if (attID == m_targetEID2)
					{
						m_pt2Initialized = true;
						m_cachedPt2 = bs;
					}
					else
					{
							// error
							return;
					}
					if (m_targetEID2 > -1)
					{
						if (!m_pt2Initialized || !m_pt1Initialized)
							return;
					}
					DrawableSharedPtr border;
					DrawableSharedPtr borderPlane = 0;
					if (m_targetEID2 > -1)
					{
						 border = nvutil::createPathLine ( m_cachedPt1[0], m_cachedPt1[1],  m_cachedPt1[2], 
							m_cachedPt2[0], m_cachedPt2[1], m_cachedPt2[2]);
					}
					else
					{
						if (m_type == "PathLine")
						{
						Group::ChildrenIterator secondChilld = TransformWriteLock (node)->beginChildren();
						Vec3f bst = -TransformWriteLock (nvutil::sharedPtr_cast<Transform>(*secondChilld))->getTranslation();

						//TransformWriteLock(node)->getTranslation();
						//bst = bs;
						border = nvutil::createPathLine ( bst[0],bst[1],bst[2],bs[0],bs[1],bs[2]);
						}
						else if (m_type == "Callout")
						{
							Vec3f t (TransformReadLock (m_elementRoot)->getTranslation());
								Mat33f ori (TransformReadLock (m_elementRoot)->getOrientation());
								//t = ori*t;
							border = nvutil::createCallout ( t[0],t[1],t[2],bs[0]-t[0],bs[1]-t[1],bs[2]-t[2]);
							borderPlane = nvutil::createPlane (t[0],t[1],10,10,1.0,1.0,t[2]);
						}
					}

					StateSetSharedPtr ss = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
						Vec3f(0.0f,0.0f,0.9f),								 // diffuseColor,
						Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
						25.0f,								// specularExponent,
						Vec3f( 0.5f, 0.5f, 0.5f ),  // emissiveColor,
						0.2f,								// opacity,
						0.0f,								// reflectivity,

						1.0f );		

					GeoNodeSharedPtr geo = GeoNode::create();
					GeoNodeWriteLock(geo)->addDrawable(ss,border);
					GroupSharedPtr group;
					group = Group::create();

					if (borderPlane)
					{
					  GeoNodeSharedPtr geo2 = GeoNode::create();
						GeoNodeWriteLock(geo2)->addDrawable(ss,borderPlane);
						nvsg::BillboardSharedPtr billgroup = nvsg::Billboard::create ();
						BillboardWriteLock (billgroup)->setAlignment (nvsg::Billboard::BA_VIEWER);
						BillboardWriteLock (billgroup)->addChild(geo2);
						GroupWriteLock(billgroup)->addChild (geo);
						GroupWriteLock(group)->addChild (billgroup);
					}
					else
					
					 
					
					GroupWriteLock(group)->addChild (geo);
					//SceneWriteLock(TempScene)->setRootNode(group);


					NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
					TransformSharedPtr elementRoot;
					if (m_elementRoot != NULL)
					{
						elementRoot =m_elementRoot;// Transform::create();
						Group::ChildrenIterator itr = TransformWriteLock (elementRoot)->beginChildren();
						while (itr != TransformWriteLock (elementRoot)->endChildren())
						{
							TransformWriteLock (elementRoot)->removeChild (itr);
							TransformWriteLock (elementRoot)->beginChildren();
						}
					} 
					else
					{
						elementRoot =  Transform::create();
					}
					if (!m_elementRoot)
					{
					nvmath::Mat44f idmat = nvmath::Mat44f(true);
					TransformWriteLock (elementRoot)->setMatrix(idmat);
					TransformWriteLock (elementRoot)->setCenter(Vec3f(0.0f,0.0f,0.0f));
					TransformWriteLock (elementRoot)->setScaling(Vec3f(1.0f,1.0f,1.0f));
					}
					TransformWriteLock (elementRoot)->addChild(group);
					TransformWriteLock (elementRoot)->setUserData ((void*)this);
					TransformWriteLock (elementRoot)->setName ("AvocadoElement");					
					TransformWriteLock(elementRoot)->setTraversalMask (1);
					if (!m_intr.isVisible)
						TransformWriteLock(elementRoot)->setTraversalMask (0);
					if (!m_elementRoot)
					{
						TransformWriteLock (root)->addChild(elementRoot);
					//	TransformWriteLock(root)->removeChild (m_elementRoot);
						m_elementRoot = elementRoot;
					}
		}
	}
	void AvocadoEngineDocAnnotationElement::createScene(SceneSharedPtr &scene,std::string sessionFolder) 
	{
		//nvutil::loadScene (m_fileName,m_scene);
		// add the new scene root under the current view root node

		if (m_type == "SectionPlane")
		{
			SceneSharedPtr TempScene = Scene::create();
			nvmath::Box3f box = SceneReadLock(scene)->getBoundingBox();
			Vec3f bs ( box.getSize());
			DrawableSharedPtr border = nvutil::createPlaneBorders ( -bs[0]*0.5f,-bs[1]*0.5f,bs[0],1.5f*bs[1]);
			DrawableSharedPtr drawable = nvutil::createPlane ( -bs[0]*0.5f,-bs[1]*0.5f,bs[0],1.5f*bs[1]);
			StateSetSharedPtr ss = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
				Vec3f(0.0f,0.0f,0.9f),								 // diffuseColor,
				Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
				25.0f,								// specularExponent,
				Vec3f( 0.5f, 0.5f, 0.5f ),  // emissiveColor,
				0.2f,								// opacity,
				0.0f,								// reflectivity,
				1.0f );		

			StateSetSharedPtr ss2 = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
				Vec3f(0.0f,0.0f,0.0f),								 // diffuseColor,
				Vec3f( 0.0f, 0.0f,0.0f ),  // specularColor,
				1.0f,								// specularExponent,
				Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.0f,								// reflectivity,
				1.0f );		
			//StateSetWriteLock (ss)->addAttribute();
			nvsg::BlendAttributeSharedPtr bl = nvsg::BlendAttribute::create();
			BlendAttributeWriteLock (bl)->setBlendFunctions(BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);

			StateSetWriteLock(ss)->addAttribute (bl);

			vector<string> localSearchPaths;
			string err = "";
			bool f = false;

		//	localSearchPaths.push_back("C:\\Education\\AvocadoNet\\Avocado\\Release\\media\\effects\\");
			 localSearchPaths.push_back(nvutil::GetProccessDirectory() + string ("effects\\"));
		     //localSearchPaths.push_back(nvutil::GetProccessDirectory() + string ("textures\\"));

			CgFxSharedPtr cgfx = nvsg::CgFx::createFromFile("simple.cgfx",localSearchPaths,err,f);
			if (cgfx)
			{
				StateSetWriteLock(ss)->addAttribute(cgfx);
			}

#if 0
			nvsg::AlphaTestAttributeSharedPtr at = nvsg::AlphaTestAttribute::create();
			AlphaTestAttributeWriteLock (at)->setAlphaFunction(AF_ALWAYS);
			nvsg::BlendAttributeSharedPtr bl = nvsg::BlendAttribute::create();
			BlendAttributeWriteLock (bl)->setBlendFunctions(BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			nvsg::FaceAttributeSharedPtr fa = nvsg::FaceAttribute::create();
			FaceAttributeWriteLock (fa)->setCullMode(false,false);
			DepthAttributeSharedPtr da = nvsg::DepthAttribute::create();
			DepthAttributeWriteLock (da)->setDepthTestingEnabled(false);	
#endif
#if 0	
			{
				CgFxSharedPtr cgfx = nvsg::CgFx::create();
				vector<string> localSearchPaths;
				bool f = false;
				string err = "";
				string modulePath;
				string curDir;
				nvutil::GetCurrentDir(curDir);
				localSearchPaths.push_back(curDir);

				nvutil::GetModulePath(modulePath);
				if ( find( localSearchPaths.begin(), localSearchPaths.end(), modulePath ) == localSearchPaths.end() )
				{
					localSearchPaths.push_back(modulePath);
				}

				localSearchPaths.push_back(string("c:\\SceniX72/media/effects"));
				localSearchPaths.push_back(string("c:\\SceniX72/media/textures"));
				CgFxWriteLock(cgfx)->createFromFile("c:\\simple.cgfx",localSearchPaths,err,f);
				if (cgfx)
				{
					//CgFxEffectSharedPtr ef = CgFxWriteLock(cgfx)->getEffect();
					//CgFxTechnique tech = CgFxEffectWriteLock(ef)->getTechniqueByName("FixedFunctionLighting");

					//CgFxEffectWriteLock(ef)->setCurrentTechnique(tech);

					StateSetWriteLock(ss)->addAttribute(cgfx);
				}

				//CgFxWriteLock(cgfx)->li

			}
#endif	
			/*	StateSetWriteLock(ss)->addAttribute(at);
			StateSetWriteLock(ss)->addAttribute(bl);
			StateSetWriteLock(ss)->addAttribute(fa);
			StateSetWriteLock(ss)->addAttribute(da);*/

			GeoNodeSharedPtr geo = GeoNode::create();
			GeoNodeWriteLock(geo)->addDrawable(ss,drawable);
			GeoNodeWriteLock(geo)->addDrawable(ss2,border);

			GroupSharedPtr group = Group::create();
			GroupWriteLock(group)->addChild (geo);
			SceneWriteLock(TempScene)->setRootNode(group);
			//			SceneWriteLock(m_scene)->addAttribute();



			NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
			/*Add cliping planes to the root*/
			m_plane = nvsg::ClipPlane::create();
			ClipPlaneWriteLock (m_plane)->setEnabled(true);
			ClipPlaneWriteLock (m_plane)->setNormal( Vec3f(0.0f,0.0f,1.0f));
			GroupWriteLock (root)->addClipPlane (m_plane);
			NodeSharedPtr child = group;//SceneWriteLock(m_scene)->getRootNode();
			TransformSharedPtr elementRoot = Transform::create();
			nvmath::Mat44f idmat = nvmath::Mat44f(true);
			TransformWriteLock (elementRoot)->setMatrix(idmat);
			TransformWriteLock (elementRoot)->setCenter(Vec3f(0.0f,0.0f,0.0f));
			TransformWriteLock (elementRoot)->setScaling(Vec3f(1.0f,1.0f,1.0f));
			TransformWriteLock (elementRoot)->addChild(child);
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
				//node->setTraversalMask();
			}
		} else if (m_type == "SpotLight")
			{
				SceneSharedPtr TempScene = Scene::create();
					nvmath::Box3f box = SceneReadLock(scene)->getBoundingBox();
					nvmath::Sphere3f sphere = SceneReadLock(scene)->getBoundingSphere();
					Vec3f bs ( box.getSize());
					DrawableSharedPtr border ;//=
					{
						border = nvutil::createCylinder(1.0f,1.0f,64,32,true,0.0f,2.0f*nvmath::PI,true);
					}
					StateSetSharedPtr ss = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
						Vec3f(0.0f,0.0f,0.9f),								 // diffuseColor,
						Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
						25.0f,								// specularExponent,
						Vec3f( 0.5f, 0.5f, 0.5f ),  // emissiveColor,
						1.0f,								// opacity,
						0.0f,								// reflectivity,

						1.0f );		

					GeoNodeSharedPtr geo = GeoNode::create();
					GeoNodeWriteLock(geo)->addDrawable(ss,border);
					int mask = 1;
				    for (int viewId=0;viewId<8;viewId++)
					mask |= (int(pow(2.0,2*viewId+4)));// >> 1);
						nvsg::DrawableWriteLock(border)->setTraversalMask (mask);

					GroupSharedPtr group = Group::create();
					GroupWriteLock(group)->addChild (geo);
					SceneWriteLock(TempScene)->setRootNode(group);
					NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
					TransformSharedPtr elementRoot = Transform::create();
					nvmath::Mat44f idmat = nvmath::Mat44f(true);
					TransformWriteLock (elementRoot)->setMatrix(idmat);
					//TransformWriteLock (elementRoot)->setCenter(Vec3f(0.0f,0.0f,0.0f));
					//TransformWriteLock (elementRoot)->setScaling(Vec3f(1.0f,1.0f,1.0f));
					TransformWriteLock (elementRoot)->addChild(group);
					TransformWriteLock (elementRoot)->setUserData ((void*)this);
					TransformWriteLock (elementRoot)->setName ("AvocadoElement");
					TransformWriteLock(elementRoot)->setTraversalMask (1);
					m_light = nvutil::createSpotLight (Vec3f(0.0f,0.0f,0.0f),Vec3f(0.0f,0.0f,-1.0f),nvmath::PI_QUARTER,0.8f*Vec3f(1.0f,1.0f,0.9f));
					 SpotLightWriteLock (m_light)->setName ("AvocadoAnnotation");
					// SceneWriteLock(m_scene)-
						 TransformWriteLock (root)->addLightSource (m_light);
					TransformWriteLock (root)->addChild(elementRoot);
					m_elementRoot = elementRoot;
			}
			else if (m_type == "Line" || m_type == "PathLine" || m_type == "Callout")
				{
					SceneSharedPtr TempScene = Scene::create();
					nvmath::Box3f box = SceneReadLock(scene)->getBoundingBox();
					Vec3f bs ( box.getSize());
					DrawableSharedPtr border = nvutil::createLine ( -bs[0]*0.5f,-bs[1]*0.5f,bs[0],1.5f*bs[1]);
					StateSetSharedPtr ss = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
						Vec3f(0.0f,0.0f,0.9f),								 // diffuseColor,
						Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
						25.0f,								// specularExponent,
						Vec3f( 0.5f, 0.5f, 0.5f ),  // emissiveColor,
						0.2f,								// opacity,
						0.0f,								// reflectivity,

						1.0f );		

					GeoNodeSharedPtr geo = GeoNode::create();
					GeoNodeWriteLock(geo)->addDrawable(ss,border);

					GroupSharedPtr group = Group::create();
					GroupWriteLock(group)->addChild (geo);
					SceneWriteLock(TempScene)->setRootNode(group);
					NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
					TransformSharedPtr elementRoot = Transform::create();
					nvmath::Mat44f idmat = nvmath::Mat44f(true);
					TransformWriteLock (elementRoot)->setMatrix(idmat);
					TransformWriteLock (elementRoot)->setCenter(Vec3f(0.0f,0.0f,0.0f));
					TransformWriteLock (elementRoot)->setScaling(Vec3f(1.0f,1.0f,1.0f));
					TransformWriteLock (elementRoot)->addChild(group);
					TransformWriteLock (elementRoot)->setUserData ((void*)this);
					TransformWriteLock (elementRoot)->setName ("AvocadoElement");
					TransformWriteLock(elementRoot)->setTraversalMask (1);
					TransformWriteLock (root)->addChild(elementRoot);

					m_elementRoot = elementRoot;
					
				}
				if (m_type == "PathLine")
					{
						/*ParamListSharedPtr pss = ParamList::createNew ();
						pss->PushInt ("eid",this->GetID());
						pss->PushInt ("eid",m_targetEID);
						avocado::OnSendAvocadoDocGeneralStringMessage ("RegisterAttachment",m_docId,pss->SerializeList());*/
					}
				if (m_type == "Plane" || m_type == "Cube" || m_type == "Sphere" || m_type == "Cylinder")
				{
					SceneSharedPtr TempScene = Scene::create();
					nvmath::Box3f box = SceneReadLock(scene)->getBoundingBox();
					nvmath::Sphere3f sphere = SceneReadLock(scene)->getBoundingSphere();
					Vec3f bs ( box.getSize());
					DrawableSharedPtr border ;//=
		
					if (m_type == "Cube")
					{
						border = nvutil::createTessellatedBox(16);// ( -bs[0]*0.5f,-bs[1]*0.5f,bs[0],1.5f*bs[1]);
					}
					else if (m_type == "Sphere")
					{
						border = nvutil::createSphere(64,32,sphere.getRadius () * 0.7f);
					}
					else if (m_type == "Plane")
					{
						border = nvutil::createTessellatedPlane(16);
					}
					else if (m_type =="Cylinder")
					{
						border = nvutil::createCylinder(sphere.getRadius () * 0.7f*2.0f,sphere.getRadius () * 0.7f *22.f,64,32);
					}
					StateSetSharedPtr ss = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
						Vec3f(0.0f,0.0f,0.9f),								 // diffuseColor,
						Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
						25.0f,								// specularExponent,
						Vec3f( 0.5f, 0.5f, 0.5f ),  // emissiveColor,
						1.0f,								// opacity,
						0.0f,								// reflectivity,

						1.0f );		

					GeoNodeSharedPtr geo = GeoNode::create();
					GeoNodeWriteLock(geo)->addDrawable(ss,border);
					
					int mask = 1;
				    for (int viewId=0;viewId<8;viewId++)
					mask |= (int(pow(2.0,2*viewId+4)));// >> 1);
						nvsg::DrawableWriteLock(border)->setTraversalMask (mask);

					GroupSharedPtr group = Group::create();
					GroupWriteLock(group)->addChild (geo);
					SceneWriteLock(TempScene)->setRootNode(group);
					NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
					TransformSharedPtr elementRoot = Transform::create();
					nvmath::Mat44f idmat = nvmath::Mat44f(true);
					TransformWriteLock (elementRoot)->setMatrix(idmat);
					TransformWriteLock (elementRoot)->setCenter(Vec3f(0.0f,0.0f,0.0f));
					TransformWriteLock (elementRoot)->setScaling(Vec3f(1.0f,1.0f,1.0f));
					TransformWriteLock (elementRoot)->addChild(group);
					TransformWriteLock (elementRoot)->setUserData ((void*)this);
					TransformWriteLock (elementRoot)->setName ("AvocadoElement");
					TransformWriteLock(elementRoot)->setTraversalMask (1);
					TransformWriteLock (root)->addChild(elementRoot);
					m_elementRoot = elementRoot;
				}
	}
	void AvocadoEngineDocAnnotationElement::removeFromScene (SceneSharedPtr &scene)
	{
		NodeSharedPtr root = SceneWriteLock(scene)->getRootNode();
		if (m_type == "SectionPlane")
			GroupWriteLock (root)->removeClipPlane (m_plane);
		else
			if (m_type == "SpotLight")
			{
				GroupWriteLock (root)->removeLightSource (m_light);
			}
		GroupWriteLock (root)->removeChild (m_elementRoot);
	}
	void AvocadoEngineDocAnnotationElement::setLocation (float *mat,bool updateLastSaved)
	{
		nvmath::Mat44f mmat (true);
		for (int j=0;j<4;j++)
			for (int i=0;i<4;i++)
				mmat[j][i] = mat[i+4*j];
		//if (updateLastSaved)
		//m_lastSavedLocation = mmat;
		TransformWriteLock (m_elementRoot)->setMatrix (mmat );//* TransformWriteLock (m_elementRoot)->getMatrix ());
	}
	bool AvocadoEngineDocAnnotationElement::setAnnotationParam (string paramName,string valStr)
	{
		
		bool found = false;
		for (size_t i=0;i<m_intr.annotationData.size ();i++)
		{
			if (m_intr.annotationData[i].first == paramName)
			{
				m_intr.annotationData[i].second = valStr;
				found = true;
				break;
			}
		}
		if (!found)
			m_intr.annotationData.push_back (pair<string,string> ( paramName,valStr));

		if (m_type == "SpotLight")
		{
			Vec3f vColor = Vec3f (0.0f,0.0f,0.0f);
			float val = 0.0f;
			if (paramName == "SpotLightAmbient" || paramName == "SpotLightDiffuse" || paramName == "SpotLightSpecular")
			{
				int color;
				stringstream fval (valStr);
				fval >> color;
					unsigned long iColor = (unsigned long )color;
						int R = int (  (   (unsigned char)(  (unsigned long)(iColor)  & 0xff)) );
						long G = long (  ( ( (unsigned char)( ( (unsigned long)( (unsigned short)iColor)  >> 8 ) & 0xff)) ));
						long B = long (  ( ( (unsigned char)( ( (unsigned long)(iColor) ) >> 16) & 0xff) ));

					vColor = Vec3f (float(R)/255.0f,float(G)/255.0f,float(B)/255.0f);
			}
			else
			{
				stringstream fval (valStr);
				fval >> val;
			}

				if (m_light)
				{
					if (paramName == "SpotLightAmbient")
						SpotLightWriteLock (m_light)->setAmbientColor (vColor);
					else if (paramName == "SpotLightDiffuse")
						SpotLightWriteLock (m_light)->setDiffuseColor (vColor);
					else if (paramName == "SpotLightSpecular")
						SpotLightWriteLock (m_light)->setSpecularColor (vColor);
					else if (paramName == "SpotLightIntensity")
					{
						SpotLightWriteLock (m_light)->setIntensity (val);
					}
					else if (paramName == "SpotLightAttunuation")
					{
						SpotLightWriteLock (m_light)->setCutoffAngle (nvmath::PI_HALF * val);
					}
					else if (paramName == "SpotLightExponent")
					{
						SpotLightWriteLock (m_light)->setFalloffExponent (val * 10.0f);
					}
					else if (paramName == "SpotLightLinear")
					{
						Vec3f att = SpotLightWriteLock(m_light)->getAttenuation ();
						
						att[1] = val / 10.f;
						att[0] = 1.0f;//max(1.0f, 1.0f - att[1] - att[2]);
						SpotLightWriteLock (m_light)->setAttenuation (att);
					}
					else if (paramName == "SpotLightQuadric")
					{
						Vec3f att = SpotLightWriteLock(m_light)->getAttenuation ();
						att[2] = val / 100.0f;
						att[0] = 1.0f;//max(0.0f, 1.0f - att[1] - att[2]);
						SpotLightWriteLock (m_light)->setAttenuation (att);
					}
					return true;				
				}
		}

		return false;
	}
	void AvocadoEngineDocAnnotationElement::annotationMoved (SceneSharedPtr &scene)
	{
		if (m_type == "SpotLight") 
		{
			Vec3f t (TransformReadLock (m_elementRoot)->getTranslation());
			Mat33f ori (TransformReadLock (m_elementRoot)->getOrientation());

			SpotLightWriteLock (m_light)->setPosition (t); 
			Vec3f dirr = nvmath::Vec3f (0.0,0.0,1.0) * ori;
			dirr.normalize();
			SpotLightWriteLock (m_light)->setOrientation (TransformReadLock (m_elementRoot)->getOrientation());//setDirection (dirr);//-t*ori);//ori *nvmath::Vec3f (0.0,0.0,1.0));
		} else
		if (m_type == "SectionPlane")
		{
			Vec3f t (TransformReadLock (m_elementRoot)->getTranslation());
			Mat33f ori (TransformReadLock (m_elementRoot)->getOrientation());
			ori.invert();
			Vec3f za (0.0f,0.0f,1.0f);
			za= ori *za ;
			za.normalize();
			/* This code matches the clipping plane normals and offset according to the transform*/
			/* This is a pain because the clipplane defines a *half* plane so we are forced to make the adjusments. */

			float alpha = 2.0f;
			Vec3f zas (t[0],t[1],t[2]);
			ori.invert();
			zas= ori *zas ;
			//bool x = ((za[2] < 0.0f && zas[2] > 0.0f) || (za[2] > 0.0f && zas[2] < 0.0f));
			t = ori *t;
			Vec3f tnorm = t;
			tnorm.normalize ();
			//float size = sqrt(t[0]*t[0] + t[1]*t[1] + t[2]*t[2]);
			float cosa =  tnorm[0] * za[0] + tnorm[1] * za[1] + tnorm[2]*za[2];
			float size = zas[2];
			bool x = ( cosa < 0.0f);
			alpha = x  ? 1.0f : -1.0f;

			ClipPlaneWriteLock (m_plane)->setNormal(-alpha*za);
			ClipPlaneWriteLock (m_plane)->setOffset(alpha*(size));
			// works like magic.. just clean it up a biit.
		}
		return;
	}

	AvocadoAnnotations::AvocadoAnnotations (): AvocadoDocModule ("AnnotationsModule")
	{

	}

	AvocadoAnnotations::~AvocadoAnnotations ()
	{
		
		ClearDocFileElements();
	}

	AvocadoDocModule* AvocadoAnnotations::create ()
	{
		return ( (AvocadoDocModule *) new AvocadoAnnotations);
	}

	bool AvocadoAnnotations::OnUnload()
	{
		return true;
	}

	bool AvocadoAnnotations::OnRegister()
	{
		return true;
	}

	void AvocadoAnnotations::ClearDocFileElements()
	{
		m_elementHash.clear();
		for (size_t i=0;i<m_annotations.size();i++)
		{
			delete m_annotations[i];
			m_annotations[i] = NULL;
		}
		m_annotations.clear ();
	}
	AvocadoEngineDocAnnotationElement * AvocadoAnnotations::GetAnnotationPtrById (int elemId)
	{
		AvocadoEngineDocAnnotationElement * ret = 0;
		// first check cache
		AnnotationElementHasIterator hit = m_elementHash.find (elemId);
		if (hit != m_elementHash.end ())
		{
			// great
			return hit->second;
		}
		else
		{
			std::vector<AvocadoEngineDocAnnotationElement *>::iterator it = GetAnnotationById(elemId);
			if (it == m_annotations.end())
				return 0;
			ret = *it;
			m_elementHash.insert (std::pair<int,AvocadoEngineDocAnnotationElement *>(elemId,ret));
		}
		return ret;
	}
	std::vector<AvocadoEngineDocAnnotationElement *>::iterator AvocadoAnnotations::GetAnnotationById (int elemId)
	{
		std::vector<AvocadoEngineDocAnnotationElement *>::iterator it = m_annotations.begin ();
		while (it != m_annotations.end ())
		{
			if ((*it)->GetID () == elemId)
			{
				return it;
			}
			it++;
		}
		return m_annotations.end ();
	}

	int AvocadoAnnotations::AddAnnotationElement(std::string params)
	{
		int attachmentID = -1;
		int attachmentID2 = -1;
		string itype = "";
		int iid = -1;
		bool isvis = true;
		string iname = "";
		vector <pair<string,string>> t_meta;
		ParamListSharedPtr ppl = ParamList::createFromString (params);
		if (ppl->GetParam ("AnnotationType"))
		{
			StringParam *sptype = (StringParam*)ppl->GetParam ("AnnotationType");
			sptype->GetValue (itype);
		}
		if (itype !="")
		{

			if (ppl->GetParam ("Name"))
			{
				StringParam *sptype = (StringParam*)ppl->GetParam ("Name");
				sptype->GetValue (iname);
			}
			if (ppl->GetParam ("AttachmentID"))
			{
				IntParam *sptype = (IntParam*)ppl->GetParam ("AttachmentID");
				sptype->GetValue (attachmentID);
			}
			if (ppl->GetParam ("AttachmentID2"))
			{
				IntParam *sptype = (IntParam*)ppl->GetParam ("AttachmentID2");
				sptype->GetValue (attachmentID2);
			}
			if (ppl->GetParam ("AnnoID"))
			{
				IntParam *sptype = (IntParam*)ppl->GetParam ("AnnoID");
				sptype->GetValue (iid);
			}
			if (ppl->GetParam ("isVisible"))
			{
				BoolParam *sptype = (BoolParam*)ppl->GetParam ("isVisible");
				sptype->GetValue (isvis);
			}
			// start read locations
			float mat[16];
			bool hasLocation = false;
			for (int j=0;j<16;j++)
			{
				char cc[30];
				itoa(j,cc,10);
				mat[j] = 1.0f;
				if (ppl->GetParam ("Location" + string (cc)))
				{
					FloatParam *irm_vn = (FloatParam*)ppl->GetParam ("Location" + string (cc));
					if (irm_vn)
					{
						irm_vn->GetValue (mat[j]);
						hasLocation = true;
					}
				}
			}
			//end read locations
			IntParam *irm = (IntParam*)ppl->GetParam ("AnnotationParamCount");
			if (irm)
			{
				int mc = 0;
				irm->GetValue (mc);
				if (mc > 0)
				{
					for (int j=0;j<mc;j++)
					{
						char cc[30];
						itoa(j,cc,10);
						StringParam *irm_vn = (StringParam*)ppl->GetParam ("AnnotationParamName" + string (cc));
						StringParam *irm_vd = (StringParam*)ppl->GetParam ("AnnotationParamValue" + string (cc));
						string varName,varData;
						irm_vn->GetValue (varName);
						irm_vd->GetValue (varData);
						pair <string , string> newMeta (varName,varData);
						t_meta.push_back(newMeta);
					}
				}
			}
			AvocadoEngineDocAnnotationElement *docElem = new AvocadoEngineDocAnnotationElement(m_docId,m_name,itype);
			if (iid == -1)
				iid = (int)(m_smartIDGen->RequestUniqueID());
			//int id = (int)(m_smartIDGen->RequestUniqueID());
			std::stringstream idStr;
			idStr << iid;
			//std::string name = "annotationElem_Section";//  + idStr.str();
			std::string name = iname;
			//if (itype != "SectionPlane")
			if (name == "")
				name = itype+ " " + idStr.str ();
			docElem->SetID(iid);
			docElem->SetName (name);
			//docElem->m_intr.isVisible = isvis;

			docElem->setTargetAttachement (attachmentID,attachmentID2);
			docElem->createScene(m_scene,m_sessionFolder);
			if (hasLocation)
				docElem->setLocation (mat,true);
		
			docElem->annotationMoved (m_scene);
		    m_annotations.push_back (docElem);
			// Apply annotation params.
			for (size_t xx = 0;xx< t_meta.size ();xx++)
			{
				docElem->setAnnotationParam (t_meta[xx].first,t_meta[xx].second);
				//HandleAvocadoDocGeneralStringMessage ("SetAnnotationParam",m_docId,,
			}
			docElem->SetVisibility (isvis);
			return iid;
		}
		return false;
	}

	bool AvocadoAnnotations::DeleteAnnotationElement(std::vector<AvocadoEngineDocAnnotationElement *>::iterator &it)
	{
		//std::vector<AvocadoEngineDocAnnotationElement *>::iterator it = GetAnnotationById (elemId);
		
		AnnotationElementHasIterator hit= m_elementHash.find ((*it)->GetID());

		if (hit !=m_elementHash.end())
			m_elementHash.erase (hit);

		(*it)->removeFromScene (m_scene);
		delete *it;
		m_annotations.erase (it);
		return true;
	}
	void AvocadoAnnotations::OnElementSelected (int eid,int vid,bool sub)
	{
		AvocadoEngineDocAnnotationElement *it = GetAnnotationPtrById(eid);
		if (it == 0)
			return;
		if (!sub)
			(it)->m_subDrawable = 0;
		string msg = AvocadoEngineDocElement::GetParamsFromElement(it);
		OnSendAvocadoGeneralStringMessage("HightlightElement", vid, msg);
	}

	void AvocadoAnnotations::OnElementUnSelected (int eid,int vid)
	{
		AvocadoEngineDocAnnotationElement * it = GetAnnotationPtrById(eid);
		if (it == 0)
			return;
		string msg = AvocadoEngineDocElement::GetParamsFromElement(it);
		OnSendAvocadoGeneralStringMessage("UnHightlightElement", vid, msg);
	}
	void AvocadoAnnotations::OnElementPreSelected (int eid,int vid)
	{
		AvocadoEngineDocAnnotationElement * it = GetAnnotationPtrById(eid);
		if (it == 0)
			return;
		string msg = AvocadoEngineDocElement::GetParamsFromElement(it);
		OnSendAvocadoGeneralStringMessage("PreHightlightElement", vid, msg);
	}
	bool AvocadoAnnotations::AnnotationMoved(int eid)
	{
		AvocadoEngineDocAnnotationElement * it = GetAnnotationPtrById(eid);
		if (it == 0)
			return false;
		(it)->annotationMoved(m_scene);
		
		//for (size_t ki=0;ki < (it)->m_attachments.size ();ki++)
		int atts[2];
		atts[0] = (it)->GetTargetEID();
		atts[1] = (it)->GetTargetEID2();
		for (size_t ki = 0;ki<2;ki++)
		{
				//int aid = (it)->m_attachments[ki];
			    int aid = atts[ki];
				if (aid > -1)
				{
				ParamListSharedPtr ppls= ParamList::createNew ();
					ppls->PushInt ("eid",eid);
					ppls->PushInt ("eid",aid);

				avocado::OnSendAvocadoDocGeneralStringMessage ("NotifyAttachmentMove",m_docId,ppls->SerializeList());
				}
		}
		return true;
	}
	bool AvocadoAnnotations::HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint)
	{
		bool ret = false;
		
		if (msg == "AddAnnotationElement")
		{
			int elemId = AddAnnotationElement(paramStr);

			

			//AvocadoEngineDocAnnotationElement * it = 
			AvocadoEngineDocAnnotationElement *docElem = GetAnnotationPtrById(elemId);

			ParamListSharedPtr ppl = ParamList::createNew();
			//ppl->PushInt("eid",elemId);
			ParamListWriteLock(ppl)->PushPtr("elem",(void *)docElem);
			ret = OnSendAvocadoDocGeneralStringMessage("AddDocElement",docId, ParamListWriteLock(ppl)->SerializeList());

			if (docElem->GetTargetEID() > -1)
			{
			ParamListSharedPtr pss = ParamList::createNew ();
						pss->PushInt ("eid",docElem->GetID());
						pss->PushInt ("eid",docElem->GetTargetEID() );
						avocado::OnSendAvocadoDocGeneralStringMessage ("RegisterAttachment",m_docId,pss->SerializeList());
			}
			if (docElem->GetTargetEID2() > -1)
			{
			ParamListSharedPtr pss = ParamList::createNew ();
						pss->PushInt ("eid",docElem->GetID());
						pss->PushInt ("eid",docElem->GetTargetEID2() );
						avocado::OnSendAvocadoDocGeneralStringMessage ("RegisterAttachment",m_docId,pss->SerializeList());
			}
			NotifyElementsChanged ();
			needRepaint = true;
		}
		else 
			if (msg == "DeleteDocCommonElement")
			{
				ParamListSharedPtr pl = ParamList::createFromString(paramStr);
				int eid;
				bool updateUI;

				ParamListWriteLock(pl)->PopBool(updateUI);
				ParamListWriteLock(pl)->PopInt(eid);
				std::vector<AvocadoEngineDocAnnotationElement *>::iterator it = GetAnnotationById(eid);
				if (it == this->m_annotations.end ())
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
				//DeleteElementChildren (eid,docId);
				//
				// End delete children
				ParamListSharedPtr newPL = ParamList::createNew ();
				newPL->PushInt ("eid",eid);
				// currently unused - needs to be implemented !!!
				OnSendAvocadoDocGeneralStringMessage("DeleteDocElement",docId,newPL->SerializeList());
				DeleteAnnotationElement(it);
				if (updateUI)
				{
					NotifyElementsChanged ();
					needRepaint = true;
				}

				ret = false;
			} else
				if (msg == "DeleteAnnotationElement")
				{
					/*
					ParamListSharedPtr pl = ParamList::createFromString(paramStr);
					int eid;
					ParamListWriteLock(pl)->PopInt(eid);
					DeleteAnnotationElement(eid);
					// currently unused - needs to be implemented !!!
					ret = OnSendAvocadoDocGeneralStringMessage("DeleteDocElement",docId,paramStr/*ppl->SerializeList()*///);
					return false;
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
						Param *p = ParamListWriteLock(pl)->GetParam(i);
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
					ParamListSharedPtr pl = ParamList::createFromString(paramStr);
					int eid;
					
					ParamListWriteLock(pl)->PopInt (eid);
					if (AnnotationMoved (eid))
					{
						
					   needRepaint = true;
					}
				} 
				else if (msg == "SetAnnotationParam")
				{
					ParamListSharedPtr pl = ParamList::createFromString(paramStr);
					int eid;
					std::string color;
					string paramName;
					ParamListWriteLock(pl)->PopString (paramName);
					ParamListWriteLock(pl)->PopString (color);
					ParamListWriteLock(pl)->PopInt (eid);
					
					AvocadoEngineDocAnnotationElement *elem = GetAnnotationPtrById (eid);

					if (elem->setAnnotationParam(paramName,color))
					{
						NotifyElementsChanged ();
						needRepaint = true;
					}
				}
				else if (msg == "HideElement")
				{
					ParamListSharedPtr pl = ParamList::createFromString(paramStr);
					int eid,vid;
					ParamListWriteLock(pl)->PopInt (vid);
					ParamListWriteLock(pl)->PopInt (eid);
					AvocadoEngineDocAnnotationElement * it = this->GetAnnotationPtrById(eid);
					if (it !=0)
					{
						if (it->SetVisibility (false))
						{
						//(it)->m_intr.isVisible = false;
						//NodeWriteLock((it)->m_elementRoot)->setTraversalMask (0);
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
					AvocadoEngineDocAnnotationElement * it = this->GetAnnotationPtrById(eid);
					if (it != 0)
					{
						if (it->SetVisibility (true))

						//(it)->m_intr.isVisible = true;
						//NodeWriteLock((it)->m_elementRoot)->setTraversalMask (~0); // or maybe ~0 or per view..
						{
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
					std::vector<AvocadoEngineDocAnnotationElement *>::iterator it = m_annotations.begin();
					while (it != m_annotations.end())
					{
						if ((*it)->SetVisibility (true))
						{
//						if ((*it)->m_intr.isVisible == false)
//						{
//							(*it)->m_intr.isVisible = true;
//							NodeWriteLock((*it)->m_elementRoot)->setTraversalMask (~0); // or maybe ~0 or per view..
							needRepaint = true;
//						}
						}
						it++;
					}
					if (needRepaint)
						NotifyElementsChanged ();
				}
				return ret;
	}
}
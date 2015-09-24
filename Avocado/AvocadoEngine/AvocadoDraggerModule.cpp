/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoDraggerModule.h"
#include "AvocadoEngineObject.h"
#include <math.h>
#include <nvgl/RenderContextGL.h>
#include <nvtraverser/SearchTraverser.h>
#include <nvtraverser/RayIntersectTraverser.h>
#include <nvgl/RenderTargetGLFB.h>
#include <nvsg/FrustumCamera.h>
#include <nvsg/Transform.h>
#include <nvsg/Group.h>
#include <nvsg/Billboard.h>
#include <nvgl/ScenerendererGL2.h>
#include "SceneFunctions.h"
#include "MeshGenerator.h"

using namespace avocado;
using namespace nvmath;
using namespace nvsg;
namespace avocado
{
	class DraggerRotationAxisElement : public AvocadoEngineViewElement
	{
		public:
		DraggerRotationAxisElement(int m_viewId, 
						   string ownerModuleName,
			               Quatf orientation,
						   Vec3f color) :
		AvocadoEngineViewElement (m_viewId,ownerModuleName)
		{	
			m_axisOrientation = orientation;
			CreateColors(color);
			m_elementRoot = CreateAxis (orientation,color);
		}
	
		~DraggerRotationAxisElement () {}
		void BindElement ()
		{
				TransformWriteLock (m_elementRoot)->setUserData((void*)((AvocadoEngineViewElement*)this));
		}
		void SetHighlight (bool h)
		{
			if (h)
			{
				GeoNodeWriteLock (m_geoNode)->replaceStateSet (m_stateSetH,m_stateSet);
				//GeoNodeWriteLock (m_arrowGeoNode)->replaceStateSet (m_stateSetH,m_stateSet);
			}
			else
			{
				GeoNodeWriteLock (m_geoNode)->replaceStateSet (m_stateSet,m_stateSetH);
			//	GeoNodeWriteLock (m_arrowGeoNode)->replaceStateSet (m_stateSet,m_stateSetH);
			}
		}
		void CreateColors (Vec3f color)
		{
			m_stateSetH = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
													color,								 // diffuseColor,
													Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
													25.0f,								// specularExponent,
													Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
													1.0f,								// opacity,
													0.5f,								// reflectivity,
													1.0f );		
			color *= 0.75f;
			m_stateSet = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
													color,								 // diffuseColor,
													Vec3f( 0.5f, 0.5f, 0.5f ),  // specularColor,
													10.0f,								// specularExponent,
													Vec3f( 0.3f, 0.3f, 0.3f ),  // emissiveColor,
													0.3f,								// opacity,
													0.5f,								// reflectivity,
													1.0f );		

		}
		void SetFull (bool full)
		{
			if (full)
			{
				DrawableWriteLock (m_drawable)->setTraversalMask(0);
				DrawableWriteLock (m_drawableFull)->setTraversalMask(~0);
			}
			else
			{
				DrawableWriteLock (m_drawable)->setTraversalMask(~0);
				DrawableWriteLock (m_drawableFull)->setTraversalMask(0);
			}
		}
		TransformSharedPtr CreateAxis (Quatf orientation, Vec3f color)
		{
			m_drawable = nvutil::createTorus(40,80,80.0f,4.5f,true);
			m_drawableFull = nvutil::createTorus (40,80,50.0f,4.5f);
			
			DrawableWriteLock (m_drawable)->setTraversalMask(~0);
			DrawableWriteLock (m_drawableFull)->setTraversalMask(0);

		     m_geoNode = GeoNode::create();					
			GeoNodeWriteLock(m_geoNode)->addDrawable(m_stateSet,m_drawable);
			GeoNodeWriteLock(m_geoNode)->addDrawable(m_stateSet,m_drawableFull);

			TransformSharedPtr axisTransform = Transform::create ();
			TransformWriteLock (axisTransform)->setOrientation (orientation);
			TransformWriteLock (axisTransform)->setName ("AvocadoViewElement");
			TransformWriteLock (axisTransform)->addChild (m_geoNode);
			
			return axisTransform;
		}

		Quatf  m_axisOrientation;
		// Normal and Highlighted appearance.
		StateSetSharedPtr m_stateSet;
		StateSetSharedPtr m_stateSetH;
		// Axis geomtry.
		DrawableSharedPtr m_drawable;
		DrawableSharedPtr m_drawableFull;
		GeoNodeSharedPtr m_geoNode;
	};

	class DraggerScaleElement : public AvocadoEngineViewElement
	{
		public:
		DraggerScaleElement(int m_viewId, 
						   string ownerModuleName,
						   Vec3f color) :
		AvocadoEngineViewElement (m_viewId,ownerModuleName)
		{	
			CreateColors(color);
			m_elementRoot = CreateAxis (color);
		
		}
	
		~DraggerScaleElement () {}
		void BindElement ()
		{
				TransformWriteLock (m_elementRoot)->setUserData((void*)((AvocadoEngineViewElement*)this));
		}
		void SetHighlight (bool h)
		{
			if (h)
			{
				GeoNodeWriteLock (m_geoNode)->replaceStateSet (m_stateSetH,m_stateSet);
				
			}
			else
			{
				GeoNodeWriteLock (m_geoNode)->replaceStateSet (m_stateSet,m_stateSetH);
			
			}
		}
		void CreateColors (Vec3f color)
		{
			m_stateSetH = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
													color,								 // diffuseColor,
													Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
													25.0f,								// specularExponent,
													Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
													1.0f,								// opacity,
													0.5f,								// reflectivity,
													1.0f );		

			color *= 0.75f;
			m_stateSet = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
													color,								 // diffuseColor,
													Vec3f( 0.5f, 0.5f, 0.5f ),  // specularColor,
													10.0f,								// specularExponent,
													Vec3f( 0.3f, 0.3f, 0.3f ),  // emissiveColor,
													0.3f,								// opacity,
													0.5f,								// reflectivity,
													1.0f );		

		}
		TransformSharedPtr CreateAxis (Vec3f color)
		{
			m_drawable = nvutil::createSphere(40,80,16.0f);//(40,80,80.0f,4.5f,true);
		    m_geoNode = GeoNode::create();					
			GeoNodeWriteLock(m_geoNode)->addDrawable(m_stateSet,m_drawable);

			TransformSharedPtr axisTransform = Transform::create ();
			//TransformWriteLock (axisTransform)->setOrientation (orientation);
			TransformWriteLock (axisTransform)->setName ("AvocadoViewElement");
			TransformWriteLock (axisTransform)->addChild (m_geoNode);
			
			return axisTransform;
		}
		// Normal and Highlighted appearance.
		StateSetSharedPtr m_stateSet;
		StateSetSharedPtr m_stateSetH;
		// Axis geomtry.
		DrawableSharedPtr m_drawable;
		GeoNodeSharedPtr m_geoNode;
	};

	class DraggerAxisElement : public AvocadoEngineViewElement
	{
		public:
		DraggerAxisElement(int m_viewId, 
						   string ownerModuleName,
			               Quatf orientation,
						   Vec3f color) :
		AvocadoEngineViewElement (m_viewId,ownerModuleName)
		{	
			m_axisOrientation = orientation;
			CreateColors(color);
			m_elementRoot = CreateAxis (orientation,color);
		
		}
	
		~DraggerAxisElement () 
		{
			if (m_elementRoot) 
			TransformWriteLock (m_elementRoot)->setUserData(0);
		}
		void BindElement ()
		{
				TransformWriteLock (m_elementRoot)->setUserData((void*)((AvocadoEngineViewElement*)this));
		}
		void SetHighlight (bool h)
		{
			if (h)
			{
				GeoNodeWriteLock (m_geoNode)->replaceStateSet (m_stateSetH,m_stateSet);
				GeoNodeWriteLock (m_arrowGeoNode)->replaceStateSet (m_AstateSetH,m_AstateSet);
			}
			else
			{
				GeoNodeWriteLock (m_geoNode)->replaceStateSet (m_stateSet,m_stateSetH);
				GeoNodeWriteLock (m_arrowGeoNode)->replaceStateSet (m_AstateSet,m_AstateSetH);
			}
		}
		void CreateColors (Vec3f color)
		{

			m_stateSetH = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
													color,								 // diffuseColor,
													Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
													25.0f,								// specularExponent,
													Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
													1.0f,								// opacity,
													0.5f,								// reflectivity,
													1.0f );	
			m_AstateSetH = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
													color,								 // diffuseColor,
													Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
													25.0f,								// specularExponent,
													Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
													1.0f,								// opacity,
													0.5f,								// reflectivity,
													1.0f );	

			color *= 0.75f;
			m_stateSet = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
													color,								 // diffuseColor,
													Vec3f( 0.5f, 0.5f, 0.5f),  // specularColor,
													15.0f,								// specularExponent,
													Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
													1.0f,								// opacity,
													0.5f,								// reflectivity,
													1.0f );	
			m_AstateSet = nvutil::createDefinedMaterial(Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
													color,								 // diffuseColor,
													Vec3f( 0.5f, 0.5f, 0.5f),  // specularColor,
													15.0f,								// specularExponent,
													Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
													1.0f,								// opacity,
													0.5f,								// reflectivity,
													1.0f );	

		}
		TransformSharedPtr CreateAxis (Quatf orientation, Vec3f color)
		{
			const float ds = 1.00f;
			 m_drawable = nvutil::createCylinder (6.0f*ds,110.0f*ds,64,32);
			
			 m_geoNode = GeoNode::create();					
			GeoNodeWriteLock(m_geoNode)->addDrawable(m_stateSet,m_drawable);

			  m_arrowDrawable =  nvutil::createCylinder (10.0f*ds,30.0f*ds,64,32,true,0.0f,2.0f*PI,true);//nvutil::createTetrahedron();
			  m_arrowGeoNode = GeoNode::create();
			GeoNodeWriteLock(m_arrowGeoNode)->addDrawable(m_AstateSet,m_arrowDrawable);

			// This fixes the X (red) axis direction. No idea why.
			color[0] = -color[0];
			
			TransformSharedPtr axisArrowTransform = Transform::create ();
		//	TransformWriteLock (axisArrowTransform)->setScaling (Vec3f(10.0,10.0,10.0));
			TransformWriteLock (axisArrowTransform)->setTranslation (125.0*ds*Vec3f(color[0],color[1],color[2]));
			
			TransformWriteLock (axisArrowTransform)->setOrientation (orientation);
		    TransformWriteLock (axisArrowTransform)->addChild (m_arrowGeoNode);
			
			TransformSharedPtr axisTransform = Transform::create ();
			TransformWriteLock (axisTransform)->setOrientation (orientation);
			TransformWriteLock (axisTransform)->setTranslation (ds*55.0*Vec3f(color[0],color[1],color[2]));
		    //TransformWriteLock (axisTransform)->setName ("AvocadoViewElement");
			TransformWriteLock (axisTransform)->addChild (m_geoNode);
			TransformSharedPtr mainTrans = Transform::create ();
			 TransformWriteLock (mainTrans)->setName ("AvocadoViewElement");
			TransformWriteLock (mainTrans)->addChild (axisTransform);
			TransformWriteLock (mainTrans)->addChild (axisArrowTransform);

			color[0] = - color[0];
			return mainTrans;
		}

		Quatf  m_axisOrientation;
		// Normal and Highlighted appearance.
		StateSetSharedPtr m_stateSet;
		StateSetSharedPtr m_stateSetH;
		StateSetSharedPtr m_AstateSet;
		StateSetSharedPtr m_AstateSetH;
		// Axis geomtry.
		DrawableSharedPtr m_drawable;
		GeoNodeSharedPtr m_geoNode;
		// Arrow geometry.
		nvsg::DrawableSharedPtr m_arrowDrawable;
		GeoNodeSharedPtr m_arrowGeoNode;
	};

	AvocadoDragger::AvocadoDragger ()
	: AvocadoViewModule ("DraggerModule")
	{
			m_currentElementOwner = "";
			m_currentElementID = -1;
			m_enableRotate=false;
			m_enableScale=false;
			m_enableTranslate=false;
	}

	AvocadoDragger::~AvocadoDragger ()
	{
	}

	AvocadoViewModule* AvocadoDragger::create ()
	{
		return ( (AvocadoViewModule *) new AvocadoDragger);
	}
	void AvocadoDragger::CreateFrontViewCamera ()
	{
	/*	if (!m_frontViewState)
			return;
		CreateEmptyCamera (m_frontViewState);
		nvutil::createDefaultHeadLight (ViewStateReadLock(m_frontViewState)->getCamera());*/
	}

	bool AvocadoDragger::OnUnload ()
	{
		for (size_t i=0;i<m_draggerElements.size();i++)
		{
			if (i < 3)
			{
				DraggerAxisElement* d = (DraggerAxisElement*)m_draggerElements[i];
				delete d;
			} else
				if (i<6)
				{
					DraggerRotationAxisElement* d = (DraggerRotationAxisElement*)m_draggerElements[i];
					delete d;
				}
				else
				{
					DraggerScaleElement* d = (DraggerScaleElement*)m_draggerElements[i];
					delete d;
				}
		}
		m_draggerElements.clear();
		m_draggerRoot = 0;
		m_currentElement = 0;
		m_currentSelecion = 0;
		return true;
	}

	bool AvocadoDragger::OnRegister ()
	{
			m_draggerAxisPicked = false;
			CreateDraggerRoot();
			//CreateFrontViewCamera ();
			m_axis = 0;
			m_draggerVisible = false;
			m_first = true;
			m_x = 0;
			m_currentPrePicked = -1;
			m_y = 0;
	//	optimizeUnifyVertices ((ViewStateReadLock (viewState)->getScene()));
		return true;
	}
	void AvocadoDragger::AddAxisElement (Quatf ori, Vec3f color)
	{
		DraggerAxisElement *axis = new DraggerAxisElement (m_viewId,
										  m_name,
										  ori,
										  color);

		axis->SetID(int (m_draggerElements.size()));
		m_draggerElements.push_back ( (AvocadoEngineViewElement*)axis );
		axis->BindElement();
	}
	void AvocadoDragger::AddRotationAxisElement (Quatf ori, Vec3f color)
	{
		DraggerRotationAxisElement *axis = new DraggerRotationAxisElement (m_viewId,
										  m_name,
										  ori,
										  color);

		axis->SetID(int (m_draggerElements.size()));
		m_draggerElements.push_back ( (AvocadoEngineViewElement*)axis );
		axis->BindElement();
	}
	void AvocadoDragger::AddScaleElement (Vec3f color)
	{
		DraggerScaleElement *axis = new DraggerScaleElement (m_viewId,
										  m_name,
										  color);

		axis->SetID(int (m_draggerElements.size()));
		m_draggerElements.push_back ( (AvocadoEngineViewElement*)axis );
		axis->BindElement();
	}
	void AvocadoDragger::CreateDraggerRoot()
	{
		/* Verify that dragger was not already initialized. */
		if (m_draggerRoot)
			return;
		if (m_draggerElements.size())
			return;

		/* X Axis */
		AddAxisElement(Quatf(0.0,0.0,1.0,1.0),
										Vec3f(1.0,0.0,0.0));

		/* Y Axis */
		AddAxisElement(Quatf(0.0,0.0,0.0,1.0),
										Vec3f(0.0,1.0,0.0));
		/* Z Axis */
		AddAxisElement(Quatf(1.0,0.0,0.0,1.0),
										Vec3f(0.0,0.0,1.0));

		/* X Rotation-Axis */
		AddRotationAxisElement(Quatf(0.0,0.0,1.0,-1.0),
										Vec3f(1.0,0.0,0.0));
		/* Y Rotation-Axis */
		AddRotationAxisElement(Quatf(0.0,0.0,0.0,1.0),
										Vec3f(0.0,1.0,0.0));

		/* Z Rotation Axis */
		AddRotationAxisElement(Quatf(1.0,0.0,0.0,-1.0),
										Vec3f(0.0,0.0,1.0));
		
	
		/* Scale ball */
		AddScaleElement(Vec3f(1.0,1.0,1.0));

		m_draggerRoot = Transform::create ();
		for (size_t i=0;i<m_draggerElements.size();i++)
		{
			TransformWriteLock (m_draggerRoot)->addChild (m_draggerElements[i]->m_elementRoot);
		}	
	}

	bool AvocadoDragger::HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint)
	{
		bool ret = false;

		switch (msg)
		{
		case AVC_MOUSE_MOVE:
			{
				if (!(zDelta & 0x0001) && (!m_first))
				{
					  m_draggerAxisPicked = false;
					  m_first = true;
				}
				ret = OnMouseMove (x, y, needRepaint);
				break;
			}
		case AVC_MOUSE_LUP:
			{
				ret = OnMouseLUp ();
				break;
			}
		};
		return ret;
	}

	bool AvocadoDragger::HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string ("Module: " + m_name + string(" - ") + msg + string(" (")+paramStr+ string(")\n")).c_str());

		bool ret = false;

		if (msg == "HightlightElement")
		{

			AvocadoEngineDocElement* docElem =AvocadoEngineDocElement::GetElementFromParams(paramStr);// (AvocadoEngineDocElement*)ptr;
			GroupSharedPtr element = docElem->m_elementRoot;
			if (docElem->m_elementRoot != m_currentElement)
			{
				m_currentElementOwner = "";
				m_currentElementID = -1;
				m_currentElement = 0;
				m_currentSub = 0;
				if (m_draggerVisible)
					needRepaint = true;
				/*ret =*/ OnUnPick ();
			}
			if (element)
			{
				m_currentElement = element;
				m_currentElementOwner = docElem->m_ownerModule;
				m_currentSub = docElem->m_subDrawable;
				m_currentElementID = docElem->GetID();
				m_currentElementDocID = docElem->m_docId;
				
				if (docElem->m_refGeoNode == "SectionPlane" && docElem->m_ownerModule == "AnnotationsModule")
					//GetName() == "annotationElem_Section")
				{
						for (size_t i=0;i<m_draggerElements.size();i++)
						{
							
							TransformWriteLock (m_draggerElements[i]->m_elementRoot)->setTraversalMask (~0);//->m_elementRoot);
						}
						((DraggerRotationAxisElement*)m_draggerElements[3])->SetFull(true);
						((DraggerRotationAxisElement*)m_draggerElements[4])->SetFull(true);

						TransformWriteLock (m_draggerElements[0]->m_elementRoot)->setTraversalMask (0);
						TransformWriteLock (m_draggerElements[1]->m_elementRoot)->setTraversalMask (0);
						TransformWriteLock (m_draggerElements[5]->m_elementRoot)->setTraversalMask (0);
						//TransformWriteLock (m_draggerElements[i])->setTraversalMask (0);
				}
				else
				{					
						/*for (size_t i=0;i<m_draggerElements.size();i++)
						{
							TransformWriteLock (m_draggerElements[i]->m_elementRoot)->setTraversalMask (~0);
						}*/
						UpdateVisibilities ();
						((DraggerRotationAxisElement*)m_draggerElements[3])->SetFull(false);
						((DraggerRotationAxisElement*)m_draggerElements[4])->SetFull(false);
				}
				OnPick();

				if (m_draggerVisible)
					needRepaint = true;
			}
		}
		else if (msg == "UnHightlightElement")
		{
			AvocadoEngineDocElement* docElem = AvocadoEngineDocElement::GetElementFromParams(paramStr);//(AvocadoEngineDocElement*)ptr;
			if (docElem->m_elementRoot == m_currentElement)
			{
			m_currentElementOwner = "";
			m_currentElementID = -1;
			m_currentElement = 0;
			m_currentSub = 0;
			if (m_draggerVisible)
				needRepaint = true;
			/*ret =*/ OnUnPick ();
			}
		}
		else if (msg == "CameraChanged")
		{
			if (m_draggerVisible)
				SetDraggerOrientation();
		}
		else if (msg == "NotifyDocElementMove")
		{
			if (m_draggerVisible)
			{
				// TODO : ONLY REPAINT IF DRAGGER ACTUALLY MOVES
				SetDraggerOrientation();
				needRepaint = true;
			}
		}
		else if ((msg == "UnPreHightlightElement") || (msg == "PreHightlightElement"))
		{
			if (m_draggerVisible && m_first)
				OnDraggerUnPrePick (needRepaint);
		}
		else if (msg == "OnViewPick")
		{
			int axis;
			bool prePick;
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			 ParamListWriteLock(pl)->PopInt( axis);
			 ParamListWriteLock(pl)->PopBool(prePick);
			if (m_draggerVisible)
			{
				if (!prePick)
					ret = OnDraggerPick (axis);
				else
				{	
					if (m_first)
					OnDraggerPrePick (axis,needRepaint);	
				}
			}
		}
		else if ( msg == "SetViewParam")
		{
			string srcname;
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			if (pl->IsEmpty())
				return false;
			Param* p = pl->GetParam(0);
					string pval;
			((StringParam*)p)->GetName (srcname);
			((StringParam*)p)->GetValue (pval);
			if (srcname == "DraggerFilters")
			{
				int iis;
				stringstream ss (pval);
				ss >> iis;
				m_enableRotate = (!(iis & 2) == 0);
				m_enableScale = (!(iis & 4) == 0);
				m_enableTranslate = (!(iis & 1) == 0);
				UpdateVisibilities ();
				needRepaint = true;
			}
		}
		return ret;
	}
	void AvocadoDragger::UpdateVisibilities ()
	{
		int scaleMask = ~0;
		int rotMask = ~0;
		int transMask = ~0;

		if (!m_enableScale)
			scaleMask = 0;
		if (!m_enableTranslate)
			transMask = 0;
		if (!m_enableRotate)
			rotMask = 0;

		TransformWriteLock (m_draggerElements[0]->m_elementRoot)->setTraversalMask (transMask);
		TransformWriteLock (m_draggerElements[1]->m_elementRoot)->setTraversalMask (transMask);
		TransformWriteLock (m_draggerElements[2]->m_elementRoot)->setTraversalMask (transMask);

		TransformWriteLock (m_draggerElements[3]->m_elementRoot)->setTraversalMask (rotMask);
		TransformWriteLock (m_draggerElements[4]->m_elementRoot)->setTraversalMask (rotMask);
		TransformWriteLock (m_draggerElements[5]->m_elementRoot)->setTraversalMask (rotMask);

		TransformWriteLock (m_draggerElements[6]->m_elementRoot)->setTraversalMask (scaleMask);

	}
	 Vec3f AvocadoDragger::GetElementCenter ()
	{
		Vec3f ElementCenter ;
		if (m_currentSub)
		{
			nvsg::Drawable::OwnerIterator it ;
			ObjectWeakPtr obj = nvsg::DrawableReadLock (m_currentSub)->getOwner((nvsg::DrawableReadLock (m_currentSub)->ownersBegin()));
			ObjectWeakPtr obj2 = nvsg::GeoNodeReadLock (obj)->getOwner((nvsg::GeoNodeReadLock (obj)->ownersBegin()));
			
			ElementCenter =	NodeReadLock (obj2)->getBoundingBox().getCenter() ;
			Trafo ElementMat  ;
			ElementMat.setIdentity ();
			ElementMat.setTranslation (ElementCenter);

			while (obj2)
			{
				GroupWeakPtr objww = 0;
				if (nvsg::GroupReadLock (obj2)->ownersBegin() !=
				    nvsg::GroupReadLock (obj2)->ownersEnd())
				{
				 objww = nvsg::GroupReadLock (obj2)->getOwner((nvsg::GroupReadLock (obj2)->ownersBegin()));
				}
				//TransformSharedPtr tsw (objww);
				obj2=objww;
				if (obj2)
				{
					TransformWeakPtr parentTTF = dynamic_cast< TransformWeakPtr >( objww );
					
					if (parentTTF&& m_currentElement != parentTTF)
					{
						Trafo parentTrfooo;
						parentTrfooo = TransformReadLock (parentTTF)->getTrafo();
						ElementMat.setMatrix (  ElementMat.getMatrix () * parentTrfooo.getMatrix());
					}
				}
					//ElementCenter +=  (TransformReadLock (parentTTF)->getTranslation() );
			}

			ElementCenter = ElementMat.getTranslation ();
			Group::ChildrenConstIterator itt = (TransformReadLock (m_currentElement))->beginChildren ();
			GroupSharedPtr gg (nvutil::sharedPtr_cast<Group>(*itt));
		//	ElementCenter +=  (TransformReadLock (gg)->getTranslation() );	


			ElementCenter*= TransformReadLock (m_currentElement)->getScaling()[0];
			ElementCenter = ElementCenter * Mat33f(TransformReadLock (m_currentElement)->getOrientation()) ;
			ElementCenter+= (TransformReadLock (m_currentElement)->getTranslation());

		}
		else
		{
			ElementCenter =((TransformReadLock (m_currentElement)->getBoundingBox()).getCenter() );
		}

		return ElementCenter;
	}
	 Vec3f AvocadoDragger::SetDraggerOrientation2(Vec3f invec)
	 {
		 Trafo tra,FrontViewToWorld, ViewToWorld,ViewProj,Element;
		Vec3f ElementCenter;
		ElementCenter = invec;
		Element.setTranslation (ElementCenter);
		Element.setOrientation(TransformReadLock (m_currentElement)->getOrientation());//
		// regular scene space.
		Trafo viewProj,worldToView;
		viewProj.setMatrix (CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getProjection ());
		worldToView.setMatrix(CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getWorldToViewMatrix ());
		// front view scene space..
		Trafo f_viewProjI,f_worldToViewI;
		f_viewProjI.setMatrix (CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getInverseProjection ());
		f_worldToViewI.setMatrix(CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getViewToWorldMatrix ());

		//Element.setMatrix ( worldToView.getMatrix() * Element.getMatrix() );
		Element.setMatrix ( viewProj.getMatrix() * Element.getMatrix() );
		Element.setMatrix (f_viewProjI.getMatrix() * Element.getMatrix());
		//Element.setMatrix (Element.getMatrix()*f_worldToViewI.getMatrix() );

		ElementCenter = Element.getTranslation ();
		Vec3f draggerCenter = ElementCenter;
		return draggerCenter;
	 }

	 void AvocadoDragger::SetDraggerOrientation ()
	 {
		Trafo tra,FrontViewToWorld, WorldToView,ViewProj,Element,viewProj,f_viewProjI;
		Vec3f ElementCenter;
		ElementCenter = GetElementCenter();
		Element.setTranslation (ElementCenter);
		
		tra.setOrientation(CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getOrientation());
		WorldToView.setMatrix (CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getWorldToViewMatrix());
		FrontViewToWorld.setMatrix (CameraReadLock(ViewStateReadLock(m_frontViewState)->getCamera())->getViewToWorldMatrix());
		f_viewProjI.setMatrix (CameraReadLock(ViewStateReadLock(m_frontViewState)->getCamera())->getInverseProjection ());
		//f_worldToViewI.setMatrix(CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getViewToWorldMatrix ());
		viewProj.setMatrix (CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getProjection ());

		Element.setMatrix ( Element.getMatrix()*WorldToView.getMatrix()*viewProj.getMatrix()  *f_viewProjI.getMatrix()   * FrontViewToWorld.getMatrix()  );
		tra.setCenter( Element.getTranslation());


		//Element.setMatrix (  Element.getMatrix() *tra.getMatrix() );
		//Element.setOrientation(WorldToView.getOrientation());
		//Element.setMatrix  (Element.getMatrix()  *FrontViewToWorld.getInverse()/*f_viewProjI.getMatrix() */  ) ;

		 
		ElementCenter = Element.getTranslation ();

		float Zdelta = max (0.001f,0.3f*ElementCenter[2]/-500.5f);

		TransformWriteLock(m_draggerRoot)->setTranslation(ElementCenter);

		Mat33f orii (TransformReadLock (m_currentElement)->getOrientation());
		Mat33f oritt   (WorldToView.getOrientation() );

		TransformWriteLock(m_draggerRoot)->setOrientation(Quatf (orii*oritt));
		TransformWriteLock(m_draggerRoot)->setScaling(1.1f*Vec3f(Zdelta,Zdelta,Zdelta));
	 }
#if 0
	void AvocadoDragger::SetDraggerOrientation()
	{
		Trafo tra,FrontViewToWorld, ViewToWorld,ViewProj,Element;
		Vec3f ElementCenter;
		ElementCenter = GetElementCenter();
		Element.setTranslation (ElementCenter);
		
		tra.setOrientation(CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getOrientation());

		ViewToWorld.setMatrix (CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getWorldToViewMatrix());
		
		FrontViewToWorld.setMatrix (CameraReadLock(ViewStateReadLock(m_frontViewState)->getCamera())->getViewToWorldMatrix());
		
		Element.setMatrix ( Element.getMatrix()*ViewToWorld.getMatrix()   * FrontViewToWorld.getMatrix()  );
		tra.setCenter( Element.getTranslation());
		Element.setMatrix (  Element.getMatrix() *tra.getMatrix() );

		ElementCenter = Element.getTranslation();

		Element.setOrientation(ViewToWorld.getOrientation());
		Mat44f pos = (Element.getMatrix() * FrontViewToWorld.getInverse()) ;
		Trafo pp ;
		pp.setMatrix(pos);
		ElementCenter = pp.getTranslation ();
		
		float scfff = 50.0f / ElementCenter[2] ;
		float Zdelta = (ElementCenter[2]/-600.5f);//
		//float disssf = CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getFocusDistance ();
		//Zdelta = ElementCenter[2] / (-600.0f);
		//float Zdelta = -scfff/600.0f;
		pp.setTranslation (ElementCenter);
		pp.setMatrix (pp.getMatrix ()*FrontViewToWorld.getMatrix());
		
		TransformWriteLock(m_draggerRoot)->setTranslation(ElementCenter);

		//SetDraggerOrientation2();
		///
		///
		Mat33f orii (TransformReadLock (m_currentElement)->getOrientation());
		Mat33f oritt   (ViewToWorld.getOrientation() );
		

		//Element.setOrientation );
		///
		//Element.setOrientation(/*ViewToWorld.getOrientation()*/);
		TransformWriteLock(m_draggerRoot)->setOrientation(/*Element.getOrientation()*/ Quatf (orii*oritt));
		//
		//TransformWriteLock(m_draggerRoot)->setOrientation(ViewToWorld.getOrientation());
		//
		//TransformWriteLock(m_draggerRoot)->setScaling(Vec3f(10.0f,10.0f,10.0f));
		//static int scaleset = 1;
		TransformWriteLock(m_draggerRoot)->setScaling(1.1f*Vec3f(Zdelta,Zdelta,Zdelta));
		if (0)
		{
		//TransformWriteLock(m_draggerRoot)->setScaling(Vec3f(Zdelta,Zdelta,Zdelta));
		float camZ = CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getPosition()[2];
		Sphere3f spge = TransformWriteLock(m_draggerRoot)->getBoundingSphere();
		float myradius = spge.getRadius();
		float myprojectedradius = myradius ;/// camZ;
		float mynewradiud = 10.0f/myprojectedradius;
		float myscale = mynewradiud / myradius;
		TransformWriteLock(m_draggerRoot)->setScaling(Vec3f(myscale,myscale,myscale));
		//scaleset = 1;
		}
		ZoomCameraToDragger ();

	}

#endif
	void AvocadoDragger::ZoomCameraToDragger ()
	{
		if (!m_frontViewState)
			return;
		if (!m_draggerRoot)
			return;
		ZoomCameraToNode (m_frontViewState,m_draggerRoot);
	}

	bool AvocadoDragger::OnPick ()
	{	
		if (!m_draggerVisible || m_currentSub)
		{
			
			if (!m_draggerVisible)
			{
			NodeSharedPtr frontlayerRoot = SceneReadLock (ViewStateReadLock (m_frontViewState)->getScene())->getRootNode();
			GroupWriteLock(frontlayerRoot)->addChild (m_draggerRoot);
			
			ZoomCameraToDragger ();
			}
			SetDraggerOrientation ();
			m_draggerVisible = true;
			m_first = true;
			m_draggerAxisPicked = false;	
		} 
		else if (m_draggerVisible)
		{
			m_draggerAxisPicked = false;	
		}
		return true;
	}

	bool AvocadoDragger::OnUnPick ()
	{
		if (m_draggerVisible)
		{
			NodeSharedPtr frontlayerRoot = SceneReadLock (ViewStateReadLock (m_frontViewState)->getScene())->getRootNode();	
			GroupWriteLock (frontlayerRoot)->removeChild (m_draggerRoot);
							
			m_draggerVisible = false;
			m_draggerAxisPicked = false;
			m_currentSelecion = NULL;
			m_first = true;
			
		//	InvokePaintView (m_viewId);
		}
		return false;
	}
	bool AvocadoDragger::OnDraggerPrePick (int x,bool &needRepaint)
	{
		if (x != m_currentPrePicked)
		{
			if (m_currentPrePicked!= -1)
			{
				if (m_currentPrePicked < 3)
				((DraggerAxisElement*)m_draggerElements[m_currentPrePicked])->SetHighlight(false);
				else if (m_currentPrePicked <6)
				((DraggerRotationAxisElement*)m_draggerElements[m_currentPrePicked])->SetHighlight(false);
				else
				((DraggerScaleElement*)m_draggerElements[m_currentPrePicked])->SetHighlight(false);		
			
			}
			//currentPrePicked 
			if (x < 3)
				((DraggerAxisElement*)m_draggerElements[x])->SetHighlight(true);
			else if (x <6)
				((DraggerRotationAxisElement*)m_draggerElements[x])->SetHighlight(true);
			else
				((DraggerScaleElement*)m_draggerElements[x])->SetHighlight(true);
			
			m_currentPrePicked = x;
			needRepaint = true;
		}
		return true;
	}
	bool AvocadoDragger::OnDraggerUnPrePick (bool &needRepaint)
	{
		if (m_currentPrePicked != -1)
		{
			if (m_currentPrePicked< 3)
				((DraggerAxisElement*)m_draggerElements[m_currentPrePicked])->SetHighlight(false);
			else if (m_currentPrePicked< 6)
				((DraggerRotationAxisElement*)m_draggerElements[m_currentPrePicked])->SetHighlight(false);
		    else
				((DraggerScaleElement*)m_draggerElements[m_currentPrePicked])->SetHighlight(false);

			m_currentPrePicked = -1;
			needRepaint = true;
		}
		return true;
	}
	bool AvocadoDragger::OnDraggerPick (int x)
	{
		if (m_axis == x)	
		m_draggerAxisPicked = !m_draggerAxisPicked;
		else
		{
			m_draggerAxisPicked = true;
			m_axis = x;
			m_first = true;
		}
		return true;
	}
	bool AvocadoDragger::FixRotationDir (float *p_x,float *p_y)
	{
		
		if (m_axis == 3)
		{
			Vec3f xvec(0.0f,float(*p_x),float(*p_y));
			Vec3f yvec(0.0f,0.0f,float(*p_y));
			Vec3f xaxis (0.0f,1.0f,1.0f);
			Vec3f yaxis (0.0f,0.0f,-1.0f);
			Quatf ori (CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getOrientation());
			Mat33f Mat (ori) ;
			Mat33f ElementMat ((TransformReadLock (m_currentElement)->getOrientation())) ;
			Mat.invert();
			xaxis =  ElementMat*Mat/*ElementMat*/ *xaxis;
			xvec =  ElementMat*Mat/*ElementMat*/ *xvec;
			xvec.normalize();
			xaxis.normalize();
			float xcosa = xvec[0]*xaxis[0] + xvec[1]*xaxis[1] + xvec[2]*xaxis[2];

		   if (xaxis[1] < 0.0f)//xcosa < 0 )
				return true;
		//	else
		//		return false;
		}
		else
		if (m_axis == 4)
		{
		}
		else
		if (m_axis ==5)
		{
		}
		return false;
	}
	void AvocadoDragger::GetDeltas (float *p_x,float *p_y)
	{
		Vec3f vec(float(*p_x),float(*p_y),0.0f);
		Trafo ViewToWorld,tvec;

		tvec.setTranslation (vec);
		ViewToWorld.setMatrix (CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getInverseProjection());
		tvec.setMatrix (tvec.getMatrix() * ViewToWorld.getMatrix());

		Quatf ori (CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getOrientation());
		Mat33f Mat (ori) ;
		Mat33f ElementMat ((TransformReadLock (m_currentElement)->getOrientation())) ;
		Mat.invert();
		//ElementMat.invert();
		Vec3f xaxis (1.0,0.0,0.0);
		Vec3f yaxis (0.0,1.0,0.0);
		Vec3f zaxis (0.0,1.0,0.0);
		xaxis =  ElementMat*Mat/*ElementMat*/ *xaxis;
		yaxis =  ElementMat*Mat/*ElementMat */*yaxis;
		zaxis =  ElementMat*Mat/*ElementMat */*zaxis;
		// TODO : Add per axis intensity..
		float camZ  = TransformWriteLock(m_draggerRoot)->getScaling()[0];
		int axis = m_axis;
		if (m_axis > 2)
		 axis = m_axis - 3;
		{
			if (xaxis[axis]  < 0.0 )
			{
				(*p_x) = (*p_x) *(-1);
			}
			if (yaxis[axis]  > 0.0  )
			{
				(*p_y) = (*p_y) *(-1);
			}
		}
		(*p_y) = float((*p_y)) *(camZ);
		(*p_x) = float((*p_x)) *(camZ);

	}

	bool AvocadoDragger::OnMouseMove (int x, int y, bool &needRepaint)
	{
		if (m_draggerAxisPicked)
		{
			int vpH = m_renderTarget->getHeight();
		//	 y = vpH - y;
			if (m_first)
			{
				m_first = false;
				m_x = x;
				m_y = y;
				return true;
			}
			float p_dx = float(x- m_x );
			float p_dy = float(y - m_y);
			if (m_axis<3)
			 GetDeltas (&p_dx,&p_dy);
			if (m_draggerRoot && m_currentElement)
			{
				//m_y = y;
				//m_x = x;
				
				TransformWriteLock r2 (m_currentElement);
				// Assaf - Temp 
				if (m_axis < 3)
				{
					Vec3f p = r2->getTranslation ();
					Mat33f ElementMat ((TransformReadLock (m_currentElement)->getOrientation())) ;
					ElementMat.invert();
					Vec3f delv (0.0f,0.0f,0.0f);
					delv[m_axis] +=((p_dx+p_dy) );
					delv = ElementMat*delv;
					//p[m_axis] += ((float(p_dx)+float(p_dy)) );
					p+=delv;
					m_y = y;
					m_x = x;
				
					//r2->setCenter  (r2->getBoundingBox().getCenter()  );
					r2->setTranslation  (p );

					// this is ment to fix the center of rotation.
					/*
					Group::ChildrenIterator itt = (r2)->beginChildren ();
					TransformSharedPtr gg (nvutil::sharedPtr_cast<Transform>(*itt));
					if (gg)
					{
						Vec3f trasss = TransformReadLock (gg)->getTranslation() ;
						trasss += delv;
						TransformWriteLock(gg)->setTranslation(trasss);
					}*/

					
				}
				else if (m_axis != 6){
				// Assaf- Temp.
				
				// lets find the angle , using consinus rule.
				float a = sqrt(float(m_x)*float(m_x) + float(m_y)*float(m_y));
				float b = sqrt(float(x)*float(x) + float(y)*float(y));
				float c = sqrt (p_dx*p_dx + p_dy*p_dy);
				// The consinus law : a*a + b*b - 2*a*b*cosA = c*c
				double cosA = (-c*c + a*a + b*b )/ (2*a*b);
				double dang = std::acos (cosA);
				float ang = float (dang);
				Mat44f p= r2->getMatrix();

			  float halfWndX = float(m_renderTarget->getWidth())  * 0.5f;
			  float halfWndY = float(m_renderTarget->getHeight()) * 0.5f;
			   Vec2f p0( (float(m_x) - halfWndX)  / halfWndX
					  , (float(halfWndY) - m_y)  / halfWndY);

			  Vec2f p1( (float(x) - halfWndX)  / halfWndX
					  , (float(halfWndY) - y)  / halfWndY);

			  if (p0 != p1)
			  {
				float dx = p1[0] - p0[0];
				float dy = p1[1] - p0[1];
				dx *= PI_HALF;
				dy *= PI_HALF;
				ang = dy+dx;
				if (m_axis == 3 || m_axis == 5)
					ang = -ang;
			  }

				// GetDeltas (&p_dx,&p_dy);
				if (FixRotationDir (&p_dx,&p_dy))
				 ang = -1.0f*ang;
				
			//	if ((p_dy  < 0.0f) || (p_dx  < 0.0f)) 
				//	ang *= -1.0f;
				 Mat33f camo (CameraReadLock(ViewStateReadLock(m_viewState)->getCamera())->getOrientation());
				 camo.invert();
				Vec3f axis = Vec3f(0.0,0.0,0.0);
				axis[m_axis-3] = 1.0f;

				

			//	Vec3f pax = axis * camo;
			//	if (pax[m_axis-3] < 0)
			//		ang = -ang;
				//if (p_dy < 0)
				//	ang = -ang;
				Mat33f rot (axis,ang*4.0f);//ang / 200.0f*PI) ;
				//rot = rot * camo;
				//rot *= m_viewState->get
				//p*=rot;
				Trafo t;
				t.setMatrix (p);
				Quatf ori=t.getOrientation ();
				Mat33f res (ori);
				//t.setCenter (-t.getTranslation());
				Vec3f  trs = t.getTranslation ();
			//	t.setTranslation(-r2->getBoundingBox().getCenter());//-trs);//Vec3f(0.0f,0.0f,0.0f));
				t.setOrientation (Quatf(rot*res));
			//	t.setTranslation (trs);
				r2->setMatrix(t.getMatrix());
				//r2->setTranslation(r2->getBoundingBox().getCenter());
			} else
			{
				Vec3f sca = r2->getScaling();
				float del = 0.0;
				float camZ  = TransformWriteLock(m_draggerRoot)->getScaling()[0];
				del = /*camZ*/float(p_dx+p_dy)/5.0f;
				float perc = (100.0f + del) / 100.0f;
				   sca[0] *=perc; 
					sca[1]*=perc;
					sca[2] *=perc;
					
					if (sca[0] > 0.01f && sca[1] > 0.01f && sca[2] > 0.01f)
				r2->setScaling(sca);
			}
				//rot = Mat33f::
				//p[m_axis] += ((float(p_dx)+float(p_dy)) );
				m_y = y;
				m_x = x;
				//r2->setTranslation  (p );
				ParamListSharedPtr pl = ParamList::createNew();
				 ParamListWriteLock(pl)->PushInt("eid",m_currentElementID);
				OnSendAvocadoDocGeneralStringMessage("NotifyDocElementMove",m_currentElementDocID, ParamListWriteLock(pl)->SerializeList());
			}
		}
		return false;
	}

	bool AvocadoDragger::OnMouseLUp()
	{
		m_first = true;
		m_draggerAxisPicked = false;
		return false;
	}

}

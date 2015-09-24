/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoPipelineModule.h"
#include "AvocadoEngineObject.h"
#include <nvgl/RenderContextGL.h>
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include "SceneFunctions.h"
#include <nvsg/Drawable.h>
#include <nvtraverser\SearchTraverser.h>
#include "MeshGenerator.h"
#include <nvsg/BlendAttribute.h>
#include <nvsg/AlphaTestAttribute.h>

using namespace nvmath;
using namespace nvsg;

namespace avocado
{
	AvocadoPipeline::AvocadoPipeline () : AvocadoViewModule ("PipelineModule")
	{
		m_highlightGroupCount = 0;
		m_currentHighlightedGroup = 0;
		m_currentPreHighlightedGroup = 0;
		m_currentHighlightedDrawable = 0;
		m_currentPreHighlightedDrawable = 0;
		m_savedRenderStyle = string ("");
		m_highSS = nvutil::createDefinedMaterial(Vec3f( 0.0f, 0.0f, 0.0f ),  // ambientColor,
										Vec3f(0.0f,0.8f,0.0f),								 // diffuseColor,
													Vec3f( 0.0f, 0.0f, 0.0f ),  // specularColor,
													0.0f,								// specularExponent,
													Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
													0.7f,								// opacity,
													0.0f,								// reflectivity,
													1.0f );	
		BlendAttributeSharedPtr bl = BlendAttribute::create();
//		AlphaTestAttributeSharedPtr al = AlphaTestAttribute::create();
//		AlphaTestAttributeWriteLock(al)->setAlphaFunction ( AF_ALWAYS);

 		BlendAttributeWriteLock (bl)->setBlendFunctions(BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
//		BlendAttributeWriteLock (bl)->setBlendFunctions(BF_ONE_MINUS_SOURCE_ALPHA,BF_ONE_MINUS_DESTINATION_ALPHA);
		
		FaceAttributeSharedPtr fs = FaceAttribute::create();

		FaceAttributeWriteLock (fs)->setTwoSidedLighting (true);
		//FaceAttributeWriteLock (fs)->setPolygonOffset(
		FaceAttributeWriteLock (fs)->setCullMode (false,false);

		StatePass::AttributeIterator saciOne = StateSetWriteLock(m_highSS)->findAttribute( OC_FACEATTRIBUTE );
		StateSetWriteLock(m_highSS)->removeAttribute(saciOne);
	//	StatePass::AttributeIterator saciOne = StateSetWriteLock(m_highSS)->findAttribute( OC_DEPTHATTRIBUTE );
	//	StateSetWriteLock(m_highSS)->removeAttribute(saciOne);
		 saciOne = StateSetWriteLock(m_highSS)->findAttribute( OC_BLENDATTRIBUTE );
		StateSetWriteLock(m_highSS)->removeAttribute(saciOne);

		StateSetWriteLock(m_highSS)->addAttribute(bl);
		StateSetWriteLock(m_highSS)->addAttribute(fs);
	//	StateSetWriteLock(m_highSS)->addAttribute(al);
	}

	AvocadoPipeline::~AvocadoPipeline ()
	{
	}

	AvocadoViewModule* AvocadoPipeline::create ()
	{
		 return ( (AvocadoViewModule *) new AvocadoPipeline);
	}

	bool AvocadoPipeline::OnUnload()
	{
	/*	if (m_pipeline)
			m_pipeline.reset();
		if (m_savedSS)
			m_savedSS.reset();
		if (m_highSS)
			m_highSS.reset();*/
		return true;
	}

	bool AvocadoPipeline::OnRegister()
	{
		m_pipeline = new SceneRendererPipeline(m_viewId);
		m_pipeline->setViewState(m_viewState);
		m_pipeline->setSceneRenderer (m_sceneRendererGL2);
		//m_pipeline->setSceneRenderer (m_sceneRendererRT);	// togged through UI
		m_pipeline->setRenderTarget (m_renderTarget);
		m_pipeline->setFrontViewState (m_frontViewState);
		m_pipeline->init (m_renderContextGL,m_renderTarget);
		m_mainHighlightMask =  int(pow(2.0,2*m_viewId+3));
		m_subHighlightMask =  int(pow(2.0,2*m_viewId+4));
		return true;
	}
	void AvocadoPipeline::HighlighStateSetCurrentDrawable (bool h)
	{
		if (m_currentHighlightedDrawable)
		{
			GeoNode::StateSetIterator it ;
			ObjectWeakPtr obj = DrawableReadLock (m_currentHighlightedDrawable)->getOwner((DrawableReadLock (m_currentHighlightedDrawable)->ownersBegin()));
			GeoNodeWriteLock geo (obj);
			it = geo->findStateSet(m_currentHighlightedDrawable);
			if (*it)
			{	
				if (!h)
				{
					geo->replaceStateSet(m_savedSS,*it);
					m_savedSS = *it;
				}
				else
				{
					m_savedSS = *it;
					geo->replaceStateSet(m_highSS,m_savedSS);
				}
			}
		}
		
	}

	bool AvocadoPipeline::HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string ("Module: " + m_name + string(" - ") + msg + string(" (")+paramStr+ string(")\n")).c_str());

		bool ret = false;

	    if (msg == "Render")
		{
			if ( m_pipeline /*&& m_pipeline->getViewState ()*/)
			{
				HighlighStateSetCurrentDrawable(true);
				// This smart little thingy makes sure that everyone is in-synched and happy.
				//m_pipeline->getSceneRenderer()->
				m_pipeline->render();
				HighlighStateSetCurrentDrawable(false);
					
			}
			ret = true;
		}
		else if (msg == "PreHightlightElement")
		{
			ret = false;
			if (!m_currentHighlightedDrawable)
			{
				AvocadoEngineDocElement* docElem = AvocadoEngineDocElement::GetElementFromParams(paramStr);//(AvocadoEngineDocElement*)ptr;
				GroupSharedPtr element = docElem->m_elementRoot;
				DrawableSharedPtr drawable = docElem->m_subDrawable;
				if ((!drawable)|| (!m_currentHighlightedGroup && m_currentPreHighlightedGroup != element ))
				{
					int mask = int(pow(2.0,2*m_viewId+4));
					m_pipeline->SetDoingPreHihglight (false);
					GroupWriteLock(element)->setTraversalMask (GroupWriteLock(element)->getTraversalMask () | (mask));
					if (m_currentPreHighlightedGroup)
						GroupWriteLock(m_currentPreHighlightedGroup)->setTraversalMask (GroupWriteLock(m_currentPreHighlightedGroup)->getTraversalMask () & (~mask));
					m_currentPreHighlightedGroup = element;
					needRepaint = true;
					m_pipeline->setHasHighlightedObject (true);
				} else
				{
					if (m_currentHighlightedGroup && m_currentPreHighlightedDrawable != drawable )
					{
							int mask = int(pow(2.0,2*m_viewId+3));
						m_pipeline->SetDoingPreHihglight (true);
						DrawableWriteLock(drawable)->setTraversalMask (DrawableWriteLock(drawable)->getTraversalMask () | (mask));
						if (m_currentPreHighlightedDrawable)
							DrawableWriteLock(m_currentPreHighlightedDrawable)->setTraversalMask (DrawableWriteLock(m_currentPreHighlightedDrawable)->getTraversalMask () & (~mask));
						m_currentPreHighlightedDrawable = drawable;
						needRepaint = true;
						m_pipeline->setHasHighlightedObject (true);
					}
				}
			}
		}
		else if (msg == "UnPreHightlightElement")
		{
			ret = false; // Dragger might want to get this also.
			if (!m_currentHighlightedGroup)
			{
				if (m_currentPreHighlightedGroup || m_currentPreHighlightedDrawable)
					needRepaint = true;
				m_pipeline->SetDoingPreHihglight (false);
				int mask = int(pow(2.0,2*m_viewId+4));
				if (m_currentPreHighlightedGroup)
					GroupWriteLock(m_currentPreHighlightedGroup)->setTraversalMask (GroupWriteLock(m_currentPreHighlightedGroup)->getTraversalMask () & (~mask));

				mask = int(pow(2.0,2*m_viewId+3));
				if (m_currentPreHighlightedDrawable)
					DrawableWriteLock(m_currentPreHighlightedDrawable)->setTraversalMask (DrawableWriteLock(m_currentPreHighlightedDrawable)->getTraversalMask () & (~mask));
			
				m_currentPreHighlightedDrawable = 0;

				m_currentPreHighlightedGroup = 0;
				m_pipeline->setHasHighlightedObject (false);
			} else
			{
			//	if (m_pipeline->GetDoingPreHihglight())
				{
					// We have a main selection , in sub-selectio mode , but mouse moved over background.
					// highlight  main selection
				}
			//	m_currentPreHighlightedDrawable =0;
			}
		}
		else if (msg == "HightlightElement")
		{
		
			AvocadoEngineDocElement* docElem = AvocadoEngineDocElement::GetElementFromParams(paramStr);//(AvocadoEngineDocElement*)ptr;
			GroupSharedPtr element = docElem->m_elementRoot;
			DrawableSharedPtr drawable = docElem->m_subDrawable;
			if (element)
			{
				int mask = int(pow(2.0,2*m_viewId+3));
				if (m_currentHighlightedDrawable)
				{
					DrawableWriteLock prev_node(m_currentHighlightedDrawable);
					prev_node->setTraversalMask (prev_node->getTraversalMask () & ~mask);
				}

				GroupWriteLock(element)->setTraversalMask (GroupWriteLock(element)->getTraversalMask () | mask);				/* Sub hi1ghlight */
				if (drawable)
				{
	/*				if (m_currentHighlightedDrawable)
					{
						DrawableWriteLock prev_node(m_currentHighlightedDrawable);
						prev_node->setTraversalMask (prev_node->getTraversalMask () & ~mask);
					}*/
					if (m_currentHighlightedDrawable == drawable)
					{
						ParamListSharedPtr pl2 = ParamList::createNew();
						ParamListWriteLock(pl2)->PushBool ("prepick",false);
						ParamListWriteLock(pl2)->PushInt ("vid",m_viewId);
						OnSendAvocadoDocGeneralStringMessage("OnPickNothing", docElem->m_docId, ParamListWriteLock(pl2)->SerializeList());
							return true;
					}

					DrawableWriteLock node(drawable);
					node->setTraversalMask (node->getTraversalMask () | mask);

					m_pipeline->SetDoingPreHihglight (true);
				} 
				else
				{	
					mask = int(pow(2.0,2*m_viewId+4));
					m_pipeline->SetDoingPreHihglight (false);
					GroupWriteLock(element)->setTraversalMask (GroupWriteLock(element)->getTraversalMask () | (mask));
				}

			m_currentHighlightedDrawable = drawable;		
			m_currentHighlightedGroup = element;
			m_highlightGroupCount ++;
			m_pipeline->setHasHighlightedObject (true);
			}
		} 
		else 
		if (msg == "UnHightlightElement")
		{
			AvocadoEngineDocElement* docElem = AvocadoEngineDocElement::GetElementFromParams(paramStr);//(AvocadoEngineDocElement*)ptr;
			GroupSharedPtr element = docElem->m_elementRoot;
			int mask = int(pow(2.0,2*m_viewId+3));

			DrawableSharedPtr drawable =docElem->m_subDrawable;
			/* Sub hi1ghlight */
			if (drawable)
			{
				DrawableWriteLock node(drawable);
				node->setTraversalMask (node->getTraversalMask () & ~mask);
			}
			/* Main high light*/
			if (docElem->m_parentGroupID == -1)
			{
			GroupWriteLock(element)->setTraversalMask (GroupWriteLock(element)->getTraversalMask () & ~mask);
			
			mask = int(pow(2.0,2*m_viewId+4));
			GroupWriteLock(element)->setTraversalMask (GroupWriteLock(element)->getTraversalMask () & (~(mask)));
			}
			m_pipeline->SetDoingPreHihglight (false);
			m_highlightGroupCount--;
			if (m_highlightGroupCount == 0)
				m_pipeline->setHasHighlightedObject (false);
			
			m_currentHighlightedGroup = 0;
			m_currentHighlightedDrawable = 0;
		}
		else if (msg == "OnTogHighlights")
		{
			if (m_pipeline /*&& m_pipeline->getViewState ()*/)
			{
				   m_pipeline->ToggleHighlighting ();
				   needRepaint = true;
				   // nvutil::ZoomCameraToNode (m_viewState,NULL);
			}
			ret = true;
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
			if (srcname == "UseOptix")
			{
				if (pval == "1")
				{
					if (m_pipeline && m_pipeline->SetBoolUseOptix (true))
					{
						m_pipeline->setSceneRenderer (m_sceneRendererRT);
						needRepaint = true;
					}
				}
				else
				{
					if (m_pipeline && m_pipeline->SetBoolUseOptix (false))
					{
							m_pipeline->setSceneRenderer (m_sceneRendererGL2);
							needRepaint = true;
					}
				}
			   ret = true;
			} 
			else if (srcname == string ("RenderStyle"))
			{
				if (pval != m_savedRenderStyle)
				{
				if (pval == "3")
				{
						m_pipeline->ToggleHLR (true);
						m_pipeline->ToggleCellShading(false);
				} else if (pval == "2")
				{
						m_pipeline->ToggleHLR (false);
						m_pipeline->ToggleCellShading(true);
				} else if (pval == "4")
				{
						m_pipeline->ToggleHLR (false);
						m_pipeline->ToggleFlatShading(true);
				}
				else
				{
					    m_pipeline->ToggleCellShading (false);
						m_pipeline->ToggleHLR (false);
				}
				
					m_savedRenderStyle = pval;
					needRepaint = true;
				}

			}
		}
		else if (msg == "ManipulationStart")
		{
			m_pipeline->SetHighlighting (false);
		} else if (msg == "ManipulationEnd")
		{
			m_pipeline->SetHighlighting (true);
			needRepaint = true;
		} else
		if (msg  ==  "SetDocParam")
		{
				ParamListSharedPtr pl = ParamList::createFromString (paramStr);
				if (pl->GetParam ("backColor"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("backColor");
					string val;
					ip->GetValue (val);
					stringstream ii (val);
					int ival;
					ii >> ival;
					int r = GetRValue(ival);
					int g = GetGValue(ival);
					int b = GetBValue(ival);
					m_pipeline->SetSceneBackColor (r,g,b);
					needRepaint = true;
				} else
				if (pl->GetParam ("backimage"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("backimage");
					string val;
					ip->GetValue (val);
				
					//m_pipeline->SetSceneBackImage (val);
				//	needRepaint = true;
				} else
				if (pl->GetParam ("PostEffectBrightness"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectBrightness");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcBrightness (fval);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectContrast"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectContrast");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcContrast (fval);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectHue"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectHue");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcHue (fval);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectSaturation"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectSaturation");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcSaturation (fval);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectNoise"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectNoise");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcNoise (fval);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectSoften"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectSoften");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcSoften (fval);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectSoftenEnabled"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectSoftenEnabled");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcSoftenEnabled ((fval > 0));
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectAOEnabled"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectAOEnabled");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcAOEnabled ((fval > 0));
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectAOIntensity"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectAOIntensity");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcAOIntensity (fval );
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectNoiseEnabled"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectNoiseEnabled");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcNoiseEnabled ((fval > 0));
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectSaturationEnabled"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectSaturationEnabled");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcHueEnabled ((fval > 0));
					m_pipeline->SetPostProcSaturationEnabled ((fval > 0));
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectBrightnessEnabled"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectBrightnessEnabled");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcContrastEnabled ((fval > 0));
					m_pipeline->SetPostProcBrightnessEnabled ((fval > 0));
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectEnabled"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectEnabled");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcEnabled ((fval > 0));
					needRepaint = true;
				} else
				if (pl->GetParam ("PostEffectGamma"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectGamma");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcGAMMA (fval * 3.0f);// > 0.0 ? 2.2f : 1.0f);
					needRepaint = true;
				}
				else
				if (pl->GetParam ("PostEffectGammaEnabled"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectGammaEnabled");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcGAMMAEnabled (fval > 0.0);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectFXAA"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectFXAA");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcFXAA (fval > 0.0 ? 2.0f : 1.0f);
					needRepaint = true;
				} 
				else if (pl->GetParam ("PostEffectToneMap"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectToneMap");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcTONEMAP (fval );//> 0.0f ? 1.0f : 0.0f);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectToneMapEnabled"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectToneMapEnabled");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcTONEMAPEnabled (fval > 0.0f);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectDOFEnabled"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectDOFEnabled");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcDOFEnabled (fval > 0.0f);
					needRepaint = true;
				}
				else if (pl->GetParam ("PostEffectDOFPlane"))
				{
					StringParam* ip = (StringParam*)pl->GetParam ("PostEffectDOFPlane");
					string val;
					ip->GetValue (val);

					float fval;
					istringstream strstrm (val);
					strstrm >> fval;

					m_pipeline->SetPostProcDOFPlane (fval);
					needRepaint = true;
				}
		}
		else
			if (msg == "SaveViewToFileFull")
		{
			m_pipeline->SaveContextToFile (paramStr,3300,2600,false,true);// m_nvsgViewData->m_renderTargetGL->getWidth(),
				//m_nvsgViewData->m_renderTargetGL->getHeight());
		} 
		else  if (msg == "SaveViewToBitmapFile")
		{
			ParamListSharedPtr  ppl = ParamList::createFromString (paramStr);
			int height,width;
			string filename;
			bool forceOptix = true;
			ppl->PopBool (forceOptix);
			ppl->PopInt (height);
			ppl->PopInt (width);
			ppl->PopString (filename);

			m_pipeline->SaveContextToFile (filename,width,height,true,false);//forceOptix);
		}
		else if (msg == "SaveViewToFile")
		{
			// used for thumnails and stuff. not OLE.
			m_pipeline->SaveContextToFile (paramStr,256,256,false,false);
		} 
		return ret;
	}
}
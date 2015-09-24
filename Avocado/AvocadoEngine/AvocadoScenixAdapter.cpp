/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#include "AvocadoScenixAdapter.h"
#include <nvgl/RenderContextGL.h>
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include <nvsg/PointLight.h>
#include <nvsg/Material.h>
#include <nvutil/Tools.h>
#include "MeshGenerator.h"
#include <nvsg/LightSource.h>
#include <nvsg/LightingAttribute.h>
#include <nvgl/GLObjectRenderer.h>
#include <nvgl/RenderTargetGLFBO.h>
#include <nvgl/RendererGLFSQ.h>
#include "SceneFunctions.h"
#include <nvrt/RTContext.h>
#include <nvutil/Trace.h>
#include "AvocadoMaterials.h"
#include "AvocadoParams.h"
#include "../../3rdParty/jpeg-8d/Jpegfile.h"
#include <nvsg/BufferHost.h>
using namespace nvmath;
using namespace nvsg;

namespace avocado
{

	// CNVSGViewData

	CNVSGViewData::CNVSGViewData(){
		m_viewParams = ParamList::createFromString ("string avocadoView=first;");
		m_doingAA = true;
		m_exportFBO = 0;
	}
	CNVSGViewData::~CNVSGViewData(){
		ResetAllContexts();
	}
	void CNVSGViewData::SetCameraLocation (float *mat)
	{
		CameraSharedPtr cam (ViewStateWriteLock (m_viewState)->getCamera());
		Mat44f mmat (true);
		for (int j=0;j<4;j++)
			for (int i=0;i<4;i++)
				mmat[j][i] = mat[i+4*j];

		CameraWriteLock (cam)->setWorldToViewMatrix (mmat);
	}
	void CNVSGViewData::GetCameraLocation (float **mat)
	{
		CameraSharedPtr cam (ViewStateReadLock (m_viewState)->getCamera());
		Mat44f nvmat = CameraReadLock (cam)->getWorldToViewMatrix ();
		const float *fmat = nvmat.getPtr();
		for (size_t i=0;i<16;i++)
			(*mat)[i] = fmat[i];
	}
	void CNVSGViewData::SetupDefaultLight(ViewStateSharedPtr	&viewState)
	{
		//createDefaultHeadLight (ViewStateReadLock (viewState)->getCamera(),Vec3f ( 0.0f, -1.0f,0.0f));
		//createDefaultHeadLight (ViewStateReadLock (viewState)->getCamera(),Vec3f (-1.0f,-1.0f,-1.0f));
		createDefaultHeadLight (ViewStateReadLock (viewState)->getCamera(),Vec3f (0.0f,0.0f,4.0f));
		//createDefaultHeadLight(ViewStateReadLock (viewState)->getCamera(),Vec3f (-1.0f,-1.0f,-1.0f) ,true);
	}

	void CNVSGViewData::ResetAllContexts()
	{
		m_renderContextGL->makeCurrent();
		m_sceneRendererGL2.reset();
		//m_renderTargetGL.reset();
		m_renderContextGL->makeNoncurrent();
		//m_renderContextGL.reset();
		m_viewState.reset();
	}

	bool CNVSGViewData::SetViewState(SceneSharedPtr &scene)
	{
		nvutil::bindScene(scene,m_viewState);
		setupDefaultViewState(scene,m_viewState);
		SetupDefaultLight(m_viewState);

		if ( m_sceneRendererGL2 )
		{
			nvui::RendererOptionsWriteLock options( ViewStateReadLock(m_viewState)->getRendererOptions() );

			if ( options->hasProperty( (m_sceneRendererGL2->getOptionStringTransparencyDepthMask() ) ) )
			{
				options->setValue<bool>( options->getProperty(  m_sceneRendererGL2->getOptionStringTransparencyDepthMask()), false );
			}
			else
			{
				options->addProperty<nvgl::GLObjectRenderer::FaceCullMode> (m_sceneRendererGL2->getOptionStringFaceCullMode(),
					m_sceneRendererGL2->getOptionStringFaceCullMode(),nvgl::GLObjectRenderer::CULLMODE_NONE);

				bool bf;
			if (avocado::GetEngineOptionBool ("backface_culling", &bf))
			{
				if ( bf )
				{
						options->addProperty<nvgl::GLObjectRenderer::FaceCullMode> (m_sceneRendererGL2->getOptionStringFaceCullMode(),
							m_sceneRendererGL2->getOptionStringFaceCullMode(),nvgl::GLObjectRenderer::CULLMODE_BACK);
				}
			}
				
				
				options->addProperty<bool> (m_sceneRendererGL2->getOptionStringTransparencyDepthMask(),
					m_sceneRendererGL2->getOptionStringTransparencyDepthMask(),false);

				options->addProperty<bool> (m_sceneRendererGL2->getOptionStringSeparateSpecularColor(),
					m_sceneRendererGL2->getOptionStringSeparateSpecularColor(),true);
				options->addProperty<bool> (m_sceneRendererGL2->getOptionStringLightModelTwoSide (),
					m_sceneRendererGL2->getOptionStringLightModelTwoSide(),false);
				options->addProperty<bool> (m_sceneRendererGL2->getOptionStringFrustumCulling(),
					m_sceneRendererGL2->getOptionStringFrustumCulling(),true);
				options->addProperty<bool> (m_sceneRendererGL2->getOptionStringTessellationEnabled(),
					m_sceneRendererGL2->getOptionStringTessellationEnabled(),false);

				options->addProperty<bool> (m_sceneRendererGL2->getOptionStringSizeCulling(),
					m_sceneRendererGL2->getOptionStringSizeCulling(),true);

				options->addProperty<float> (m_sceneRendererGL2->getOptionStringSizeCullingFactor(),
					m_sceneRendererGL2->getOptionStringSizeCullingFactor(),1.0);

				options->addProperty<int> (m_sceneRendererGL2->getOptionStringRenderModeGroup(),
					m_sceneRendererGL2->getOptionStringRenderModeGroup(),-1);

				options->addProperty<unsigned int> (m_sceneRendererGL2->getOptionStringTraversalMaskOverride(),
					m_sceneRendererGL2->getOptionStringTraversalMaskOverride(),0);
			}
			
			m_sceneRendererGL2->setViewState( m_viewState );
		}

		m_frontLayerViewState = ViewState::create();
		SceneSharedPtr frontScene = Scene::create();
		//nvutil::loadDraggerScene(frontScene);
		TransformSharedPtr frontRoot = Transform::create();
		SceneWriteLock(frontScene)->setRootNode(frontRoot);
		ViewStateWriteLock (m_frontLayerViewState)->setScene(frontScene);
		//if (!m_frontViewState)
		//		return;
		CreateEmptyCamera (m_frontLayerViewState);
		//nvutil::createDefaultHeadLight (ViewStateReadLock(m_frontLayerViewState)->getCamera());

		//setupDefaultViewState (frontScene,m_frontLayerViewState);
		return true;
	}
	bool CNVSGViewData::SaveContextToFile (string fileName, unsigned int width,unsigned int height,bool bitmap,bool tryOptix )
	{
		unsigned int actualWidth = 3300;//width;
		unsigned int actualHeight = 2600;//height;
		bool sizeChanged = false;

		if (width > 400 && height > 400)
		{
			actualWidth = width;
			actualHeight = height;
		} 
		else
		{
			actualWidth = 800;
			actualHeight = 800;
		}
		/// 2200 X 1300 works
		m_renderContextGL->makeCurrent();

		if (m_exportFBO.get() != 0)
		{
				unsigned int oldW,oldH;
				m_exportFBO->getSize (oldW,oldH);
				if (oldW != actualWidth || oldH != actualHeight)
				{
					sizeChanged = true;
				}
		}

		if (m_exportFBO.get() == 0 || sizeChanged)
		{


		 m_exportFBO = nvgl::RenderTargetGLFBO::create(m_renderContextGL);
					m_exportFBO->setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		m_exportFBO->setClearDepth(1.0);
		m_exportFBO->setClearStencil(0);
		// Render to rectangle texture.
		m_exportFBO->setAttachment(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0,
			nvgl::TextureGLRectangle::create(GL_RGBA8,actualWidth,actualHeight, GL_BGRA, GL_UNSIGNED_BYTE));
		//	nvgl::TextureGL2D::create (GL_RGBA8,2200,1300,0,GL_BGR,GL_UNSIGNED_BYTE));
		// Depth and Stencil are Renderbuffers.
		nvgl::SmartRenderbufferGL depthStencil(nvgl::RenderbufferGL::create(GL_DEPTH24_STENCIL8));
		m_exportFBO->setAttachment(nvgl::RenderTargetGLFBO::DEPTH_ATTACHMENT,   depthStencil);
		m_exportFBO->setAttachment(nvgl::RenderTargetGLFBO::STENCIL_ATTACHMENT, depthStencil);

		std::vector<GLenum> drawBuffers;
		drawBuffers.push_back(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0);
		m_exportFBO->setDrawBuffers(drawBuffers);
		m_exportFBO->setSize(actualWidth, actualHeight);
		} 
		//this->m_sceneRendererGL2->setRenderTarget (m_exportFBO);

		unsigned int width22;
		unsigned int height22;
		
		m_renderTargetGL->getSize(width22, height22);
		m_exportFBO->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear all.

		//this->m_sceneRendererRT->setRenderTarget (m_exportFBO);
		// this->m_sceneRendererRT->render ();
		if (tryOptix)//bitmap  && tryOptix)
			// cases - 
			// printing/ ole doc/bmp export.
		//	this->m_sceneRendererRT->render  (m_viewState,m_exportFBO);
		    this->m_sceneRendererGL2->render  (m_viewState,m_exportFBO);
		else
			this->m_sceneRendererGL2->render  (m_viewState,m_exportFBO);
	
		//this->m_sceneRendererGL2->setRenderTarget (m_renderTargetGL);
		//BufferSharedPtr buffer =BufferHost::create(); // = Image::Image ();
		//	m_renderTargetGL->copyToBuffer ( nvgl::RenderTargetGLFB::COLOR_ATTACHMENT0,Image::PixelFormat::IMG_RGB,Image::PixelDataType::IMG_BYTE,buffer);
		TextureHostSharedPtr te = m_exportFBO->getTextureHost ();
		TextureHostWriteLock (te)->convertPixelFormat (Image::IMG_RGB);
		TextureHostWriteLock (te)->scale (0,width,height);
		BufferSharedPtr buffer = TextureHostWriteLock (te)->getPixels();
		char *buf;

		size_t bufsize = BufferReadLock(buffer)->getSize ();
		buf = new char [bufsize];
		BufferReadLock(buffer)->getData(0,bufsize,buf);
		JpegFile file;
		if (!bitmap)
		{
			JpegFile::VertFlipBuf(buf,(((unsigned int)bufsize)/height),height);
			file.RGBToJpegFile (fileName,buf,width,height,true,99);
		}
		else
		{
			JpegFile::VertFlipBuf(buf,(((unsigned int)bufsize)/height),height);
			file.RGBToJpegFile (fileName + string("tmp"),buf,width,height,true,99);
			char *dataBuf = JpegFile::JpegFileToRGB (
				fileName + string("tmp"),
				&width,
				&height,
				std::string(fileName),
				true);
			delete [] dataBuf;

			// Should also delete the temp jpg or do the export directly..


			//JpegFile::VertFlipBuf(buf,(bufsize/height),height);
			//file.RGBToBitmapFile (fileName,buf,width,height,true,70);
		}
		return true;
	}
	void CNVSGViewData::SetAntiAliasing (bool enable)
	{
		int msAA = (enable ? 8 : 0);
		if (enable != m_doingAA)
		{
			m_doingAA = enable;
			nvgl::RenderContextGLFormat format = m_renderContextGL->getFormat ();
			format.setMultisample (msAA);
			
		  //  m_renderContextGL = 
		}
		//nvgl::SmartRenderContextGL newcon (
	}
	void CNVSGViewData::SetGlobalHeadLight(float ff)
	{
		CameraSharedPtr cam;
		{
			ViewStateReadLock viewStateLock( m_viewState );
			cam  = viewStateLock->getCamera();
		}
		Camera::HeadLightIterator hit = CameraWriteLock (cam)->beginHeadLights();
		LightSourceSharedPtr plt ((*hit));

		PointLightWriteLock pointLight (plt);
		//pointLight->setSpecularColor( ff*Vec3f(1.0f,1.0f, 1.0f) );
		pointLight->setIntensity (ff);
	}

	bool CNVSGViewData::SetupRenderContextGL(void *phWnd,bool &optiXsupport, bool &antiAliasingSupport,bool from_hwnd,const nvgl::SmartRenderContextGL &shared)
	{
		nvgl::RenderContextGLFormat format;
		format.setMultisample(8);//
		antiAliasingSupport = true;
		if (from_hwnd)
			m_renderContextGL = nvgl::RenderContextGL::create( nvgl::RenderContextGL::FromHWND(*((HWND *)phWnd), &format, shared) );
		else
			m_renderContextGL = nvgl::RenderContextGL::create( nvgl::RenderContextGL::FromHDC (*((HDC *)phWnd), shared));

		if (!m_renderContextGL)
		{
			format.setMultisample(0);
			// try again..
			if (from_hwnd)
				m_renderContextGL = nvgl::RenderContextGL::create( nvgl::RenderContextGL::FromHWND(*((HWND *)phWnd), &format, shared) );
			else
				m_renderContextGL = nvgl::RenderContextGL::create( nvgl::RenderContextGL::FromHDC (*((HDC *)phWnd), shared));

			antiAliasingSupport = false;
		}
		// OptiX will attach to the OpenGL context made active above
		m_renderTargetGL = nvgl::RenderTargetGLFB::create( m_renderContextGL ); 
		
		m_renderTargetGL->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		m_renderContextGL->makeCurrent();
		bool rt_disabled = false;
	    avocado::GetEngineOptionBool ("disable_raytracing", &rt_disabled);
		if (rt_disabled)
		{
			optiXsupport = false;
			return true;
		}
		try
		{
		   m_renderContextRT = nvrt::RTContext::create();
		}
		catch (...)
		{
		}
		if (m_renderContextRT)
		{
			NVSG_ASSERT(m_renderContextRT);
			//NVSG_TRACE_OUT();
			if ( nvrt::RTContextWriteLock(m_renderContextRT)->isValid () && nvrt::RTIsSupported())
				optiXsupport = true;
			else
				optiXsupport = false;
			if (optiXsupport)
			//if (optixSupport)
			m_sceneRendererRT = nvrt::SceneRendererRTBuiltinGL::create( m_renderContextRT );
			if (!m_sceneRendererRT)
			{
				//NVSG_TRACE_OUT("RT render target failed to initialize.");
			}
		}
		else
		{
			m_sceneRendererRT = 0;
			//NVSG_TRACE_OUT("RT Context failed to initialize.");
			optiXsupport = false;
		}
		return true;
	}

	bool CNVSGViewData::SetupSceneRendererGL2()
	{
		//#ifndef _USE_OPTIX
		m_sceneRendererGL2 = nvgl::SceneRendererGL2::create();
		//	pass the render target and the view state to it
		m_sceneRendererGL2->setRenderTarget( m_renderTargetGL);
		//#endif
		return true;
	}

	void CNVSGViewData::SetRenderContextSize(int cx, int cy)
	{
		if (m_renderTargetGL)
		{
			// I dont understand why MFC sends so many resize events.. we will shoot them down right here.
			//.. silly windows.. is this happening in all windows apps or mfc apps ? or just me..
			unsigned int wx = 0;
			unsigned int wy = 0;
			m_renderTargetGL->getSize (wx,wy);
			if (wx == cx && wy == cy)
				return;

			m_renderTargetGL->setSize(cx, cy);
		}
	}
	using namespace nvmath;
	// CNVSGDocData
	SmartIDGenerator IDGenerator::createNew()
	{
		SmartIDGenerator newObj =  SmartIDGenerator(new IDGenerator);
		return newObj;
	};

	CNVSGDocData::CNVSGDocData()
	{
		m_globalAmbientIntensity = 0.0f;
		m_globalAmbientColor[0] = 1.0f;
		m_globalAmbientColor[1] = 1.0f;
	    m_globalAmbientColor[2] = 1.0f;

		m_globalExposure = 0.0f;
		m_roomType = "";
		m_cachedScale [0] = 1.0f;
		m_cachedScale [1] = 1.0f;
		m_cachedScale [2] = 1.0f;
		m_cachedBgImage = "";
		m_docParams = ParamList::createFromString ("string avocadoView=first;");
		m_filename = "C:\\Scenix72\\media\\scenes\\shader\\8omni.nbf";
		m_smartIDGen = IDGenerator::createNew(); // This should be loaded and saved to file , of course.
	}
	void CNVSGDocData::ResetRoomData ()
	{
		m_cachedScale [0] = 1.0f;
		m_cachedScale [1] = 1.0f;
		m_cachedScale [2] = 1.0f;
		m_roomType = "";
		m_cachedBgImage = "";
		GroupWriteLock (SceneWriteLock (GetScene())->getRootNode ())->removeChild (m_cachedRoom);
		m_cachedRoom = 0;
		m_cachedRoomGeo = 0;
	}
	CNVSGDocData::~CNVSGDocData()
	{
		ResetScene();
		//m_smartIDGen.reset();
	}
	void CNVSGDocData::SetSessionFolder (string folder)
	{
		m_sessionFolder = folder;
		return ;
	}
	void CNVSGDocData::ResetScene()
	{
		//m_scene.reset();
	}
	bool CNVSGDocData::SetRoomSize (float size)
	{
		return true;
	}
	bool CNVSGDocData::SetGlobalAmbientColor(float expVal )
	{
		unsigned long iColor = (unsigned long)expVal;
		
		int R = int (  (   (unsigned char)(  (unsigned long)(iColor)  & 0xff)) );
		long G = long (  ( ( (unsigned char)( ( (unsigned long)( (unsigned short)iColor)  >> 8 ) & 0xff)) ));
		long B = long (  ( ( (unsigned char)( ( (unsigned long)(iColor) ) >> 16) & 0xff) ));
		if (R < 0)
			R = 0;
		if (G < 0)
			G = 0;
		if (B < 0)
			B = 0;
		m_globalAmbientColor[0] =  float(R) / 255.0f;
		m_globalAmbientColor[1] = float(G) / 255.0f;
		m_globalAmbientColor[2] = float(B) / 255.0f ;
		SceneWriteLock (m_scene)->setAmbientColor (Vec3f (m_globalAmbientColor[0] * m_globalAmbientIntensity,
													m_globalAmbientColor[1] * m_globalAmbientIntensity,
													m_globalAmbientColor[2] * m_globalAmbientIntensity));
		return true;
	}
	bool CNVSGDocData::SetGlobalAmbientIntensity (float expVal )
	{
		if (expVal < 0.0f || expVal > 1.0f)
			return false;
		m_globalAmbientIntensity = expVal;
		SceneWriteLock (m_scene)->setAmbientColor (Vec3f (m_globalAmbientColor[0] * m_globalAmbientIntensity,
													m_globalAmbientColor[1] * m_globalAmbientIntensity,
													m_globalAmbientColor[2] * m_globalAmbientIntensity));
		return true;
	}

	bool CNVSGDocData::SetGlobalExposure (float expVal )
	{
		if (expVal < -10.0f || expVal > 10.0f)
			return false;

		m_globalExposure = expVal;
		if (m_cachedRoomGeo)
		{
			nvsg::GeoNode::StateSetIterator ssi = GeoNodeWriteLock (m_cachedRoomGeo)->beginStateSets ();
			nvsg::StatePass::AttributeIterator saciOne  = StateSetWriteLock (*ssi)->findAttribute( OC_CGFX );
			nvsg::CgFxSharedPtr tessCgFx =  nvutil::sharedPtr_cast<CgFx>( *saciOne ) ;

			MaterialBase2::SetFloatUniform (tessCgFx , "Exposure", m_globalExposure);
			
			nvsg::StatePass::AttributeIterator ssMaterial  = StateSetWriteLock (*ssi)->findAttribute( OC_MATERIAL );
			nvsg::MaterialSharedPtr tessMaterial =  nvutil::sharedPtr_cast<Material>( *ssMaterial ) ;
			
			
		//	float ambientExposure = /*pow (2.0,m_globalExposure + 2.47393);*/ float ((m_globalExposure + 10.0f) / 20.0f);
			double ambientExposure = pow (2.0,double(m_globalExposure ) + 2.47393);

			ambientExposure = 	log(ambientExposure*0.184874 + 1.0)/0.184874;
			ambientExposure *= 0.18;
		    if (ambientExposure > 2.0)
				ambientExposure = 2.0;
			//nvsg::MaterialWriteLock (tessMaterial)->setAmbientColor (Vec3f (ambientExposure,ambientExposure,ambientExposure));
			//nvsg::MaterialWriteLock (tessMaterial)->setDiffuseColor (Vec3f (ambientExposure,ambientExposure,ambientExposure));
			nvsg::MaterialWriteLock (tessMaterial)->setEmissiveColor  (0.5f*Vec3f (float (ambientExposure),float(ambientExposure),float(ambientExposure)));


		}
		return true;
	}
	bool CNVSGDocData::SetRoomScale (int axis, float scaleVal )
	{
		if (scaleVal <=0.01f)
					scaleVal = 0.01f;

		if (m_roomType == "sphere")
		{
			Vec3f sv = TransformReadLock (m_cachedRoom)->getScaling ();
			sv = sv / m_cachedScale [0];
			sv *= scaleVal;
			TransformWriteLock (m_cachedRoom)->setScaling (sv);
			m_cachedScale [0] = scaleVal;
		//	m_cachedScale [1] = scaleVal;
		//	m_cachedScale [2] = scaleVal;

		} else
		if (m_roomType == "cylinder")
		{
			Vec3f sv = TransformReadLock (m_cachedRoom)->getScaling ();
			if (axis == 0)
			{
				sv[axis] = sv[axis] / m_cachedScale [axis];
				sv[axis] *= scaleVal;
				m_cachedScale [0] = scaleVal;
			}
			else
			{
				//sv[2] = sv[2] / m_cachedScale [2];
				//sv[2] *= scaleVal;
				sv[axis] = sv[axis] / m_cachedScale [axis];
				sv[axis] *= scaleVal;
				m_cachedScale [axis] = scaleVal;
			//	m_cachedScale [2] = scaleVal;
			}
			Vec3f oldtrans = TransformWriteLock (m_cachedRoom)->getTranslation ();
			float deltt = oldtrans[1] -( TransformWriteLock (m_cachedRoom)->getBoundingBox ().getSize ()[1] / 2.0f);
			TransformWriteLock (m_cachedRoom)->setScaling (sv);
			 deltt = deltt + (TransformWriteLock (m_cachedRoom)->getBoundingBox ().getSize ()[1] / 2.0f);
			//Vec3f oldtrans2 = TransformWriteLock (m_cachedRoom)->getTranslation ();
		    TransformWriteLock (m_cachedRoom)->setTranslation (  Vec3f (0.0f,deltt,0.0f) );//Vec3f (0.0f,oldtrans[1]/sv[1],0.0f));
		}
		else
		if (m_roomType == "cube")
		{
			Vec3f sv = TransformReadLock (m_cachedRoom)->getScaling ();
			sv[axis] = sv[axis] / m_cachedScale [axis];
			sv[axis] *= scaleVal;
			Vec3f oldtrans = TransformWriteLock (m_cachedRoom)->getTranslation ();
			
			float deltt = oldtrans[1] -( TransformWriteLock (m_cachedRoom)->getBoundingBox ().getSize ()[1] / 2.0f);

		//	TransformWriteLock (m_cachedRoom)->setTranslation (Vec3f (0.0f,0.0f,0.0f));
			TransformWriteLock (m_cachedRoom)->setScaling (sv);
			
			 deltt = deltt + (TransformWriteLock (m_cachedRoom)->getBoundingBox ().getSize ()[1] / 2.0f);

			//Vec3f oldtrans2 = TransformWriteLock (m_cachedRoom)->getTranslation ();
		    TransformWriteLock (m_cachedRoom)->setTranslation (  Vec3f (0.0f,deltt,0.0f) );//Vec3f (0.0f,oldtrans[1]/sv[1],0.0f));
			m_cachedScale [axis] = scaleVal;
		}
		return true;
	}

	nvsg::StateSetSharedPtr CNVSGDocData::PrepareRoomStateSet ()
	{
		
		//float ambientExposure = (m_globalExposure + 10.0f) / 20.0f;
		double ambientExposure = pow (2.0,double(m_globalExposure ) + 2.47393);

			ambientExposure = 	log(ambientExposure*0.184874 + 1.0)/0.184874;
			ambientExposure *= 0.18;
		    if (ambientExposure > 2.0)
				ambientExposure = 2.0;

		StateSetSharedPtr ss = nvutil::createDefinedMaterial(Vec3f (0.0f,0.0f,0.0f),//Vec3f(ambientExposure,ambientExposure,ambientExposure), //0.5f, 0.5f, 0.5f ),  // ambientColor,
			Vec3f(0.0f,0.0f,0.0f) ,//ambientExposure,ambientExposure,ambientExposure),								 // diffuseColor,
			//0.33f*Vec3f(float(ambientExposure) ,float(ambientExposure ),float(ambientExposure)),  // emissiveColor,
			Vec3f( 0.0f, 0.0f, 0.0f ),  // specularColor,
			0.1f,								// specularExponent,
			0.5f*Vec3f(float(ambientExposure) ,float(ambientExposure ),float(ambientExposure)),  // emissiveColor,
			1.0f,								// opacity,
			0.0f,								// reflectivity,

			1.0f );		
		FaceAttributeSharedPtr fa = FaceAttribute::create ();
		//BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
	    //BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_COLOR,BF_ONE);
		FaceAttributeWriteLock (fa)->setCullMode (false,true);
		StateSetWriteLock (ss)->addAttribute (fa);

		LightingAttributeSharedPtr la = LightingAttribute::create ();
		LightingAttributeWriteLock (la)->setEnabled (false);
		StateSetWriteLock (ss)->addAttribute (la);
		//m_cachedScale [0] = 1.0f;
		//m_cachedScale [1] = 1.0f;
		//m_cachedScale [2] = 1.0f;
		std::string error;
		std::string err2;
		std::vector<std::string> sp;
		bool b = false;
		//sp.push_back("C:\\Education\\AvocadoNet\\Avocado\\Release\\media\\effects\\");
		//sp.push_back ("C:/Education/AvocadoNet/Avocado/Release/media/effects/");

		sp.push_back(nvutil::GetProccessDirectory() + string ("effects\\"));
		sp.push_back(nvutil::GetProccessDirectory() + string ("textures\\"));
		sp.push_back(m_sessionFolder + string ("\\textures\\")); 

	//	CgFxSharedPtr hcg = CgFx::createFromFile(nvutil::GetProccessDirectory() + string ("effects\\")+std::string ("skydome_hdr.fx"),sp,err2,b);
		//CgFxSharedPtr hcg = CgFx::createFromFile("C:/Education/AvocadoNet/Avocado/Release/media/effects/skydome_hdr.fx",sp,err2,b);
		CgFxSharedPtr hcg = CgFx::create ();
		CgFxEffectWriteLock effect( CgFxReadLock( hcg )->getEffect() );

		string file;
		if (nvutil::FindFileFirst( "skydome_hdr.fx", sp, file ))
		{
			effect->createFromFile (file,sp,err2);
		}

		if (m_cachedBgImage != "")
		{
			unsigned int hx = 0;
			unsigned int hy = 0;
			avocado::MaterialBase2::SetSamplerUniformStatic (hcg,"textureHDR_RGBA",m_cachedBgImage,hx,hy,m_sessionFolder);
			MaterialBase2::SetFloatUniform (hcg , "Exposure", m_globalExposure);
			//avocado::MaterialBase2::SetSimpleTexture  (ss,m_cachedBgImage,0.0f,0.0f,1.0f,1.0f);
			{
				
				TextureHostSharedPtr tex ;//= TextureHost::createFromFile (m_cachedBgImage,sp);//"C:\\Education\\AvocadoNet\\Avocado\\Release\\media\\textures\\crosscube4.hdr",sp);
				sp.push_back(nvutil::GetProccessDirectory(false) );
				if (nvutil::loadTextureHost (m_cachedBgImage,tex,sp))
				{
				TextureHostWriteLock( tex )->convertToTextureTarget(	NVSG_TEXTURE_2D );
				TextureHostWriteLock( tex )->createMipmaps ();
				TextureAttributeItemSharedPtr hTexAttribItem = TextureAttributeItem::create();
				TextureAttributeSharedPtr hTexAttrib = TextureAttribute::create(); 
				// acquire write access to the TextureAttributeItem to assign a Texture
				{ // limit write access to this scope
					TextureAttributeItemWriteLock texAttribItem(hTexAttribItem);
					texAttribItem->setTexture(tex );
					texAttribItem->setMagFilterMode(nvsg::TFM_MAG_LINEAR);
					texAttribItem->setMinFilterMode(nvsg::TFM_MIN_LINEAR_MIPMAP_LINEAR);//TFM_MIN_LINEAR);

					//texAttribItem->setEnvMode( TEM_REPLACE );
				/*	texAttribItem->setTexGenMode( TCA_S, nvsg::TGM_SPHERE_MAP );
					  texAttribItem->setTexGenMode( TCA_T, TGM_SPHERE_MAP );
					  texAttribItem->setTexGenMode( TCA_R, TGM_SPHERE_MAP );*/
					  
					texAttribItem->setWrapMode (nvsg::TWCA_R,nvsg::TWM_CLAMP);
					texAttribItem->setWrapMode (nvsg::TWCA_S,nvsg::TWM_CLAMP);
					texAttribItem->setWrapMode (nvsg::TWCA_T,nvsg::TWM_CLAMP);
					/*texAttribItem->setWrapMode (nvsg::TWCA_R,nvsg::TWM_REPEAT);
					texAttribItem->setWrapMode (nvsg::TWCA_S,nvsg::TWM_REPEAT);
					texAttribItem->setWrapMode (nvsg::TWCA_T,nvsg::TWM_REPEAT);*/
				} // write access will be released after leaving this scope

				// acquire write access to the TextureAttribute to bind the 
				// TextureAttributeItem created before
				{ // limit write access to this scope
					TextureAttributeWriteLock texAttrib(hTexAttrib);
					texAttrib->bindTextureAttributeItem( hTexAttribItem, 0 );
				} // write access will be released after leaving this scope

				StateSetWriteLock (ss)->addAttribute (hTexAttrib);
				}
			}
		}

		StateSetWriteLock (ss)->addAttribute (hcg);
		return ss;
	}
	bool CNVSGDocData::SetRoomType (string type)
	{
		DrawableSharedPtr drw ;
		if (type == "none")
		{
			if (m_cachedRoom && m_scene)
				GroupWriteLock (SceneWriteLock (m_scene)->getRootNode ())->removeChild (m_cachedRoom);
			m_cachedRoomGeo = 0;
			m_cachedRoom = 0;
			return true;
		}
		if (type == "sphere")
		{
			drw = nvutil::createSphere(32,64,1.0f,true);
		}
		else if (type == "cylinder")
		{
			drw= nvutil::createCylinderWithCap (1,1,64,128);
		}
		else if (type == "cube")
		{
			drw = nvutil::createCube(true);
		}
		m_roomType = type;
		nvsg::StateSetSharedPtr ss = PrepareRoomStateSet();
		GeoNodeSharedPtr geow = GeoNode::create ();
		GeoNodeWriteLock (geow)->addDrawable (ss,drw);
		GeoNodeWriteLock (geow)->setName("AvocadoRoom");

		if (m_cachedRoom)
		{
			TransformWriteLock (m_cachedRoom)->removeChild (m_cachedRoomGeo);
			GroupWriteLock (SceneWriteLock (m_scene)->getRootNode ())->removeChild (m_cachedRoom);
			m_cachedRoom = 0;
		}
		if (!m_cachedRoom)
		{
			m_cachedRoom = Transform::create();
			Sphere3f scnspher =  GroupWriteLock (SceneWriteLock (m_scene)->getRootNode())->getBoundingSphere ();
			float scafact = std::max (scnspher.getRadius (),1.0f);
			TransformWriteLock (m_cachedRoom )->setScaling (Vec3f (scafact *2.5f,scafact*2.5f,scafact*2.5f));
			
			TransformWriteLock (m_cachedRoom )->addChild (geow);
			Box3f bbbox = TransformWriteLock (m_cachedRoom)->getBoundingBox ();
			Box3f scnbbbox =  GroupWriteLock (SceneWriteLock (m_scene)->getRootNode())->getBoundingBox();
			
			float hsize = bbbox.getSize ()[1];
			float shsize = scnbbbox.getSize()[1];
			// shpere is centered at the scene origin. Other types, cylinder and cube, are translated such that bottom will align with scene bottom.
			if (type != "sphere")
				TransformWriteLock (m_cachedRoom)->setTranslation (Vec3f (0.0f,hsize / 2.0f -shsize / 2.0f ,0.0f));

			GroupWriteLock (SceneWriteLock (m_scene)->getRootNode ())->addChild (m_cachedRoom);
		} else
		{
			// dead code here.. should not reach this section.
			    TransformWriteLock (m_cachedRoom)->removeChild (m_cachedRoomGeo);
				Sphere3f scnspher =  GroupWriteLock (SceneWriteLock (m_scene)->getRootNode())->getBoundingSphere ();
				float scafact = std::max (scnspher.getRadius (),1.0f);
				TransformWriteLock (m_cachedRoom )->setScaling (Vec3f (scafact *2.5f,scafact*2.5f,scafact*2.5f));
				TransformWriteLock (m_cachedRoom )->addChild (geow);
				
		}
		
	//	Box3f bboxnew = GeoNodeWriteLock ( geow )->getBoundingBox();

		//		TransformWriteLock (m_cachedRoom)->setTranslation (Vec3f (0.0f,bboxnew.getSize()[1],bboxnew.getSize()[2]));
		TransformWriteLock (m_cachedRoom )->setTraversalMask (1);

		m_cachedRoomGeo = geow;
		return true;
	}
	bool CNVSGDocData::SetSceneBackImage (string filename)
	{

		TextureHostSharedPtr m_backimage ;
		std::vector<std::string> spats;
		spats.push_back (nvutil::GetProccessDirectory () + "textures\\");//"C:\\Education\\SceniX72\\media\\textures\\")
		if (m_cachedBgImage != filename)
		{
			m_cachedBgImage =  filename;
			if (m_cachedRoomGeo != 0)
			{
				nvsg::StateSetSharedPtr ss = PrepareRoomStateSet();

			GeoNodeWriteLock (m_cachedRoomGeo)->replaceAllStateSets (ss);
			}
		}
		return true;
	}
	bool CNVSGDocData::SetSceneBackColor (int r, int g, int b)
	{
		float f0 = float (r) / 255.0f;
		float f1 = float (g) / 255.0f;
		float f2 = float (b) / 255.0f;

		SceneWriteLock (m_scene)->setBackColor (Vec4f (f0,f1,f2,0.0f));
		return true;
	}
	bool CNVSGDocData::SetLightsPreset (int pre,vector <LightSourceSharedPtr> &return_lights)
	{
		nvutil::SetLightsPreset (m_scene,pre,return_lights);
		return true;
	}
	bool CNVSGDocData::SetLightDirection (float val,int lightIndex,int axis)
	{
		nvutil::SetLightDirection (m_scene,val,lightIndex,axis);
		return true;
	}
	bool CNVSGDocData::SetLightIntensity (float val,int lightIndex)
	{
		nvutil::SetLightIntensity (m_scene,val,lightIndex);
		return true;
	}
	bool CNVSGDocData::SetLightEnabled (bool val,int lightIndex)
	{
		nvutil::SetLightEnabled (m_scene,val,lightIndex);
		return true;
	}
	bool CNVSGDocData::SetLightAmbientColor (float val,int lightIndex)
	{
		unsigned long iColor = (unsigned long)val;
		int R = int (  (   (unsigned char)(  (unsigned long)(iColor)  & 0xff)) );
		long G = long (  ( ( (unsigned char)( ( (unsigned long)( (unsigned short)iColor)  >> 8 ) & 0xff)) ));
		long B = long (  ( ( (unsigned char)( ( (unsigned long)(iColor) ) >> 16) & 0xff) ));
		nvutil::SetLightAmbientColor (m_scene,float (R) / 255.0f,float(G)/255.0f,float(B)/255.0f,lightIndex);
		return true;
	}
	bool CNVSGDocData::SetLightDiffuseColor (float val,int lightIndex)
	{
		unsigned long iColor = (unsigned long)val;
		int R = int (  (   (unsigned char)(  (unsigned long)(iColor)  & 0xff)) );
		long G = long (  ( ( (unsigned char)( ( (unsigned long)( (unsigned short)iColor)  >> 8 ) & 0xff)) ));
		long B = long (  ( ( (unsigned char)( ( (unsigned long)(iColor) ) >> 16) & 0xff) ));
		nvutil::SetLightDiffuseColor (m_scene,float (R) / 255.0f,float(G)/255.0f,float(B)/255.0f,lightIndex);
		return true;
	}
	bool CNVSGDocData::SetLightSpecularColor (float val,int lightIndex)
	{
		unsigned long iColor = (unsigned long)val;
		int R = int (  (   (unsigned char)(  (unsigned long)(iColor)  & 0xff)) );
		long G = long (  ( ( (unsigned char)( ( (unsigned long)( (unsigned short)iColor)  >> 8 ) & 0xff)) ));
		long B = long (  ( ( (unsigned char)( ( (unsigned long)(iColor) ) >> 16) & 0xff) ));
		nvutil::SetLightSpecularColor (m_scene,float (R) / 255.0f,float(G)/255.0f,float(B)/255.0f,lightIndex);
		return true;
	}
	void CNVSGDocData::SetDefScene()
	{
		//Create our root node here.
		m_scene = Scene::create();
		GroupSharedPtr gr = Group::create();
		SceneWriteLock(m_scene)->setRootNode(gr);

	}

}
/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#include "AvocadoScenixAdapter.h"
#include <nvgl/RenderContextGL.h>
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include <nvsg/PointLight.h>
#include <nvsg/LightSource.h>
#include <nvgl/GLObjectRenderer.h>
#include "SceneFunctions.h"
#include <nvrt/RTContext.h>
#include "AvocadoParams.h"
#include "../../3rdParty/jpeg-8d/Jpegfile.h"
#include "../SceniX72/inc/nvsg/nvsg/BufferHost.h"
namespace avocado
{

// CNVSGViewData

CNVSGViewData::CNVSGViewData(){
m_viewParams = ParamList::createFromString ("string avocadoView=first;");
m_doingAA = true;
}
CNVSGViewData::~CNVSGViewData(){
	ResetAllContexts();
}
void CNVSGViewData::SetCameraLocation (float *mat)
{
	CameraSharedPtr cam (ViewStateWriteLock (m_viewState)->getCamera());
		nvmath::Mat44f mmat (true);
		for (int j=0;j<4;j++)
		for (int i=0;i<4;i++)
			mmat[j][i] = mat[i+4*j];

	CameraWriteLock (cam)->setWorldToViewMatrix (mmat);
}
void CNVSGViewData::GetCameraLocation (float **mat)
{
	CameraSharedPtr cam (ViewStateReadLock (m_viewState)->getCamera());
	nvmath::Mat44f nvmat = CameraReadLock (cam)->getWorldToViewMatrix ();
	const float *fmat = nvmat.getPtr();
	for (size_t i=0;i<16;i++)
		(*mat)[i] = fmat[i];
}
void CNVSGViewData::SetupDefaultLight(ViewStateSharedPtr	&viewState)
{
	createDefaultHeadLight (ViewStateReadLock (viewState)->getCamera(),nvmath::Vec3f ( 0.0f, 0.0f,0.0f));
		createDefaultHeadLight (ViewStateReadLock (viewState)->getCamera(),nvmath::Vec3f (-1.0f,-1.0f,-1.0f));

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
					m_sceneRendererGL2->getOptionStringFaceCullMode(),nvgl::GLObjectRenderer::CULLMODE_BACK);
				
				options->addProperty<bool> (m_sceneRendererGL2->getOptionStringTransparencyDepthMask(),
					m_sceneRendererGL2->getOptionStringTransparencyDepthMask(),false);
				
				options->addProperty<bool> (m_sceneRendererGL2->getOptionStringSeparateSpecularColor(),
					m_sceneRendererGL2->getOptionStringSeparateSpecularColor(),true);
			
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
bool CNVSGViewData::SaveContextToFile (string fileName, unsigned int width,unsigned int height,bool bitmap )
{
	 m_renderContextGL->makeCurrent();
	 //nvsg::BufferSharedPtr buffer =nvsg::BufferHost::create(); // = nvsg::Image::Image ();
//	m_renderTargetGL->copyToBuffer ( nvgl::RenderTargetGLFB::COLOR_ATTACHMENT0,nvsg::Image::PixelFormat::IMG_RGB,nvsg::Image::PixelDataType::IMG_BYTE,buffer);
	 nvsg::TextureHostSharedPtr te = m_renderTargetGL->getTextureHost ();
	 nvsg::TextureHostWriteLock (te)->convertPixelFormat (nvsg::Image::IMG_RGB);
	 nvsg::TextureHostWriteLock (te)->scale (0,width,height);
	 nvsg::BufferSharedPtr buffer = nvsg::TextureHostWriteLock (te)->getPixels();
	char *buf;

	size_t bufsize = BufferReadLock(buffer)->getSize ();
	buf = new char [bufsize];
	BufferReadLock(buffer)->getData(0,bufsize,buf);
	JpegFile file;
	if (!bitmap)
	{
		JpegFile::VertFlipBuf(buf,(bufsize/height),height);
		file.RGBToJpegFile (fileName,buf,width,height,true,70);
	}
	else
	{
		JpegFile::VertFlipBuf(buf,(bufsize/height),height);
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
	}
	//nvgl::SmartRenderContextGL newcon (
}
void CNVSGViewData::SetGlobalAmbient(float ff)
{
	CameraSharedPtr cam;
		{
			ViewStateReadLock viewStateLock( m_viewState );
			cam  = viewStateLock->getCamera();
		}	
		Camera::HeadLightIterator hit = CameraWriteLock (cam)->beginHeadLights();
	nvsg::LightSourceSharedPtr plt (*hit);

	nvsg::PointLightWriteLock pointLight (plt);
	pointLight->setAmbientColor( nvmath::Vec3f(ff, ff, ff) );
     
	//nvsg::PointLightSharedPtr pointLightHdl = nvsg::PointLight::create();
      {
		  /*
        PointLightWriteLock pointLight(pointLightHdl);
        pointLight->setAmbientColor( nvmath::Vec3f(0.2f, 0.2f, 0.2f) );
        pointLight->setDiffuseColor( nvmath::Vec3f(0.8f, 0.9f, 0.9f) ); //0.9 1.0 1.0
        pointLight->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
        pointLight->setAttenuation( 1.0f, 0.0f, 0.0f );
        pointLight->setPosition( offset );  // Default offset is zero.
		*/
      }

  //    CameraWriteLock(camera)->addHeadLight( pointLightHdl);

}
void CNVSGViewData::SetGlobalHeadLight(float ff)
{
	CameraSharedPtr cam;
		{
			ViewStateReadLock viewStateLock( m_viewState );
			cam  = viewStateLock->getCamera();
		}
	Camera::HeadLightIterator hit = CameraWriteLock (cam)->beginHeadLights();
	nvsg::LightSourceSharedPtr plt ((*hit));

	nvsg::PointLightWriteLock pointLight (plt);
	pointLight->setSpecularColor( ff*nvmath::Vec3f(1.0f,1.0f, 1.0f) );
     
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

    m_renderContextRT = nvrt::RTContext::create();

	if (m_renderContextRT)
	{
		NVSG_ASSERT(m_renderContextRT);
		if ( nvrt::RTContextWriteLock(m_renderContextRT)->isValid ())
			optiXsupport = true;
		else
			optiXsupport = false;
		//if (optixSupport)
		m_sceneRendererRT = nvrt::SceneRendererRTBuiltinGL::create( m_renderContextRT );
	}
	else
	{
		m_sceneRendererRT = 0;
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
		m_renderTargetGL->setSize(cx, cy);
}

// CNVSGDocData
SmartIDGenerator IDGenerator::createNew()
{
		SmartIDGenerator newObj =  SmartIDGenerator(new IDGenerator);
		return newObj;
};

CNVSGDocData::CNVSGDocData()
{
	m_docParams = ParamList::createFromString ("string avocadoView=first;");
	m_filename = "C:\\Scenix72\\media\\scenes\\shader\\8omni.nbf";
	m_smartIDGen = IDGenerator::createNew(); // This should be loaded and saved to file , of course.
}

CNVSGDocData::~CNVSGDocData()
{
	ResetScene();
	//m_smartIDGen.reset();
}

void CNVSGDocData::ResetScene()
{
	//m_scene.reset();
}
void CNVSGDocData::SetSceneBackImage (string filename)
{
	
	TextureHostSharedPtr m_backimage ;
	std::vector<std::string> spats;
	spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\"); 
	m_backimage = TextureHost::createFromFile (filename,spats);
	if ( (TextureHostWriteLock( m_backimage ))->convertToTextureTarget(	NVSG_TEXTURE_2D ))
	{
		if (TextureHostWriteLock( m_backimage )->convertPixelFormat (Image::IMG_RGB))
		{
					SceneWriteLock (m_scene)->setBackImage (m_backimage);
		}
	}
	
}
void CNVSGDocData::SetSceneBackColor (int r, int g, int b)
{
	float f0 = float (r) / 255.0f;
	float f1 = float (g) / 255.0f;
	float f2 = float (b) / 255.0f;

	SceneWriteLock (m_scene)->setBackColor (nvmath::Vec4f (f0,f1,f2,0.0f));

}
void CNVSGDocData::SetLightsPreset (int pre)
{
	nvutil::SetLightsPreset (m_scene,pre);
}
void CNVSGDocData::SetDefScene()
{
	//nvutil::loadDefScene(m_scene);
	m_scene = Scene::create();
	//SceneWriteLock(m_scene);
	GroupSharedPtr gr = Group::create();
	//SceneReadLock(m_scene)->getRootNode();
	SceneWriteLock(m_scene)->setRootNode(gr);
	
	//nvutil::loadScene (m_filename,m_scene);
}

}
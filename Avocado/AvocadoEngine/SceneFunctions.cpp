// Copyright NVIDIA Corporation 2009-2010
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#include "SceneFunctions.h"
#include "FFPToCgFxTraverser.h"
#include <nvutil/PlugIn.h>
#include <nvsg/PlugInterface.h>
#include <nvsg/PlugInterfaceID.h>
#include <nvutil/Tools.h>
#include <nvsg/ErrorHandling.h>

#include <nvui/RendererOptions.h>

#include <nvsg/Scene.h>
#include <nvsg/ViewState.h>
#include <nvsg/PointLight.h>
#include <nvsg/PerspectiveCamera.h>
#include <nvsg/ParallelCamera.h>
#include <nvsg/DirectedLight.h>
#include <nvsg/SpotLight.h>
#include <nvsg/Material.h>
// optimize
#include <nvtraverser/AnalyzeTraverser.h>
#include <nvtraverser/CombineTraverser.h>
#include <nvtraverser/DestrippingTraverser.h>
#include <nvtraverser/EliminateTraverser.h>
#include <nvtraverser/IdentityToGroupTraverser.h>
#include <nvtraverser/NormalizeTraverser.h>
#include <nvtraverser/SearchTraverser.h>
#include <nvtraverser/StatisticsTraverser.h>
#include <nvtraverser/TriangulateTraverser.h>
#include <nvtraverser/UnifyTraverser.h>

#include "MeshGenerator.h"
#include <nvsg/StateSet.h>
#include <nvsg/Transform.h>
#include <nvsg/Billboard.h>
#include <nvtraverser/SearchTraverser.h>

// picking
#include <nvtraverser/RayIntersectTraverser.h>

#include <nvutil/DbgNew.h>

using namespace std;

using namespace nvsg;
using namespace nvui;
using namespace nvutil;
using namespace nvmath;
using namespace nvtraverser;

namespace nvutil
{
	/* 60 Degress FOV */
	

	void bindScene(const nvsg::SceneSharedPtr &scene, nvsg::ViewStateSharedPtr &viewState)
	{
		if ( !viewState )
        {
          viewState = ViewState::create();
        }
        ViewStateWriteLock( viewState )->setScene( scene );
	}



	void loadDefScene(nvsg::SceneSharedPtr &scene)
	{
		scene = nvsg::Scene::create();
		nvsg::DrawableSharedPtr d = nvutil::createPlane(-500.0f,-500.0f,1000.0f,1000.0f);
		nvsg::GeoNodeSharedPtr g = nvsg::GeoNode::create();
		nvsg::BillboardSharedPtr b = nvsg::Billboard::create();
		nvsg::GroupSharedPtr gr = nvsg::Group::create();
		nvsg::StateSetSharedPtr stateSet = nvutil::createDefinedMaterial( 
										nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // ambientColor,
										nvmath::Vec3f( 1.0f, 1.0f, 1.0f ),  // diffuseColor,
										nvmath::Vec3f( 1.0f, 1.0f, 1.0f ),  // specularColor,
										50.0f,								// specularExponent,
										nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
										1.0f,								// opacity,
										0.5f,								// reflectivity,
										1.0f );								// indexOfRefraction
		nvsg::GeoNodeWriteLock (g)->addDrawable (stateSet,d);
		nvsg::BillboardWriteLock(b)->addChild(g);
		nvsg::BillboardWriteLock(b)->setAlignment(nvsg::Billboard::BA_VIEWER);
		nvsg::GroupWriteLock(gr)->addChild(b);
		nvsg::SceneWriteLock(scene)->setRootNode(gr);
	}

  void loadScene(const std::string & filename, nvsg::SceneSharedPtr &scene, const std::vector<std::string> &searchPaths, PlugInCallback *callback )
  {
    // appropriate search paths for the
    // loader dll and the sample file.
	nvsg::ViewStateSharedPtr tmpViewState;
    string modulePath;
    string curDir;
    string dir;
    vector<string> localSearchPaths = searchPaths;

    nvutil::GetCurrentDir(curDir);
    localSearchPaths.push_back(curDir);

    nvutil::GetModulePath(modulePath);
    if ( find( localSearchPaths.begin(), localSearchPaths.end(), modulePath ) == localSearchPaths.end() )
    {
      localSearchPaths.push_back(modulePath);
    }
	std::string dllPath = nvutil::GetProccessDirectory (false);
    if ( find( localSearchPaths.begin(), localSearchPaths.end(), dllPath ) == localSearchPaths.end() )
    {
      localSearchPaths.push_back(dllPath);
    }
    // also add the scene path, if it's not the current path (-> dir.empty()) and not yet added
    nvutil::GetDrvAndDirFromPath(filename, dir);
    if ( !dir.empty() && find( localSearchPaths.begin(), localSearchPaths.end(), dir ) == localSearchPaths.end() )
    {
	
      localSearchPaths.push_back(dir);

    }

    // for supplied models add additional resource paths to make sure
    // that required resources will be found by subsequent loaders
    size_t pos = dir.rfind("media");
    if ( pos != string::npos )
    {
      string nvsgsdk(dir.substr(0, pos));
      localSearchPaths.push_back(nvsgsdk + "media/effects");
      localSearchPaths.push_back(nvsgsdk + "media/textures");
    }
	 //localSearchPaths.push_back("c:/scenix72/media/textures");

    // receive the unique scene loader plug interface ID from the file extension 
    string ext;
    GetFileExtFromPath( filename, ext );
    UPIID piid = nvutil::UPIID( ext.c_str(), nvutil::UPITID( UPITID_SCENE_LOADER, UPITID_VERSION ) );

    nvutil::PlugIn * plug;
    if ( nvutil::getInterface(localSearchPaths, piid, plug) )
    {
      SmartPtr<nvsg::SceneLoader> loader( reinterpret_cast<nvsg::SceneLoader*>(plug) );
	  
      loader->setCallback( callback );

	  if ( !scene )
	  {
		 
		scene = loader->load( filename, localSearchPaths, tmpViewState );
		//nvutil::convertFFPToCGFX (tmpViewState);
		tmpViewState.reset();
	  }
      if ( !scene )
      {
        tmpViewState.reset();
      }

      loader->setCallback( NULL );
	  nvutil::releaseInterface(piid);
    }
  }

  bool saveScene( const std::string & filename, nvsg::SceneSharedPtr &scene, const nvsg::ViewStateSharedPtr & viewState, PlugInCallback *callback )
  {
    bool result = false;
    // define a unique plug-interface ID for SceneLoader
    const UPITID PITID_SCENE_SAVER(UPITID_SCENE_SAVER, UPITID_VERSION);

    string modulePath, dir;
    vector<string> searchPaths;
    GetCurrentDir(dir);
    searchPaths.push_back(dir);

    GetModulePath(modulePath);
    GetDirFromPath(modulePath, dir);
    searchPaths.push_back(dir);

    string ext;
    GetFileExtFromPath(filename, ext);
    UPIID piid = UPIID(ext.c_str(), PITID_SCENE_SAVER);

    PlugIn * plug;

    if ( getInterface( searchPaths, piid, plug ) )
    {
      SceneSaver *ss = reinterpret_cast<SceneSaver *>(plug);
      try
      {
        //SceneSharedPtr scene( ViewStateReadLock(viewState)->getScene() ); // DAR HACK Change SceneSaver interface later.
        result = ss->save( scene, viewState, filename );
      }
      catch(const NVSGException& e) // catch unexpected SceniX errors
      {
        std::cerr << "SceniX critical error: " << e.getErrorMessage().c_str() << std::endl;
      }
      catch(...) // catch all others
      {
      }
    }

    return result;
  }
    void ZoomCameraToNode (ViewStateSharedPtr viewState,nvsg::NodeSharedPtr node)
	{
		CameraSharedPtr camera;
		{
			ViewStateReadLock viewStateLock( viewState );
			camera  = viewStateLock->getCamera();
		}
		if (!camera)
			return;
		Sphere3f sphere( /*NodeReadLock(node)*/ NodeWriteLock(SceneReadLock(ViewStateReadLock(viewState)->getScene())->getRootNode ())->getBoundingSphere() );
        PerspectiveCameraWriteLock cameraLock(camera);
		//ParallelCameraWriteLock cameraLock(camera);
		cameraLock->setFieldOfView (float(AVOCADO_DEFAULT_FOV) );
		if ( isPositive(sphere) )
		{
		//	cameraLock->setFieldOfView (float(AVOCADO_DEFAULT_FOV) );
#if 0
			//sphere.setRadius(sphere.getRadius()*5.0f);
            cameraLock->zoom( sphere, float(AVOCADO_DEFAULT_FOV) );
#endif
		}
	}
  void CreateEmptyCamera (ViewStateSharedPtr viewState)
  {
	CameraSharedPtr camera;
	 RendererOptionsSharedPtr options;
	{
		ViewStateReadLock viewStateLock( viewState );
		camera  = viewStateLock->getCamera();
		  options = viewStateLock->getRendererOptions();
	}
	if (!camera)
	{
		// otherwise create a new one
		camera = nvsg::PerspectiveCamera::create();
//camera = nvsg::ParallelCamera::create();
		{
			PerspectiveCameraWriteLock cameraLock(camera);
		//	ParallelCameraWriteLock cameraLock(camera);
			//cameraLock->zoom(100.0f);
			cameraLock->zoom(1.0f);
			cameraLock->setName( "ViewCamera" );
		}
		ViewStateWriteLock(viewState)->setCamera( camera );

	}
	 // a viewstate needs a valid renderer options object
    if( !options )
    {
      ViewStateWriteLock(viewState)->setRendererOptions( RendererOptions::create() );
	 
    }    
  }

  bool setupDefaultViewState( nvsg::SceneSharedPtr &scene, const nvsg::ViewStateSharedPtr & viewState )
  {
    if (!viewState)
    {
      return false;
    }

    CameraSharedPtr camera;
    RendererOptionsSharedPtr options;

    {
      ViewStateReadLock viewStateLock( viewState );
      camera  = viewStateLock->getCamera();
      options = viewStateLock->getRendererOptions();
    }

    // the viewstate does not have an active camera, set one
    if ( !camera )
    {
      if( !scene )
      {
        return false;
      }
	   // optimizeUnifyVertices (scene);
      SceneReadLock sceneLock(scene);
	
      // if there are cameras in the scene choose the first one
      if ( sceneLock->getNumberOfCameras() )
      {
        camera = *sceneLock->beginCameras();
      }
      else
      {
        // otherwise create a new one
        camera = nvsg::PerspectiveCamera::create();
		//   camera = nvsg::ParallelCamera::create();
        {
          PerspectiveCameraWriteLock cameraLock(camera);
		//	 ParallelCameraWriteLock cameraLock(camera);
          cameraLock->setName( "ViewCamera" );

          // Make scene fit into the viewport.
          if (sceneLock->getRootNode())
          {
			  cameraLock->zoom (1.0f);
			cameraLock->setFieldOfView ( float(AVOCADO_DEFAULT_FOV));

            Sphere3f sphere( sceneLock->getBoundingSphere() );
            if ( isPositive(sphere) )
            {
              cameraLock->zoom( sphere, float(AVOCADO_DEFAULT_FOV) );
            }
			
          }
        }
      }
      ViewStateWriteLock(viewState)->setCamera( camera );
    }
	else
	{
		 if( !scene )
		  {
			return false;
		  }
	    // optimizeUnifyVertices (scene);
		 SceneReadLock sceneLock(scene);
		// ParallelCameraWriteLock cameraLock(camera);
	     PerspectiveCameraWriteLock cameraLock(camera);
		 if (sceneLock->getRootNode())
          {
			  Sphere3f sphere( NodeReadLock(sceneLock->getRootNode())->getBoundingSphere() );
            if ( isValid(sphere) )
            {
             // cameraLock->zoom(100.1f);//sphere, float(nvmath::PI_QUARTER) );
				cameraLock->zoom(sphere, float(AVOCADO_DEFAULT_FOV) );
			 
				// Needed for ortho cam ..
		
				cameraLock->setPosition(  -2*sphere.getRadius() * cameraLock->getDirection());
			  float dist = nvmath::distance (cameraLock->getPosition(),sphere.getCenter());
			  cameraLock->setFocusDistance (dist);
			  ViewStateWriteLock(viewState)->setTargetDistance(dist);
			  
		//	theCamera->setPosition(theCamera->getPosition()  + step * theCamera->getDirection());
            }
          }
	}
	 
    // a viewstate needs a valid renderer options object
    if( !options )
    {
      ViewStateWriteLock(viewState)->setRendererOptions( RendererOptions::create() );
	 
    }    

    return true;
  }
  void SetLightDirection (nvsg::SceneSharedPtr scene,float val, int LightIndex, int axis)
  {
	  NodeSharedPtr root = SceneWriteLock(scene)->getRootNode ();
	  stringstream lightName;
	  lightName << "AvocadoDirected" << LightIndex;

	  Group::LightSourceIterator lsit = GroupWriteLock(root)->beginLightSources ();
	  while (lsit != GroupWriteLock(root)->endLightSources ())
	  {
		  if (LightSourceWriteLock (*lsit)->getName () == lightName.str ())
		  {
			  DirectedLightWriteLock theLight (*lsit);
			  Vec3f dir = theLight->getDirection ();
			  dir [axis] = val;
			  dir.normalize ();
			  theLight->setDirection (dir);
			  break;
		  }
		  ++lsit;
	  }
  }
  void SetLightIntensity (nvsg::SceneSharedPtr scene,float val, int LightIndex)
  {
	  NodeSharedPtr root = SceneWriteLock(scene)->getRootNode ();
	  stringstream lightName;
	  lightName << "AvocadoDirected" << LightIndex;

	  Group::LightSourceIterator lsit = GroupWriteLock(root)->beginLightSources ();
	  while (lsit != GroupWriteLock(root)->endLightSources ())
	  {
		  if (LightSourceWriteLock (*lsit)->getName () == lightName.str ())
		  {
			  DirectedLightWriteLock theLight (*lsit);
			  theLight->setIntensity (val);
			  break;
		  }
		  ++lsit;
	  }
  }
   void SetLightEnabled (nvsg::SceneSharedPtr scene,bool val, int LightIndex)
  {
	  NodeSharedPtr root = SceneWriteLock(scene)->getRootNode ();
	  stringstream lightName;
	  lightName << "AvocadoDirected" << LightIndex;

	  Group::LightSourceIterator lsit = GroupWriteLock(root)->beginLightSources ();
	  while (lsit != GroupWriteLock(root)->endLightSources ())
	  {
		  if (LightSourceWriteLock (*lsit)->getName () == lightName.str ())
		  {
			  DirectedLightWriteLock theLight (*lsit);
			  theLight->setEnabled (val);
			  break;
		  }
		  ++lsit;
	  }
  }
   void SetLightAmbientColor (nvsg::SceneSharedPtr scene,float valR,float valG,float valB, int LightIndex)
  {
	  NodeSharedPtr root = SceneWriteLock(scene)->getRootNode ();
	  stringstream lightName;
	  lightName << "AvocadoDirected" << LightIndex;

	  Group::LightSourceIterator lsit = GroupWriteLock(root)->beginLightSources ();
	  while (lsit != GroupWriteLock(root)->endLightSources ())
	  {
		  if (LightSourceWriteLock (*lsit)->getName () == lightName.str ())
		  {
			  DirectedLightWriteLock theLight (*lsit);
			 // theLight->setEnabled (val);
			  theLight->setAmbientColor (Vec3f (valR,valG,valB));
			  break;
		  }
		  ++lsit;
	  }
  }
  void SetLightDiffuseColor (nvsg::SceneSharedPtr scene,float valR,float valG,float valB, int LightIndex)
  {
	  NodeSharedPtr root = SceneWriteLock(scene)->getRootNode ();
	  stringstream lightName;
	  lightName << "AvocadoDirected" << LightIndex;

	  Group::LightSourceIterator lsit = GroupWriteLock(root)->beginLightSources ();
	  while (lsit != GroupWriteLock(root)->endLightSources ())
	  {
		  if (LightSourceWriteLock (*lsit)->getName () == lightName.str ())
		  {
			  DirectedLightWriteLock theLight (*lsit);
			 
			    theLight->setDiffuseColor (Vec3f (valR,valG,valB));
			  break;
		  }
		  ++lsit;
	  }
  }
    void SetLightSpecularColor (nvsg::SceneSharedPtr scene,float valR,float valG,float valB, int LightIndex)
  {
	  NodeSharedPtr root = SceneWriteLock(scene)->getRootNode ();
	  stringstream lightName;
	  lightName << "AvocadoDirected" << LightIndex;

	  Group::LightSourceIterator lsit = GroupWriteLock(root)->beginLightSources ();
	  while (lsit != GroupWriteLock(root)->endLightSources ())
	  {
		  if (LightSourceWriteLock (*lsit)->getName () == lightName.str ())
		  {
			  DirectedLightWriteLock theLight (*lsit);
			   theLight->setSpecularColor (Vec3f (valR,valG,valB));
			  break;
		  }
		  ++lsit;
	  }
  }
  bool DrinkMaterialsFromStateSet (nvsg::StateSetSharedPtr ss, float ambient[3],float diffuse[3],float specular[3],float &shin,float &opac,string &orgTex)
  {
	  	      nvsg::StatePass::AttributeIterator ssTex  = StateSetWriteLock (ss)->findAttribute( OC_TEXTUREATTRIBUTE );
			if (ssTex != StateSetWriteLock (ss)->endAttributes ())
			{
				nvsg::TextureAttributeSharedPtr tessTex =  nvutil::sharedPtr_cast<TextureAttribute>( *ssTex ) ;
				nvsg::TextureAttributeItemSharedPtr tai = TextureAttributeWriteLock (tessTex)->getTextureAttributeItem(0);
				if (tai)
				{
					orgTex = TextureAttributeItemWriteLock (tai)->getTextureFileName();
				}

			}
	        nvsg::StatePass::AttributeIterator ssMaterial  = StateSetWriteLock (ss)->findAttribute( OC_MATERIAL );
			if (ssMaterial != StateSetWriteLock (ss)->endAttributes ())
			{
				nvsg::MaterialSharedPtr tessMaterial =  nvutil::sharedPtr_cast<Material>( *ssMaterial ) ;
				nvmath::Vec3f ambientV = nvsg::MaterialWriteLock (tessMaterial)->getAmbientColor (); //front
				nvmath::Vec3f diffuseV = nvsg::MaterialWriteLock (tessMaterial)->getDiffuseColor (); //front
				nvmath::Vec3f specularV = nvsg::MaterialWriteLock (tessMaterial)->getSpecularColor (); //front
				shin = nvsg::MaterialWriteLock (tessMaterial)->getSpecularExponent (); //front
				opac = nvsg::MaterialWriteLock (tessMaterial)->getOpacity (); //front
				ambient[0] = ambientV[0];
				ambient[1] = ambientV[1];
				ambient[2] = ambientV[2];
				diffuse[0] = diffuseV[0];
				diffuse[1] = diffuseV[1];
				diffuse[2] = diffuseV[2];
				specular [0] = specularV[0];
				specular[1] = specularV[1];
				specular[2] = specularV[2];
				return true;
			}
			return false;
  }
  void SetLightsPreset (nvsg::SceneSharedPtr scene,int pre,vector <LightSourceSharedPtr> &return_lights)
  {
	  NodeSharedPtr root = SceneWriteLock(scene)->getRootNode ();

	  vector <LightSourceSharedPtr> lights;
      
	  Group::LightSourceIterator lsit = GroupWriteLock(root)->beginLightSources ();
	  while (lsit != GroupWriteLock(root)->endLightSources ())
	  {
		  if (LightSourceWriteLock (*lsit)->getName () == "AvocadoAnnotation")
			  lights.push_back (*lsit);
		  ++lsit;
	  }
	  GroupWriteLock  (root)->clearLightSources ();
	  if (pre == 0)
	  {
		    nvsg::DirectedLightSharedPtr dl = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight(dl);
			pLight->setName ("AvocadoDirected1");
			pLight->setAmbientColor( nvmath::Vec3f(0.2f, 0.2f, 0.2f) );
			pLight->setDiffuseColor( nvmath::Vec3f(0.9f, 1.0f, 1.0f) ); //0.9 1.0 1.0
			pLight->setSpecularColor( nvmath::Vec3f(0.6f, 0.6f, 0.6f) ); // 0.5
		    nvmath::Vec3f dir1 = nvmath::Vec3f ( -1.0f, -1.0f, -1.0f);
			dir1.normalize();
			pLight->setDirection (dir1);
			pLight->setIntensity (0.8f);
			lights.push_back (dl);
		    return_lights.push_back (dl);

			nvsg::DirectedLightSharedPtr dl2 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight2(dl2);
			pLight2->setName ("AvocadoDirected2");
			pLight2->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight2->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 0.9f) ); //0.9 1.0 1.0
			pLight2->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir2 = nvmath::Vec3f ( 0.5f,-0.7f,-1.0f);
			dir2.normalize();
			pLight2->setDirection (dir2);	pLight2->setIntensity (0.8f);
			lights.push_back (dl2);
			return_lights.push_back (dl2);

			nvsg::DirectedLightSharedPtr dl3 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight3(dl3);
			pLight3->setName ("AvocadoDirected3");
			pLight3->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight3->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 1.0f) ); //0.9 1.0 1.0
			pLight3->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir3 = nvmath::Vec3f ( 0.75f,-1.0f, 0.75f);
			dir3.normalize();
			pLight3->setDirection (dir3);
			pLight3->setIntensity (0.9f);
			pLight3->setEnabled (false);
			lights.push_back (dl3);
			return_lights.push_back (dl3);
	  }
	  if (pre == 1)
	  {
		  		    nvsg::DirectedLightSharedPtr dl = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight(dl);
			pLight->setName ("AvocadoDirected1");
			pLight->setAmbientColor( nvmath::Vec3f(0.2f, 0.2f, 0.2f) );
			pLight->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 0.9f) ); //0.9 1.0 1.0
			pLight->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir1 = nvmath::Vec3f (0.4f,-1.0f,-1.0f);
			dir1.normalize();
			pLight->setDirection (dir1);
			pLight->setIntensity (0.8f);
			lights.push_back (dl);
		    return_lights.push_back (dl);

		//	nvsg::SpotLightSharedPtr dl2 = nvsg::SpotLight::create();
		//    SpotLightWriteLock pLight2(dl2);
			nvsg::DirectedLightSharedPtr dl2 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight2(dl2);
			pLight2->setName ("AvocadoDirected2");
			//pLight2->setPosition (nvmath::Vec3f (-1.0,-1.0,-1.0));
			pLight2->setAmbientColor( nvmath::Vec3f(0.2f, 0.2f, 0.2f) );
			pLight2->setDiffuseColor( nvmath::Vec3f(1.0f, 0.9f, 0.9f) ); //0.9 1.0 1.0
			pLight2->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir2 = nvmath::Vec3f ( 1.0f, -1.0f, 1.0f);
			dir2.normalize();
			pLight2->setDirection (dir2);
			pLight2->setIntensity (0.8f);
			lights.push_back (dl2);
			return_lights.push_back (dl2);

			nvsg::DirectedLightSharedPtr dl3 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight3(dl3);
			pLight3->setName ("AvocadoDirected3");
			pLight3->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight3->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 1.0f) ); //0.9 1.0 1.0
			pLight3->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir3 = nvmath::Vec3f ( 0.75f,-1.0f, 0.75f);
			dir3.normalize();
			pLight3->setDirection (dir3);
			pLight3->setIntensity (0.9f);
			pLight3->setEnabled (false);
			lights.push_back (dl3);
			return_lights.push_back (dl3);
	  }
	  else if  (pre == 2)
	  {
		  // spot light was taken from ufos_all_glows.. I added directed light. should cover entire space.
		  	//	    nvsg::SpotLightSharedPtr dl = nvsg::SpotLight::create();
		   // SpotLightWriteLock pLight(dl);
			
			nvsg::DirectedLightSharedPtr dl = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight(dl);

			pLight->setName ("AvocadoDirected1");
			//pLight->setAttenuation( 0.0f, 1.0f/sqrt(2.0f), 1.0f/sqrt(2.0f) );
			pLight->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight->setDiffuseColor( nvmath::Vec3f(0.9f, 0.9f, 0.9f) ); //0.9 1.0 1.0
			pLight->setSpecularColor( nvmath::Vec3f(0.6f, 0.6f, 0.6f) ); // 0.5
			nvmath::Vec3f dir1 = nvmath::Vec3f ( -0.235349f ,-0.583537f ,0.777236f);
			dir1.normalize();
			pLight->setDirection (dir1);
			pLight->setIntensity (1.0f);
			
			//pLight->setPosition(nvmath::Vec3f(-1.74917f, -13.2174f, -38.8674f));
			lights.push_back (dl);
		    return_lights.push_back (dl);

			nvsg::DirectedLightSharedPtr dl2 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight2(dl2);
			pLight2->setName ("AvocadoDirected2");
			pLight2->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight2->setDiffuseColor( nvmath::Vec3f(0.92f, 0.92f, 0.9f) ); //0.9 1.0 1.0
			pLight2->setSpecularColor( nvmath::Vec3f(0.6f, 0.6f, 0.6f) ); // 0.5
			nvmath::Vec3f dir2 = nvmath::Vec3f (-0.0933888f ,0.0868561f, -0.991834f);
			dir2.normalize();
			pLight2->setDirection (dir2);
			//pLight->setp-1.74917 -13.2174 -38.8674
			pLight2->setIntensity (1.0f);
			lights.push_back (dl2);
			return_lights.push_back (dl2);

			nvsg::DirectedLightSharedPtr dl3 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight3(dl3);
			pLight3->setName ("AvocadoDirected3");
			pLight3->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight3->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 1.0f) ); //0.9 1.0 1.0
			pLight3->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir3 = nvmath::Vec3f ( 0.75f,-1.0f, 0.75f);
			dir3.normalize();
			pLight3->setDirection (dir3);
			pLight3->setIntensity (0.9f);
			pLight3->setEnabled (false);
			lights.push_back (dl3);
			return_lights.push_back (dl3);
	  }
	  else if (pre == 3)
	  {
		  		    nvsg::DirectedLightSharedPtr dl = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight(dl);
			pLight->setName ("AvocadoDirected1");
			pLight->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight->setDiffuseColor( nvmath::Vec3f(0.9f, 1.0f, 1.0f) ); //0.9 1.0 1.0
			pLight->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			
			nvmath::Vec3f dir1 = nvmath::Vec3f (  0.5f,  -0.9f,  0.2f);
			dir1.normalize();
			pLight->setDirection (dir1);
			pLight->setIntensity (1.0f);
			lights.push_back (dl);
		    return_lights.push_back (dl);

			nvsg::DirectedLightSharedPtr dl2 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight2(dl2);
			pLight2->setName ("AvocadoDirected2");
			pLight2->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight2->setDiffuseColor( nvmath::Vec3f(0.99f, 0.99f, 0.94f) ); //0.9 1.0 1.0
			pLight2->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir2 = nvmath::Vec3f ( 0.5f,-0.9f,-1.0f);
			dir2.normalize();
			pLight2->setDirection (dir2);		pLight2->setIntensity (0.5f);
			lights.push_back (dl2);
			return_lights.push_back (dl2);

			nvsg::DirectedLightSharedPtr dl3 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight3(dl3);
			pLight3->setName ("AvocadoDirected3");
			pLight3->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight3->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 1.0f) ); //0.9 1.0 1.0
			pLight3->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir3 = nvmath::Vec3f ( 0.75f,-1.0f, 0.75f);
			dir3.normalize();
			pLight3->setDirection (dir3);
			pLight3->setIntensity (0.9f);
			lights.push_back (dl3);
			return_lights.push_back (dl3);
	  }
	  else if (pre == 4)
	  {
		  		    nvsg::DirectedLightSharedPtr dl = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight(dl);
			pLight->setName ("AvocadoDirected1");
			pLight->setAmbientColor( nvmath::Vec3f(0.1f, 0.1f, 0.1f) );
			pLight->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 0.9f) ); //0.9 1.0 1.0
			pLight->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir1 = nvmath::Vec3f (   1.0f, -1.0f, 1.0f);
			dir1.normalize();
			pLight->setDirection (dir1);
			pLight->setIntensity (0.9f);
			lights.push_back (dl);
		    return_lights.push_back (dl);

			nvsg::DirectedLightSharedPtr dl2 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight2(dl2);
			pLight2->setName ("AvocadoDirected2");
			pLight2->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight2->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 0.9f) ); //0.9 1.0 1.0
			pLight2->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir2 = nvmath::Vec3f (-0.6f,-0.03f,-1.0f);
			dir2.normalize();
			pLight2->setDirection (dir2);
			pLight2->setIntensity (0.5f);
			lights.push_back (dl2);
			return_lights.push_back (dl2);

			nvsg::DirectedLightSharedPtr dl3 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight3(dl3);
			pLight3->setName ("AvocadoDirected3");
			pLight3->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight3->setDiffuseColor( nvmath::Vec3f(0.9f, 0.97f, 0.9f) ); //0.9 1.0 1.0
			pLight3->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir3 = nvmath::Vec3f (  0.0f,-1.0f, 0.70f);
			dir3.normalize();
			pLight3->setDirection (dir3);
			pLight3->setIntensity (0.9f);
			lights.push_back (dl3);
			return_lights.push_back (dl3);
	  }
	  else if (pre == 5)
	  {
		  
			nvsg::DirectedLightSharedPtr dl = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight(dl);
			pLight->setName ("AvocadoDirected1");
			pLight->setAmbientColor( nvmath::Vec3f(0.1f, 0.1f, 0.1f) );
			pLight->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 0.9f) ); //0.9 1.0 1.0
			pLight->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir1 = nvmath::Vec3f (1.0f, -1.0f, 1.0f);
			dir1.normalize();
			pLight->setDirection (dir1);
			pLight->setIntensity (0.7f);
			lights.push_back (dl);
		    return_lights.push_back (dl);

			nvsg::DirectedLightSharedPtr dl2 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight2(dl2);
			pLight2->setName ("AvocadoDirected2");
			pLight2->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight2->setDiffuseColor( nvmath::Vec3f(1.0f, 1.0f, 0.9f) ); //0.9 1.0 1.0
			pLight2->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir2 = nvmath::Vec3f ( 0.25f,-1.0f,  1.0f);
			dir2.normalize();
			pLight2->setDirection (dir2);
			pLight2->setIntensity (0.9f);
			lights.push_back (dl2);
			return_lights.push_back (dl2);

			nvsg::DirectedLightSharedPtr dl3 = nvsg::DirectedLight::create();
		    DirectedLightWriteLock pLight3(dl3);
			pLight3->setName ("AvocadoDirected3");
			pLight3->setAmbientColor( nvmath::Vec3f(0.0f, 0.0f, 0.0f) );
			pLight3->setDiffuseColor( nvmath::Vec3f(0.9f, 0.9f, 0.87f) ); //0.9 1.0 1.0
			pLight3->setSpecularColor( nvmath::Vec3f(0.4f, 0.4f, 0.4f) ); // 0.5
			nvmath::Vec3f dir3 = nvmath::Vec3f (0.5f,-1.0f, 1.0f);
			dir3.normalize();
			pLight3->setDirection (dir3);
			pLight3->setIntensity (0.5f);
			lights.push_back (dl3);
			return_lights.push_back (dl3);
	  }
	  for (size_t k=0;k<lights.size();k++)
	  {
		  GroupWriteLock  (root)->addLightSource (lights[k]);
	  }
  }
  std::string GetProccessDirectory (bool media)
		{ 
			// windows specific stuff inside the engine is never a good thing :(
			 char  szPath[MAX_PATH];
#ifdef _UNICODE
			 HMODULE mh = GetModuleHandle (L"AvocadoEngine.dll");
#else
			 HMODULE mh = GetModuleHandle ("AvocadoEngine.dll");
#endif
			if (GetModuleFileNameA(mh, szPath, ARRAYSIZE(szPath)))
			{
				std::string mfp (szPath);
				mfp = mfp.substr (0,mfp.rfind ("\\"));
				//mfp += "\\";
				if (media)
				{
				    mfp += string ("\\media\\");
				}
				return mfp; 
			}
			if (media)
			  return string(".\\media\\");
			return string ("");
		}
  bool createDefaultHeadLight( const nvsg::CameraSharedPtr & camera, nvmath::Vec3f offset , bool isspot)
  {
    if ( camera )
    {
		if (!isspot)
		{
			  nvsg::PointLightSharedPtr pointLightHdl = nvsg::PointLight::create();
			  {
				PointLightWriteLock pointLight(pointLightHdl);
				pointLight->setAmbientColor( nvmath::Vec3f(0.2f, 0.2f, 0.2f) );
				pointLight->setDiffuseColor( nvmath::Vec3f(0.9f, 1.0f, 1.0f) ); //0.9 1.0 1.0
				pointLight->setSpecularColor( nvmath::Vec3f(0.5f, 0.5f, 0.45f) ); // 0.5
				pointLight->setAttenuation( 1.0f, 0.0,0.0);//1.0f/sqrt(2.0f), 1.0f/sqrt(2.0f) );
				pointLight->setPosition( offset );  // Default off set is zero.
				//pointLight->seto
				//pointLight->setIntensity (1.0f);
				pointLight->setIntensity (0.85f);
			  }
		

			  CameraWriteLock(camera)->addHeadLight( pointLightHdl);
		} 
		else
		{
			  nvsg::SpotLightSharedPtr pointLightHdl = nvsg::SpotLight::create();
			  {
				SpotLightWriteLock pointLight(pointLightHdl);
				pointLight->setAmbientColor( nvmath::Vec3f(0.2f, 0.2f, 0.2f) );
				pointLight->setDiffuseColor( nvmath::Vec3f(0.95f, 0.99f, 0.99f) ); //0.9 1.0 1.0
				pointLight->setSpecularColor( nvmath::Vec3f(0.5f, 0.5f, 0.46f) ); // 0.5
				pointLight->setAttenuation( 0.0f, 1.0f/sqrt(2.0f), 1.0f/sqrt(2.0f) );
				pointLight->setPosition( offset );  // Default offset is zero.
				pointLight->setIntensity (0.5f);
				pointLight->setDirection(nvmath::Vec3f (0.0f,0.0f,-1.0f));
				//pointLight->setIntensity (1.0f);
			  }
		

			  CameraWriteLock(camera)->addHeadLight( pointLightHdl);
		} 
    }

    return( !!camera );
  }

  void optimizeScene( const nvsg::SceneSharedPtr & scene, bool ignoreNames, bool identityToGroup
                    , unsigned int combineFlags, unsigned int eliminateFlags, unsigned int unifyFlags
                    , float epsilon )
  {
    if ( identityToGroup )
    {
      SmartPtr<IdentityToGroupTraverser> itgt( new IdentityToGroupTraverser );
      itgt->setIgnoreNames( ignoreNames );
      itgt->apply( scene );
    }

  //  loop over optimizers until nothing changed
#define UNDEFINED_TRAVERSER ~0
#define ELIMINATE_TRAVERSER 0
#define COMBINE_TRAVERSER   1
#define UNIFY_TRAVERSER     2
    int lastModifyingTraverser = UNDEFINED_TRAVERSER;

    bool modified;
    do 
    {
      modified = false;
      //  first eliminate redundant/degenerated objects
      if ( eliminateFlags && ( lastModifyingTraverser != ELIMINATE_TRAVERSER ) )
      {
        SmartPtr<EliminateTraverser> et( new EliminateTraverser );
        et->setIgnoreNames( ignoreNames );
        et->setEliminateTargets( eliminateFlags );
        et->apply( scene );
        if ( et->getTreeModified() )
        {
          modified = true;
          lastModifyingTraverser = ELIMINATE_TRAVERSER;
        }
      }

      // second combine compatible objects
      if ( combineFlags  && ( lastModifyingTraverser != COMBINE_TRAVERSER ) )
      {
        SmartPtr<CombineTraverser> ct( new CombineTraverser );
        ct->setIgnoreNames( ignoreNames );
        ct->setCombineTargets( combineFlags );
        ct->apply( scene );
        if ( ct->getTreeModified() )
        {
          modified = true;
          lastModifyingTraverser = COMBINE_TRAVERSER;
        }
      }

      // third unify all equivalent objects
      if ( unifyFlags  && ( lastModifyingTraverser != UNIFY_TRAVERSER ) )
      {
        SmartPtr<UnifyTraverser> ut( new UnifyTraverser );
        ut->setIgnoreNames( ignoreNames );
        ut->setUnifyTargets( unifyFlags );
        if ( unifyFlags & UnifyTraverser::UT_VERTICES )
        {
          ut->setEpsilon( epsilon );
        }
        ut->apply( scene );
        if ( ut->getTreeModified() )
        {
          modified = true;
          lastModifyingTraverser = UNIFY_TRAVERSER;

          if ( unifyFlags & UnifyTraverser::UT_VERTICES )
          {
            // after unifying vertices we need to re-normalize the normals
            SmartPtr<NormalizeTraverser> nt( new NormalizeTraverser );
            nt->apply( scene );
          }
        }
      }
    } while( modified );
  }

  void optimizeForRaytracing( const nvsg::SceneSharedPtr & scene)
  {
    bool ignoreNames = false;

    //  first some preprocessing optimizers
    //  -> no specific order here
    {
      SmartPtr<IdentityToGroupTraverser> tr( new IdentityToGroupTraverser );
      tr->setIgnoreNames( ignoreNames );
      tr->apply( scene );
    }

    {
      SmartPtr<DestrippingTraverser> tr( new DestrippingTraverser );
      tr->apply( scene );
    }

    {
      SmartPtr<TriangulateTraverser> tr( new TriangulateTraverser );
      tr->apply( scene );
    }

    //  loop over optimizers until nothing changed
    bool modified = false;
    float vertexEpsilon = FLT_EPSILON;
    do
    {
      //  first eliminate redundant/degenerated objects
      {
        SmartPtr<EliminateTraverser> tr( new EliminateTraverser );
        tr->setIgnoreNames( ignoreNames );
        tr->setEliminateTargets( EliminateTraverser::ET_ALL_TARGETS_MASK );
        tr->apply( scene );
        modified = tr->getTreeModified();
      }

      // second combine compatible objects
      {
        SmartPtr<CombineTraverser> tr( new CombineTraverser );
        tr->setIgnoreNames( ignoreNames );
        tr->setCombineTargets( CombineTraverser::CT_ALL_TARGETS_MASK );
        tr->apply( scene );
        modified = tr->getTreeModified();
      }
     } while( modified );
  }
  void convertFFPToCGFX(  nvsg::SceneSharedPtr & scene, bool flipYZ)
  {
    bool ignoreNames = false;

    //  first some preprocessing optimizers
    //  -> no specific order here
    {

		FFPToCgFxTraverser *ffpt = new FFPToCgFxTraverser ;
		ffpt->SetFlipYZ (flipYZ);
      SmartPtr<FFPToCgFxTraverser> tr(ffpt );
      //tr->setIgnoreNames( ignoreNames );
	  NodeSharedPtr ro = SceneWriteLock (scene)->getRootNode ();
	  tr->apply(ro);

	   tr.reset();
	  // delete ffpt;
	}
  }
  void convertFFPToCGFXNode(  nvsg::NodeSharedPtr & node,bool flipYZ)
  {
    bool ignoreNames = false;

    //  first some preprocessing optimizers
    //  -> no specific order here
    {

		FFPToCgFxTraverser *ffpt = new FFPToCgFxTraverser ;
		ffpt->SetFlipYZ (flipYZ);
      SmartPtr<FFPToCgFxTraverser> tr(ffpt );
      //tr->setIgnoreNames( ignoreNames );
	  tr->apply( node) ;//SceneWriteLock (scene)->getRootNode () );

	   tr.reset();
	  // delete ffpt;
	}
  }

  void optimizeUnifyVertices( const nvsg::SceneSharedPtr & scene )
  {
    unsigned int unifySelection = 0;
    unifySelection |= UnifyTraverser::UT_VERTICES;

    SmartPtr<UnifyTraverser> tr( new UnifyTraverser );
    tr->setIgnoreNames( false );
    tr->setUnifyTargets( unifySelection );
    tr->setEpsilon( FLT_EPSILON );
    tr->apply( scene );

    // after unifying vertices we need to re-normalize the normals
    SmartPtr<NormalizeTraverser> nt( new NormalizeTraverser );
    nt->apply( scene );
  }

  SmartPtr<RayIntersectTraverser> applyPicker( const ViewStateSharedPtr &viewStatePtr, const SmartRenderTarget &renderTarget, int windowX, int windowY )
  {
    NVSG_ASSERT( viewStatePtr );
    NVSG_ASSERT( renderTarget );

    ViewStateReadLock viewState( viewStatePtr );

    SmartPtr<RayIntersectTraverser> picker;

    unsigned int windowWidth, windowHeight;
    renderTarget->getSize( windowWidth, windowHeight );

    CameraSharedPtr pCam = viewState->getCamera();
    if ( pCam && isPtrTo<FrustumCamera>(pCam) ) // requires a frustum camera attached to the ViewState
    {
      picker = new RayIntersectTraverser;

      // calculate ray origin and direction from the input point
      Vec3f rayOrigin;
      Vec3f rayDir;

      FrustumCameraReadLock fc(sharedPtr_cast<FrustumCamera>(pCam));
      fc->getPickRay( windowX, windowHeight - 1 - windowY, windowWidth, windowHeight, rayOrigin, rayDir );

      // run the intersect traverser for intersections with the given ray
      picker->setRay( rayOrigin, rayDir );
      picker->setViewportSize( windowWidth, windowHeight );
      picker->apply( viewStatePtr );
    }
    return picker;
  }

  float getIntersectionDistance( const ViewStateSharedPtr &smartViewState, const SmartRenderTarget &renderTarget, int windowX, int windowY )
  {
    float result = -1.0f;
    SmartPtr<RayIntersectTraverser> picker = applyPicker( smartViewState, renderTarget, windowX, windowY );
    if (picker && picker->getNumberOfIntersections() > 0)
    {
      result = picker->getNearest().getDist();
    }

    return result;
  }

  bool intersectObject( const nvsg::ViewStateSharedPtr &smartViewState, const SmartRenderTarget &renderTarget,
                        unsigned int windowX, unsigned int windowY, nvtraverser::Intersection & result )
  {
    SmartPtr<RayIntersectTraverser> picker = applyPicker( smartViewState, renderTarget, windowX, windowY );
    if (picker && picker->getNumberOfIntersections() > 0)
    {
      result = picker->getNearest();
      return true;
    }

    return false;
  }

  bool saveTextureHost( const std::string & filename, const TextureHostSharedPtr & tih )
  {
    string ext;
    GetFileExtFromPath(filename, ext);

    // load the saver plug-in  - this should be configured
    UPIID piid = UPIID(ext.c_str(), UPITID(UPITID_TEXTURE_SAVER, UPITID_VERSION) );

    PlugIn * plug = 0;

    std::string modulePath, dir;
    std::vector<std::string> searchPaths;
    GetCurrentDir(dir);
    searchPaths.push_back(dir);

    GetModulePath(modulePath);
    GetDirFromPath(modulePath, dir);
    searchPaths.push_back(dir);

    //
    // MMM - TODO - Update me for stereo images
    //
    TextureSaver * ts;
    if ( getInterface( searchPaths, piid, plug ) )
    {
      ts = reinterpret_cast<TextureSaver *>(plug);
    }
    else
    {
      // signal some kind of error
      return false;
    }

    bool retval;

    // save the file
    retval = ts->save( tih, filename );

    return retval;
  }

  bool loadTextureHost( const std::string & filename, TextureHostSharedPtr & tih, const std::vector<std::string> &searchPaths )
  {
    tih.reset();
    // appropriate search paths for the loader dll and the sample file.
    string modulePath;
    string curDir;
    string dir;
    vector<string> localSearchPaths = searchPaths;

    nvutil::GetCurrentDir(curDir);
    localSearchPaths.push_back(curDir);

    nvutil::GetModulePath(modulePath);
    if ( find( localSearchPaths.begin(), localSearchPaths.end(), modulePath ) == localSearchPaths.end() )
    {
      localSearchPaths.push_back(modulePath);
    }
#ifdef _WIN32
	// not good..remove this its not helping anyway
	//std::string dllPath = nvutil::GetProccessDirectory(false);
	//nvutil::SetCurrentDir(dllPath);
#endif

    // also add the texture's path, if it's not the current path (-> dir.empty()) and not yet added
    nvutil::GetDrvAndDirFromPath(filename, dir);
    if ( !dir.empty() && find( localSearchPaths.begin(), localSearchPaths.end(), dir ) == localSearchPaths.end() )
    {
      localSearchPaths.push_back(dir);
    }

    // HACK Add SceniX media folders for supplied models add additional resource paths
    // to make sure that required resources will be found by subsequent loaders.
    size_t pos = dir.rfind("media");
    if ( pos != string::npos )
    {
      string nvsgsdk( dir.substr(0, pos) );
      localSearchPaths.push_back(nvsgsdk + "media/effects");
      localSearchPaths.push_back(nvsgsdk + "media/textures");
    }

    string ext;
    GetFileExtFromPath(filename, ext);

    UPIID piid = UPIID( ext.c_str(), UPITID(UPITID_TEXTURE_LOADER, UPITID_VERSION) );

    PlugIn * plug = 0;

    // TODO - Update me for stereo images
    TextureLoader * tls;
    if ( getInterface( localSearchPaths, piid, plug ) )
    {
      tls = reinterpret_cast<TextureLoader *>(plug);
    }
    else
    {
      // signal some kind of error
      return false;
    }

    std::string foundFile;
    if ( FindFileFirst( filename, localSearchPaths, foundFile) ) // lookup the file
    {
      tih = tls->load( foundFile );
    }
//	plug->setCallback( NULL );
//	nvutil::releaseInterface(piid);
    
	return tih;
  }

} // namespace nvutil

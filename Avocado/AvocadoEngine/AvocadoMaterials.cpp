
#include "AvocadoMaterials.h"
#include "nvsg/CgFx.h"
#include "SceneFunctions.h"
#include "nvutil\nvutil.h"
#include <nvutil/Tools.h>
#include <nvmath/nvmath.h>
#include <nvsg/ViewState.h>
#include <nvsg/Face.h>
#include <nvsg/FaceAttribute.h>
#include <nvsg/GeoNode.h>
#include <nvsg/Material.h>
#include <nvsg/Node.h>
#include <nvsg/StateSet.h>
#include <nvsg/TextureAttribute.h>
#include <nvsg/Transform.h>
#include <nvsg/BlendAttribute.h>
#include <nvsg/DepthAttribute.h>
#include <nvutil/Tools.h>
#include <nvutil/PlugIn.h>
#include <nvsg/PlugInterface.h>
#include <nvsg/PlugInterfaceID.h>
#include "nvutil/DbgNew.h" // this must be the last include
using namespace nvsg;
namespace avocado
{
	// material interface
	bool AvocadoMaterialInterfaceFromString (ParamListSharedPtr &ppl,AvocadoMaterialInterface *materialData)
	{
		bool res = ppl->GetFloatValueByName ("baseColorR",materialData->baseColor[0]);
		res |= ppl->GetFloatValueByName ("baseColorG",materialData->baseColor[1]);
        res |= ppl->GetFloatValueByName ("baseColorB",materialData->baseColor[2]);
		res |= ppl->GetFloatValueByName ("ambient",materialData->ambient);
		res |= ppl->GetFloatValueByName ("diffuse",materialData->diffuse);
		res |= ppl->GetFloatValueByName ("specular",materialData->specular);
        res |= ppl->GetFloatValueByName ("shininess",materialData->shininess);
        res |= ppl->GetFloatValueByName ("opacity",materialData->opacity);
        res |= ppl->GetFloatValueByName ("reflectivity",materialData->reflectivity);
		res |= ppl->GetFloatValueByName ("ior",materialData->ior);
        res |= ppl->GetFloatValueByName ("frenselExp",materialData->frenselExp);
        res |= ppl->GetFloatValueByName ("reflectionMin",materialData->reflectionMin);
        res |= ppl->GetFloatValueByName ("reflectionMax",materialData->reflectionMax);
		res |= ppl->GetFloatValueByName ("roughness",materialData->roughness);
        res |= ppl->GetFloatValueByName ("polished",materialData->polished);
        res |= ppl->GetFloatValueByName ("exposure",materialData->exposure);
        res |= ppl->GetFloatValueByName ("bumpHeight",materialData->bumpHeight);
		res |= ppl->GetFloatValueByName ("bumpScale",materialData->bumpScale);
		res |= ppl->GetBoolValueByName ("bumpIsNormalMap",materialData->bumpIsNormalMap);
		 res |= ppl->GetBoolValueByName ("bumpEnabled",materialData->bumpEnabled);
        res |= ppl->GetBoolValueByName ("isWood",materialData->isWood);
        res |= ppl->GetFloatValueByName ("wooble",materialData->wooble);
        res |= ppl->GetFloatValueByName ("woodScale",materialData->woodScale);
		res |= ppl->GetFloatValueByName ("woodOffset0",materialData->woodOffset[0]);
        res |= ppl->GetFloatValueByName ("woodOffset1",materialData->woodOffset[1]);
        res |= ppl->GetFloatValueByName ("woodOffset2",materialData->woodOffset[2]);
		res |= ppl->GetFloatValueByName ("woodColor0",materialData->woodColor[0]);
		res |= ppl->GetFloatValueByName ("woodColor1",materialData->woodColor[1]);
		res |= ppl->GetFloatValueByName ("woodColor2",materialData->woodColor[2]);
		res |= ppl->GetFloatValueByName ("woodColor3",materialData->woodColor[3]);
        res |= ppl->GetFloatValueByName ("ringScale",materialData->ringScale);
		 ppl->GetStringValueByName ("environmentMap",materialData->environmentMap);
         ppl->GetStringValueByName ("bumpTexture",materialData->bumpTexture);
		 ppl->GetStringValueByName ("textureFilename",materialData->textureFilename);
		 res |= ppl->GetFloatValueByName ("texAngle",materialData->texAngle);
		 res |= ppl->GetFloatValueByName ("texScaleU",materialData->texScaleU);
		 res |= ppl->GetFloatValueByName ("texScaleV",materialData->texScaleV);
		 res |= ppl->GetFloatValueByName ("texOffsetU",materialData->texOffsetU);
		 res &= ppl->GetFloatValueByName ("texOffsetV",materialData->texOffsetV);
		 res &= ppl->GetFloatValueByName ("texBlend",materialData->texBlend);
		 res &= ppl->GetBoolValueByName ("texRepeat",materialData->texRepeat);
		 	
		  res &= ppl->GetBoolValueByName ("texEnabled",materialData->texEnabled);
		  res &= ppl->GetBoolValueByName ("isHair",materialData->isHair);
		  res |= ppl->GetFloatValueByName ("hairLengh",materialData->hairLengh);
		  res |= ppl->GetFloatValueByName ("hairDens",materialData->hairDens);
		   ppl->GetStringValueByName ("skinTex",materialData->skinTex);
       
		     res |= ppl->GetBoolValueByName ("isSkin",materialData->isSkin);
		 res |= ppl->GetFloatValueByName ("skinShin",materialData->skinShin);
		 res |= ppl->GetFloatValueByName ("skinOil",materialData->skinOil);
		   ppl->GetStringValueByName ("hairTex",materialData->hairTex);
		   ppl->GetBoolValueByName ("inherit",materialData->inherit_from_parent);
		 return res;
	}
	void AvocadoMaterialInterfaceToString (ParamListSharedPtr &ppl, AvocadoMaterialInterface *mb)
	{
		ppl->PushFloat ("baseColorR",mb->baseColor[0]);
		ppl->PushFloat ("baseColorG",mb->baseColor[1]);
		ppl->PushFloat ("baseColorB",mb->baseColor[2]);

		ppl->PushFloat ("ambient",mb->ambient);
		ppl->PushFloat ("diffuse",mb->diffuse);
		ppl->PushFloat ("specular",mb->specular);
		ppl->PushFloat ("shininess",mb->shininess);
		ppl->PushFloat ("opacity",mb->opacity);
		ppl->PushFloat ("reflectivity",mb->reflectivity);
		ppl->PushFloat ("ior",mb->ior);
		ppl->PushFloat ("frenselExp",mb->frenselExp);
		ppl->PushFloat ("reflectionMin",mb->reflectionMin);
		ppl->PushFloat ("reflectionMax",mb->reflectionMax);
		ppl->PushFloat ("roughness",mb->roughness);
		ppl->PushFloat ("polished",mb->polished);
		ppl->PushFloat ("exposure",mb->exposure);
		ppl->PushFloat ("bumpHeight",mb->bumpHeight);
		ppl->PushFloat ("bumpScale",mb->bumpScale);
		ppl->PushBool ("bumpIsNormalMap",mb->bumpIsNormalMap);
		ppl->PushBool ("isWood",mb->isWood);
		ppl->PushFloat ("wooble",mb->wooble);
		ppl->PushFloat ("woodScale",mb->woodScale);
		ppl->PushFloat ("woodOffset0",mb->woodOffset[0]);
		ppl->PushFloat ("woodOffset1",mb->woodOffset[1]);
		ppl->PushFloat ("woodOffset2",mb->woodOffset[2]);
		ppl->PushFloat ("woodColor0",mb->woodColor[0]);
		ppl->PushFloat ("woodColor1",mb->woodColor[1]);
		ppl->PushFloat ("woodColor2",mb->woodColor[2]);
		ppl->PushFloat ("woodColor3",mb->woodColor[3]);
		ppl->PushFloat ("ringScale",mb->ringScale);
		if (mb->environmentMap != "")
			ppl->PushString ("environmentMap",mb->environmentMap);
		if (mb->bumpTexture != "")
			 ppl->PushString ("bumpTexture",mb->bumpTexture);
		if (mb->textureFilename != "")
			ppl->PushString ("textureFilename",mb->textureFilename);

		ppl->PushFloat ("texAngle",mb->texAngle);
		ppl->PushFloat ("texScaleU",mb->texScaleU);
		ppl->PushFloat ("texScaleV",mb->texScaleV);
		ppl->PushFloat ("texOffsetU",mb->texOffsetU);
		ppl->PushFloat ("texOffsetV",mb->texOffsetV);
		ppl->PushFloat ("texBlend",mb->texBlend);
		ppl->PushBool ("texRepeat",mb->texRepeat);
		ppl->PushBool ("bumpEnabled",mb->bumpEnabled);
		ppl->PushBool ("texEnabled",mb->texEnabled);
		
		ppl->PushBool ("isSkin",mb->isSkin);
		ppl->PushFloat ("skinOil",mb->skinOil);
		ppl->PushFloat ("skinShin",mb->skinShin);
		if (mb->skinTex!= "")
			 ppl->PushString ("skinTex",mb->skinTex);

		ppl->PushBool ("isHair",mb->isHair);
		ppl->PushFloat ("hairDens",mb->hairDens);
		ppl->PushFloat ("hairLengh",mb->hairLengh);
		if (mb->hairTex != "")
			ppl->PushString ("hairTex",mb->hairTex);
		ppl->PushBool ("inherit",mb->inherit_from_parent);
		
	}
	string AvocadoMaterialInterface::ToString ()
	{
		ParamListSharedPtr pl = ParamList::createNew ();
		AvocadoMaterialInterfaceToString(pl,this);

		return pl->SerializeList ();
	}
	void AvocadoMaterialInterface::FromString (string instr)
	{
		ParamListSharedPtr pl = ParamList::createFromString (instr);
		AvocadoMaterialInterfaceFromString(pl,this);

		return ;
	}
	// material base
	static std::vector<std::pair<std::string,nvsg::TextureHostSharedPtr>> GlobalTextureStack2;
	static std::vector <std::pair<std::string,nvsg::CgFxSharedPtr>> GlobalShaderCache ;
	void FreeGlobalMaterialCache ()
	{
		for (size_t k=0;k<GlobalTextureStack2.size ();k++)
			GlobalTextureStack2[k].second=0;
		GlobalTextureStack2.clear();
		for (size_t k=0;k<GlobalShaderCache.size ();k++)
			GlobalShaderCache[k].second=0;
		GlobalShaderCache.clear();
	}

	//class MaterialBase2 {
	//public:
		MaterialBase2::~MaterialBase2 ()
		{
			//m_cachedStateSet = 0;
		}
		std::string MaterialBase2::GetProccessDirectory (bool media)
		{ 
			return nvutil::GetProccessDirectory(media);
#if 0
			// windows specific stuff inside the engine is never a good thing :(
			 char  szPath[MAX_PATH];
			if (GetModuleFileNameA(NULL, szPath, ARRAYSIZE(szPath)))
			{
				std::string mfp (szPath);
				mfp = mfp.substr (0,mfp.rfind ("\\"));
				mfp += string ("\\media\\");
				return mfp; 
			}
			return string(".\\media\\");
#endif
		}
		MaterialBase2::MaterialBase2 (AvocadoMaterialInterface *m,std::string sessionFolder)
		{
			mat_data = m;
			m_sessionFolder = sessionFolder;
		}

		//string m_shaderFileName;
		//AvocadoMaterialInterface *mat_data;
		nvmath::Vec3f MaterialBase2::GetSpecularColor ()
		{
				float specR,specG,specB;
				if (mat_data->specular >=0)
				{
				// Specular is always brigther. Use 1 for white , 0 for base color.
					specR = 1.0f * mat_data->specular + (1.0f-mat_data->specular) * mat_data->baseColor[0];
					specG = 1.0f * mat_data->specular + (1.0f-mat_data->specular) * mat_data->baseColor[1];
					specB = 1.0f * mat_data->specular + (1.0f-mat_data->specular) * mat_data->baseColor[2];
				} else
				{
					float specAbs = 1.0f- abs (mat_data->specular);
					specR = (specAbs) * mat_data->baseColor[0];
					specG = (specAbs) * mat_data->baseColor[1];
					specB = (specAbs) * mat_data->baseColor[2];
				}
				return nvmath::Vec3f (specR,specG,specB);
		}
		nvmath::Vec3f MaterialBase2::GetAmbientColor ()
		{
			return (mat_data->ambient * nvmath::Vec3f (mat_data->baseColor[0],mat_data->baseColor[1],mat_data->baseColor[2]));//nvmath::Vec3f (1.0f,1.0f,1.0f));
		}
		nvmath::Vec3f MaterialBase2::GetDiffuseColor ()
		{
				float diffR,diffG,diffB;
				// Special logic for darker / brighther diffuse colors.
				// Dark diffuse is good for metal , bright is good for plastic.
				if (mat_data->diffuse >= 0)
				{
					diffR = 1.0f * mat_data->diffuse + (1.0f-mat_data->diffuse) * mat_data->baseColor[0];
					diffG = 1.0f * mat_data->diffuse + (1.0f-mat_data->diffuse) * mat_data->baseColor[1];
					diffB = 1.0f * mat_data->diffuse + (1.0f-mat_data->diffuse) * mat_data->baseColor[2];
				} else
				{
					float diffuseAbs = 1.0f-abs (mat_data->diffuse);
					diffR =  diffuseAbs  * mat_data->baseColor[0];
					diffG =  diffuseAbs * mat_data->baseColor[1];
					diffB =  diffuseAbs * mat_data->baseColor[2];
				}
				return nvmath::Vec3f (diffR,diffG,diffB);
		}
		bool MaterialBase2::SetFloatUniform (nvsg::CgFxSharedPtr tessCgFx , string paramName, float value)
		{
			bool result = true;
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName (paramName);
			if (pone)
			{
				float vf = value;
				CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf);
			}
			return result;
		}
		bool MaterialBase2::SetFloatVec3Uniform (CgFxSharedPtr tessCgFx , string paramName, float value1,float value2,float value3,float value4)
		{
			bool result = true;
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName (paramName);
			if (pone)
			{
				float vfp[4];
				vfp[0]= value1;
				vfp[1] = value2;
				vfp[2] = value3;
				vfp[3] = value4;
				CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,vfp);
			}
			return result;
		}
		bool MaterialBase2::SetSamplerUniformStatic (CgFxSharedPtr tessCgFx , string paramName, string filename,unsigned int &width,unsigned int &height,string sessionFolder)
		{
			bool result = true;
			NVSG_TRACE();
			TextureHostSharedPtr tex = 0;
			std::vector<std::string> searchPaths; 
		 

			searchPaths.push_back (string (sessionFolder)+ string ("\\textures\\"));
			searchPaths.push_back (string (GetProccessDirectory())+ string ("textures\\"));
			searchPaths.push_back (string (GetProccessDirectory(false)));
			// Should be done during engine initialization.
			//nvutil::addPlugInSearchPath (string (GetProccessDirectory(false)));
			

			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();

			for (size_t j=0;j<GlobalTextureStack2.size();j++)
			{
				if ((GlobalTextureStack2[j].first == filename ))
				{
					tex = GlobalTextureStack2[j].second;
					break;
				}
			}
			if (tex == 0)
			{
				// not found in global cache. lets create it and add to cache.
				string file;
				if (  nvutil::FindFileFirst( filename, searchPaths, file ) )
				{
					nvutil::loadTextureHost (file,tex,searchPaths);
					//tex = TextureHost::createFromFile (file,searchPaths);//,TextureHost::F_PRESERVE_IMAGE_DATA_AFTER_UPLOAD); 
					if (tex)
						GlobalTextureStack2.push_back (pair <string,TextureHostSharedPtr>(filename,tex));
					else
					{
						NVSG_TRACE_OUT(string (string("Texture loading failed for: ") + file).c_str());
					}
				} 
				else
				{
					NVSG_TRACE_OUT(string (string("Texture not found : ") + file).c_str());
					//NVSG_TRACE_OUT(string("Related avocado material data: \n"  +mat_data->ToString ()).c_str());
				}
			}

			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName (paramName);
			
			if (samEnv && tex && ((TextureHostWriteLock( tex ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv) )))
			{
					CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,tex);
					width = TextureHostWriteLock (tex)->getWidth();
					height = TextureHostWriteLock (tex)->getHeight();
			}
		
			return result;
		}
		bool MaterialBase2::SetSamplerUniform (CgFxSharedPtr tessCgFx , string paramName, string filename,unsigned int &width,unsigned int &height)
		{
			return (SetSamplerUniformStatic (tessCgFx,paramName,filename,width,height,m_sessionFolder));
		}
		
		nvsg::CgFxSharedPtr MaterialBase2::SearchShaderCache (string ifilename)
		{
			for (size_t k=0;k< GlobalShaderCache.size ();k++)
				
			{
				string name = GlobalShaderCache[k].first;
				if (name == ifilename)
				{
					CgFxSharedPtr org = GlobalShaderCache[k].second;
					return org->clone();
				}
			}
			return CgFxSharedPtr();
		}
		nvsg::CgFxSharedPtr MaterialBase2::LoadAvocadoShader (std::string ifilename )
			{
				NVSG_TRACE();
			CgFxSharedPtr tessCgFx = 0;//SearchShaderCache (ifilename);
			std::string err;
			if (tessCgFx == 0 || tessCgFx == CgFxSharedPtr())
			{
				tessCgFx = CgFx::create();  
				CgFxEffectWriteLock effect( CgFxReadLock( tessCgFx )->getEffect() );
				std::string file;
				
				std::vector<std::string> spats;
				spats.push_back (string (GetProccessDirectory())+ string ("effects\\"));
				spats.push_back (string (GetProccessDirectory())+ string ("scenes\\"));
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));
				spats.push_back (string (m_sessionFolder)+ string ("\\textures\\"));
				
				if (   !nvutil::FindFileFirst( ifilename, spats, file )
					|| !effect->createFromFile( file, spats, err ) )
				{
					NVSG_TRACE_OUT(string("Shader compilation failed with following errors: \n" + err).c_str());
					NVSG_TRACE_OUT(string("Related avocado material data: \n"  +mat_data->ToString ()).c_str());
					return( CgFxSharedPtr() );
				}
				//GlobalShaderCache.push_back (pair<string,CgFxSharedPtr> (ifilename,tessCgFx));
		    }
			return tessCgFx;
		}
		 MaterialSharedPtr MaterialBase2::createMaterialAttribute( const nvmath::Vec3f &ambientColor,
                                             const nvmath::Vec3f &diffuseColor,
                                             const nvmath::Vec3f &specularColor,
                                             const float specularExponent,
                                             const nvmath::Vec3f &emissiveColor,
                                             const float opacity,
                                             const float reflectivity,
                                             const float indexOfRefraction )
		  {
			// Create a Material
			MaterialSharedPtr materialPtr = Material::create();
			{
			  MaterialWriteLock mw( materialPtr );
			  mw->setAmbientColor( ambientColor ,MF_FRONT_AND_BACK);
			  mw->setDiffuseColor( diffuseColor,MF_FRONT_AND_BACK );
			  mw->setSpecularColor( specularColor,MF_FRONT_AND_BACK );
			  mw->setSpecularExponent( specularExponent ,MF_FRONT_AND_BACK);
			  mw->setEmissiveColor( emissiveColor,MF_FRONT_AND_BACK );
			  if (opacity < 1.0)
				  mw->setOpacityColor ( nvmath::Vec3f (diffuseColor[0]*opacity,diffuseColor[1]*opacity,diffuseColor[2]*opacity)/*Vec3f (0.5f,0.5f,0.5f)*/,MF_FRONT_AND_BACK);
			  else
			  mw->setOpacityColor ( nvmath::Vec3f (1.0f,1.0f,1.0f)/*Vec3f (0.5f,0.5f,0.5f)*/,MF_FRONT_AND_BACK);
			  mw->setOpacity( opacity,MF_FRONT_AND_BACK );
			  //mw->setOpacityColor ( Vec3f (1.0f,1.0f,1.0f)/*Vec3f (0.5f,0.5f,0.5f)*/,MF_FRONT_AND_BACK);
			  mw->setReflectivity( reflectivity ,MF_FRONT_AND_BACK);
			  mw->setIndexOfRefraction( indexOfRefraction ,MF_FRONT_AND_BACK); 
			}
			return materialPtr;
		  }
		 
		  StateSetSharedPtr MaterialBase2::createDefinedMaterial( const nvmath::Vec3f &ambientColor,
												   const nvmath::Vec3f &diffuseColor,
												   const nvmath::Vec3f &specularColor,
												   const float specularExponent,
												   const nvmath::Vec3f &emissiveColor,
												   const float opacity,
												   const float reflectivity,
												   const float indexOfRefraction )
		  {
			// Create a Material
			MaterialSharedPtr materialPtr = createMaterialAttribute( ambientColor,
																	 diffuseColor,
																	 specularColor,
																	 specularExponent,
																	 emissiveColor,
																	 opacity,
																	 reflectivity,
																	 indexOfRefraction );

			// Create a StateSet
			StateSetSharedPtr statePtr = StateSet::create();
			StateSetWriteLock( statePtr )->addAttribute( materialPtr );

			return statePtr;
		  }
		void MaterialBase2::UpdateSimpleMaterial ( nvsg::StateSetSharedPtr nss)
		{
		   StatePass::AttributeIterator saciOne  = StateSetWriteLock(nss)->findAttribute( OC_MATERIAL );
		   StateSetWriteLock(nss)->removeAttribute(saciOne);
		  MaterialSharedPtr materialPtr = createMaterialAttribute (
			  GetAmbientColor(), 
			  GetDiffuseColor (), 
			  GetSpecularColor (),
			  mat_data->shininess,	
			  nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor - not just yet.
			  mat_data->opacity, 
			  mat_data->reflectivity, 
			  mat_data->ior 
			  );
		   StateSetWriteLock( nss )->addAttribute( materialPtr );
		   if (mat_data->opacity<1.0)
		   {
						StatePass::AttributeIterator blendAttr  = StateSetWriteLock(nss)->findAttribute( OC_BLENDATTRIBUTE );
						if (blendAttr != StateSetWriteLock (nss)->endAttributes ())
							StateSetWriteLock(nss)->removeAttribute(blendAttr);
						  StatePass::AttributeIterator faceAttr  = StateSetWriteLock(nss)->findAttribute( OC_FACEATTRIBUTE );
						if (faceAttr != StateSetWriteLock (nss)->endAttributes ())
							StateSetWriteLock(nss)->removeAttribute(faceAttr);
					   
						StatePass::AttributeIterator depthAttr  = StateSetWriteLock(nss)->findAttribute( OC_DEPTHATTRIBUTE );
						if (depthAttr != StateSetWriteLock (nss)->endAttributes ())
							StateSetWriteLock(nss)->removeAttribute(depthAttr);
						
							FaceAttributeSharedPtr fa = FaceAttribute::create ();
							DepthAttributeSharedPtr da = DepthAttribute::create ();
							BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
							BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
							FaceAttributeWriteLock (fa)->setCullMode (false,false);

							DepthAttributeWriteLock (da)->setDepthWritingEnabled (false);
							DepthAttributeWriteLock (da)->setDepthTestingEnabled (true);
							StateSetWriteLock (nss)->addAttribute (fa);
							StateSetWriteLock (nss)->addAttribute (ba);
							StateSetWriteLock (nss)->addAttribute (da);

		    }
		   else
		   {
			   		    StatePass::AttributeIterator blendAttr  = StateSetWriteLock(nss)->findAttribute( OC_BLENDATTRIBUTE );
						if (blendAttr != StateSetWriteLock (nss)->endAttributes ())
							StateSetWriteLock(nss)->removeAttribute(blendAttr);
						  StatePass::AttributeIterator faceAttr  = StateSetWriteLock(nss)->findAttribute( OC_FACEATTRIBUTE );
						if (faceAttr != StateSetWriteLock (nss)->endAttributes ())
							StateSetWriteLock(nss)->removeAttribute(faceAttr);
						StatePass::AttributeIterator depthAttr  = StateSetWriteLock(nss)->findAttribute( OC_DEPTHATTRIBUTE );
						if (depthAttr != StateSetWriteLock (nss)->endAttributes ())
							StateSetWriteLock(nss)->removeAttribute(depthAttr);

						DepthAttributeSharedPtr da = DepthAttribute::create ();
						DepthAttributeWriteLock (da)->setDepthWritingEnabled (true);
						DepthAttributeWriteLock (da)->setDepthTestingEnabled (true);
						StateSetWriteLock (nss)->addAttribute (da);
						
						
							/*FaceAttributeSharedPtr fa = FaceAttribute::create ();
							BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
							BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_COLOR,BF_ONE);
							FaceAttributeWriteLock (fa)->setCullMode (false,true);
							StateSetWriteLock (nss)->addAttribute (fa);
							StateSetWriteLock (nss)->addAttribute (ba);*/
		   }
		}
		void MaterialBase2::UpdateSimpleUniform ( nvsg::StateSetSharedPtr nss , string uni)
		{
				NVSG_TRACE();
				if (uni == "" || nss == 0)
					return;
				NVSG_TRACE_OUT (string (string ("Updading uniform ") + uni).c_str());
		   StatePass::AttributeIterator saciOne  = StateSetWriteLock(nss)->findAttribute( OC_CGFX );
		   CgFxSharedPtr tessCgFx  ( nvutil::sharedPtr_cast<CgFx>( *saciOne ) );

		   if (uni == "ior")
			   SetFloatUniform (tessCgFx, "indexOfRefraction",mat_data->ior);
		   else if (uni == "reflectivity")
			   SetFloatUniform (tessCgFx, "reflectivity",mat_data->reflectivity);
			else if (uni=="minrefl")
				SetFloatUniform (tessCgFx, "minRefl",mat_data->reflectionMin);
			else if (uni=="maxrefl")
				SetFloatUniform (tessCgFx, "maxRefl",mat_data->reflectionMax);
			else if (uni=="fresnel")
				SetFloatUniform (tessCgFx, "fresnelEXP",mat_data->frenselExp);
		//	{
		//		// for bump mapped shader (and actually in all the beckmann functions ) , we use a fixed frens parameter.. should be tuned up as well..no time..
			//	SetFloatUniform (tessCgFx, "minRefl",mat_data->reflectionMin);
		//		SetFloatUniform (tessCgFx, "maxRefl",mat_data->reflectionMax);
		//		SetFloatUniform (tessCgFx, "fresnelEXP",mat_data->frenselExp);
			//}
			else if (uni=="roughness")
				SetFloatUniform (tessCgFx, "roughness",mat_data->roughness);
			else if (uni=="exposure")
				SetFloatUniform (tessCgFx, "Exposure",mat_data->exposure);
			else if (uni=="polishness")
				SetFloatUniform (tessCgFx, "GrayTarget",mat_data->polished);
				else if (uni=="ringscale")
				SetFloatUniform (tessCgFx, "RingScale",mat_data->ringScale);
				else if (uni=="woodscale")
				SetFloatUniform (tessCgFx, "WoodScale",mat_data->woodScale);
				else if (uni=="woodcolor" || uni=="woodintensity")
				{
					SetFloatVec3Uniform (tessCgFx, "WoodColor",mat_data->woodColor[0],mat_data->woodColor[1],mat_data->woodColor[2],mat_data->woodColor[3]);
				}
					else if (uni=="wooble")
				SetFloatUniform (tessCgFx, "AmpScale",mat_data->wooble);
					else if (uni=="texturescaleu" || uni == "texturescalev" || uni=="textureoffsetu" || uni == "textureoffsetv" )
						SetFloatVec3Uniform (tessCgFx, "diffuseMapPosition",mat_data->texScaleU,mat_data->texScaleV,mat_data->texOffsetU,mat_data->texOffsetV);
					else if (uni=="textureangle")
						SetFloatUniform (tessCgFx, "diffuseMapRotation",mat_data->texAngle);
			
					else if (uni=="bumpheight")
				SetFloatUniform (tessCgFx, "bumpHeight",mat_data->bumpHeight);
				else if (uni=="bumpscale")
				   SetFloatUniform (tessCgFx, "bumpScale",mat_data->bumpScale);
					else if (uni=="bumptexture" || uni == "isnormal")
					{
						unsigned int vx,vy;
						if (mat_data->bumpTexture!="" && mat_data->bumpTexture!="empty")
						{
							SetSamplerUniform (tessCgFx, "normalMapSampler",mat_data->bumpTexture,vx,vy);
							if (mat_data->bumpIsNormalMap)
							{
								// This tells the shader that we are using a normal map and not a height map.
								
								vx = 0;
								vy = 0;
							}
							SetFloatUniform (tessCgFx, "texDimWidth",float(vx));
							SetFloatUniform (tessCgFx, "texDimHeight",float(vy));
						}
					} else if (uni == "texturefilename")
				    {
						unsigned int vx,vy;
						if (mat_data->textureFilename!="" && mat_data->textureFilename!="empty")
						{
						  	SetSamplerUniform (tessCgFx, "diffuseMapSampler",mat_data->textureFilename,vx,vy);
						   SetFloatVec3Uniform (tessCgFx, "diffuseMapPosition",mat_data->texScaleU,mat_data->texScaleV,mat_data->texOffsetU,mat_data->texOffsetV);
						  SetSimpleTexture (nss,mat_data->textureFilename ,mat_data->texOffsetU,mat_data->texOffsetV,mat_data->texScaleU,mat_data->texScaleV);
						}
					} else if (uni == "hairlength")
					{
							SetFloatUniform (tessCgFx, "furLength",mat_data->hairLengh);
					}
					else
						if (uni == "hairdensity")
						{
							SetFloatUniform (tessCgFx, "stiffness",mat_data->hairDens);
				
						} 
					else if (uni == "skintexturefilename")
				    {
						unsigned int vx,vy;
						if (mat_data->skinTex!="" && mat_data->skinTex!="empty")
						{
						  	SetSamplerUniform (tessCgFx, "diffuseMapSampler",mat_data->skinTex,vx,vy);
						   // SetFloatVec3Uniform (tessCgFx, "diffuseMapPosition",mat_data->texScaleU,mat_data->texScaleV,mat_data->texOffsetU,mat_data->texOffsetV);
						   // SetSimpleTexture (nss);
						}
					}
				    else if (uni == "hairtexturefilename")
				    {
						unsigned int vx,vy;
						if (mat_data->hairTex!="" && mat_data->hairTex!="empty")
						{
						  	SetSamplerUniform (tessCgFx, "diffuseMapSampler",mat_data->hairTex,vx,vy);
						   // SetFloatVec3Uniform (tessCgFx, "diffuseMapPosition",mat_data->texScaleU,mat_data->texScaleV,mat_data->texOffsetU,mat_data->texOffsetV);
						   // SetSimpleTexture (nss);
						}
					}
			SetFloatVec3Uniform (tessCgFx, "lacquer",mat_data->baseColor[0],mat_data->baseColor[1],mat_data->baseColor[2],1.0f);
			//SetFloatUniform (tessCgFx, "roughness",mat_data->roughness);
			//SetFloatUniform (tessCgFx, "Exposure",mat_data->exposure);
		}

		void MaterialBase2::SetSimpleTexture (nvsg::StateSetSharedPtr nss,string texture,float offu, float offv, float sclu,float sclv)
		{
			NVSG_TRACE();
			{
				StatePass::AttributeIterator saciOneo  = StateSetWriteLock(nss)->findAttribute( OC_TEXTUREATTRIBUTE );
				//if (saciOneo != 0)
				StateSetWriteLock(nss)->removeAttribute(saciOneo);

				TextureHostSharedPtr tex;
				for (size_t j=0;j<GlobalTextureStack2.size();j++)
				{
					if ((GlobalTextureStack2[j].first == texture ))
					{
						tex = GlobalTextureStack2[j].second;
					}
				}
				// generate a TextureAttributeItem to be bound to the TextureAttribute below
				TextureAttributeItemSharedPtr hTexAttribItem = TextureAttributeItem::create();
				TextureAttributeSharedPtr hTexAttrib = TextureAttribute::create(); 
				// acquire write access to the TextureAttributeItem to assign a Texture
				{ // limit write access to this scope
					TextureAttributeItemWriteLock texAttribItem(hTexAttribItem);
					//TextureHostWriteLock (tex)->setTextureTarget (NVSG_TEXTURE_CUBE);
					texAttribItem->setTexture(tex );
					
					texAttribItem->setTranslation (nvmath::Vec3f (offu,offv,0.0f));
					texAttribItem->setScaling (nvmath::Vec3f (sclu,sclv,1.0f));
					//texAttribItem->sette
					//texAttribItem->setTexGenMode (nvsg::TCA_R,nvsg::TGM_EYE_LINEAR);
					//texAttribItem->setTexGenMode (nvsg::TCA_T,nvsg::TGM_EYE_LINEAR);
					//texAttribItem->setTexGenMode (nvsg::TCA_S,nvsg::TGM_EYE_LINEAR);
					texAttribItem->setMagFilterMode(nvsg::TFM_MAG_LINEAR);
					texAttribItem->setMinFilterMode(nvsg::TFM_MIN_LINEAR);
					//texAttribItem->setEnvMode( TEM_REPLACE );
					// texAttribItem->setTexGenMode( TCA_S, TGM_REFLECTION_MAP );
					 // texAttribItem->setTexGenMode( TCA_T, TGM_REFLECTION_MAP );
					  //texAttribItem->setTexGenMode( TCA_R, TGM_REFLECTION_MAP );
					//texAttribItem->setWrapMode (nvsg::TWCA_R,nvsg::TWM_CLAMP);
					//texAttribItem->setWrapMode (nvsg::TWCA_S,nvsg::TWM_CLAMP);
					//texAttribItem->setWrapMode (nvsg::TWCA_T,nvsg::TWM_CLAMP);
					texAttribItem->setWrapMode (nvsg::TWCA_R,nvsg::TWM_REPEAT);
					texAttribItem->setWrapMode (nvsg::TWCA_S,nvsg::TWM_REPEAT);
					texAttribItem->setWrapMode (nvsg::TWCA_T,nvsg::TWM_REPEAT);

					//texAttribItem->setOrientation(nvmath::Quatf ());// (nvmath::Vec3f (mat_data->texOffsetU,mat_data->texOffsetV,0.0f));

				} // write access will be released after leaving this scope

				// acquire write access to the TextureAttribute to bind the 
				// TextureAttributeItem created before
				{ // limit write access to this scope
					TextureAttributeWriteLock texAttrib(hTexAttrib);
					
					texAttrib->bindTextureAttributeItem( hTexAttribItem, 0 );
				} // write access will be released after leaving this scope
				//8888
				// StatePass::AttributeIterator saciOne  = StateSetWriteLock(nss)->findAttribute( OC_MATERIAL );
				//StateSetWriteLock(nss)->removeAttribute(saciOne);

				StateSetWriteLock (nss)->addAttribute (hTexAttrib);
			}
		}
		nvsg::StateSetSharedPtr MaterialBase2::CreateFFPShader ()
		{
			NVSG_TRACE();
			bool skipFres = false;
			// First lets create the state set with the common params.
			nvsg::StateSetSharedPtr	newStateSet = createDefinedMaterial (
			  GetAmbientColor(), 
			  GetDiffuseColor (), 
			  GetSpecularColor (),
			  mat_data->shininess,	
			  nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor - not just yet.
			  mat_data->opacity, 
			  mat_data->reflectivity, 
			  mat_data->ior 
			  );
			 DepthAttributeSharedPtr depthPtr = nvsg::DepthAttribute::create();
			{
			  DepthAttributeWriteLock wda(depthPtr);

			  wda->setDepthTestingEnabled(true);
			  wda->setDepthWritingEnabled((mat_data->opacity < 1.0f ? false : true));
			}
			StateSetWriteLock (newStateSet)->addAttribute (depthPtr);
			StateSetWriteLock (newStateSet)->setName (mat_data->name);
			return newStateSet;
		}
		nvsg::StateSetSharedPtr MaterialBase2::CreateShaderLogic (string &m_shaderFileName)
		{
			NVSG_TRACE();
			bool skipFres = false;
			// First lets create the state set with the common params.
			nvsg::StateSetSharedPtr	newStateSet = createDefinedMaterial (
			  GetAmbientColor(), 
			  GetDiffuseColor (), 
			  GetSpecularColor (),
			  mat_data->shininess,	
			  nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor - not just yet.
			  mat_data->opacity, 
			  mat_data->reflectivity, 
			  mat_data->ior 
			  );

			StateSetWriteLock (newStateSet)->setName (mat_data->name);
			CgFxSharedPtr tessCgFx;
			// Now Pick a shader and set specific params.
			if (mat_data->isSkin)
			{
				if (!mat_data->isHair)
				{
					m_shaderFileName = "SkinPerfect.cgfx";
					tessCgFx = LoadAvocadoShader (m_shaderFileName);
					if (mat_data->skinTex != "")
					{
						unsigned int texW1 = 0;
						unsigned int texH1 = 0;
						SetSamplerUniform (tessCgFx, "diffuseMapSampler",mat_data->skinTex,texW1,texH1);
					}
				} else
				{
					m_shaderFileName = "SkinAndHairGeom.cgfx";
					tessCgFx = LoadAvocadoShader (m_shaderFileName);
					SetFloatUniform (tessCgFx, "stiffness",mat_data->hairDens);
					SetFloatUniform (tessCgFx, "furLength",mat_data->hairLengh);
				//SetFloatUniform (tessCgFx, "WoodOffsetY",mat_data->woodOffset[1]);
					if (mat_data->hairTex != "")
					{
						unsigned int texW1 = 0;
						unsigned int texH1 = 0;
						SetSamplerUniform (tessCgFx, "diffuseMapSampler",mat_data->hairTex,texW1,texH1);
					}
				}
				//if (mat_data->bumpTexture != string (""))
				//{
				//	SetFloatUniform (tessCgFx, "bumpHeight",mat_data->bumpHeight);
				//	unsigned int texW = 0;
				//	unsigned int texH = 0;
				//	SetSamplerUniform (tessCgFx, "normalMapSampler",mat_data->bumpTexture,texW,texH);
				//}
			}  else
			if (mat_data->isWood)
			{
				m_shaderFileName = "BumpReflectWood.cgfx";
				tessCgFx = LoadAvocadoShader (m_shaderFileName);
				if (!tessCgFx || tessCgFx == CgFxSharedPtr() )
							return StateSetSharedPtr();

				SetFloatUniform (tessCgFx, "RingScale",mat_data->ringScale);
				SetFloatUniform (tessCgFx, "AmpScale",mat_data->wooble);
				SetFloatUniform (tessCgFx, "WoodScale",mat_data->woodScale);
				SetFloatUniform (tessCgFx, "WoodOffsetX",mat_data->woodOffset[0]);
				SetFloatUniform (tessCgFx, "WoodOffsetY",mat_data->woodOffset[1]);
				SetFloatUniform (tessCgFx, "WoodOffsetZ",mat_data->woodOffset[2]);
				SetFloatVec3Uniform (tessCgFx, "WoodColor",mat_data->woodColor[0],mat_data->woodColor[1],mat_data->woodColor[2],mat_data->woodColor[3]);
				unsigned int texW1 = 0;
				unsigned int texH1 = 0;
				SetSamplerUniform (tessCgFx, "NoiseSamp","noiseL8_32x32x32.dds",texW1,texH1);
				if (mat_data->bumpTexture != string (""))
				{
					SetFloatUniform (tessCgFx, "bumpHeight",mat_data->bumpHeight);
					//SetFloatUniform (tessCgFx, "bumpScale",mat_data->bumpScale);
					unsigned int texW = 0;
					unsigned int texH = 0;
					SetSamplerUniform (tessCgFx, "normalMapSampler",mat_data->bumpTexture,texW,texH);
				}
			}
			else if (mat_data->bumpTexture != string ("") && mat_data->bumpEnabled)
			{
				if (mat_data->environmentMap != string (""))
				{
					//  bump with reflect.
					if (0)//mat_data->opacity < 1.0)
					{
						m_shaderFileName = "BumpReflectPhongHDRGlass.cgfx";
						tessCgFx = LoadAvocadoShader (m_shaderFileName);
						SetFloatUniform (tessCgFx, "GrayTarget",0.33f); 
					}
					else
					{
						m_shaderFileName = ((mat_data->textureFilename != "" && mat_data->texEnabled) ? "BumpReflectPhongHDR_t.cgfx" : "BumpReflectPhongHDR.cgfx");
					//	m_shaderFileName = "BumpReflectPhongHDR.cgfx";
						tessCgFx = LoadAvocadoShader (m_shaderFileName);
						SetFloatUniform (tessCgFx, "GrayTarget",mat_data->polished); 
						SetFloatUniform (tessCgFx, "reflectivity",mat_data->reflectivity); 
						SetFloatUniform (tessCgFx, "indexOfRefraction",mat_data->ior); 
						SetFloatVec3Uniform (tessCgFx, "lacquer",mat_data->baseColor[0],mat_data->baseColor[1],mat_data->baseColor[2],1.0f);
					} // end if transparent
					SetFloatUniform (tessCgFx, "bumpHeight",mat_data->bumpHeight);
					SetFloatUniform (tessCgFx, "bumpScale",mat_data->bumpScale);
					unsigned int texW = 0;
					unsigned int texH = 0;
					SetSamplerUniform (tessCgFx, "normalMapSampler",mat_data->bumpTexture,texW,texH);
					if (mat_data->bumpIsNormalMap)
					{
						// This tells the shader that we are using a normal map and not a height map.
						// A normal map is better quality but is also tangent space dependent.
						texW = 0;
						texH = 0;
					}
					SetFloatUniform (tessCgFx, "texDimWidth",float(texW));
					SetFloatUniform (tessCgFx, "texDimHeight",float(texH));

					// specific HDR / PAINT params.
					SetFloatUniform (tessCgFx, "Exposure",mat_data->exposure);
					SetSamplerUniform (tessCgFx, "textureHDR_RGBA",mat_data->environmentMap,texW,texH);
					//SetFloatUniform (tessCgFx, "GrayTarget",mat_data->polished); // fixed gray target for now.
				} 
				else
				{
					// simple bump , no reflect.
					if (mat_data->textureFilename != "" && mat_data->texEnabled)
					m_shaderFileName = "BumpReflectPhong_t.cgfx";
					else
					{
					m_shaderFileName = "BumpReflectPhong.cgfx";
					}
					tessCgFx = LoadAvocadoShader (m_shaderFileName);
					if (!tessCgFx || tessCgFx == CgFxSharedPtr() )
							return StateSetSharedPtr();

					SetFloatUniform (tessCgFx, "bumpHeight",mat_data->bumpHeight);
					SetFloatUniform (tessCgFx, "bumpScale",mat_data->bumpScale);
					unsigned int texW = 0;
					unsigned int texH = 0;
					SetSamplerUniform (tessCgFx, "normalMapSampler",mat_data->bumpTexture,texW,texH);
					if (mat_data->bumpIsNormalMap)
					{
						// This tells the shader that we are using a normal map and not a height map.
						// A normal map is better quality but is also tangent space dependent.
						texW = 0;
						texH = 0;
					}
					 SetFloatUniform (tessCgFx, "GrayTarget",mat_data->polished); 
					SetFloatUniform (tessCgFx, "texDimWidth",float(texW));
					SetFloatUniform (tessCgFx, "texDimHeight",float(texH));
				}  // end if environemtn bump
			}  // end if bump
			else
			{
				if (mat_data->environmentMap != string (""))
				{
					if (mat_data->opacity < 1.0f)
					{
						
						// reflection with transparency , use glss shader..
						//m_shaderFileName = "glass_hdr.fx";
						m_shaderFileName = ((mat_data->textureFilename != "" && mat_data->texEnabled) ? "car_paint_hdr_t.fx" : "glass_hdr.fx");
						tessCgFx = LoadAvocadoShader (m_shaderFileName);
						if (!tessCgFx || tessCgFx == CgFxSharedPtr() )
							return StateSetSharedPtr();

						unsigned int texW = 0;
						unsigned int texH = 0;
						SetSamplerUniform (tessCgFx, "textureHDR_RGBA",mat_data->environmentMap,texW,texH);
						// specific HDR / PAINT params.
						SetFloatUniform (tessCgFx, "Exposure",mat_data->exposure);
						SetFloatUniform (tessCgFx, "GrayTarget",mat_data->polished); // fixed gray target for now.
						SetFloatUniform (tessCgFx, "reflectivity",mat_data->reflectivity);
						SetFloatUniform (tessCgFx, "indexOfRefraction",mat_data->ior);
						//	SetFloatVec3Uniform (tessCgFx, "lacquer",baseColor[0],baseColor[1],baseColor[2],1.0f);
					}
					else
					{
						//reflective material. no bumps.
						m_shaderFileName = ((mat_data->textureFilename != "" && mat_data->texEnabled) ? "car_paint_hdr_t.fx" : "car_paint_hdr.fx");
						tessCgFx = LoadAvocadoShader (m_shaderFileName);
						if (!tessCgFx || tessCgFx == CgFxSharedPtr() )
							return StateSetSharedPtr();

						unsigned int texW = 0;
						unsigned int texH = 0;
						SetSamplerUniform (tessCgFx, "textureHDR_RGBA",mat_data->environmentMap,texW,texH);
						// specific HDR / PAINT params.
						SetFloatUniform (tessCgFx, "Exposure",mat_data->exposure);
						SetFloatUniform (tessCgFx, "GrayTarget",mat_data->polished);
						SetFloatUniform (tessCgFx, "reflectivity",mat_data->reflectivity);
						SetFloatUniform (tessCgFx, "indexOfRefraction",mat_data->ior);
						SetFloatVec3Uniform (tessCgFx, "lacquer",mat_data->baseColor[0],mat_data->baseColor[1],mat_data->baseColor[2],1.0f);
					}  // end if transparent
				} 
				else
				{
					// simple material , no bumps , no reflect.
					if (mat_data->textureFilename != "" && mat_data->texEnabled)
						m_shaderFileName = "phong_gloss_t.fx";
					else
						m_shaderFileName = "phong_gloss.fx";
					tessCgFx = LoadAvocadoShader (m_shaderFileName);
					if (!tessCgFx || tessCgFx == CgFxSharedPtr() )
							return StateSetSharedPtr();

						SetFloatUniform (tessCgFx, "GrayTarget",mat_data->polished);
				}  // end if environment-simple
			}
			
			// Simple common parameters - 
			// Roughness , frenselExp , reflectMin,reflectMax;
			if (!skipFres)
			{
				// for bump mapped shader (and actually in all the beckmann functions ) , we use a fixed frens parameter.. should be tuned up as well..no time..
				SetFloatUniform (tessCgFx, "minRefl",mat_data->reflectionMin);
				SetFloatUniform (tessCgFx, "maxRefl",mat_data->reflectionMax);
				SetFloatUniform (tessCgFx, "fresnelEXP",mat_data->frenselExp);
			}
			SetFloatUniform (tessCgFx, "roughness",mat_data->roughness);
		

			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
			 DepthAttributeSharedPtr depthPtr = nvsg::DepthAttribute::create();
			{
			  DepthAttributeWriteLock wda(depthPtr);

			  wda->setDepthTestingEnabled(true);

			  wda->setDepthWritingEnabled((mat_data->opacity < 1.0f ? false : true));
			}
			StateSetWriteLock (newStateSet)->addAttribute (depthPtr);
			if (mat_data->opacity<1.0)
			{
							FaceAttributeSharedPtr fa = FaceAttribute::create ();
							BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
							BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
							FaceAttributeWriteLock (fa)->setCullMode (false,false);
							StateSetWriteLock (newStateSet)->addAttribute (fa);
							StateSetWriteLock (newStateSet)->addAttribute (ba);
			} else
			{
				             FaceAttributeSharedPtr fa = FaceAttribute::create ();
							//BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
							//BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
							 bool bf = false;
			                 avocado::GetEngineOptionBool ("backface_culling", &bf);
							FaceAttributeWriteLock (fa)->setCullMode (false,bf);
							StateSetWriteLock (newStateSet)->addAttribute (fa);
							//StateSetWriteLock (newStateSet)->addAttribute (ba);
			}
			if (mat_data->textureFilename != "" && mat_data->texEnabled)
			{ 
						unsigned int texW = 0;
						unsigned int texH = 0;
						if (!mat_data->isSkin && !mat_data->isHair )
						{
							SetSamplerUniform (tessCgFx, "diffuseMapSampler",mat_data->textureFilename,texW,texH);
						}
						SetFloatVec3Uniform (tessCgFx, "diffuseMapPosition",mat_data->texScaleU,mat_data->texScaleV,mat_data->texOffsetU,mat_data->texOffsetV);
//8888
						SetFloatUniform (tessCgFx, "diffuseMapRotation",mat_data->texAngle);
						SetSimpleTexture (newStateSet,mat_data->textureFilename ,mat_data->texOffsetU,mat_data->texOffsetV,mat_data->texScaleU,mat_data->texScaleV);
			}
			// save the state set for later updates.
			return newStateSet;
			//return true;
		}


		// DEFAULT MATERIALS
		
	void MaterialLibraryInitDefault (avocado::AvocadoMaterialInterface &mb)
	{
			mb.name = "AvocadoDefaultMaterial";
			mb.ambient = 0.5f;
			mb.diffuse = 0.0f;
			mb.baseColor[0] = 0.5f;
			mb.baseColor[1] = 0.5f;
			mb.baseColor[2] = 0.5f;
			mb.specular = 0.25f;
			mb.shininess = 12.7f;
			mb.opacity = 1.0f;
			mb.roughness = 0.5f;
			mb.polished = 0.5f;
			mb.frenselExp = 5.0;
			mb.reflectionMin = 0.0;
			mb.reflectionMax = 0.0;
			mb.reflectivity = 0.0;
			mb.exposure = -0.65f;
			mb.ior = 0.0;
			mb.bumpHeight = 0.8f;
			mb.bumpScale = 1.0f;
			mb.bumpEnabled = true;
			mb.bumpIsNormalMap = false;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.woodScale = 8.0f;
			mb.wooble = 0.7f;
			mb.ringScale = 0.46f;
			mb.woodOffset[0] = -10.0f;
			mb.woodOffset[1] = -11.0f;
			mb.woodOffset[2] = 7.0f;
			mb.woodColor[0] = 1.0f;
			mb.woodColor[1] = 0.0f;
			mb.woodColor[2] = 0.0f;
			mb.woodColor[3] = 0.2f;
			mb.textureFilename = "";
			mb.texAngle = 0.0f;
			mb.texScaleU = 1.0f;
			mb.texScaleV = 1.0f;
			mb.texOffsetU = 0.0f;
			mb.texOffsetV = 0.0f;
			mb.texBlend = 1.0f;
			mb.texRepeat = true;
			mb.texEnabled = true;
			mb.isHair = false;
			mb.isSkin = false;
			mb.hairTex =  "myhairtexture1.dds";
		    mb.skinTex = "myskintexture1.dds";
			mb.hairDens = 0.5;
			mb.hairLengh = 0.5;
			mb.skinShin = 0.5;
			mb.skinOil = 0.5;
	}
	void NormalizeRGB (int r,int g,int b,avocado::AvocadoMaterialInterface &mb)
	{
		float rf,gf,bf;

		rf = float (r) / 255.0f;
		gf = float (g) / 255.0f;
		bf = float (b) / 255.0f;
		if (rf < 0)
			rf = 0.5f;
		if (bf < 0)
			bf = 0.5f;
		if (gf < 0)
			gf = 0.5f;
		mb.baseColor[0] = rf; mb.baseColor[1] = gf;mb.baseColor[2] = bf;
	}
	void MaterialLibraryFetch3 (int matID, int r, int g, int b,avocado::AvocadoMaterialInterface &mb)
	{
	
		if (matID == 1)
		{
			//MATT
		
				mb.ambient = 0.4f;
				mb.diffuse = -0.8f;
				mb.specular = 0.5f;
				mb.shininess = 2.0f;
				mb.roughness = 0.5f;
				mb.frenselExp = 3.5f;
				mb.opacity = 1.0f;
				mb.reflectivity = 0.0f;
				mb.reflectionMax = 0.0f;
				mb.reflectionMin = 0.0f;
				mb.polished = 1.0f;
				mb.bumpTexture = "";
				mb.environmentMap = "";
				mb.isWood = false;
				mb.wooble = 0.0f;
				mb.woodScale = 0.0f;
				mb.ringScale = 0.0f;
				mb.woodOffset[0] = 0.0;
				mb.woodOffset[0] =  0.0;
				mb.woodOffset[0] =  0.0f;
			
		} 
		else if (matID == 2)
		{
			// SIMPLE METAL	
			// Low DIFFUSE. HIGH SPECULAR COLOR, LOW EXPONENT.
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 3)
		{
			    /// STEEL	
		     	mb.ambient = 0.25f;
				mb.diffuse = -0.75f;
				mb.specular = 0.2f;
				mb.shininess = 15.3f;
				mb.roughness = 0.37f;
				mb.frenselExp = 3.5f;
				mb.opacity = 1.0f;
				mb.reflectivity = 0.1f;
				mb.reflectionMax = 0.01f;
				mb.reflectionMin = 0.01f;
				mb.polished = 1.0f;
				mb.bumpTexture = "";
				mb.environmentMap = "";
				mb.isWood = false;
				mb.wooble = 0.0f;
				mb.woodScale = 0.0f;
				mb.ringScale = 0.0f;
				mb.woodOffset[0] = 0.0;
				mb.woodOffset[0] =  0.0;
				mb.woodOffset[0] =  0.0f;
		}
		
		
		else if (matID == 4)
		{
			// SHINY METAL
			
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 5)
		{
			// PAINT
			
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 6)
		{
			// SHINY PAINT
			
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 7)
		{
			//PLASTIC
			mb.ambient = 0.2f;
			mb.diffuse = 0.0f;
			mb.specular = 1.0f;
			mb.shininess = 92.3f;
			mb.roughness = 0.1f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.2f;
			mb.reflectionMax = 0.5f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		} else if (matID == 8)
		{
			// TRASNPARENT PLASTIC
			mb.ambient = 0.2f;
			mb.diffuse = 0.0f;
			mb.specular = 1.0f;
			mb.shininess = 92.3f;
			mb.roughness = 0.1f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.2f;
			mb.reflectionMax = 0.5f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
	

		else if (matID == 9)
		{
			// RUBBER	
			// Low DIFFUSE. HIGH SPECULAR COLOR, LOW EXPONENT.
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
			else if (matID == 9)
		{
			// THIN GLASS	
			
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 11)
		{
			// THICK GLASS
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		}
		else if (matID == 12)
		{
			// SHINY GLASS
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 13)
		{
			// BRICKS
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;

		}
		else if (matID ==14)
		{
			
		// CONCREATE
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 15)
		{
			// CERAMIC
			mb.ambient = 0.4f;
			mb.diffuse = 0.05f;
			mb.specular = -1.0f;
			mb.shininess = 0.0f;
			mb.roughness = 0.07f;
			mb.frenselExp = 3.5f;
			mb.opacity = 0.5f;
			mb.reflectivity = 0.012f;
			mb.reflectionMax = 0.0f;
			mb.reflectionMin = 0.0f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}

else if (matID == 16)
		{
			// COTTON
			mb.ambient = 0.4f;
			mb.diffuse = 0.05f;
			mb.specular = -1.0f;
			mb.shininess = 0.0f;
			mb.roughness = 0.07f;
			mb.frenselExp = 3.5f;
			mb.opacity = 0.5f;
			mb.reflectivity = 0.012f;
			mb.reflectionMax = 0.0f;
			mb.reflectionMin = 0.0f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 17)
		{
			// LEATHER
			mb.ambient = 0.4f;
			mb.diffuse = 0.05f;
			mb.specular = -1.0f;
			mb.shininess = 0.0f;
			mb.roughness = 0.07f;
			mb.frenselExp = 3.5f;
			mb.opacity = 0.5f;
			mb.reflectivity = 0.012f;
			mb.reflectionMax = 0.0f;
			mb.reflectionMin = 0.0f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
else if (matID == 18)
		{
			// CRACKED LEATHER
			mb.ambient = 0.4f;
			mb.diffuse = 0.05f;
			mb.specular = -1.0f;
			mb.shininess = 0.0f;
			mb.roughness = 0.07f;
			mb.frenselExp = 3.5f;
			mb.opacity = 0.5f;
			mb.reflectivity = 0.012f;
			mb.reflectionMax = 0.0f;
			mb.reflectionMin = 0.0f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 19)
		{
			// ROUGH LEATHER
			mb.ambient = 0.4f;
			mb.diffuse = 0.05f;
			mb.specular = -1.0f;
			mb.shininess = 0.0f;
			mb.roughness = 0.07f;
			mb.frenselExp = 3.5f;
			mb.opacity = 0.5f;
			mb.reflectivity = 0.012f;
			mb.reflectionMax = 0.0f;
			mb.reflectionMin = 0.0f;
			mb.polished = 1.0f;
			mb.bumpTexture = "mybump1.dds";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 20)
		{
			// WOOD
			mb.ambient = 0.4f;
			mb.diffuse = -0.5f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.023f;
			mb.reflectionMax = 0.3f;
			mb.reflectionMin = 0.02f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = true;
			mb.wooble = 0.7f;
			mb.woodScale = 19.0f;
			mb.ringScale = 2.46f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else if (matID == 21)
		{
			// SKIN.
			mb.ambient = 0.4f;
			mb.diffuse = -0.75f;
			mb.specular = 0.3f;
			mb.shininess = 12.7f;
			mb.roughness = 0.25f;
			mb.frenselExp = 5.0f;
			mb.opacity = 1.0f;
			mb.reflectivity = 0.005f;
			mb.reflectionMax = 0.5f;
			mb.reflectionMin = 0.00f;
			mb.polished = 1.0f;
			mb.bumpTexture = "";
			mb.environmentMap = "";
			mb.isWood = false;
			mb.wooble = 0.0f;
			mb.woodScale = 0.0f;
			mb.ringScale = 0.0f;
		//	mb.woodOffset[0] = 0.0;
		//	mb.woodOffset[0] =  0.0;
		//	mb.woodOffset[0] =  0.0f;
		}
		else
		{
			MaterialLibraryInitDefault(mb);
		}
	}


	}
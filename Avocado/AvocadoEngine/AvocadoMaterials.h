#pragma once
#ifndef AVOCADO_MATERIALS_H
#define AVOCADO_MATERIALS_H
#include "AvocadoAppInterface.h"
#include "AvocadoParams.h"
#include <nvsg/CoreTypes.h>
namespace avocado {
	void FreeGlobalMaterialCache ();
	bool AvocadoMaterialInterfaceFromString (ParamListSharedPtr &ppl,AvocadoMaterialInterface *materialData);
	void AvocadoMaterialInterfaceToString (ParamListSharedPtr &ppl, AvocadoMaterialInterface *mb);

	void MaterialLibraryInitDefault (avocado::AvocadoMaterialInterface &mb);
	void NormalizeRGB (int r,int g,int b,avocado::AvocadoMaterialInterface &mb);
	void MaterialLibraryFetch3 (int matID, int r, int g, int b,avocado::AvocadoMaterialInterface &mb);



	class MaterialBase2 {
	public:
		~MaterialBase2 ();
		static std::string MaterialBase2::GetProccessDirectory (bool media = true);
		MaterialBase2 (AvocadoMaterialInterface *m, std::string sessionFolder);

		string m_sessionFolder;
		string m_shaderFileName;
		AvocadoMaterialInterface *mat_data;
		nvmath::Vec3f GetSpecularColor ();
		nvmath::Vec3f GetAmbientColor ();
		nvmath::Vec3f GetDiffuseColor ();
		static bool SetFloatUniform (nvsg::CgFxSharedPtr tessCgFx , string paramName, float value);
		bool SetFloatVec3Uniform (nvsg::CgFxSharedPtr tessCgFx , string paramName, float value1,float value2,float value3,float value4);
		static bool SetSamplerUniformStatic (nvsg::CgFxSharedPtr tessCgFx , string paramName, string filename,unsigned int &width,unsigned int &height,std::string sessionFolder);
		bool SetSamplerUniform (nvsg::CgFxSharedPtr tessCgFx , string paramName, string filename,unsigned int &width,unsigned int &height);
		
		nvsg::CgFxSharedPtr SearchShaderCache (string ifilename);
		nvsg::CgFxSharedPtr LoadAvocadoShader (std::string ifilename );
		nvsg::MaterialSharedPtr createMaterialAttribute( const nvmath::Vec3f &ambientColor,
                                             const nvmath::Vec3f &diffuseColor,
                                             const nvmath::Vec3f &specularColor,
                                             const float specularExponent,
                                             const nvmath::Vec3f &emissiveColor,
                                             const float opacity,
                                             const float reflectivity,
                                             const float indexOfRefraction );
		 void UpdateSimpleMaterial ( nvsg::StateSetSharedPtr nss);
		 void UpdateSimpleUniform ( nvsg::StateSetSharedPtr nss , string uni);
		static void SetSimpleTexture (nvsg::StateSetSharedPtr nss,string texture,float offu, float offv, float sclu,float sclv);
		nvsg::StateSetSharedPtr CreateShaderLogic (string &m_shaderFileName);
		nvsg::StateSetSharedPtr CreateFFPShader ();
		nvsg::StateSetSharedPtr MaterialBase2::createDefinedMaterial( const nvmath::Vec3f &ambientColor,
												   const nvmath::Vec3f &diffuseColor,
												   const nvmath::Vec3f &specularColor,
												   const float specularExponent,
												   const nvmath::Vec3f &emissiveColor,
												   const float opacity,
												   const float reflectivity,
												   const float indexOfRefraction );
	};
}

#endif
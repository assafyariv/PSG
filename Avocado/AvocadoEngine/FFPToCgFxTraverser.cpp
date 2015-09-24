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

#include "FFPToCgFxTraverser.h"
#include <nvsg/AlphaTestAttribute.h>
#include <nvsg/BlendAttribute.h>
#include <nvsg/CgFx.h>
#include <nvsg/DepthAttribute.h>
#include <nvsg/LightingAttribute.h>
#include <nvsg/Material.h>
#include <nvsg/Transform.h>
#include <nvsg/TextureAttribute.h>
#include <sstream>
#include <locale>
#include <nvsg/Triangles.h>

#include <nvutil/DbgNew.h>

using namespace nvtraverser;
using namespace nvsg;
using namespace nvutil;
using namespace nvmath;

FFPToCgFxTraverser::FFPToCgFxTraverser()
  : m_numStateSets(0)
{
  m_flipYZ = false;
  NVSG_TRACE();
}

FFPToCgFxTraverser::~FFPToCgFxTraverser()
{
  NVSG_TRACE();
}

void FFPToCgFxTraverser::SetFlipYZ (bool flipYZ)
{
	m_flipYZ = flipYZ;
}
bool FFPToCgFxTraverser::GetFlipYZ ()
{
	return m_flipYZ;
}
 void FFPToCgFxTraverser::handleTransform (nvsg::Transform *tra)
 {
	 nvmath::Vec3f translate;
	 nvmath::Quatf orientation;
	 if (m_flipYZ)
	 {
		orientation = tra->getOrientation();
		translate = tra->getTranslation();
	 }
	 if (m_flipYZ)
	 {
		 // this code below is still not working properly.
		//orientation[0] = -1.0f;
		nvmath::Quatf oflip = nvmath::Quatf (-1.0,0.0,0.0,1.0);
		orientation *= oflip;
		tra->setTranslation ( Vec3f (translate[0],translate[2],-translate[1]));
		tra->setOrientation ( Quatf (orientation[0],orientation[1],orientation[2],orientation[3]));
	 }
 }
void FFPToCgFxTraverser::handleVertexAttributeSet( nvsg::VertexAttributeSet *vas )
{
	std::vector<Vec3f> vertices;
	std::vector<Vec3f> normals;
	int vertCount = 0;
	int normalCount = 0;
	// STILL NEED TO HANDLE TRANSFORM NODES , ANIMATED TRANSFORM NODES, LIGHTS.
	if (m_flipYZ)
	{
	//nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type verts = nvsg::VertexAttributeSetWriteLock (vas)->getVertexBuffer(0);
		nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type verts = (vas)->getVertices ();
		nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type norms =  (vas)->getNormals();
		vertCount =  (vas)->getNumberOfVertices ();
		normalCount =  (vas)->getNumberOfNormals ();
	
		vertices.reserve (vertCount);
		normals.reserve (normalCount);
		for (int i=0;i < vertCount;i++)
		{
			vertices.push_back (nvmath::Vec3f((*verts)[0],((*verts)[2]),- ((*verts)[1])));
			verts ++;
		}
		for (int i=0;i < normalCount;i++)
		{
			Vec3f newNorm = nvmath::Vec3f((*norms)[0],(*norms)[2],-((*norms)[1]));
			newNorm.normalize ();
			normals.push_back (newNorm);
			norms++;
		}
	//	nvsg::BufferWriteLock (bsp)->getda
	}
    
	if (m_flipYZ)
	{
		(vas)->setNormals (&normals[0],normalCount);
		(vas)->setVertices (&vertices[0],vertCount);
	}
	ExclusiveTraverser::handleVertexAttributeSet( vas );
}

inline std::string fixupName( const std::string & orig )
{
  std::string result = orig;
  std::string illegal(" -.@&*()=%#!");

  while( 1 )
  {
    size_t pos = result.find_first_of( illegal );
    if( pos == std::string::npos )
    {
      break;
    }

    result[ pos ] = '_';
  }

  //wow, this is so much easier than ctype.h..
  std::locale loc;
  const std::ctype<char>& ct = std::use_facet< std::ctype<char> >(loc);
  if( ct.is( std::ctype_base::digit, result[0] ) )
  {
    result = "_" + result;
  }
  
  return result;
}

inline std::string fixupFilename( const std::string & orig )
{
  std::string result = orig;

  while( 1 )
  {
    size_t pos = result.find_first_of( '\\' );
    if( pos == std::string::npos )
    {
      break;
    }

    result[ pos ] = '/';
  }
  
  return result;
}

std::string FFPToCgFxTraverser::buildShader()
{
  std::stringstream shader;

  shader << "// Automatically generated\n";
  shader << m_currentEffect->tweakables;
  for( size_t i = 0; i < m_currentEffect->techniques.size(); i ++ )
  {
    shader << "technique " << fixupName( m_currentEffect->name ) << "\n";
    shader << "{\n";

    for( size_t j = 0; j < m_currentEffect->techniques[i].passes.size(); j ++ )
    {
      shader << "pass " << fixupName( m_currentEffect->techniques[i].passes[j].name ) << "\n";
      shader << "{\n";
      shader << m_currentEffect->techniques[i].passes[j].state << "\n";
      shader << "}\n";
    }

    shader << "}\n";
  }

  return shader.str();
}

void FFPToCgFxTraverser::handleStateSet(StateSet * stateSet)
{
  NVSG_TRACE();

  // first, see if this State Set includes a cgfx - if so we bail for now, and don't traverse the SS
  StatePass::AttributeIterator sai = stateSet->findAttribute( OC_CGFX );
  if ( sai == stateSet->endAttributes() )
  {
    m_currentEffect = new FxEffect();

    if( !stateSet->getName().empty() )
    {
      m_currentEffect->name = stateSet->getName();
    }
    else
    {
      std::stringstream ss;
      ss << "effect" << m_numStateSets++;

      m_currentEffect->name = ss.str();
    }

    ExclusiveTraverser::handleStateSet( stateSet );

    // now, apply effect to this stateset
    std::string shader = buildShader();

    bool failOnTextureLoad = false;
    std::string error;
    CgFxSharedPtr cgfxh = CgFx::createFromLump( shader, std::vector<std::string>(), error, failOnTextureLoad );

    if( cgfxh )
    {
      stateSet->addAttribute( cgfxh );
    }

    for( size_t i = 0; i < m_currentEffect->replacedAttributes.size(); i ++ )
    {
		if ( StateAttributeWriteLock(m_currentEffect->replacedAttributes[i]))
		// Dont remove the attributes,, we need them for optix,
       stateSet->removeAttribute( m_currentEffect->replacedAttributes[i] );
    }

    delete m_currentEffect;
    m_currentEffect = 0;
  }
}

void FFPToCgFxTraverser::handleStateVariant(StateVariant * variant)
{
  NVSG_TRACE();

  FxTechnique technique;

  if( !variant->getName().empty() )
  {
    technique.name = variant->getName();
  }
  else
  {
    std::stringstream ss;
    ss << "technique" << m_currentEffect->techniques.size();

    technique.name = ss.str();
  }

  m_currentEffect->techniques.push_back( technique );
  m_currentEffect->currentTechnique = &m_currentEffect->techniques.back();

  ExclusiveTraverser::handleStateVariant(variant);
}

void FFPToCgFxTraverser::handleStatePass(StatePass * pass)
{
  NVSG_TRACE();

  FxPass fxpass;

  if( !pass->getName().empty() )
  {
    fxpass.name = pass->getName();
  }
  else
  {
    std::stringstream ss;
    ss << "pass" << m_currentEffect->currentTechnique->passes.size();

    fxpass.name = ss.str();
  }

  m_currentEffect->currentTechnique->passes.push_back( fxpass );
  m_currentEffect->currentTechnique->currentPass = &m_currentEffect->currentTechnique->passes.back();

  ExclusiveTraverser::handleStatePass(pass);
}

void FFPToCgFxTraverser::handleCgFx(CgFx * cgfx)
{
  NVSG_TRACE();

  ExclusiveTraverser::handleCgFx( cgfx );
}

void FFPToCgFxTraverser::handleRTFx( nvsg::RTFx * rtfx )
{
  NVSG_TRACE();

  ExclusiveTraverser::handleRTFx( rtfx );
}

void FFPToCgFxTraverser::handleMaterial(Material * material)
{
  NVSG_TRACE();
  // fill in material parameters
  std::stringstream ss;

  Vec3f ambient  = material->getAmbientColor();
  Vec3f diffuse  = material->getDiffuseColor();
  Vec3f specular = material->getSpecularColor();
  Vec3f emissive = material->getEmissiveColor();
  float opacity  = material->getOpacity();

  ss << "LightingEnable = true;\n";
  ss << "ShadeModel = Smooth;\n";
  ss << "MaterialAmbient  = float4( " << ambient[0] << ", " << ambient[1] << ", " << ambient[2] << ", " << opacity << " );\n";
  ss << "MaterialDiffuse  = float4( " << diffuse[0] << ", " << diffuse[1] << ", " << diffuse[2] << ", " << opacity << " );\n";
  ss << "MaterialSpecular = float4( " << specular[0] << ", " << specular[1] << ", " << specular[2] << ", " << opacity << " );\n";
  ss << "MaterialEmissive = float4( " << emissive[0] << ", " << emissive[1] << ", " << emissive[2] << ", " << opacity << " );\n";
  ss << "MaterialShininess = " << material->getSpecularExponent() << ";\n";

  m_currentEffect->currentTechnique->currentPass->state += ss.str();
  // Assaf - dont remove the material attribute.. so we can drink and suck the colors out of it upon split.
  m_currentEffect->replacedAttributes.push_back( getWeakPtr<Material>( material ) );

  ExclusiveTraverser::handleMaterial( material );
}

void FFPToCgFxTraverser::handleTextureAttribute(TextureAttribute * texAttrib)
{
  NVSG_TRACE();

  m_currentEffect->replacedAttributes.push_back( getWeakPtr<TextureAttribute>( texAttrib ) );

  ExclusiveTraverser::handleTextureAttribute(texAttrib);
}

inline std::string wrapModeToString( TextureWrapMode wm )
{
  switch( wm )
  {
    case TWM_CLAMP:
      return "Clamp";
    case TWM_REPEAT:
      return "Repeat";
    case TWM_MIRROR_REPEAT:
      return "MirroredRepeat";
    default:
    case TWM_CLAMP_TO_EDGE:
      return "ClampToEdge";
    case TWM_CLAMP_TO_BORDER:
      return "ClampToBorder";
    case TWM_MIRROR_CLAMP:
      return "MirroredClamp";
    case TWM_MIRROR_CLAMP_TO_EDGE:
      return "MirroredClampToEdge";
    case TWM_MIRROR_CLAMP_TO_BORDER:
      return "MirroredClampToBorder";
  }
}

std::string minFilterToString( TextureMinFilterMode mf )
{
  switch( mf )
  {
    case TFM_MIN_NEAREST:
      return "Nearest";
    case TFM_MIN_LINEAR:
      return "Linear";
    case TFM_MIN_LINEAR_MIPMAP_LINEAR:
      return "LinearMipMapLinear";
    case TFM_MIN_NEAREST_MIPMAP_NEAREST:
      return "NearestMipMapNearest";
    case TFM_MIN_NEAREST_MIPMAP_LINEAR:
      return "NearestMipMapLinear";
    default:
    case TFM_MIN_LINEAR_MIPMAP_NEAREST:
      return "LinearMipMapNearest";
  }
}

std::string magFilterToString( TextureMagFilterMode mf )
{
  switch( mf )
  {
    default:
    case TFM_MAG_NEAREST:
      return "Nearest";
    case TFM_MAG_LINEAR:
      return "Linear";
  }
}

std::string envModeToString( TextureEnvMode em )
{
  switch( em )
  {
    default:
    case TEM_REPLACE:
      return "Replace";
    case TEM_MODULATE:
      return "Modulate";
    case TEM_DECAL:
      return "Decal";
    case TEM_BLEND:
      return "Blend";
    case TEM_ADD:
      return "Add";
    case TEM_ADD_SIGNED:
      return "AddSigned";
    case TEM_SUBTRACT:
      return "Subtract";
    case TEM_INTERPOLATE:
      return "Interpolate";
    case TEM_DOT3_RGB:
      return "Dot3RGB";
    case TEM_DOT3_RGBA:
      return "Dot3RGBA";
  }
}

void FFPToCgFxTraverser::handleTextureAttributeItem(TextureAttributeItem * texAttribItem)
{
  NVSG_TRACE();

  std::stringstream samplers;
  std::stringstream ss;

  TextureSharedPtr tex = texAttribItem->getTexture();
  TextureTarget target = tex ? TextureReadLock(tex)->getTextureTarget() : NVSG_UNSPECIFIED_TEXTURE_TARGET;

  // if we don't have a filename or no texture target then we skip this one
  if(! texAttribItem->getTextureFileName().empty() && target != NVSG_UNSPECIFIED_TEXTURE_TARGET )
  {
    unsigned int tu = getCurrentTextureUnit();
    samplers << "texture texDef" << tu << "\n";
    samplers << "<\n";
    samplers << "  string File = \"" << fixupFilename( texAttribItem->getTextureFileName() ) << "\";\n";
    samplers << ">;\n\n";

    switch( target )
    {
      case NVSG_TEXTURE_1D:
        samplers << "sampler1D tex" << tu << " = sampler_state\n{\n";
        ss << "Texture1DEnable[" << tu << "] = true;\n";
        ss << "Texture1D[" << tu << "] = tex" << tu << ";\n";
        break;

      case NVSG_TEXTURE_2D:
        samplers << "sampler2D tex" << tu << " = sampler_state\n{\n";
        ss << "Texture2DEnable[" << tu << "] = true;\n";
        ss << "Texture2D[" << tu << "] = tex" << tu << ";\n";
        break;

      case NVSG_TEXTURE_3D:
        samplers << "sampler3D tex" << tu << " = sampler_state\n{\n";
        ss << "Texture3DEnable[" << tu << "] = true;\n";
        ss << "Texture3D[" << tu << "] = tex" << tu << ";\n";
        break;

      case NVSG_TEXTURE_CUBE:
        samplers << "samplerCUBE tex" << tu << " = sampler_state\n{\n";
        ss << "TextureCubeMapEnable[" << tu << "] = true;\n";
        ss << "TextureCubeMap[" << tu << "] = tex" << tu << ";\n";
        break;

      case NVSG_TEXTURE_RECTANGLE:
        samplers << "samplerRECT tex" << tu << " = sampler_state\n{\n";
        ss << "TextureRectangleEnable[" << tu << "] = true;\n";
        ss << "TextureRectangle[" << tu << "] = tex" << tu << ";\n";
        break;

      case NVSG_TEXTURE_1D_ARRAY:
      case NVSG_TEXTURE_2D_ARRAY:
        // these can't be enabled with FFP...
        break;
    }

    ss << "TextureEnvMode[" << tu << "] = " << envModeToString( texAttribItem->getEnvMode() ) << ";\n";

    // now, finish sampler state
    samplers << " WrapS = " << wrapModeToString( texAttribItem->getWrapMode( TWCA_S ) ) << ";\n";
    samplers << " WrapT = " << wrapModeToString( texAttribItem->getWrapMode( TWCA_T ) ) << ";\n";
    samplers << " WrapR = " << wrapModeToString( texAttribItem->getWrapMode( TWCA_R ) ) << ";\n";
    samplers << " MagFilter = " << magFilterToString( texAttribItem->getMagFilterMode() ) << ";\n";
    samplers << " MinFilter = " << minFilterToString( texAttribItem->getMinFilterMode() ) << ";\n";
    samplers << " MaxAnisotropy = " << texAttribItem->getMaxAnisotropy() << ";\n";
    samplers << " Texture = <texDef" << tu << ">;\n";
    samplers << "};\n\n";

    m_currentEffect->tweakables += samplers.str();
    m_currentEffect->currentTechnique->currentPass->state += ss.str();
  }

  ExclusiveTraverser::handleTextureAttributeItem(texAttribItem);
}

void FFPToCgFxTraverser::handleAlphaTestAttribute(AlphaTestAttribute * alphaTestAttrib)
{
  NVSG_TRACE();

  std::stringstream ss;
  ss << "AlphaTestEnable = true;\n";
  ss << "AlphaFunc = float2( ";

  switch( alphaTestAttrib->getAlphaFunction() )
  {
    case AF_NEVER:
      ss << "Never";
      break;
    case AF_LESS:
      ss << "Less";
      break;
    case AF_EQUAL:
      ss << "Equal";
      break;
    case AF_LEQUAL:
      ss << "LEqual";
      break;
    case AF_GREATER:
      ss << "Greater";
      break;
    case AF_NOTEQUAL:
      ss << "NotEqual";
      break;
    case AF_GEQUAL:
      ss << "GEqual";
      break;
    case AF_ALWAYS:
      ss << "Always";
      break;
  }

  ss << ", " << alphaTestAttrib->getThreshold() << " );\n";

  m_currentEffect->currentTechnique->currentPass->state += ss.str();
  m_currentEffect->replacedAttributes.push_back( getWeakPtr<AlphaTestAttribute>( alphaTestAttrib ) );

  ExclusiveTraverser::handleAlphaTestAttribute( alphaTestAttrib );
}

inline std::string blendEnumToString( BlendFunction bf )
{
  switch( bf )
  {
    default:
    case BF_ZERO:
      return "Zero";
    case BF_ONE:
      return "One";
    case BF_DESTINATION_COLOR:
      return "DestColor";
    case BF_SOURCE_COLOR:
      return "SrcColor";
    case BF_ONE_MINUS_DESTINATION_COLOR:
      return "OneMinusDestColor";
    case BF_ONE_MINUS_SOURCE_COLOR:
      return "OneMinusSrcColor";
    case BF_SOURCE_ALPHA:
      return "SrcAlpha";
    case BF_ONE_MINUS_SOURCE_ALPHA:
      return "OneMinusSrcAlpha";
    case BF_DESTINATION_ALPHA:
      return "DestAlpha";
    case BF_ONE_MINUS_DESTINATION_ALPHA:
      return "OneMinusDestAlpha";
    case BF_SOURCE_ALPHA_SATURATE:
      return "SrcAlphaSaturate";
  }
}

void FFPToCgFxTraverser::handleBlendAttribute(BlendAttribute * blendAttrib)
{
  NVSG_TRACE();

  std::stringstream ss;
  ss << "BlendEnable = true;\n";
  ss << "BlendFunc = float2( " << blendEnumToString( blendAttrib->getSourceFunction() ) << ", " <<
                                  blendEnumToString( blendAttrib->getDestinationFunction() ) << " );\n";

  m_currentEffect->currentTechnique->currentPass->state += ss.str();
  m_currentEffect->replacedAttributes.push_back( getWeakPtr<BlendAttribute>( blendAttrib ) );

  ExclusiveTraverser::handleBlendAttribute( blendAttrib );
}

void FFPToCgFxTraverser::handleDepthAttribute(DepthAttribute * depthAttrib)
{
  NVSG_TRACE();

  std::stringstream ss;
  ss << "DepthTestEnable = " << (depthAttrib->isDepthTestingEnabled() ? "true" : "false") << ";\n";
  ss << "DepthMask = " << (depthAttrib->isDepthWritingEnabled() ? "true" : "false") << ";\n";
  ss << "DepthFunc = ";

  switch( depthAttrib->getDepthFunction() )
  {
    case DF_NEVER:
      ss << "Never";
      break;
    case DF_LESS:
      ss << "Less";
      break;
    case DF_EQUAL:
      ss << "Equal";
      break;
    case DF_LESS_OR_EQUAL:
      ss << "LEqual";
      break;
    case DF_GREATER:
      ss << "Greater";
      break;
    case DF_NOT_EQUAL:
      ss << "NotEqual";
      break;
    case DF_GREATER_OR_EQUAL:
      ss << "GEqual";
      break;
    case DF_ALWAYS:
      ss << "Always";
      break;
  }

  ss << ";\n";

  m_currentEffect->currentTechnique->currentPass->state += ss.str();
  m_currentEffect->replacedAttributes.push_back( getWeakPtr<DepthAttribute>( depthAttrib ) );

  ExclusiveTraverser::handleDepthAttribute( depthAttrib );
}

void FFPToCgFxTraverser::handleStencilAttribute(StencilAttribute * stencilAttrib)
{
  NVSG_TRACE();
  ExclusiveTraverser::handleStencilAttribute( stencilAttrib );
}

void FFPToCgFxTraverser::handleFaceAttribute(FaceAttribute * faceAttrib)
{
  NVSG_TRACE();

#if 0
  std::stringstream ss;
  bool ffc = faceAttrib->isFrontFaceCulling();
  bool bfc = faceAttrib->isBackFaceCulling();
  std::string cullm = (bfc && ffc) ? "FrontAndBack" : bfc ? "Back" : "Front";

  ss << "CullFaceEnable = " << (ffc || bfc) ? "true" : "false" << ";\n";
  ss << "CullFace = " << cullm << ";\n";

  std::string pmodef, pmodeb;

  pmodef = faceAttrib->getFrontFaceMode()

  ss << "PolygonMode = " << pmode << ";\n";

  ss << "PolygonOffsetEnable = " << (faceAttrib->isPolygonOffset() ? "true" : "false") << ";\n";
  ss << "PolygonOffset = float2( " << faceAttrib->getPolygonOffsetFactor() << ", "
                                     faceAttrib->getPolygonOffsetUnits() << " );\n";


  m_currentEffect->currentTechnique->currentPass->state += ss.str();
  m_currentEffect->replacedAttributes.push_back( getWeakPtr<DepthAttribute>( depthAttrib ) );
#endif

  ExclusiveTraverser::handleFaceAttribute( faceAttrib );
}

void FFPToCgFxTraverser::handleLineAttribute(LineAttribute * lineAttrib)
{
  NVSG_TRACE();
  ExclusiveTraverser::handleLineAttribute( lineAttrib );
}

void FFPToCgFxTraverser::handleUnlitColorAttribute(UnlitColorAttribute * unlitColor)
{
  NVSG_TRACE();
  ExclusiveTraverser::handleUnlitColorAttribute( unlitColor );
}

void FFPToCgFxTraverser::handleLightingAttribute(LightingAttribute * lAttrib)
{
  NVSG_TRACE();

  std::stringstream ss;
  ss << "LightingEnable = " << (lAttrib->getEnabled() ? "true" : "false") << ";\n";

  m_currentEffect->currentTechnique->currentPass->state += ss.str();
  m_currentEffect->replacedAttributes.push_back( getWeakPtr<LightingAttribute>( lAttrib ) );

  ExclusiveTraverser::handleLightingAttribute( lAttrib );
}

void FFPToCgFxTraverser::handlePointAttribute(PointAttribute * pointAttrib)
{
  NVSG_TRACE();
  ExclusiveTraverser::handlePointAttribute( pointAttrib );
}

void FFPToCgFxTraverser::handleRTFxSceneAttribute(RTFxSceneAttribute * p)
{
  NVSG_TRACE();

  ExclusiveTraverser::handleRTFxSceneAttribute( p );
}

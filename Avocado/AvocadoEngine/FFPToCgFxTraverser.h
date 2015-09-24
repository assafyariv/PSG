// Copyright NVIDIA Corporation 2002-2005
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

#pragma once

#include <nvtraverser/Traverser.h>
#include <nvsg/Drawable.h>
#include <nvsg/Scene.h>
#include <nvsg/StateSet.h>
#include <nvsg/TextureAttribute.h>

class FFPToCgFxTraverser : public nvtraverser::ExclusiveTraverser
{
  struct FxPass
  {
    std::string name;
    std::string state;
  };

  struct FxTechnique
  {
    std::string name;
    std::vector< FxPass > passes;
    FxPass * currentPass;
  };

  struct FxEffect
  {
    std::string name;
    std::string tweakables;
    std::vector< FxTechnique > techniques;
    FxTechnique * currentTechnique;
    std::vector< nvsg::StateAttributeWeakPtr > replacedAttributes;
  };

  public:
    FFPToCgFxTraverser();
    virtual ~FFPToCgFxTraverser();
	void SetFlipYZ (bool flipYZ);
	bool GetFlipYZ ();
  protected:
    virtual std::string buildShader();
    virtual void handleStateSet(nvsg::StateSet * sSet);
    virtual void handleStateVariant(nvsg::StateVariant * variant);
    virtual void handleStatePass(nvsg::StatePass * pass);
    virtual void handleCgFx(nvsg::CgFx * cgfx);
    virtual void handleRTFx( nvsg::RTFx * rtfx );
    virtual void handleMaterial(nvsg::Material * material);
    virtual void handleTextureAttribute(nvsg::TextureAttribute * texAttrib);
    virtual void handleTextureAttributeItem(nvsg::TextureAttributeItem * texAttribItem);
    virtual void handleAlphaTestAttribute(nvsg::AlphaTestAttribute * alphaTestAttrib);
    virtual void handleBlendAttribute(nvsg::BlendAttribute * blendAttrib);
    virtual void handleDepthAttribute(nvsg::DepthAttribute * depthAttrib);
    virtual void handleStencilAttribute(nvsg::StencilAttribute * stencilAttrib);
    virtual void handleFaceAttribute(nvsg::FaceAttribute * faceAttrib);
    virtual void handleLineAttribute(nvsg::LineAttribute * lineAttrib);
    virtual void handleUnlitColorAttribute(nvsg::UnlitColorAttribute * unlitColor);
    virtual void handleLightingAttribute(nvsg::LightingAttribute * lAttrib);
    virtual void handlePointAttribute(nvsg::PointAttribute * pointAttrib);
    virtual void handleRTFxSceneAttribute(nvsg::RTFxSceneAttribute * p);
	virtual void handleVertexAttributeSet( nvsg::VertexAttributeSet *vas );
	virtual void handleTransform (nvsg::Transform *tra);
  private:
	bool		m_flipYZ;
    unsigned int m_numStateSets;
    FxEffect * m_currentEffect;
};

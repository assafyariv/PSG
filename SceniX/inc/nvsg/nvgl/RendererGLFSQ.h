// Copyright NVIDIA Corporation 2010-2011
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

#include <nvsgcommon.h>
#include <nvgl/GLContext.h>
#include <nvgl/GLDALData.h>
#include <nvsg/TextureHost.h>
#include <nvui/Renderer.h>
#include <nvmath/Vecnt.h>
#include <nvgl/TextureGL.h>
#include <nvgl/RenderTargetGL.h>
#include <nvgl/GLObjectRenderer.h>

namespace nvgl
{

  class TextureGL;
  class TextureGL2D;
  class TextureGLRectangle;

  /*! \brief Renderer to draw a "Full Screen" (ie: viewport-filling) quad, using a supplied CgFX.
   *  \par Namespace: nvgl
   *  \remarks Many rendering effects in modern applications require a Viewport-filling quad to realize certain screen-space 
   *  or compositing effects.  This class will render this type of quad, using the supplied CgFX.  Additionally, applications may
   *  supply per vertex data as texture coordinates to be used by the CgFX while rendering.\n
   *  This class also provides several helper methods to "present" (render) a texture in a full screen quad.
   *  
   *  \note Draws a viewport filling quad on Z = 0, with the following characteristics:
   *  \code
   *         Vertex Coords                 Texture Coords (units 0 and 1)
   *  
   *     (-1,1) 3-------2 (1,1)          0:(0,1)   3-------2 0:(1,1)
   *            |       |                1:(x,y+h) |       | 1:(x+w,y+h)
   *            |       |                          |       |
   *    (-1,-1) 0-------1 (1,-1)         0:(0,0)   0-------1 0:(1,0)
   *                                     1:(x,y)             1:(x+w,y)
   *  
   *                                      Where: X,Y,W,H are either Viewport Parameters or
   *                                             X,Y=0 and W,H are output RenderTarget dimensions
   *  \endcode
   *
   *  \note Here is an example CgFX effect that could be used to render a checkerboard pattern over the scene and tint it
   *  Red.
   *  \code
   *
   *   float4 vertex( float4 vpos : POSITION ) : POSITION
   *   {
   *     // Note, vertices are already appropriately transformed.  Typically it will not be necessary to 
   *     // further transform them.  However, any TEXCOORDS that are used should be passed to the fragment program.
   *     return vpos;
   *   }
   *   
   *   float4 fragment( int2 wpos : WPOS ) : COLOR
   *   {
   *     //make a checkerboard pattern
   *     bool xp = wpos.x & 4;
   *     bool yp = wpos.y & 4;
   *     return (xp != yp) ? float4( 1.f, 0.f, 0.f, 0.5f ) : float4(0.f);
   *   }
   *   
   *   technique
   *   {
   *     pass
   *     {
   *       VertexProgram   = compile latest vertex();
   *       FragmentProgram = compile latest fragment();
   *   
   *       BlendEnable = true;
   *       BlendFunc = { SrcAlpha, OneMinusSrcAlpha };
   *     }
   *   }
   *
   *  \endcode
   *  
   *  \sa nvsg::CgFx, nvsg::CgFx::Effect, nvui::Renderer */
  class RendererGLFSQ : public nvui::Renderer
  {
    public:
      NVSG_API static nvutil::SmartPtr<RendererGLFSQ> create( const nvgl::SmartRenderTargetGL &renderTarget = nvgl::SmartRenderTargetGL() );
      NVSG_API virtual ~RendererGLFSQ(void);

      /*! \brief Set the CgFX to use when rendering this FSQ.
       *  \param cgfxh A shared pointer to the CgFx to use for this FSQ.
       * \sa getCgFx, nvsg::CgFx */
      NVSG_API virtual bool setCgFx( const nvsg::CgFxSharedPtr &cgfxh );

      /*! \brief Get the CgFX used in this FSQ.
       *  \return cgfxh A shared pointer to the CgFx used in this FSQ.
       * \sa setCgFx, nvsg::CgFx */
      NVSG_API virtual const nvsg::CgFxSharedPtr & getCgFx();

      /*! \brief Add or remove texture coordinate attributes.
       *  \remarks Adds or removes texture coordinate attributes from this FSQ.
       *  \param unit The texture attribute identifier on which to modify the texcoords.  If adding texcoords, they will be available 
       *  as TEXCOORDunit, where unit can range from 2 to 7, as TEXCOORD0 and TEXCOORD1 are used internally by the FSQ.
       *  \param coords A vector of Vec4f's to use as the data.  Note the ordering of the vertex data, as described above.  If this vector
       *  is empty, any texcoords assigned to this texcoord unit are removed.
       **/
      NVSG_API virtual void setTexCoords( unsigned int unit, const std::vector<nvmath::Vec4f> &coords );

      /*! \brief Set a Sampler's texture by name
       *  \remarks This is a convenience function to set the TextureGL or TextureHost of a Sampler.
       *  This could also be accomplished using the CgFX API as well.
       *  \param sampler The Sampler name in the CgFX.
       *  \param texture The Texture to assign to the Sampler.
       *  \return True if the named sampler was found and the texture was assigned.  False otherwise.
       *  \note An example would be the following:
       *  \code
       *   CgFX:
       *   sampler2D coolio [...];
       *   C++:
       *   fsq->setSamplerTextureByName( "coolio", theTexture );
       *  \endcode
       *
       * \sa setSamplerTextureBySemantic */
      NVSG_API virtual bool setSamplerTextureByName( const std::string &sampler, const nvsg::TextureSharedPtr &texture );

      /*! \brief Set a Sampler's texture by name using a native OpenGL texture object ID.
       *  \param sampler The Sampler name in the CgFX.
       *  \param texID The OpenGL texture object ID to assing to the Cg sampler 
       *  \return True if the named sampler was found and the texture was assigned. False otherwise.
       *  \sa setSamplerTextureBySemantic */
      NVSG_API virtual bool setSamplerTextureByName( const std::string &sampler, GLuint texID );

      /*! \brief Set a Sampler's texture by semantic
       *  \remarks This is a convenience function to set the TextureGL or TextureHost of a Sampler.
       *  This could also be accomplished using the CgFX API as well.
       *  \param semantic The semantic of the Sampler in the CgFX.
       *  \param texture The Texture to assign to the Sampler.
       *  \return True if a sampler with the given semantic was found and the texture was assigned.  False otherwise.
       *  \note An example would be the following:
       *  \code
       *   CgFX:
       *   sampler2D coolio : MYCOOLSAMPLER;
       *   C++:
       *   fsq->setSamplerTextureBySemantic( "MYCOOLSAMPLER", theTexture );
       *  \endcode
       *
       * \sa setSamplerTextureByName */
      NVSG_API virtual bool setSamplerTextureBySemantic( const std::string &semantic, const nvsg::TextureSharedPtr &texture );

      /*! \brief Set a Sampler's texture by semantic using a native OpenGL texture object ID.
       *  \param semantic The semantic of the Sampler in the CgFX.
       *  \param texID The OpenGL texture object ID to assing to the Cg sampler 
       *  \return True if a sampler with the given semantic was found and the texture was assigned.  False otherwise.
       *  \sa setSamplerTextureByName */
      NVSG_API virtual bool setSamplerTextureBySemantic( const std::string &semantic, GLuint texID );

      /*! \brief Fill the viewport with the given 2D GL textureID. 
       *  \remarks This is a convenience function to render the given OpenGL texture in a viewport-filling quad.
       *  \param textureId The GLuint representing an OpenGL texture resource.
       *  \param target The RenderTargetGL to fill with the texture.
       *  \param callRTBeginEnd Whether to wrap the Quad rendering with target->beginRendering() and target->endRendering().  In some 
       *  cases the RenderTarget may be current and calling begin/endRendering may be either unnecessary or detremental.
       **/
      NVSG_API static void presentTextureGL2D( GLuint textureId, const nvgl::SmartRenderTargetGL &target, bool callRTBeginEnd = true );

      /*! \brief Fill the viewport with the given TextureGL2D.
       *  \remarks This is a convenience function to render the given TextureGL2D in a viewport-filling quad.
       *  \param tex2d The TextureGL2D resource.
       *  \param target The RenderTargetGL to fill with the texture.
       *  \param callRTBeginEnd Whether to wrap the Quad rendering with target->beginRendering() and target->endRendering().  In some 
       *  cases the RenderTarget may be current and calling begin/endRendering may be either unnecessary or detremental.
       **/
      NVSG_API static void presentTextureGL2D( const nvgl::TextureGL2DSharedPtr &tex2d, const nvgl::SmartRenderTargetGL &target, bool callRTBeginEnd = true );

      /*! \brief Fill the viewport with the given TextureGLRectangle.
       *  \remarks This is a convenience function to render the given TextureGLRectangle in a viewport-filling quad.
       *  \param tex2d The TextureGLRectangle resource.
       *  \param target The RenderTargetGL to fill with the texture.
       *  \param callRTBeginEnd Whether to wrap the Quad rendering with target->beginRendering() and target->endRendering().  In some 
       *  cases the RenderTarget may be current and calling begin/endRendering may be either unnecessary or detremental.
       **/
      NVSG_API static void presentTextureGLRectangle( const nvgl::TextureGLRectangleSharedPtr &tex2d, const nvgl::SmartRenderTargetGL &target, bool callRTBeginEnd = true );

      REFLECTION_INFO_API( NVSG_API, RendererGLFSQ );
      BEGIN_DECLARE_STATIC_PROPERTIES
      END_DECLARE_STATIC_PROPERTIES

    protected:
      NVSG_API RendererGLFSQ( const nvgl::SmartRenderTargetGL &target );
      NVSG_API virtual void doRender( const nvui::SmartRenderTarget &renderTarget );

    private:
      bool applyCgFx();
      void createTextureInfo( const nvsg::TextureSharedPtr &texturePtr, nvgl::SmartShareGroupGLResource &res, nvgl::TextureInfoSharedPtr &info );
      void checkTextureInfoForUpdate( const nvsg::TextureSharedPtr &tex, const nvgl::TextureInfoSharedPtr &textureInfo );
      void updateTextureInfo( const nvsg::TextureHostSharedPtr &tex, const nvgl::TextureInfoSharedPtr &textureInfo );

    private:
      nvsg::CgFxSharedPtr   m_cgfx;
      nvsg::CgFxEffect     *m_cgfxEffect;
      nvsg::CgFxTechnique   m_cgfxTechnique;
      unsigned int          m_cgfxPassCount;
      unsigned int          m_texCoordsUsed;
      std::vector<nvmath::Vec4f> m_texCoords[6];
      bool                  m_preserveTexturesAfterUpload;

      // For texture object ID resource handling:
      struct TexInfo
      {
        TextureInfoSharedPtr      info;
        SmartShareGroupGLResource resource;
      };

      std::map<CGparameter, TexInfo> m_sampler;
  };

  typedef nvutil::SmartPtr<RendererGLFSQ> SmartRendererGLFSQ;

} // namespace nvgl

// Copyright NVIDIA Corporation 2002-2007
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
/** @file */

#include "nvsg/OwnedObject.h"
#include "nvsg/VertexAttributeSet.h"
#include "nvutil/HashGenerator.h"
#include "nvutil/SmartPtr.h"

namespace nvsg
{

  /*! \brief Provides an interface to perform custom pre- and post-processing with rendering a Drawable.
   * \remarks
   * A DrawCallback can be assigned to an nvsg::Drawable, using the Drawable::setDrawCallback API, to implement
   * a custom pre- and post-processing for that particular Drawable while it's being rendered. 
   * A DrawCallback must be instantiated on the heap using the \c new keyword. It's lifetime is managed through
   * reference counting provided by the nvutil::RCObject base class. Typically, a custom DrawCallback will 
   * be assigned to a Drawable directly after the DrawCallback was instantiated. Doing so, the user hands over
   * ownership of the DrawCallback to the Drawable, and is no longer responsible for destroying the DrawCallback
   * afterwards:
   * \code
   *   
   *   TrianglesWeakPtr htris = Triangles::create();
   *   {
   *     TrianglesWriteLock tris(htris);
   *     tris->setDrawCallback(new MyDrawCallback);
   *     ...
   *   }
   *   
   * \endcode
   * 
   * A DrawCallback can also be assigned to a Drawable to completely overrule the core renderer's implementation
   * as demonstrated in the following example:
   *
   * \code
   * 
   * enum { OC_MYSPHERE = OC_CUSTOMOBJECT + 1 };
   * 
   * class MySphere : public nvsg::Drawable
   * {
   * public: 
   *   MySphere() { m_objectCode = OC_MYSPHERE; }
   *   // accessors
   *   void setRadius(float r) { m_radius = r; }
   *   float getRadius() const { return m_radius; }
   *   void setSlices(int slices) { m_slices = slices; }
   *   int getSlices() const { return m_slices; }
   *   void setStacks(int stacks) { m_stacks = stacks; }
   *   int getStacks() const { return m_stacks; }
   *   
   * private:
   *   float m_radius;
   *   int m_slices, m_stacks;
   * };
   * 
   * CORE_TYPES( MySphere, Drawable );
   * OBJECT_TRAITS( MySphere, Drawable );
   *
   * class MySphereGLDraw : public nvsg::DrawCallback
   * {
   *    // overrules the core renderers implementation
   *    bool draw(const Drawable * p)
   *    {
   *      const MySphere * theSphere = dynamic_cast<const MySphere *>(p);
   *      if ( theSphere )
   *      {
   *        // [...] probably save some OpenGL state ...
   *
   *        GLUquadric * q = gluNewQuadric();
   *        gluSphere(q, theSphere->getRadius(), theSphere->getSlices(), theSphere->getStacks());
   * 
   *        // [...] restore state
   *        
   *        // indicate owner draw
   *        return true;
   *      }
   *    }
   * };
   *
   * GeoNodeWeakPtr createGnodeWithSphere()
   * {
   *   MaterialWeakPtr hdlMat = Material::create();
   *   {
   *     MaterialWriteLock theMat(hdlMat);
   *     theMat->setAmbientColor( Vec3f(0.f, 0.f, 1.f) );
   *     theMat->setDiffuseColor( Vec3f(0.4f, 0.4f, 0.4f) );
   *     theMat->setEmissiveColor( Vec3f(0.f, 0.f, 0.f) );
   *     theMat->setSpecularColor( Vec3f(1.f, 1.f, 1.f) );
   *     theMat->setSpecularExponent( 10.f );
   *     theMat->setOpacity( 1.0f );
   *   }
   *
   *   StateSetWeakPtr hdlSset = StateSet::create();
   *   {
   *     StateSetWriteLock theSset(hdlSset);
   *     theSset->addAttribute(hdlMat);
   *   } 
   *   
   *   MySphereWeakPtr hdlSphere = MySphere::create();
   *   {
   *     MySphereWriteLock theSphere(hdlSphere);
   *     theSphere->setRadius(0.8f);
   *     theSphere->setSlices(16);
   *     theSphere->setStacks(16);
   *     
   *     theSphere->setDrawCallback(new MySphereGLDraw);
   *   } 
   *   
   *   GeoNodeWeakPtr hdlGnode = GeoNode::create();
   *   {
   *     GeoNodeWriteLock theGnode(hdlGnode);
   *     theGnode->addDrawable( hdlSset, hdlSphere );
   *   } 
   *   
   *   return hdlGnode;
   * }
   * \endcode
   *
   * \sa Drawable
   */
  class DrawCallback : public nvutil::RCObject
  {
  public:
    /*! \brief Called from the render framework prior to render the indicated Drawable.
     * \param p Specifies the Drawable to render.
     * \return The default implementation returns \c true, to indicate that rendering should proceed.
     * \remarks
     * If a DrawCallback was assigned to a Drawable, this function will be called from the 
     * render framework immediately prior to render the Drawable. A derived DrawCallback
     * can override this function to perform some preprocessing immediately before the
     * Drawable will be rendered. If a derived DrawCallback's implementation returns \c false,
     * rendering of the indicated Drawable will be skipped by the render framework. */
    virtual bool preDraw(const Drawable * p) { return true; };

    /*! \brief Called from the render framework when the rendering of the indicated Drawable starts.
     * \param p Specifies the Drawable to render.
     * \return The default implementation returns \c false, to indicate that the currently configured
     * renderer should process the rendering. 
     * \remarks
     * If a DrawCallback was assigned to a Drawable, this function will be called from the render
     * framework when the rendering of the indicated Drawable starts. A derived DrawCallback can
     * override this function to process the rendering on its own. In this case, the override function
     * should return \c true, causing the render framework not to invoke the configured renderer
     * to render the indicated Drawable.\n
     * Note, that the render framework will not call this function if the preDraw implementation of
     * the same DrawCallback object returns \c false. */
    virtual bool draw(const Drawable * p) { return false; }

    /*! \brief Called from the render framework after the indicated Drawable was rendered.
     * \param p Specifies the Drawable that was rendered immediately prior to this call.
     * \remarks
     * If a DrawCallback was assigned to a Drawable, this function will be called from the render
     * immediately after the indicated Drawable was rendered. A derived DrawCallback can override
     * this function to perform some post-processing. The default implementation does nothing. \n
     * Note, that the render framework will not call this function if the preDraw implementation of
     * the same DrawCallback object returns \c false. */
    virtual void postDraw(const Drawable * p) {}

  protected:
    virtual ~DrawCallback() {}
  };

  /*! \brief The abstract base class of all drawable objects.
   *  \par Namespace: nvsg
   *  \remarks All drawable objects like PrimitiveSet or Shape derive from Drawable.\n
   *  When deriving from Drawable, you have to overload the member functions clone(), and
   *  calculateBoundingSphere(). 
   *  \sa nvutil::Incarnation, Object, PrimitiveSet, Shape */
  class Drawable : public OwnedObject<Object>
  {
    public:
      /*! \brief Destructs a Drawable.
       */
      NVSG_API virtual ~Drawable( void );

      /*! \brief Generates vertex normals 
       *  \param overwrite An optional flag indicating whether to overwrite existing vertex normals.
       *  The default is to overwrite existing data.
       *  \return \c true, if normals could be generated, otherwise \c false.
       *  \remarks The function calls the protected virtual function calculateNormals, which concrete Drawables
       * should override to provide correct calculation of vertex data.
       * \sa calculateNormals */
      NVSG_API bool generateNormals(bool overwrite = true);

      /*! \brief Generates 2D texture coordinates 
       * \param type
       * Desired texture coordinate type. Accepted are TCT_CYLINDRICAL, TCT_PLANAR, and TCT_SPHERICAL. 
       * \param texcoords 
       * Addresses the vertex attribute where to output the generated texture coords. 
       * VertexAttributeSet::NVSG_TEXCOORD0 - VertexAttributeSet::NVSG_TEXCOORD7 are allowed identifiers.
       * By default texture coords are written to VertexAttributeSet::NVSG_TEXCOORD0, 
       * \param overwrite 
       * An optional flag indicating whether to overwrite existing vertex data.
       * The default is to overwrite existing data.
       * \remarks
       * The function calls the protected virtual function calculateTexCoords, which concrete Drawables
       * should override to provide correct texture coordinate calculation.
       * \sa calculateTexCoords */
      NVSG_API void generateTexCoords( TextureCoordType type, 
                                       unsigned int texcoords = VertexAttributeSet::NVSG_TEXCOORD0, 
                                       bool overwrite = true );

      /*! \brief Generates tangents and binormals
       * \param texcoords
       * Addresses the vertex attribute to hold the input 2D texture coordinates used to calculate the tangent space.
       * By default, input texture coordinates are taken from the VertexAttributeSet::NVSG_TEXCOORD0. 
       * \param tangents
       * Addresses the vertex attribute where to output the calculated tangents. 
       * By default tangents are written to VertexAttributeSet::NVSG_TANGENT, 
       * which is aligned to the TANGENT binding semantic used by Cg for varying vertex shader input. 
       * \param binormals
       * Addresses the vertex attribute where to output the calculated binormals.
       * By default binormals are written to VertexAttributeSet::NVSG_BINORMAL, 
       * which is aligned to the BINORMAL binding semantic used by Cg for varying vertex shader input. 
       * \param overwrite 
       * An optional flag indicating whether to overwrite existing vertex data.
       * The default is to overwrite existing data.
       * \remarks
       * The function calls the protected virtual function calculateTangentSpace, which concrete Drawables
       * should override to provide correct tangent space calculation.
       * \sa calculateTangentSpace */
      NVSG_API void generateTangentSpace( unsigned int texcoords = VertexAttributeSet::NVSG_TEXCOORD0, 
                                          unsigned int tangents  = VertexAttributeSet::NVSG_TANGENT, 
                                          unsigned int binormals = VertexAttributeSet::NVSG_BINORMAL,
                                          bool overwrite = true );

      using Object::getBoundingBox;
      using Object::getBoundingSphere;

      /*! \brief Assigns the indicated DrawCallback to this Drawable.
       * \param cb Specifies the DrawCallback object
       * If a DrawCallback is assigned to a Drawable, the DrawCallback::preDraw,
       * DrawCallback::draw, and DrawCallback::postDraw functions will be invoked 
       * from the render framework when rendering the Drawable. 
       * \sa DrawCallback */
      NVSG_API virtual void setDrawCallback(DrawCallback * cb);

      /*! \brief Retrieves the last assigned DrawCallback object from the Drawable.
       * \return The DrawCallback that was last assigned to the drawable.
       * \sa setDrawCallback */
      NVSG_API DrawCallback * getDrawCallback() const;

      REFLECTION_INFO_API( NVSG_API, Drawable );
    protected:
      /*! \brief Protected constructor to prevent explicit creation.
       *  \remarks Because Drawable is an abstract class, its constructor is never used
       *  explicitly, but in the constructor of derived classes.
       *  \sa PrimitiveSet, Shape */
      NVSG_API Drawable();

      /*! \brief Protected copy constructor to prevent explicit creation.
       *  \remarks Because Drawable is an abstract class, its constructor is never used
       *  explicitly, but in the constructor of derived classes.
       *  \sa PrimitiveSet, Shape */
      NVSG_API Drawable( const Drawable& );

      /*! \brief Calculates vertex normals
       *  \param overwrite A flag indicating whether to overwrite existing vertex normals.
       *  \return \c true, if normals could be calculated, otherwise \c false.
       *  \remarks This function gets called from the generateNormals API.
       *  Concrete Drawables should override this function provide correct calculation of vertex normals.
       *  The base class implementation does nothing.
       *  \sa generateNormals */
      NVSG_API virtual bool calculateNormals(bool overwrite);

      /*! \brief Calculates 2D texture coordinates
       * \param type
       * Desired texture coordinate type. Accepted are TCT_CYLINDRICAL, TCT_PLANAR, and TCT_SPHERICAL. 
       * \param texcoords 
       * Addresses the vertex attribute where to output the generated texture coords. 
       * \param overwrite 
       * A flag indicating whether to overwrite existing vertex data.
       * \remarks
       * This function gets called from the generateNormals API.
       * Concrete Drawables should override this function to provide correct calculation of texture coords.
       * The base class implementation does nothing.
       * \sa generateNormals */
      NVSG_API virtual void calculateTexCoords(TextureCoordType type, unsigned int texcoords, bool overwrite);

      /*! \brief Calculates tangents and binormals
       * \param texcoords
       * Addresses the vertex attribute to hold the input 2D texture coordinates used to calculate the tangent space.
       * \param tangents
       * Addresses the vertex attribute where to output the calculated tangents. 
       * which is aligned to the TANGENT binding semantic used by Cg for varying vertex shader input. 
       * \param binormals
       * Addresses the vertex attribute where to output the calculated binormals.
       * which is aligned to the BINORMAL binding semantic used by Cg for varying vertex shader input. 
       * \param overwrite 
       * A flag indicating whether to overwrite existing vertex data.
       * \remarks
       * This function gets called from the generateTangentSpace API.
       * Concrete Drawables should override this function to provide correct calculation of tangents and binormals.
       * The base class implementation does nothing.
       * \sa calculateTangentSpace */
      NVSG_API virtual void calculateTangentSpace(unsigned int texcoords, unsigned int tangents, unsigned int binormals, bool overwrite);

      NVSG_API virtual const nvmath::Box3f & getBoundingBox( bool recalculate ) const;
      NVSG_API virtual const nvmath::Sphere3f & getBoundingSphere( bool recalculate ) const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant Drawable to copy from
       *  \return A reference to the assigned Drawable
       *  \remarks The assignment operator calls the assignment operator of Object and
       *  copies the bounding sphere.
       */
      NVSG_API Drawable & operator=(const Drawable & rhs);

    private:
      nvutil::SmartPtr<DrawCallback>  m_drawCB;

      mutable nvmath::Box3f     m_boundingBox;
      mutable nvmath::Sphere3f  m_boundingSphere;
  };

  NVSG_API void copy( const DrawableReadLock & src, const DrawableWriteLock & dst );

  inline void Drawable::setDrawCallback(DrawCallback * cb)
  {
    NVSG_TRACE();
    
    if ( cb && !m_drawCB || !cb && m_drawCB )
    {
      // a transfer from no callback to callback or vice versa
      // requires a RL rebuild
      notifyChange( this, NVSG_TREE_INCARNATION );
    }

    m_drawCB = cb;
  }

  inline DrawCallback * Drawable::getDrawCallback() const
  {
    NVSG_TRACE();
    return m_drawCB.get();
  }

} // namespace nvsg

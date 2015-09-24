// Copyright NVIDIA Corporation 2002-2011
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
/** \file */

#include "nvsgcommon.h"
#include "nvtraverser/Traverser.h"

namespace nvsg
{
  class AnimationState;
  class IndependentPrimitiveSet;
  class MeshedPrimitiveSet;
  class Path;
  class StrippedPrimitiveSet;
  template<typename T> class Animation;
  template<typename T> class KeyFramedAnimationDescription;
}

namespace nvtraverser
{
  /*! \brief Traverser to search for a tree object by class type or by name.
    * You want to apply a SearchTraverser on a scene or a certain node to search for tree objects by name or by class type.
    * If you search for objects by class type you can configure the SearchTraverser to search for an explicit class type or 
    * for objects that have the specified class typ as base class. 
    */
  class SearchTraverser : public SharedTraverser
  {
    public:
      /*! \brief Default-constructs a SearchTraverser
       */
      NVSG_API SearchTraverser();

      /*! \brief Returns paths for all objects found.
       * \return The function returns for each object found during traversal the full path from the starting node
       * down tho the found object. If you're only interested in the found objects itself, you should consider to 
       * use getResults instead. 
       * \sa getResults 
       */
      NVSG_API const std::vector<const nvsg::Path*> & getPaths();

      /*! \brief Returns all objects found.
       * \return The function returns all objects found during traversal.
       */
      NVSG_API const std::vector<nvsg::ObjectWeakPtr> & getResults();

      /*! \brief Sets a class name as serach criterion.
       * \param name Specifies the class name to use as search criterion. 
       * \remarks The class name needs to be fully qualified, like "class nvsg::LightSource", 
       * for a subsequent search to work as expected. Just using "LightSource" in this case would 
       * not give the expected results. By default the class name is set to an empty string, which
       * would not yield any usable results if you intend to search objects by class name.
       * Also by default the search does only consider objects of the explicit class type specified
       * by the class name. With setBaseClassSearch you can configure the search to also consider
       * objects of types derived from the class specified by class name. If you, in addition to a 
       * class name, also specified an object name as search criterion, only objects that match both, 
       * the class name and the object name will be returned as search results. 
       * \sa setObjectName, setBaseClassSearch */
      NVSG_API void setClassName( const std::string& name );

      /*! \brief Returns the class name set as search criterion.
       * \return The function retuns the class name last set using setClassName or an empty string 
       * if no class name was specified as search criterion before.
       * \sa setClassName
       */
      NVSG_API const std::string& getClassName() const;

      /*! \brief Sets an object name as search criterion.
       * \param objectName Specifies the object name to use as search criterion.
       * \remarks If you specify an object name as search criterion, that object name is
       * compared to the corresponding search candidate's object name assigned to it by means of
       * the Object::setName member function. If you, in addition to an object name, also specified
       * a class name as search criterion, only objects that match both, the class name and the object name
       * will be returned as search results. 
       * \sa Object::setName, setClassName 
       */
      NVSG_API void setObjectName(const std::string& objectName);

      /*! \brief Returns the object name set as search criterion.
       * \return The function retuns the object name last set using setObjectName or an empty string 
       * if no object name was specified as search criterion before. 
       * \sa setObjectName
       */
      NVSG_API const std::string& getObjectName() const;

      /*! \brief Specifies a particular object to search for.
       * \param ptr Object to search for.
       * \remarks If a particular object is used as search criterion, other search criteria like
       * class name or object name will be ignored.
       */
      NVSG_API void setObjectPointer( const nvsg::ObjectWeakPtr & ptr );

      /*! \brief Returns the object to search for.
       * \return The function returns the object last set using setObjectPtr. 
       */
      NVSG_API nvsg::ObjectWeakPtr getObjectPointer() const;

      /*! Configures the base class search criterion option.
       * \param enable If true, the search will also consider objects of types derived from the class
       * specified through setClassName. If false, only objects of the explicit type specified through
       * setClassName will be considered. By default, this option is set to false.
       * \sa setClassName
       */
      NVSG_API void setBaseClassSearch( bool enable );

      /*! \brief Returns the enable state of the base class search option.
       * \return The function returns the enable state of the base class search option
       * last set using setBaseClassSearch.
       * \sa setBaseClassSearch
       */
      NVSG_API bool getBaseClassSearch() const ;

      REFLECTION_INFO_API( NVSG_API, SearchTraverser );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( ClassName );
          NVSG_API DECLARE_STATIC_PROPERTY( ObjectName );
          NVSG_API DECLARE_STATIC_PROPERTY( BaseClassSearch );
      END_DECLARE_STATIC_PROPERTIES
    protected:
      //! Destructor
      NVSG_API virtual ~SearchTraverser(void);
      
      //! Set up the search and traverse the scene.
      NVSG_API virtual void  doApply( const nvsg::NodeSharedPtr & root );

      /*! \brief Add an Object to the list of found items.
       *  \param obj A pointer to the read-locked object to add */
      NVSG_API void addItem( const nvsg::Object* obj );

      //! Routine to handle a \link nvsg::Billboard Billboard \endlink node while traversing the scene graph.
      NVSG_API virtual void handleBillboard(
        const nvsg::Billboard * p //!< Points to the currently visited Billboard object.
        );

      //! Routine to handle a \link nvsg::GeoNode GeoNode \endlink while traversing the scene graph.
      NVSG_API virtual void handleGeoNode(
        const nvsg::GeoNode * p //!< Points to the currently visited GeoNode object.
      );

      //! Routine to handle a \link nvsg::Group Group \endlink node while traversing the scene graph.
      NVSG_API virtual void handleGroup(
        const nvsg::Group * p //!< Points to the currently visited Group object.
      );
      
      //! Routine to handle a \link nvsg::FlipbookAnimation FlipbookAnimation \endlink node while traversing the scene graph.
      NVSG_API virtual void handleFlipbookAnimation(
        const nvsg::FlipbookAnimation * p //!< Points to the currently visited FlipbookAnimation object.
        );

      //! Routine to handle a \link nvsg::LOD LOD \endlink (Level Of Detail) node while traversing the scene graph.
      NVSG_API virtual void handleLOD(
        const nvsg::LOD * p //!< Points to the currently visited LOD object.
      );
                                          
      //! Routine to handle a \link nvsg::Switch Switch \endlink node while traversing the scene graph.
      NVSG_API virtual void handleSwitch(
        const nvsg::Switch * p //!< Points to the currently visited Switch object.
      );
                                          
      //! Routine to handle a \link nvsg::Transform Transform \endlink node while traversing the scene graph.
      NVSG_API virtual void handleTransform(
        const nvsg::Transform * p //!< Points to the currently visited Transform object.
      );
      
      //! Routine to handle a \link nvsg::AnimatedTransform AnimatedTransform \endlink node while traversing the scene graph.
      NVSG_API virtual void handleAnimatedTransform(
        const nvsg::AnimatedTransform * p //!< Points to the currently visited AnimatedTransform object.
      );
      
      //! Routine to handle a \link nvsg::DirectedLight DirectedLight \endlink node while traversing the scene graph.
      NVSG_API virtual void handleDirectedLight(
        const nvsg::DirectedLight * p //!< Points to the currently visited DirectedLight object.
      );
      
      //! Routine to handle a \link nvsg::PointLight PointLight \endlink node while traversing the scene graph.
      NVSG_API virtual void handlePointLight(
        const nvsg::PointLight * p //!< Points to the currently visited PointLight object.
      );

      //! Routine to handle a \link nvsg::SpotLight SpotLight \endlink node while traversing the scene graph.
      NVSG_API virtual void handleSpotLight(
        const nvsg::SpotLight * p //!< Points to the currently visited SpotLight object.
      );

      //! Routine to handle a \link nvsg::Lines Lines \endlink object while traversing the scene graph.
      NVSG_API virtual void handleLines(
        const nvsg::Lines * p     //!< Points to the currently visited Lines object.
        );

      //! Routine to handle a \link nvsg::LineStrips LineStrips \endlink object while traversing the scene graph.
      NVSG_API virtual void handleLineStrips(
        const nvsg::LineStrips * p //!< Points to the currently visited LineStrips object.
        );

      //! Routine to handle a \link nvsg::Points Points \endlink object while traversing the scene graph.
      NVSG_API virtual void handlePoints(
        const nvsg::Points * p     //!< Points to the currently visited Points object.
        );

      //! Routine to handle a QuadMeshes object while traversing the scene graph.
      NVSG_API virtual void handleQuadMeshes(
        const nvsg::QuadMeshes * p //!< Points to the currently visited QuadMeshes object.
        );

      //! Routine to handle a \link nvsg::QuadPatches QuadPatches \endlink object while traversing the scene graph.
      NVSG_API virtual void handleQuadPatches(
        const nvsg::QuadPatches * p //!< Points to the currently visited QuadPatches object.
      );

      //! Routine to handle a \link nvsg::QuadPatches4x4 QuadPatches4x4 \endlink object while traversing the scene graph.
      NVSG_API virtual void handleQuadPatches4x4(
        const nvsg::QuadPatches4x4 * p //!< Points to the currently visited QuadPatches4x4 object.
      );

      //! Routine to handle a \link nvsg::Quads Quads \endlink object while traversing the scene graph.
      NVSG_API virtual void handleQuads(
        const nvsg::Quads * p //!< Points to the currently visited Quads object.
      );

      //! Routine to handle a \link nvsg::QuadStrips QuadStrips \endlink object while traversing the scene graph.
      NVSG_API virtual void handleQuadStrips(
        const nvsg::QuadStrips * p //!< Points to the currently visited QuadStrips object.
      );

      //! Routine to handle a \link nvsg::RectPatches RectPatches \endlink object while traversing the scene graph.
      NVSG_API virtual void handleRectPatches(
        const nvsg::RectPatches * p //!< Points to the currently visited RectPatches object.
      );

      //! Routine to handle a \link nvsg::Triangles Triangles \endlink object while traversing the scene graph.
      NVSG_API virtual void handleTriangles(
        const nvsg::Triangles * p //!< Points to the currently visited Triangles object.
      );

      //! Routine to handle a \link nvsg::TriFans TriFans \endlink object while traversing the scene graph.
      NVSG_API virtual void handleTriFans(
        const nvsg::TriFans * p //!< Points to the currently visited TriFans object.
      );

      //! Routine to handle a \link nvsg::TriPatches TriPatches \endlink object while traversing the scene graph.
      NVSG_API virtual void handleTriPatches(
        const nvsg::TriPatches * p //!< Points to the currently visited TriPatches object.
      );

      //! Routine to handle a \link nvsg::TriPatches4 TriPatches4 \endlink object while traversing the scene graph.
      NVSG_API virtual void handleTriPatches4(
        const nvsg::TriPatches4 * p //!< Points to the currently visited TriPatches4 object.
      );

      //! Routine to handle a \link nvsg::TriStrips TriStrips \endlink object while traversing the scene graph.
      NVSG_API virtual void handleTriStrips(
        const nvsg::TriStrips * p //!< Points to the currently visited TriStrips object.
      );

      //! Routine to handle a \link nvsg::Primitives Primitives \endlink object while traversing the scene graph.
      NVSG_API virtual void handlePrimitive(
        const nvsg::Primitive * p //!< Points to the currently visited Primitive object.
      );

      //! Routine to handle a \link nvsg::StateSet StateSet \endlink object while traversing the scene graph.
      NVSG_API virtual void handleStateSet(
        const nvsg::StateSet * p //!< Points to the currently visited StateSet object.
      );

      //! Routine to handle a \link nvsg::StateVariant StateVariant \endlink object while traversing the scene graph.
      NVSG_API virtual void handleStateVariant(
        const nvsg::StateVariant * p //!< Points to the currently visited StateVariant object.
        );

      //! Routine to handle a \link nvsg::StatePass StatePass \endlink object while traversing the scene graph.
      NVSG_API virtual void handleStatePass(
        const nvsg::StatePass * p //!< Points to the currently visited StatePass object.
        );

      //! Routine to handle a \link nvsg::BlendAttribute BlendAttribute \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleBlendAttribute(
        const nvsg::BlendAttribute * p //!< Points to the currently visited BlendAttribute object.
        );

      //! Routine to handle a \link nvsg::CgFx CgFx \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleCgFx(
        const nvsg::CgFx * p //!< Points to the currently visited CgFx object.
      );

      //! Routine to handle a \link nvsg::RTFx RTFx \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleRTFx(
        const nvsg::RTFx * p //!< Points to the currently visited RTFx object.
      );

      //! Routine to handle a \link nvsg::RTFxProgram RTFxProgram \endlink while traversing the scene graph.
      NVSG_API virtual void handleRTFxProgram(
        const nvsg::RTFxProgram * p //!< Points to the currently visited RTFxProgram object.
      );

      //! Routine to handle a \link nvsg::RTBufferAttribute RTBufferAttribute \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleRTBufferAttribute(
        const nvsg::RTBufferAttribute * p //!< Points to the currently visited RTBufferAttribute object.
      );

      //! Routine to handle a \link nvsg::DepthAttribute DepthAttribute \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleDepthAttribute(
        const nvsg::DepthAttribute * p //!< Points to the currently visited DepthAttribute object.
      );

      //! Routine to handle a \link nvsg::StencilAttribute StencilAttribute \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleStencilAttribute(
        const nvsg::StencilAttribute * p //!< Points to the currently visited StencilAttribute object.
      );

      //! Routine to handle a \link nvsg::FaceAttribute FaceAttribute \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleFaceAttribute(
        const nvsg::FaceAttribute * p //!< Points to the currently visited FaceAttribute object.
      );

      //! Routine to handle a \link nvsg::LineAttribute LineAttribute \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleLineAttribute(
        const nvsg::LineAttribute * p //!< Points to the currently visited LineAttribute object.
        );

      //! Routine to handle a \link nvsg::Material Material \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleMaterial(
        const nvsg::Material * p //!< Points to the currently visited Material object.
      );

      //! Routine to handle a \link nvsg::PointAttribute PointAttribute \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handlePointAttribute(
        const nvsg::PointAttribute * p //!< Points to the currently visited PointAttribute object.
        );

      //! Routine to handle a \link nvsg::TextureAttribute TextureAttribute \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleTextureAttribute(
        const nvsg::TextureAttribute * p //!< Points to the currently visited TextureAttribute object.
      );

      //! Routine to handle an \link nvsg::UnlitColorAttribute UnlitColorAttribute \endlink attribute while traversing the scene graph.
      NVSG_API virtual void handleUnlitColorAttribute(
        const nvsg::UnlitColorAttribute* p //!< Points to the currently visited UnlitColorAttribute object.
      );

      //! Routine to handle a \link nvsg::ParallelCamera ParallelCamera \endlink object while traversing the scene graph.
      NVSG_API virtual void handleParallelCamera(
        const nvsg::ParallelCamera * p //!< Points to the currently visited ParallelCamera object.
      );

      //! Routine to handle a \link nvsg::PerspectiveCamera PerspectiveCamera \endlink object while traversing the scene graph.
      NVSG_API virtual void handlePerspectiveCamera(
        const nvsg::PerspectiveCamera * p //!< Points to the currently visited PerspectiveCamera object.
      );

      //! Routine to handle a \link nvsg::MatrixCamera MatrixCamera \endlink object while traversing the scene graph.
      NVSG_API virtual void handleMatrixCamera(
        const nvsg::MatrixCamera * p //!< Points to the currently visited MatrixCamera object.
      );

      //! Routine to handle a \link nvsg::Animation FramedAnimationDescription<unsigned int> \endlink object while traversing the scene graph.
      NVSG_API virtual void handleFramedIndexAnimationDescription(
        const nvsg::FramedAnimationDescription<unsigned int> * p //!< Points to the currently visited Animation object.
        );

      //! Routine to handle a \link nvsg::Animation FramedAnimationDescription<Trafo> \endlink object while traversing the scene graph.
      NVSG_API virtual void handleFramedTrafoAnimationDescription(
        const nvsg::FramedAnimationDescription<nvmath::Trafo> * p //!< Points to the currently visited Animation object.
      );

      //! Routine to handle a \link nvsg::Animation FramedAnimationDescription<VertexAttribute> \endlink object while traversing the scene graph.
      NVSG_API virtual void handleFramedVertexAttributeAnimationDescription( const nvsg::FramedAnimationDescription<nvsg::VertexAttribute> * p );

      //! Routine to handle a \link nvsg::Animation LinearInterpolatedAnimationDescription<Trafo> \endlink object while traversing the scene graph.
      NVSG_API virtual void handleLinearInterpolatedTrafoAnimationDescription(
        const nvsg::LinearInterpolatedAnimationDescription<nvmath::Trafo> * p //!< Points to the currently visited Animation object.
      );

      //! Routine to handle a \link nvsg::Animation LinearInterpolatedAnimationDescription<VertexAttribute> \endlink object while traversing the scene graph.
      NVSG_API virtual void handleLinearInterpolatedVertexAttributeAnimationDescription( const nvsg::LinearInterpolatedAnimationDescription<nvsg::VertexAttribute> * p );

      //! Routine to handle a \link nvsg::VertexAttributeSet VertexAttributeSet \endlink object while traversing the scene graph.
      NVSG_API virtual void handleVertexAttributeSet( const nvsg::VertexAttributeSet *vas );

      //! Routine to handle a \link nvsg::AnimatedVertexAttributeSet AnimatedVertexAttributeSet \endlink object while traversing the scene graph.
      NVSG_API virtual void handleAnimatedVertexAttributeSet( const nvsg::AnimatedVertexAttributeSet * avas );

      //! Routine to handle a \link nvsg::Skin Skin \endlink object while traversing the scene graph.
      NVSG_API virtual void handleSkin( const nvsg::Skin * skin );

      //! Routine to handle a \link nvsg::Shape Shape \endlink object while traversing the scene graph.
      NVSG_API virtual void handleShape( const nvsg::Shape * shape );

      //! Routine to handle a \link nvsg::TextureAttributeItem TextureAttributeItem \endlink object while traversing the scene graph.
      NVSG_API virtual void handleTextureAttributeItem( const nvsg::TextureAttributeItem * texAttribItem );

      //! Routine to handle a \link nvsg::AlphaTestAttribute AlphaTestAttribute \endlink object while traversing the scene graph.
      NVSG_API virtual void handleAlphaTestAttribute( const nvsg::AlphaTestAttribute * alphaTestAttrib );

      //! Routine to handle a \link nvsg::LightingAttribute LightingAttribute \endlink object while traversing the scene graph.
      NVSG_API virtual void handleLightingAttribute( const nvsg::LightingAttribute * laAttrib );

      //! Routine to handle a \link nvsg::MapArray MapArray \endlink object while traversing the scene graph.
      NVSG_API virtual void handleMapArray( const nvsg::MapArray * mapArray );

      //! Routine to handle a \link nvsg::MapElement MapElement \endlink object while traversing the scene graph.
      NVSG_API virtual void handleMapElement( const nvsg::MapElement * mapElement );

      //! Routine to handle a \link nvsg::MapMatrix MapMatrix \endlink object while traversing the scene graph.
      NVSG_API virtual void handleMapMatrix( const nvsg::MapMatrix * mapMatrix );

      //! Routine to handle a \link nvsg::MapList MapList \endlink object while traversing the scene graph.
      NVSG_API virtual void handleMapList( const nvsg::MapList * mapList );

      //! Routine to handle a \link nvsg::Animation Animation \endlink object while traversing the scene graph.
      NVSG_API virtual void handleIndexAnimation( const nvsg::Animation<unsigned int> * anim );

      //! Routine to handle a \link nvsg::Animation Animation \endlink object while traversing the scene graph.
      NVSG_API virtual void handleTrafoAnimation( const nvsg::Animation<nvmath::Trafo> * anim );

      //! Routine to handle a \link nvsg::Animation Animation \endlink object while traversing the scene graph.
      NVSG_API virtual void handleVertexAttributeAnimation( const nvsg::Animation<nvsg::VertexAttribute> * anim );

      //! Search through an Animation<Trafo> object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Animation<nvmath::Trafo> *p //!<  Animation to search through
                                    );

      /*! \brief Search through an Animation<unsigned int> object.
       *  \param p Animation to search through.
       *  \remarks This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Animation<unsigned int> *p );

      //! Search through an Animation<VertexAttribute> object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Animation<nvsg::VertexAttribute> *p );

      /*! \brief Search through an AnimationState object.
       *  \param p AnimationState to search through.
       *  \remarks This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::AnimationState *p );

      //! Search through a Camera object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Camera *p             //!<  Camera to search through
                                  );

      //! Search through a Drawable object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Drawable *p           //!<  Drawable to search through
                                  );

      //! Search through a Group object.
      /** This function is called when Group is searched through or base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Group *p                //!<  Group to search through
                                  );

      //! Search through an IndependentPrimitiveSet.
      /** This function is called when a base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::IndependentPrimitiveSet * p    //!< IndependentPrimitiveSet to search through
                                  );

      //! Search through an KeyFramedAnimationDescription<Trafo>.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::KeyFramedAnimationDescription<nvmath::Trafo> *p  //!<  KeyFramedAnimationDescription to search through
                                  );

      //! Search through an KeyFramedAnimationDescription<VertexAttribute>.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::KeyFramedAnimationDescription<nvsg::VertexAttribute> *p );

      //! Search through a LightSource object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::LightSource *p        //!<  LightSource to search through
                                  );

      //! Search through a MeshedPrimitiveSet.
      /** This function is called when a base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::MeshedPrimitiveSet * p    //!< MeshedPrimitiveSet to search through
        );

      //! Search through a Node object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Node *p               //!<  Node to search through
                                  );

      //! Search through an Object object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Object *p             //!<  Object to search through
                                  );

      //! Search through a PatchesBase object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::PatchesBase *p        //!<  PatchesBase to search through
                                  );

      //! Search through a PrimitiveSet object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::PrimitiveSet *p          //!<  PrimitiveSet to search through
                                  );

      //! Search through a StateAttribute object.
      /** This function is called when base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::StateAttribute *p     //!<  StateAttribute to search through
                                  );

      //! Search through an StrippedPrimitiveSet.
      /** This function is called when a base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::StrippedPrimitiveSet * p    //!< StrippedPrimitiveSet to search through
                                  );

      //! Search through a Transform object.
      /** This function is called when Transform is searched or a base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Transform *p            //!<  Transform to search through
                                  );

      //! Search through a Triangles object.
      /** This function is called when a Triangles is searched or a base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::Triangles *p            //!<  Triangles to search through
                                  );

      //! Search through a VertexAttributeSet object.
      /** This function is called when a VertexAttributeSet is searched or a base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::VertexAttributeSet * p );

      //! Search through an OwnedObject<PrimitiveSet> object.
      /** This function is called when an OwnedObject<PrimitiveSet> is searched or a base class searching is enabled. */
      NVSG_API virtual void search( const nvsg::OwnedObject<nvsg::PrimitiveSet> * p );

      
    private:
      bool searchObject(const nvsg::Object* p, const std::string &classNameToHandle);

      nvutil::SmartPtr<nvsg::Path>                  m_currentPath;
      std::string                                   m_className;
      std::set<const nvsg::Object *>                m_foundObjects;
      std::string                                   m_objectName;
      nvsg::ObjectWeakPtr                           m_objectPointer;
      std::vector<const nvsg::Path*>                m_paths;
      std::vector<nvsg::ObjectWeakPtr>              m_results;
      bool                                          m_searchBaseClass;
      std::vector<nvutil::SmartPtr<nvsg::Path> >    m_smartPaths;
  };

  inline void SearchTraverser::setClassName( const std::string& name )
  {
    if ( m_className != name )
    {
      m_className = name;
      notify( PID_ClassName );
    }
  }

  inline const std::string& SearchTraverser::getClassName( ) const
  {
    return m_className;
  }

  inline void SearchTraverser::setObjectName(const std::string &objectName)
  {
    if ( m_objectName != objectName )
    {
      m_objectName = objectName;
      notify( PID_ObjectName );
    }
  }

  inline const std::string& SearchTraverser::getObjectName() const
  {
    return m_objectName;
  }

  inline void SearchTraverser::setObjectPointer( const nvsg::ObjectWeakPtr & ptr )
  {
    m_objectPointer = ptr;
  }

  inline nvsg::ObjectWeakPtr SearchTraverser::getObjectPointer( ) const
  {
    return m_objectPointer;
  }

  inline void SearchTraverser::setBaseClassSearch( bool enable )
  {
    if ( m_searchBaseClass != enable )
    {
      m_searchBaseClass = enable;
      notify( PID_BaseClassSearch );
    }
  }

  inline bool SearchTraverser::getBaseClassSearch( ) const
  {
    return m_searchBaseClass;
  }

}

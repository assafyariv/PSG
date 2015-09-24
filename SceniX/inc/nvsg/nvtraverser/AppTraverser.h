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
/** \file */

#include <set>
#include "nvsgcommon.h"
#include "nvsg/ViewState.h"
#include "nvtraverser/ModelViewTraverser.h"
#include "nvtraverser/AppTraverserCallbacks.h"
#include "nvsg/Skin.h"

#include "nvmath/nvmath.h"

namespace nvtraverser {

//! AppTraverser Class
/** This class provides the mechanism to apply changes and update the scene graph. 
  * (The 'apply' step of the application.) Use an AppTraverser derived class as a unique 
  * entry point for manipulating nodes in the scene, such as data from gadgets, 
  * animations, and interactions.
  * \note The current implementation takes care of the animation frames, camera clip planes
  * highlighting, etc...
  * \note Needs a valid ViewState, which in turns holds a valid camera. Call setViewState prior to apply().*/

class AppTraverser : public ExclusiveModelViewTraverser
{
public:
  //! Default constructor.
  NVSG_API AppTraverser();

  enum SkinningProcessorType
  {
    SKINNING_PROCESSOR_NONE,     // no skinning processor
    SKINNING_PROCESSOR_CPU,      // should be supported on all platforms
    SKINNING_PROCESSOR_GPUG80GL, // G80 skinning with geometry programs OGL
    SKINNING_PROCESSOR_CUDA,     // skinning with CUDA
    SKINNING_PROCESSOR_USER      // a user-defined skinning processor
  };

  //
  // A "skinning processor" class to implement skinning for the App Traverser.
  //
  class SkinningProcessor : public nvutil::RCObject
  {
    public:
      //! Default constructor - called before API is initialized!
      NVSG_API SkinningProcessor();

      // must be implemented
      NVSG_API virtual void process( bool requireProcessing ) = 0;
      NVSG_API virtual SkinningProcessorType getType() = 0;

      // note: only called if processor is being switched at runtime
      NVSG_API virtual void shutdown( void ) {}

      NVSG_API virtual void preTraverseScene();
      NVSG_API virtual void postTraverseScene();
      NVSG_API virtual void addJoint( const nvmath::Mat44f & modelToWorld
                                    , const nvmath::Mat44f & worldToModel
                                    , const nvsg::Transform * p );
      NVSG_API virtual void addSkin( const nvsg::PrimitiveSetWeakPtr & skinnedPrimitiveSet );
      NVSG_API virtual void addSkin( const nvsg::PrimitiveWeakPtr & skinnedPrimitive );

      NVSG_API void calculateJointMatrices( const nvsg::SkinSharedPtr & skin
                                          , std::vector<std::pair<nvmath::Mat44f,nvmath::Mat44f> > & jointMatrices );

    protected:
      // avoid stack creation since we inherit from RCObject
      NVSG_API virtual ~SkinningProcessor();

    protected:
      std::vector<nvsg::PrimitiveSetWeakPtr>                                      m_skinnedPrimitiveSets;
      std::vector<nvsg::PrimitiveWeakPtr>                                         m_skinnedPrimitives;
      std::map<nvsg::TransformWeakPtr,std::pair<nvmath::Mat44f,nvmath::Mat44f> >  m_transformToJointMatrices;
  };

protected:
  //!Default destructor.
  NVSG_API virtual ~AppTraverser();

  //! Determines if the AppTraverser really should traverse the tree.
  /** Depending on some internal data, the traversal is recognized to be necessary.
    * The AppTraverser needs to traverse if the ViewState, the Scene, or the animation
    * frame changed. */
  NVSG_API virtual bool needsTraversal( const nvsg::NodeSharedPtr & root ) const;

  //! doApply override
  NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

  // handler routine overrides
  NVSG_API virtual void handleAnimatedTransform( nvsg::AnimatedTransform * at );
  NVSG_API virtual void handleCgFx( nvsg::CgFx *cgfx );
  NVSG_API virtual void handleIndexAnimation( nvsg::Animation<unsigned int> * anim );
  NVSG_API virtual void handleTrafoAnimation( nvsg::Animation<nvmath::Trafo> * anim );
  NVSG_API virtual void handleVertexAttributeAnimation( nvsg::Animation<nvsg::VertexAttribute> * anim );
  NVSG_API virtual void handleGeoNode( nvsg::GeoNode *gnode );
  NVSG_API virtual void handleTransform( nvsg::Transform * p );

  //! Handle a Transform before traversing it.
  /** The information on the current transform is stored, then ModelViewTraverser::preTraverseTransform() is called
    * for further pre traversal processing. */
  NVSG_API virtual bool preTraverseTransform( const nvmath::Trafo *p    //!< Trafo of node to handle pre-traversal
                                            );

  //! Handle a Transform after traversing it.
  /** After calling ModelViewTraverser::postTraverseTransform() for other post traversal processing on that Transform,
  * transformation informations are restored from what was current before traversing that Transform. */
  NVSG_API virtual void postTraverseTransform( const nvmath::Trafo *p   //!< Trafo of node to handle post-traversal
                                             );

  /*! \brief Handle each PrimitiveSet on traversal.
   *  \param pset A pointer to the write-locked PrimitiveSet currently traversed.
   *  \remarks For each PrimitiveSet (like Triangles, Quads,...) this function is called. If \a pset
   *  holds a Skin and a SkinningProcessor is currently active, \a pset is added as a skinned
   *  PrimitiveSet to that SkinningProcessor for later usage. 
   *  \sa SkinningProcessor */
  NVSG_API virtual void traversePrimitiveSet( nvsg::PrimitiveSet * pset );

  /*! \brief Handle each Primitive on traversal.
   *  \param pset A pointer to the write-locked PrimitiveSet currently traversed.
   *  \remarks For each PrimitiveSet (like Triangles, Quads,...) this function is called. If \a pset
   *  holds a Skin and a SkinningProcessor is currently active, \a pset is added as a skinned
   *  PrimitiveSet to that SkinningProcessor for later usage. 
   *  \sa SkinningProcessor */
  NVSG_API virtual void traversePrimitive( nvsg::Primitive * prim );

  /*! \brief Handle each Object after traversal
   *  \param object A pointer to the read-locked object that was just traversed.
   *  \remarks Each app callbacks of the \c object is called.
   *  \sa Callback */
  NVSG_API virtual void postTraverseObject( const nvsg::Object * object );

  // data accessible for derived classes
  bool          m_bApplyingCgFx;  //!< Flag that signals if a CgFX effect needs to be handled (tangentspace calculation).
  unsigned int  m_animationFrame; //!< Current animation frame.

public:

  //! Check if a particular skinning processor is supported.
  /** Check current hardware to see if a particular skinning processor is
   * supported.  
   * \remarks Note that most "processors" require a current Rendering ctext for
   * their detection routines to function correctly.  Therefore, in order to
   * check whether a particular skinning processor is supported, the application
   * should retrieve the AppTraverser from the RenderArea, once the RenderArea
   * has been created, and then call this routine on the skinning processor in
   * question.  This will ensure that an appropriate context is available.
   * \sa RenderArea::getAppTraverser
   * */
  NVSG_API bool isSkinningProcessorSupported( SkinningProcessorType type );

  //! Set the skinning processor, by type.
  /** \note Always deletes the old processor before setting a new one.
   * \return \c true if processor was supported, \c false otherwise
   */
  NVSG_API bool setSkinningProcessor( SkinningProcessorType type ); 

  //! Get a pointer to the current skinning processor.
  NVSG_API SkinningProcessor * getSkinningProcessor();

  //! Set the skinning processor by pointer.
  /** This can be used to set custom skinning processors. 
   * \param a The new skinning processor to use, or NULL.  If the parameter
   *          is NULL, then no skinning processor will be used, and therefore
   *          skinning will not be performed.
   * \remarks The skinning processors are reference counted in this routine.
   * */ 
  NVSG_API void setSkinningProcessor( SkinningProcessor * a );

  /*! \brief Force traversal on the next frame.
   *  \remarks The AppTraverser tries to traverse the scene as seldom as possible. In some
   *  circumstances it might be necessary to force a traversal. That can be done with this function.
   *  \sa needsTraversal */
  NVSG_API void forceTraversal();

  //////////////////////////////////////////////////////

private:

  // Some data to cache so we can decide if we really have to apply the animation data. 
  // This does not help to prevent from unneeded traversal steps since we can't 
  // decide for derived classes. 
  nvsg::ViewStateWeakPtr                  m_pLastViewState;
  nvsg::NodeWeakPtr                       m_pLastRoot;
  unsigned int                            m_animationTick;
  int                                     m_animationDelta;
  unsigned int                            m_lastAnimationTick;
  int                                     m_lastAnimationDelta;
  bool                                    m_bApplyAnimationKey;
  std::stack<nvmath::Vec3f>               m_currentScaling;
  nvutil::SmartPtr<SkinningProcessor>     m_skinningProcessor;
  bool                                    m_forcedTraversal;
  nvsg::Transform                       * m_currentTransform;
  std::set<nvsg::AnimationStateWeakPtr>   m_animations;
};

inline void AppTraverser::forceTraversal()
{
  m_forcedTraversal = true;
}

  struct AppTraverserCallbackData
  {
    const nvutil::TransformStack * transformStack;
  };

} // namespace nvtraverser

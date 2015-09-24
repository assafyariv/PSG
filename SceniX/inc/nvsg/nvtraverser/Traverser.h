// Copyright NVIDIA Corporation 2002-2010
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
#include "nvsg/CoreTypes.h" // forward declarations of core type
#include "nvsg/GeoNode.h"
#include "nvsg/Object.h" // ObjectCode defines
#include "nvsg/Group.h"
#include "nvsg/Path.h"  // for traversal id
#include "nvsg/ViewState.h"
#include "nvutil/BitMask.h"
#include "nvutil/Conversion.h"
#include "nvutil/MemFunTbl.h"
#include "nvutil/ProblemLog.h"
#include "nvutil/RCObject.h" // base class definition
#include "nvutil/Reflection.h"
#include <stack>
#include <memory>

namespace nvsg
{
  template<typename T> class Animation;
  template<typename T> class FramedAnimationDescription;
  template<typename T> class LinearInterpolatedAnimationDescription;
}

namespace nvtraverser
{
  /*! \brief Virtual base class providing the common interface part of ExclusiveTraverser and SharedTraverser.
   *  \remarks A traverser serves as a link between a scene graph and a defined operation to be
   *  performed on the scene graph. That is, according to a certain scene graph's hierarchy, a
   *  traverser iterates over all scene graph components and performs a defined operation for
   *  each by means of an overloadable handler routine.
   *  The Traverser class offers base functionality for all traversers. There are two traversers
   *  derived from Traverser: SharedTraverser and ExclusiveTraverser, that provide read-only
   *  access and read-write access, respectively, to the elements of the scene graph.
   *  \sa ExclusiveTraverser, SharedTraverser */
  class Traverser : public nvutil::RCObject, public nvutil::Reflection
  {
    public:
      /*! \brief Set a ViewState to be used with this Traverser.
       *  \param viewState The ViewState to be used.
       *  \remarks If \a viewState is a different pointer than the current ViewState pointer, its
       *  reference count is incremented and the reference count of a previously set ViewState is
       *  decremented. Any caches for the ViewState are marked as invalid.
       *  \note The ViewState's TraversalMask will be used in conjunction with any TraversalMaskOverride to direct
       *  scene traversal.  See setTraversalMask for more info.
       *  \sa setTraversalMaskOverride, setTraversalMask, Object::setTraversalMask */
      NVSG_API virtual void setViewState( const nvsg::ViewStateSharedPtr & viewState );

      /*! \brief Set the TraversalMask to be used with this Traverser.
       *  \param mask The mask to be used.
       *  \remarks This method provides a way to set the traversal mask for this Traverser if no ViewState is 
       *  specified.  If a ViewState is specified, the TraversalMask in the ViewState will be used instead.
       *  \note The TraversalMask is used in conjuction with the OverrideTraversalMask and every scene graph node's 
       *  TraversalMask to determine whether the node (and therefore possibly the entire subgraph) is traversed.  
       *  The traverser's override traversal mask is OR'd with the node's traversal mask and that result is ANDed with 
       *  the traversal mask.  If the result is nonzero then the node is traversed, otherwise it is ignored.  IE:
       *  ( ( (Traverser::TraversalMaskOverride | Object::TraversalMask) & ViewState::TraversalMask ) != 0 ) the node is
       *  traversed.
       *  \note Setting the TraversalMask to 0 will cause no nodes to be traversed.  Setting the TraversalMask to ~0 and
       *  the TraversalMaskOverride to ~0 will cause all nodes to be traversed regardless of the Object::TraversalMask.
       *  \note The default traversal mask is always ~0 so that all nodes are traversed.
       *  \sa getTraversalMask, getTraversalMaskOverride, ViewState::getTraversalMask, Object::getTraversalMask */
      NVSG_API virtual void setTraversalMask( unsigned int mask );

      /*! \brief Get the current TraversalMask value
       *  \return mask The mask to be used.
       *  \sa setTraversalMask */
      NVSG_API virtual unsigned int getTraversalMask() const;

      /*! \brief Set an override TraversalMask to be used with this Traverser.
       *  \param mask The mask to be used.
       *  \remarks This method provides a way to override the Object::TraversalMask that is used when determining
       *  whether to traverse scene graph objects.  This mask is OR'd with each nvsg::Object's TraversalMask before
       *  being ANDed with the Traverser or ViewState's TraversalMask.  See Traverser::setTraversalMask for more info.
       *  \note The default TraversalMaskOverride is 0 so that it does not affect traversal.
       *  \sa getTraversalMaskOverride, ViewState::setTraversalMask, Object::setTraversalMask */
      NVSG_API virtual void setTraversalMaskOverride( unsigned int mask );

      /*! \brief Get the current TraversalMask override
       *  \return mask The mask in use.
       *  \sa setTraversalMask */
      NVSG_API virtual unsigned int getTraversalMaskOverride() const;

      /*! \brief Start traversal of the scene attached to the current ViewState
       *  \remarks If preApply is successful, the Scene and the ViewState are locked, the
       *  traversal is executed by calling doApply, the Scene and the ViewState are unlocked
       *  again, and postApply is called with \a root.
       *  \sa apply */
      NVSG_API void apply( );

      /*! \brief Start traversal from a specified node.
       *  \param root The Node to start traversal with.
       *  \remarks If preApply is successful, the Scene and the ViewState are locked, the
       *  traversal is executed by calling doApply, the Scene and the ViewState are unlocked
       *  again, and postApply is called with \a root.
       *  \note Some traverser need a valid ViewState object which has to be set via setViewState.
       *  \sa apply */
      NVSG_API void apply( const nvsg::NodeSharedPtr & root );

      /*! \brief Start the traversal of a Scene.
       *  \param scene The Scene to traverse.
       *  \remarks This convenience function creates a temporary ViewState which is either a clone
                   of an ViewState set before or a newly created ViewState. Afterwards the \a scene
                   is attached to the ViewState and traversal starts. After traversal the orignal
                   ViewState will be restored.
       *  \sa apply */
      NVSG_API void apply( const nvsg::SceneSharedPtr & scene );

      /*! \brief Start the traversal of the scene attached to the ViewState
       *  \param scene A smart pointer to the ViewState containing a scene to traverse
       *  \remarks This convenience function temporary changed the ViewState of this traverser.
       *  \sa apply */
      NVSG_API void apply( const nvsg::ViewStateSharedPtr &viewstate );

      /*! \brief Immediately interrupt the current traversal.
       *  \remarks After this function has been called, no more Nodes are visited in the current
       *  traversal. The traversal point is back-tracked to the root without initiating any new
       *  actions.
       *  \note This function is especially useful in a multithreaded environment, where one
       *  thread traverses a Scene while an other thread is allowed to interrupt the traversal
       *  under some circumstances.
       *  \sa wasTraversalInterrupted */
      NVSG_API void interruptTraversal();

      /*! \brief Check for traversal interruption.
       *  \return \c true if the traversal was interrupted, otherwise \c false.
       *  \note This function is especially useful if an operation on a Scene is a complex one,
       *  that needs multiple traversals. If one of those traversals was interrupted, the
       *  following traversals should be omitted.
       *  \sa interruptTraversal */
      NVSG_API bool wasTraversalInterrupted();

    protected:
      /*! \brief Default constructor.
       *  \note The constructor is protected, and hence, a Traverser object cannot be
       *  instantiated directly, but only by deriving from Traverser. */
      NVSG_API Traverser();

      /*! \brief Protected destructor to prevent instantiation. */
      NVSG_API virtual ~Traverser();

      /*! \brief Interface for locking the ViewState.
       *  \remarks This function is called from the framework when the ViewState set by
       *  setViewState is to be locked.
       *  \sa setViewState, unlockViewState */
      NVSG_API virtual void lockViewState() = 0;

      /*! \brief Interface for unlocking the ViewState.
       *  \remarks This function is called from the framework when the ViewState set by
       *  setViewState is to be unlocked. 
       *  \sa setViewState, lockViewState */
      NVSG_API virtual void unlockViewState() = 0;

      /*! \brief Interface for initiating the traversal.
       *  \param root The Node to start traversal at.
       *  \remarks This function is called from the framework after the Scene and the ViewState,
       *  if set, are locked. The actual traversal is assumed to be done here.
       *  \sa preApply, postApply, lockViewState, unlockViewState */
      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root ) = 0;

      /*! \brief Interface for performing the object traversal.
       *  \param p The Object to traverse.
       *  \remarks This function is called by the framework to really traverse an object.
       *  \sa traverseObject */
      NVSG_API virtual void doTraverseObject( const nvsg::ObjectSharedPtr & p ) = 0;

      /*! \brief Interface for performing traversal of a Drawable.
       *  \param p The Drawable to traverse.
       *  \remarks This function is called by the framework to traverse a Drawable. */
      NVSG_API virtual void doTraverseDrawable( const nvsg::DrawableSharedPtr & p ) = 0;

      /*! \brief Start traversing an Object.
       *  \param obj The Object to traverse.
       *  \remarks If the traversal was interrupted, the function immediately returns. Otherwise
       *  the virtual function doTraverseObject is called.
       *  \note The behavior is undefined if \a obj points to an invalid location.
       *  \sa doTraverseObject */
      NVSG_API void traverseObject( const nvsg::ObjectSharedPtr & obj );

      /*! \brief Notifier called from the framework directly before traversal starts.
       *  \param root The Node to be traversed.
       *  \return The default implementation always return \c true in order to continue traversal
       *  of \a root to proceed.
       *  \remarks Custom traversers might override this notifier to process some pre-traversal
       *  work. A customized preApply notifier should return \c true to force the traversal of
       *  the specified root node to proceed. Returning \c false prevents the specified root node
       *  from being traversed.\n
       *  At the time of call, a custom override can rely on the optional Scene and the optional 
       *  ViewState both _not_ being locked for read or write access by the calling thread. 
       *  \note If the function returns \c false, traversal of \a root is skipped.
       *  \note Not calling the base implementation might result in undefined runtime behavior.
       *  \par Example:
       *  \code
       *    bool MyTraverser::preApply( const nvsg::NodeSharedPtr & root )
       *    {
       *      //  Do some pre-traversal bookholding or whatever
       *      //  ...
       *
       *      //  call the base implementation and return
       *      return( Traverser::preApply( root ) );
       *    }
       *  \endcode
       *  \sa postApply */
      NVSG_API virtual bool preApply( const nvsg::NodeSharedPtr & root );

      /*! \brief Notifier called from the framework directly after traversal.
       *  \param root The Node that was just traversed.
       *  \remarks Custom traversers might override this notifier to process some post-traversal
       *  work. It is strongly recommended for custom overrides to call the base implementation
       *  in their code.\n
       *  At the time of call, a custom override can rely on the optional Scene and the optional 
       *  ViewState both _not_ being locked for read or write access by the calling thread. 
       *  \note Not calling the base implementation might result in undefined runtime behavior.
       *  \sa preApply */
      NVSG_API virtual void postApply( const nvsg::NodeSharedPtr & root );

      /*! \brief Notifier called from the framework directly before a Group is traversed.
       *  \param grp A pointer to the constant Group to traverse.
       *  \return The default implementation always returns \c true, in order to continue
       *  traversal.
       *  \remarks Custom traversers can override this function for any tasks that need to be
       *  done directly before the Group \a grp is traversed.
       *  \note If the function returns \c false, traversal of \a grp is skipped.
       *  \note Not calling the base implementation might result in undefined runtime behavior.
       *  \sa postTraverseGroup */
      NVSG_API virtual bool preTraverseGroup( const nvsg::Group * grp );

      /*! \brief Notifier called from the framework directly after a Group has been traversed.
       *  \param grp A pointer to the constant Group that was just traversed.
       *  \remarks Custom traversers can override this function for any tasks that need to be
       *  done directly after the Group \a grp has been traversed.
       *  \note Not calling the base implementation might result in undefined runtime behavior.
       *  \sa preTraverseGroup */
      NVSG_API virtual void postTraverseGroup( const nvsg::Group * grp );

      /*! \brief Notifier called from the framework directly before the Drawable objects
       *  associated with one StateSet are traversed.
       *  \param gnode A pointer to the constant GeoNode currently traversed.
       *  \param ssci Iterator of the StateSet in \a gnode associated with the Drawable objects
       *  that are going to be traversed.
       *  \return The default implementation always returns \c true to continue traversal.
       *  \remarks Custom traversers can override this function for any tasks that need to be
       *  done directly before the Drawable objects of the GeoNode \a gnode associated with the
       *  StateSet of \a gnode at index \a geomIndex are traversed.
       *  \note If the function returns \c false, traversal of the specified Drawable objects is
       *  skipped.
       *  \note Not calling the base implementation might result in undefined runtime behavior.
       *  \sa postTraverseDrawables */
      NVSG_API virtual bool preTraverseDrawables( const nvsg::GeoNode* gnode, nvsg::GeoNode::StateSetConstIterator ssci );

      /*! \brief Notifier called from the framework directly after the Drawable objects
       *  associated with one StateSet have been traversed.
       *  \param gnode A pointer to the constant GeoNode currently traversed.
       *  \param ssci Iterator of the StateSet in \a gnode associated with the Drawable objects
       *  that were just traversed.
       *  \remarks Custom traversers can override this function for any tasks that need to be
       *  done directly after the Drawable objects of the GeoNode \a gnode associated with the
       *  StateSet of \a gnode at index \a geomIndex are traversed.
       *  \note Not calling the base implementation might result in undefined runtime behavior.
       *  \sa preTraverseDrawables */
      NVSG_API virtual void postTraverseDrawables( const nvsg::GeoNode* gnode, nvsg::GeoNode::StateSetConstIterator ssci );

      /*! \brief Signals the beginning of a Drawable traversal.
       * \param drawable Specifies the Drawable for which traversal will be initiated.
       * \return The default implementation always returns \c true, which forces the framework
       * to proceed traversal of the indicated Drawable. 
       * \remarks The function will be called from the framework to signal the beginning of
       * a Drawable traversal. A custom traverser can override this function if a preprocessing
       * is required immediately prior to traverse the indicated Drawable. If an override 
       * implementation returns \c false, it forces the framework to skip traversal of the 
       * indicated Drawable. The postTraverseDrawable notifier will not be called in this case. 
       * \sa postTraverseDrawable */
      NVSG_API virtual bool preTraverseDrawable( const nvsg::Drawable * drawable );

      /*! \brief Signals ending of a Drawable traversal.
       * \param drawable Specifies the Drawable for which traversal ends. 
       * \remarks The function will be called from the framework to signal that the
       * traversal of the indicated Drawable has been completed. A custom traverser
       * can override this function if a post-processing is required immediately after
       * the indicated Drawable has been traversed. */
      NVSG_API virtual void postTraverseDrawable( const nvsg::Drawable * drawable );

      /*! \brief Dispatcher to the particular overrides to traverse Group-derived Nodes.
       *  \param grp A pointer to the constant Group-derived Node to traverse.
       *  \remarks If preTraverseGroup() returned true, the lights contained in \a grp are
       *  traversed, and then, depending on the actual type a special traversal is performed. 
       *  After that, postTraverseGroup() is called.
       *  \sa preTraverseGroup, postTraverseGroup */
      template <typename T> void traverseGroup( const T * grp );

      /*! \brief Initiates GeoNode traversal.
       * \param gnode The GeoNode to traverse.
       * \remarks The function traverses through all geometries of the GeoNode in order.
       * For each geometry the assigned StateSet is traversed prior to the assigned Drawables.
       * The explicit StateSet traversal is initiated from inside the base implementation 
       * of preTraverseDrawables, which can be overridden for a certain purposes. However, 
       * an override should always call the base implementation in its implementation. 
       * Otherwise, a proper traversal of an assigned StateSet is not guaranteed. If 
       * preTraverseDrawables returns \c false, traversal of the Drawables assigned to the 
       * currently processed geometry will be skipped. In this case also the call to 
       * postTraverseDrawables will be skipped for that particular geometry.
       * \sa preTraverseDrawables, postTraverseDrawables */
      NVSG_API void traverseGeoNode( const nvsg::GeoNode * gnode );

      /*! \brief Traversal of a Shape.
       *  \param shape A pointer to the constant Shape to traverse.
       *  \remarks Traverses all PrimitiveSet objects of \a shape. */
      NVSG_API void traverseShape( const nvsg::Shape * shape );

      /*! \brief Traversal of a Primitive.
       *  \param prim A pointer to the constant Primitive to traverse.
       */
      NVSG_API void traversePrimitive( const nvsg::Primitive * prim );

      template<typename T>
      void traverseAnimation( const nvsg::Animation<T> * anim );

      NVSG_API void traverseAnimatedVertexAttributeSet( const nvsg::AnimatedVertexAttributeSet * avas );

      NVSG_API void traverseStateSet( const nvsg::StateSet * stateSet );

      /*! \brief Traversal of a StateVariant.
       *  \param variant A pointer to the constant StateVariant to traverse.
       *  \remarks Traverses all StatePass objects of \a variant.
       *  \sa traverseStatePass */
      NVSG_API void traverseStateVariant( const nvsg::StateVariant * variant );

      /*! \brief Traversal of a StatePass.
       *  \param pass A pointer to the constant StatePass to traverse.
       *  \remarks Traverses all StateAttribute objects of \a pass.
       *  \sa traverseStateVariant */
      NVSG_API void traverseStatePass( const nvsg::StatePass * pass );

      /*! \brief Traverse a StateAttribute */
      NVSG_API void traverseStateAttribute( const nvsg::StateAttribute * attribute );

      /*! \brief Traverse a SceneAttribute */
      NVSG_API void traverseSceneAttribute( const nvsg::SceneAttribute * attribute );

      /*! \brief Traversal of a TextureAttribute.
       *  \param texAttrib A pointer to the constant TextureAttribute to traverse.
       *  \remarks Traverses all TextureAttributeItem objects of \a texAttrib. */
      NVSG_API void traverseTextureAttribute( const nvsg::TextureAttribute * texAttrib );

      /*! \brief Traversal of a MapList.
       *  \param mapList A pointer to the constant MapList to traverse.
       *  \remarks A ViewState holds a MapList, containing two-dimensional maps, that can be used
       *  to render, for example, overlays for menus or textual information to top of the scene.
       *  That MapList is traversed here.
       *  \sa ViewState::setMapList, ViewState::getMapList */
      NVSG_API void traverseMapList( const nvsg::MapList * mapList );

      NVSG_API void traverseLockedObject( const nvsg::Object * obj );

      /*! \brief Called right after traversal of an object.
       *  \param object A pointer to the read-locked object just traversed.
       *  \remarks The default implementation does nothing.
       */
      NVSG_API virtual void postTraverseObject( const nvsg::Object * object );

      /*! \brief Get the current texture unit associated with current TextureAttributeItem, while traversing a TextureAttribute.
       *  \return The current texture unit.
       *  \sa handleTextureAttribute, handleTextureAttributeItem */
      NVSG_API unsigned int getCurrentTextureUnit() const;

      // traverser bits
      enum
      {
        // tree flags
        NVSG_SCENE_DIRTY           = BIT0
      , NVSG_ROOT_DIRTY            = BIT1
      , NVSG_TREE_DIRTY_MASK       = NVSG_SCENE_DIRTY | NVSG_ROOT_DIRTY
        // view state flags
      , NVSG_VIEWSTATE_DIRTY       = BIT2
      , NVSG_CAMERA_DIRTY          = BIT3
      , NVSG_VIEW_DIRTY_MASK       = NVSG_VIEWSTATE_DIRTY | NVSG_CAMERA_DIRTY
        // bounding volume
      , NVSG_BV_DIRTY              = BIT4
      // traversal flags
      , NVSG_TRAVERSAL_INTERRUPTED = BIT5
      };

      unsigned int                    m_currentAttrib;
      nvutil::MemFunTbl<Traverser>    m_mftbl;          //!< The table of handler functions.
      nvutil::ProblemLog              m_problemLog;     //!< Problems occurred during construction go here.
      nvsg::NodeWeakPtr               m_root;           //!< A pointer to the current root node.
      nvsg::SceneSharedPtr            m_scene;          //!< A pointer to the current scene.
      unsigned int                    m_traverserBits;  //!< The bitfield of flags for a Traverser.
      nvsg::ViewStateSharedPtr        m_viewState;      //!< A pointer to the current view state.
      nvsg::CameraSharedPtr           m_camera;

      // several incarnation counters to monitor related changes
      nvutil::Incarnation m_rootIncarnation;
      nvutil::Incarnation m_bvIncarnation;
      nvutil::Incarnation m_vertexAttributeSetIncarnation;
      nvutil::Incarnation m_primitiveIncarnation;
      nvutil::Incarnation m_hintsIncarnation;
      nvutil::Incarnation m_cameraIncarnation;
      nvutil::Incarnation m_sceneIncarnation;

      // NOTE: must not encode this into m_traverserBits, because it must remain volatile
      bool volatile m_interrupt;    //!< A flag, indicating an interrupt request

    private:
      NVSG_API unsigned int     getObjectTraversalCode( const nvsg::Object * object );  // NOTE: used inline, therefore needs to be exported via NVSG_API

      // initiates specialized Drawable traversal.
      // this is to support pre- and post-traversal on abstract Drawable level
      void traverseDrawable( const nvsg::DrawableSharedPtr & hdrawable );

      // traverse overrides for Group derived (have only read-only access to objects)
      NVSG_API void traverse( const nvsg::Group * );
      NVSG_API void traverse( const nvsg::FlipbookAnimation * );
      NVSG_API void traverse( const nvsg::Switch * );

      unsigned int  m_currentTextureUnit;   //!< used in traverseTextureAttribute

      unsigned int  m_traversalMask;           //!< traversal mask to use
      unsigned int  m_traversalMaskOverride;   //!< traversal mask override to use
  };

  template <typename T> 
  void Traverser::traverseGroup(const T * grp)
  {
    NVSG_TRACE();
    if ( preTraverseGroup(grp) )
    {
      for ( nvsg::Group::LightSourceConstIterator glsci = grp->beginLightSources() ; glsci != grp->endLightSources() ; ++glsci )
      {
        traverseObject( *glsci );
      }

      // initiate group traversal
      traverse(grp);
      postTraverseGroup(grp);
    }
  }

  /*! \brief Base class providing an interface for read-only traversing of a given scene graph.
  *  \remarks The SharedTraverser class offers base functionality for all traversers that intend
  *  to provide read-only operations on a given scene graph. For all known and concrete \link
  *  nvsg::Object Objects \endlink this class provides a set of overloadable handler routines,
  *  each ensuring that all components following the actual object in the graph hierarchy will be
  *  traversed correctly. Hence, for derived traversers it is recommended to always call the base
  *  implementation of an overridden handler routine for traversing purposes.\n
  *  To provide new read-only operations to be applied on known and concrete components or
  *  objects arranged in a scene graph, it is sufficient to derive a new traverser from either
  *  SharedTraverser or one of its derived traversers, and override the corresponding handler
  *  routines as needed.
  *  \sa ExclusiveTraverser */
  class SharedTraverser : public Traverser
  {
    public:
      /*! \brief Default constructor. */
      NVSG_API SharedTraverser();

      /*! \brief Get the current TraversalMask value
       *  \return mask The mask to be used.
       *  \remarks If a ViewState has been set on this SharedTraverser, this method will return the
       *  ViewState's TraversalMask.  Otherwise, it will return the traversal mask set with Traverser::setTraversalMask.
       *  \sa setTraversalMask */
      NVSG_API virtual unsigned int getTraversalMask() const;

    protected:
      /*! \brief Protected Destructor to prevent instantiation. */
      NVSG_API virtual ~SharedTraverser() {}
      
      /*! \brief Template function to add a handler routine for a new class derived from Object.
       *  \param objectCode Object code to identify an object type at run-time.
       *  \param handler Specifies the address of the handler routine.
       *  \remarks The function registers the handler routine specified by \a handler to handle a
       *  concrete class, derived from Object, that is explicitly identified by \a objectCode.\n
       *  A handler routine must be a member function of a Traverser-derived class. It must return
       *  void and expect a pointer to the concrete object as a first parameter, and can have one
       *  additional (optional) argument of arbitrary type. A handler routine must not have a
       *  default argument! If a handler routine is intended to remain overloadable, it should be
       *  declared virtual.\n
       *  For derived classes intended to provide new operations for known objects arranged in a
       *  scene graph, it is strongly recommended not to add new handler routines for those objects
       *  but to override the corresponding handler routine. In this context, we refer to 'known
       *  objects' as objects that are known by the Traverser base class and for which a
       *  corresponding handler routine is provided.\n
       *  However, \c addObjectHandler is indispensable if newly invented objects, and hence, 
       *  objects that are unknown by the Traverser base class, need to be considered for
       *  traversing.
       *  \note Any previously registered handler routine for the specified object code will be
       *  overridden by the new handler.
       *  \par Example:
       *  \code
       *    addObjectHandler( 
       *      OC_NEWGROUP   // object code for the new object NewGroup
       *                    // this must be provided by the author of NewGroup
       *      , &MySharedTraverser::handleNewGroup // handler function  
       *    );    
       *  \endcode */
      template <typename T, typename U> 
      void addObjectHandler( unsigned int objectCode, void (T::*handler)(const U*) );

      /*! \brief Template function to add a handler routine for a new class derived from Object.
       *  \param objectCode Object code to identify an object type at run-time.
       *  \param handler Specifies the address of the handler routine.
       *  \remarks The function registers the handler routine specified by \a handler to handle a
       *  concrete class, derived from Object, that is explicitly identified by \a objectCode.\n
       *  A handler routine must be a member function of a Traverser-derived class. It must return
       *  void and expect a pointer to the concrete object as a first parameter, and can have one
       *  additional (optional) argument of arbitrary type. A handler routine must not have a
       *  default argument! If a handler routine is intended to remain overloadable, it should be
       *  declared virtual.\n
       *  For derived classes intended to provide new operations for known objects arranged in a
       *  scene graph, it is strongly recommended not to add new handler routines for those objects
       *  but to override the corresponding handler routine. In this context, we refer to 'known
       *  objects' as objects that are known by the Traverser base class and for which a
       *  corresponding handler routine is provided.\n
       *  However, \c addObjectHandler is indispensable if newly invented objects, and hence, 
       *  objects that are unknown by the Traverser base class, need to be considered for
       *  traversing.
       *  \note Any previously registered handler routine for the specified object code will be
       *  overridden by the new handler.
       *  \par Example:
       *  \code
       *    addObjectHandler( 
       *      OC_NEWGROUP   // object code for the new object NewGroup
       *                    // this must be provided by the author of NewGroup
       *      , &MySharedTraverser::handleNewGroup // handler function  
       *    );    
       *  \endcode */
      template <typename T, typename U, typename V> 
      void addObjectHandler( unsigned int objectCode, void (T::*handler)(const U*, V) );

      /*! \brief Override of the ViewState locking interface.
       *  \remarks This function is called from the framework to lock the current ViewState, if any,
       *  for reading.
       *  \sa setViewState, unlockViewState */
      NVSG_API virtual void lockViewState();

      /*! \brief Override of the ViewState unlocking interface.
       *  \remarks This function is called from the framework to unlock the current ViewState, if
       *  any, from reading.
       *  \sa setViewState, lockViewState */
      NVSG_API virtual void unlockViewState();

      /*! \brief Override of the traversal initiating interface.
       *  \param root The Node to start traversal at.
       *  \remarks This function is called from the framework after the Scene and the ViewState,
       *  if set, are locked. The actual traversal is done here.
       *  \sa preApply, postApply, lockViewState, unlockViewState */
      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

      /*! \brief Function for performing the object traversal.
       *  \param p The Object to traverse.
       *  \remarks This function is called by the framework to really traverse an object.
       *  \sa traverseObject */
      NVSG_API virtual void doTraverseObject( const nvsg::ObjectSharedPtr & p );

      /*! \brief Function for performing traversal of a Drawable.
       *  \param p The Drawable to traverse.
       *  \remarks This function is called by the framework to traverse a Drawable. */
      NVSG_API virtual void doTraverseDrawable( const nvsg::DrawableSharedPtr & p );

      /*! \brief Handler function for a ParallelCamera.
       *  \param camera A pointer to the read-locked ParallelCamera being traversed.
       *  \remarks This function is called from the framework whenever a ParallelCamera is
       *  encountered on traversal. The ParallelCamera \a camera is already read-locked.\n
       *  The base implementation traverses through all the head lights of this camera.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleMatrixCamera, handlePerspectiveCamera */
      NVSG_API virtual void handleParallelCamera( const nvsg::ParallelCamera * camera );

      /*! \brief Handler function for a PerspectiveCamera.
       *  \param camera A pointer to the read-locked PerspectiveCamera being traversed.
       *  \remarks This function is called from the framework whenever a PerspectiveCamera is
       *  encountered on traversal. The PerspectiveCamera \a camera is already read-locked.\n
       *  The base implementation traverses through all the head lights of this camera.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleMatrixCamera, handleParallelCamera */
      NVSG_API virtual void handlePerspectiveCamera( const nvsg::PerspectiveCamera * camera );

      /*! \brief Handler function for a MatrixCamera.
       *  \param camera A pointer to the read-locked MatrixCamera being traversed.
       *  \remarks This function is called from the framework whenever a MatrixCamera is
       *  encountered on traversal. The MatrixCamera \a camera is already read-locked.\n
       *  The base implementation traverses through all the head lights of this camera.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleParallelCamera, handlePerspectiveCamera */
      NVSG_API virtual void handleMatrixCamera( const nvsg::MatrixCamera * camera );

      /*! \brief Handler function for a Group.
       *  \param group A pointer to the read-locked Group being traversed.
       *  \remarks This function is called from the framework whenever a Group is encountered on
       *  traversal. The Group \a group is already read-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup */
      NVSG_API virtual void handleGroup( const nvsg::Group * group );

      /*! \brief Handler function for a FlipbookAnimation.
       *  \param anim A pointer to the read-locked FlipbookAnimation being traversed.
       *  \remarks This function is called from the framework whenever a FlipbookAnimation is
       *  encountered on traversal. The FlipbookAnimation \a anim is already read-locked.\n
       *  The base implementation first traverses the associated IndexAnimation, and then calls
       *  Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup, IndexAnimation */
      NVSG_API virtual void handleFlipbookAnimation( const nvsg::FlipbookAnimation * anim );

      /*! \brief Handler function for a Transform.
       *  \param trafo A pointer to the read-locked Transform being traversed.
       *  \remarks This function is called from the framework whenever a Transform is encountered
       *  on traversal. The Transform \a trafo is already read-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup */
      NVSG_API virtual void handleTransform( const nvsg::Transform * trafo );

      /*! \brief Handler function for an AnimatedTransform.
       *  \param trafo A pointer to the read-locked AnimatedTransform being traversed.
       *  \remarks This function is called from the framework whenever an AnimatedTransform is
       *  encountered on traversal. The AnimatedTransform \a trafo is already read-locked.\n
       *  The base implementation first traverses the associated TrafoAnimation, and then calls
       *  Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup, TrafoAnimation */
      NVSG_API virtual void handleAnimatedTransform( const nvsg::AnimatedTransform * trafo );

      /*! \brief Handler function for an LOD.
       *  \param lod A pointer to the read-locked LOD being traversed.
       *  \remarks This function is called from the framework whenever an LOD is encountered on
       *  traversal. The LOD \a lod is already read-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup */
      NVSG_API virtual void handleLOD( const nvsg::LOD * lod );

      /*! \brief Handler function for a Switch.
       *  \param swtch A pointer to the read-locked Switch being traversed.
       *  \remarks This function is called from the framework whenever a Switch is encountered on
       *  traversal. The Switch \a switch is already read-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup */
      NVSG_API virtual void handleSwitch( const nvsg::Switch * swtch );

      /*! \brief Handler function for a Billboard.
       *  \param billboard A pointer to the read-locked Billboard being traversed.
       *  \remarks This function is called from the framework whenever a Billboard is encountered on
       *  traversal. The Billboard \a billboard is already read-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup */
      NVSG_API virtual void handleBillboard( const nvsg::Billboard * billboard );

      /*! \brief Handler function for a DirectedLight.
       *  \param light A pointer to the read-locked DirectedLight being traversed.
       *  \remarks This function is called from the framework whenever a DirectedLight is
       *  encountered on traversal. The DirectedLight \a light is already read-locked.\n
       *  The base implementation just calls traverseLight().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseLight */
      NVSG_API virtual void handleDirectedLight( const nvsg::DirectedLight * light );

      /*! \brief Handler function for a PointLight.
       *  \param light A pointer to the read-locked PointLight being traversed.
       *  \remarks This function is called from the framework whenever a PointLight is encountered
       *  on traversal. The PointLight \a light is already read-locked.\n
       *  The base implementation just calls traverseLight().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseLight */
      NVSG_API virtual void handlePointLight( const nvsg::PointLight * light );

      /*! \brief Handler function for a SpotLight.
       *  \param light A pointer to the read-locked SpotLight being traversed.
       *  \remarks This function is called from the framework whenever a SpotLight is encountered on
       *  traversal. The SpotLight \a light is already read-locked.\n
       *  The base implementation just calls traverseLight().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseLight */
      NVSG_API virtual void handleSpotLight( const nvsg::SpotLight * light );

      /*! \brief Handler function for a GeoNode.
       *  \param gnode A pointer to the read-locked GeoNode being traversed.
       *  \remarks This function is called from the framework whenever a GeoNode is encountered on
       *  traversal. The GeoNode \a gnode is already read-locked.\n
       *  The base implementation just calls Traverser::traverseGeoNode().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGeoNode */
      NVSG_API virtual void handleGeoNode( const nvsg::GeoNode * gnode );

      /*! \brief Handler function for a RectPatches.
       *  \param patches A pointer to the read-locked RectPatches being traversed.
       *  \remarks This function is called from the framework whenever a RectPatches is encountered on
       *  traversal. The RectPatches \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleRectPatches( const nvsg::RectPatches * patches );

      /*! \brief Handler function for a QuadPatches.
       *  \param patches A pointer to the read-locked QuadPatches being traversed.
       *  \remarks This function is called from the framework whenever a QuadPatches is encountered on
       *  traversal. The QuadPatches \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuadPatches( const nvsg::QuadPatches * patches );

      /*! \brief Handler function for a QuadPatches4x4.
       *  \param patches A pointer to the read-locked QuadPatches4x4 being traversed.
       *  \remarks This function is called from the framework whenever a QuadPatches4x4 is encountered
       *  on traversal. The QuadPatches4x4 \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuadPatches4x4( const nvsg::QuadPatches4x4 * patches );

      /*! \brief Handler function for a Quads.
       *  \param quads A pointer to the read-locked Quads being traversed.
       *  \remarks This function is called from the framework whenever a Quads is encountered on
       *  traversal. The Quads \a quads is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuads( const nvsg::Quads * quads );

      /*! \brief Handler function for a Triangles.
       *  \param triangles A pointer to the read-locked Triangles being traversed.
       *  \remarks This function is called from the framework whenever a Triangles is encountered on
       *  traversal. The Triangles \a quads is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriangles( const nvsg::Triangles * triangles );

      /*! \brief Handler function for a Lines.
       *  \param lines A pointer to the read-locked Lines being traversed.
       *  \remarks This function is called from the framework whenever a Lines is encountered on
       *  traversal. The Lines \a lines is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleLines( const nvsg::Lines * lines );

      /*! \brief Handler function for a Points.
       *  \param points A pointer to the read-locked Points being traversed.
       *  \remarks This function is called from the framework whenever a Points is encountered on
       *  traversal. The Points \a points is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handlePoints( const nvsg::Points * points );

      /*! \brief Handler function for a QuadsMeshes.
       *  \param meshes A pointer to the read-locked QuadsMeshes being traversed.
       *  \remarks This function is called from the framework whenever a QuadsMeshes is encountered
       *  on traversal. The QuadsMeshes \a meshes is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuadMeshes( const nvsg::QuadMeshes * meshes );

      /*! \brief Handler function for a QuadStrips.
       *  \param strips A pointer to the read-locked QuadStrips being traversed.
       *  \remarks This function is called from the framework whenever a QuadStrips is encountered
       *  on traversal. The QuadStrips \a strips is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuadStrips( const nvsg::QuadStrips * strips );

      /*! \brief Handler function for a TriFans.
       *  \param fans A pointer to the read-locked TriFans being traversed.
       *  \remarks This function is called from the framework whenever a TriFans is encountered on
       *  traversal. The TriFans \a fans is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriFans( const nvsg::TriFans * fans );

      /*! \brief Handler function for a TriPatches.
       *  \param patches A pointer to the read-locked TriPatches being traversed.
       *  \remarks This function is called from the framework whenever a TriPatches is encountered on
       *  traversal. The TriPatches \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriPatches( const nvsg::TriPatches * patches );

      /*! \brief Handler function for a TriPatches4.
       *  \param patches A pointer to the read-locked TriPatches4 being traversed.
       *  \remarks This function is called from the framework whenever a TriPatches4 is encountered on
       *  traversal. The TriPatches4 \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriPatches4( const nvsg::TriPatches4 * patches );

      /*! \brief Handler function for a TriStrips.
       *  \param strips A pointer to the read-locked TriStrips being traversed.
       *  \remarks This function is called from the framework whenever a TriStrips is encountered on
       *  traversal. The TriStrips \a strips is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriStrips( const nvsg::TriStrips * strips );

      /*! \brief Handler function for a LineStrips.
       *  \param strips A pointer to the read-locked LineStrips being traversed.
       *  \remarks This function is called from the framework whenever a LineStrips is encountered
       *  on traversal. The LineStrips \a strips is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleLineStrips( const nvsg::LineStrips * strips );

      /*! \brief Handler function for a Primitive
       *  \param primitive A pointer to the read-locked Primitive being traversed.
       *  \remarks This function is called from the framework whenever a Primitive is encountered on
       *  traversal. The Primitive \a primitive is already read-locked.\n
       *  The base implementation just calls Traverser::traversePrimitive().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitive */
      NVSG_API virtual void handlePrimitive( const nvsg::Primitive * primitive );

      /*! \brief Handler function for a Shape.
       *  \param shape A pointer to the read-locked Shape being traversed.
       *  \remarks This function is called from the framework whenever a Shape is encountered on
       *  traversal. The Shape \a shape is already read-locked.\n
       *  The base implementation just calls Traverser::traverseShape().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseShape */
      NVSG_API virtual void handleShape( const nvsg::Shape * shape );

      /*! \brief Handler function for a StateSet.
       *  \param stateSet A pointer to the read-locked StateSet being traversed.
       *  \remarks This function is called from the framework whenever a StateSet is encountered on
       *  traversal. The StateSet \a stateSet is already read-locked.\n
       *  The base implementation just traverses the active StateVariant.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa StateVariant */
      NVSG_API virtual void handleStateSet( const nvsg::StateSet * stateSet );

      /*! \brief Handler function for a StateVariant.
       *  \param variant A pointer to the read-locked StateVariant being traversed.
       *  \remarks This function is called from the framework whenever a StateVariant is encountered
       *  on traversal. The StateVariant \a variant is already read-locked.\n
       *  The base implementation just call Traverser::traverseStateVariant().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseStateVariant */
      NVSG_API virtual void handleStateVariant( const nvsg::StateVariant * variant );

      /*! \brief Handler function for a StatePass.
       *  \param pass A pointer to the read-locked StatePass being traversed.
       *  \remarks This function is called from the framework whenever a StateSet is encountered on
       *  traversal. The StatePass \a pass is already read-locked.\n
       *  The base implementation just call Traverser::traverseStatePass().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseStatePass */
      NVSG_API virtual void handleStatePass( const nvsg::StatePass * pass );

      /*! \brief Handler function for a CgFx.
       *  \param cgfx A pointer to the read-locked CgFx being traversed.
       *  \remarks This function is called from the framework whenever a CgFx is encountered on
       *  traversal. The CgFx \a cgfx is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleCgFx( const nvsg::CgFx * cgfx );

      /*! \brief Handler function for an RTFx.
       *  \param rtfx A pointer to the read-locked RTFx being traversed.
       *  \remarks This function is called from the framework whenever an RTFx is encountered on
       *  traversal. The RTFx \a rtfx is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleRTFx( const nvsg::RTFx * rtfx );

      NVSG_API virtual void handleRTFxProgram( const nvsg::RTFxProgram * rtfxProgram );

      /*! \brief Handler function for an RTBufferAttribute.
       *  \param rtba A pointer to the read-locked RTBufferAttribute being traversed.
       *  \remarks This function is called from the framework whenever an RTBufferAttribute is encountered on
       *  traversal. The RTFx \a rtfx is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleRTBufferAttribute( const nvsg::RTBufferAttribute * rtba );

      /*! \brief Handler function for a Material.
       *  \param material A pointer to the read-locked Material being traversed.
       *  \remarks This function is called from the framework whenever a Material is encountered on
       *  traversal. The Material \a material is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleMaterial( const nvsg::Material * material );

      /*! \brief Handler function for a TextureAttribute.
       *  \param texAttrib A pointer to the read-locked TextureAttribute being traversed.
       *  \remarks This function is called from the framework whenever a TextureAttribute is
       *  encountered on traversal. The TextureAttribute \a texAttrib is already read-locked.\n
       *  The base implementation just calls Traverser::traverseTextureAttribute().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseTextureAttribute */
      NVSG_API virtual void handleTextureAttribute( const nvsg::TextureAttribute * texAttrib );

      /*! \brief Handler function for a TextureAttributeItem.
       *  \param texAttribItem A pointer to the read-locked TextureAttributeItem being traversed.
       *  \remarks This function is called from the framework whenever a TextureAttributeItem is
       *  encountered on traversal. The TextureAttributeItem \a texAttribItem is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa getCurrentTextureUnit */
      NVSG_API virtual void handleTextureAttributeItem( const nvsg::TextureAttributeItem * texAttribItem );

      /*! \brief Handler function for a AlphaTestAttribute.
      *  \param alphaTestAttrib A pointer to the read-locked AlphaTestAttribute being traversed.
      *  \remarks This function is called from the framework whenever a AlphaTestAttribute is
      *  encountered on traversal. The AlphaTestAttribute \a alphaTestAttrib is already read-locked.\n
      *  The base implementation just does nothing.
      *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
      *  recommended to always call the base class implementation in order to properly continue
      *  traversal. */
      NVSG_API virtual void handleAlphaTestAttribute( const nvsg::AlphaTestAttribute * alphaTestAttrib );

      /*! \brief Handler function for a BlendAttribute.
       *  \param blendAttrib A pointer to the read-locked BlendAttribute being traversed.
       *  \remarks This function is called from the framework whenever a BlendAttribute is
       *  encountered on traversal. The BlendAttribute \a blendAttrib is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleBlendAttribute( const nvsg::BlendAttribute * blendAttrib );

      /*! \brief Handler function for a DepthAttribute.
       *  \param depthAttrib A pointer to the read-locked DepthAttribute being traversed.
       *  \remarks This function is called from the framework whenever a DepthAttribute is
       *  encountered on traversal. The DepthAttribute \a depthAttrib is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleDepthAttribute( const nvsg::DepthAttribute * depthAttrib );

      /*! \brief Handler function for a StencilAttribute.
       * \param stencilAttrib Specifies the StencilAttriburte to process.
       * \remarks This function is called from the framework whenever a StencilAttribute is
       * encountered on traversal. The StencilAttribute attribute will be read-locked by the
       * framework prior to call this handler function. The read lock will be released again,
       * after the function returns. The base implementation performs no operation. */
      NVSG_API virtual void handleStencilAttribute( const nvsg::StencilAttribute * stencilAttrib );

      /*! \brief Handler function for a FaceAttribute.
       *  \param faceAttrib A pointer to the read-locked FaceAttribute being traversed.
       *  \remarks This function is called from the framework whenever a FaceAttribute is
       *  encountered on traversal. The FaceAttribute \a faceAttrib is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleFaceAttribute( const nvsg::FaceAttribute * faceAttrib );

      /*! \brief Handler function for a LightingAttribute.
       *  \param laAttrib A pointer to the read-locked LightingAttribute being traversed.
       *  \remarks This function is called from the framework whenever a LightingAttribute is
       *  encountered on traversal. The LightingAttribute \a laAttrib is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleLightingAttribute( const nvsg::LightingAttribute * laAttrib );

      /*! \brief Handler function for a LineAttribute.
       *  \param lineAttrib A pointer to the read-locked LineAttribute being traversed.
       *  \remarks This function is called from the framework whenever a LineAttribute is
       *  encountered on traversal. The LineAttribute \a lineAttrib is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleLineAttribute( const nvsg::LineAttribute * lineAttrib );

      /*! \brief Handler function for a PointAttribute.
       *  \param pointAttrib A pointer to the read-locked PointAttribute being traversed.
       *  \remarks This function is called from the framework whenever a PointAttribute is
       *  encountered on traversal. The PointAttribute \a pointAttrib is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handlePointAttribute( const nvsg::PointAttribute * pointAttrib );

      /*! \brief Handler function for an UnlitColorAttribute.
       *  \param unlitColor A pointer to the read-locked UnlitColorAttribute being traversed.
       *  \remarks This function is called from the framework whenever an UnlitColorAttribute is
       *  encountered on traversal. The UnlitColorAttribute \a unlitColor is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleUnlitColorAttribute( const nvsg::UnlitColorAttribute * unlitColor );

      /*! \brief Handler function for a MapArray.
       *  \param mapArray A pointer to the read-locked MapArray being traversed.
       *  \remarks This function is called from the framework whenever a MapArray is encountered on
       *  traversal. The MapArray \a mapArray is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. 
       *  \sa handleMapElement, handleMapMatrix */
      NVSG_API virtual void handleMapArray( const nvsg::MapArray * mapArray );

      /*! \brief Handler function for a MapElement.
       *  \param mapElement A pointer to the read-locked MapElement being traversed.
       *  \remarks This function is called from the framework whenever a MapElement is
       *  encountered on traversal. The MapElement \a mapElement is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleMapArray, handleMapMatrix */
      NVSG_API virtual void handleMapElement( const nvsg::MapElement * mapElement );

      /*! \brief Handler function for a MapMatrix.
       *  \param mapMatrix A pointer to the read-locked MapMatrix being traversed.
       *  \remarks This function is called from the framework whenever a MapMatrix is encountered on
       *  traversal. The MapMatrix \a mapMatrix is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleMapArray, handleMapElement */
      NVSG_API virtual void handleMapMatrix( const nvsg::MapMatrix * mapMatrix );

      /*! \brief Handler function for a MapList.
       *  \param mapList A pointer to the read-locked MapList being traversed.
       *  \remarks This function is called from the framework whenever a MapList is encountered on
       *  traversal. The MapList \a mapList is already read-locked.\n
       *  The base implementation just calls Traverser::traverseMapList().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseMapList */
      NVSG_API virtual void handleMapList( const nvsg::MapList * mapList );

      /*! \brief Handler function for a VertexAttributeSet.
       *  \param vas A pointer to the read-locked VertexAttributeSet being traversed.
       *  \remarks This function is called from the framework whenever a VertexAttributeSet is
       *  encountered on traversal. The VertexAttributeSet \a vas is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleVertexAttributeSet( const nvsg::VertexAttributeSet *vas );

      /*! \brief Handler function for an AnimatedVertexAttributeSet.
       *  \param avas A pointer to the read-locked AnimatedVertexAttributeSet being traversed.
       *  \remarks This function is called from the framework whenever an AnimatedVertexAttributeSet
       *  is encountered on traversal. The AnimatedVertexAttributeSet \a avas is already read-locked.\n
       *  The base implementation traverses each VertexAttributeAnimation of \a avas and holds
       *  the index of the currently traversed VertexAttributeAnimation in the protected
       *  member variable \c m_currentAttrib. */
      NVSG_API virtual void handleAnimatedVertexAttributeSet( const nvsg::AnimatedVertexAttributeSet * avas );

      /*! \brief Handler function for a Skin.
       *  \param skin A pointer to the read-locked Skin being traversed.
       *  \remarks This function is called from the framework whenever a Skin is encountered on
       *  traversal. The Skin \a skin is already read-locked.\n
       *  The base implementation just does nothing. */
      NVSG_API virtual void handleSkin( const nvsg::Skin * skin );

      /*! \brief Handle function for an Animation of indices.
       *  \param anim A pointer to the read-locked Animation being traversed.
       *  \remarks This function is called from the framework whenever an Animation of indices is
       *  encountered on traversal. The Animation \a anim is already read-locked.\n
       *  The base implementation just does nothing. */
      NVSG_API virtual void handleIndexAnimation( const nvsg::Animation<unsigned int> * anim );

      /*! \brief Handle function for an Animation of Trafos
       *  \param anim A pointer to the read-locked Animation being traversed.
       *  \remarks This function is called from the framework whenever an Animation of Trafos is
       *  encountered on traversal. The Animation \a anim is already read-locked.\n
       *  The base implementation just does nothing. */
      NVSG_API virtual void handleTrafoAnimation( const nvsg::Animation<nvmath::Trafo> * anim );

      /*! \brief Handle function for an Animation of VertexAttributes.
       *  \param anim A pointer to the read-locked Animation being traversed.
       *  \remarks This function is called from the framework whenever an Animation of
       *  VertexAttributes is encountered on traversal. Then Animation \a anim is already
       *  read-locked.\n
       *  The base implementation just does nothing. */
      NVSG_API virtual void handleVertexAttributeAnimation( const nvsg::Animation<nvsg::VertexAttribute> * anim );

      /*! \brief Handler function for a FramedAnimationDescription<unsigned int>.
       *  \param anim A pointer to the read-locked FramedAnimationDescription<unsigned int> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  FramedAnimationDescription<unsigned int> is encountered on traversal. The
       *  FramedAnimationDescription<unsigned int> \a anim is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleFramedIndexAnimationDescription( const nvsg::FramedAnimationDescription<unsigned int> * anim );

      /*! \brief Handler function for a FramedAnimationDescription<nvmath::Trafo>.
       *  \param trafo A pointer to the read-locked FramedAnimationDescription<nvmath::Trafo> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  FramedAnimationDescription<nvmath::Trafo> is encountered on traversal. The
       *  FramedAnimationDescription<nvmath::Trafo> \a trafo is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleFramedTrafoAnimationDescription( const nvsg::FramedAnimationDescription<nvmath::Trafo> * trafo );

      /*! \brief Handler function for a FramedAnimationDescription<VertexAttribute>.
       *  \param p A pointer to the read-locked FramedAnimationDescription<VertexAttribute> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  FramedAnimationDescription<VertexAttribute> is encountered on traversal. The
       *  FramedAnimationDescription<VertexAttribute> \a p is already read-locked.\n
       *  The base implementation just does nothing
       *  \sa handleLinearInterpolatedVertexAttributeAnimationDescription */
      NVSG_API virtual void handleFramedVertexAttributeAnimationDescription( const nvsg::FramedAnimationDescription<nvsg::VertexAttribute> * p );

      /*! \brief Handler function for a LinearInterpolatedAnimationDescription<nvmath::Trafo>.
       *  \param trafo A pointer to the read-locked LinearInterpolatedAnimationDescription<nvmath::Trafo> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  LinearInterpolatedAnimationDescription<nvmath::Trafo> is encountered on traversal. The
       *  LinearInterpolatedAnimationDescription<nvmath::Trafo> \a trafo is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleLinearInterpolatedTrafoAnimationDescription( const nvsg::LinearInterpolatedAnimationDescription<nvmath::Trafo> * trafo );

      /*! \brief Handler function for a LinearInterpolatedAnimationDescription<VertexAttribute>.
       *  \param p A pointer to the read-locked LinearInterpolatedAnimationDescription<VertexAttribute> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  LinearInterpolatedAnimationDescription<VertexAttribute> is encountered on traversal. The
       *  LinearInterpolatedAnimationDescription<VertexAttribute> \a p is already read-locked.\n
       *  The base implementation just does nothing
       *  \sa handleFramedVertexAttributeAnimationDescription */
      NVSG_API virtual void handleLinearInterpolatedVertexAttributeAnimationDescription( const nvsg::LinearInterpolatedAnimationDescription<nvsg::VertexAttribute> * p );

      /*! \brief Handler function for an RTFxSceneAttribute.
       *  \param rtfxsa A pointer to the read-locked RTFxSceneAttribute being traversed.
       *  \remarks This function is called from the framework whenever an RTFxSceneAttribute is encountered
       *  on traversal. The RTFxSceneAttribute \a rtfxsa is already read-locked.\n
       *  The base implementation just calls Traverser::traverseSceneAttribute().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseSceneAttribute */
      NVSG_API virtual void handleRTFxSceneAttribute( const nvsg::RTFxSceneAttribute * rtfxsa );

      /*! \brief Function for common handling of Camera classes.
       *  \param camera A pointer to the read-locked Camera being traversed.
       *  \remarks This function is called from the framework whenever a Camera is encountered
       *  on traversal. The Camera \a camera is already read-locked.\n
       *  The base implementation traverses the headlights of \a camera.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa LightSource */
      NVSG_API virtual void traverseCamera( const nvsg::Camera * camera );

      /*! \brief Function for common handling of FrustumCamera classes.
       *  \param camera A pointer to the read-locked FrustumCamera being traversed.
       *  \remarks This function is called from the framework whenever a FrustumCamera is encountered
       *  on traversal. The FrustumCamera \a camera is already read-locked.\n
       *  The base implementation traverses the headlights of \a camera.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa LightSource */
      NVSG_API virtual void traverseFrustumCamera( const nvsg::FrustumCamera * camera );

      /*! \brief Function for common handling of LightSource classes.
       *  \param light A pointer to the read-locked LightSource being traversed.
       *  \remarks This function is called from the framework whenever a LightSource is encountered
       *  on traversal. The LightSource \a light is already read-locked.\n
       *  The base implementation traverses the Animation associated with \a light, if there is one.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa TrafoAnimation */
      NVSG_API virtual void traverseLight( const nvsg::LightSource * light );

      /*! \brief Function for common handling of PrimitiveSet classes.
       *  \param pset A pointer to the read-locked PrimitiveSet being traversed.
       *  \remarks This function is called from the framework whenever a PrimitiveSet is encountered
       *  on traversal. The PrimitiveSet \a pset is already read-locked.\n
       *  The base implementation first traverses the Skin, if there is one, and then the
       *  VertexAttributeSet of \a pset, if there is one.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa VertexAttributeSet, Skin */
      NVSG_API virtual void traversePrimitiveSet( const nvsg::PrimitiveSet * pset );

      /*! \brief Function for common handling of Primitive classes.
       *  \param p A pointer to the read-locked Primitive being traversed.
       *  \remarks This function is called from the framework whenever a Primitive is encountered
       *  on traversal. The Primitive \a p is already read-locked.\n
       *  The base implementation first traverses the Skin, if there is one, and then the
       *  VertexAttributeSet of \a p, if there is one. 
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa VertexAttributeSet, Skin */
      NVSG_API virtual void traversePrimitive( const nvsg::Primitive * p );

    protected:
      std::auto_ptr<nvsg::CameraReadLock>     m_lockedCamera;
      std::auto_ptr<nvsg::SceneReadLock>      m_lockedScene;
      std::auto_ptr<nvsg::ViewStateReadLock>  m_lockedViewState;

    private:
      const nvsg::PrimitiveSet  * m_currentPrimitiveSet;  //!< used int traversePrimitiveSet
  };

/*! \brief Base class providing an interface for read-write traversing of a given scene graph.
 *  \remarks The ExclusiveTraverser class offers base functionality for all traversers that intend
 *  to provide read-write operations on a given scene graph. For all known and concrete \link
 *  nvsg::Object Objects \endlink this class provides a set of overloadable handler routines,
 *  each ensuring that all components following the actual object in the graph hierarchy will be
 *  traversed correctly. Hence, for derived traversers it is recommended to always call the base
 *  implementation of an overridden handler routine for traversing purposes.\n
 *  To provide new read-write operations to be applied on known and concrete components or
 *  objects arranged in a scene graph, it is sufficient to derive a new traverser from either
 *  ExclusiveTraverser or one of its derived traversers, and override the corresponding handler
 *  routines as needed.
 *  \sa SharedTraverser */
  class ExclusiveTraverser : public Traverser
  {
    public:
      /*! \brief Default constructor. */
      NVSG_API ExclusiveTraverser();

      //! Query if the latest traversal did modify the tree.
      /** \returns \c true if the tree was modified, otherwise false */
      NVSG_API bool getTreeModified( void ) const;

      /*! \brief Get the current TraversalMask value
       *  \return mask The mask to be used.
       *  \remarks If a ViewState has been set on this ExclusiveTraverser, this method will return the
       *  ViewState's TraversalMask.  Otherwise, it will return the traversal mask set with Traverser::setTraversalMask.
       *  \sa setTraversalMask */
      NVSG_API virtual unsigned int getTraversalMask() const;

    protected:
      /*! \brief Protected Destructor to prevent instantiation. */
      NVSG_API virtual ~ExclusiveTraverser() {}

      /*! \brief Template function to add a handler routine for a new class derived from Object.
       *  \param objectCode Object code to identify an object type at run-time.
       *  \param handler Specifies the address of the handler routine.
       *  \remarks The function registers the handler routine specified by \a handler to handle a
       *  concrete class, derived from Object, that is explicitly identified by \a objectCode.\n
       *  A handler routine must be a member function of a Traverser-derived class. It must return
       *  void and expect a pointer to the concrete object as a first parameter, and can have one
       *  additional (optional) argument of arbitrary type. A handler routine must not have a
       *  default argument! If a handler routine is intended to remain overloadable, it should be
       *  declared virtual.\n
       *  For derived classes intended to provide new operations for known objects arranged in a
       *  scene graph, it is strongly recommended not to add new handler routines for those objects
       *  but to override the corresponding handler routine. In this context, we refer to 'known
       *  objects' as objects that are known by the Traverser base class and for which a
       *  corresponding handler routine is provided.\n
       *  However, \c addObjectHandler is indispensable if newly invented objects, and hence, 
       *  objects that are unknown by the Traverser base class, need to be considered for
       *  traversing.
       *  \note Any previously registered handler routine for the specified object code will be
       *  overridden by the new handler.
       *  \par Example:
       *  \code
       *    addObjectHandler( 
       *      OC_NEWGROUP   // object code for the new object NewGroup
       *                    // this must be provided by the author of NewGroup
       *      , &MyExclusiveTraverser::handleNewGroup // handler function  
       *    );    
       *  \endcode */
      template <typename T, typename U> 
      void addObjectHandler( unsigned int objectCode, void (T::*handler)(U*) );

      /*! \brief Template function to add a handler routine for a new class derived from Object.
       *  \param objectCode Object code to identify an object type at run-time.
       *  \param handler Specifies the address of the handler routine.
       *  \remarks The function registers the handler routine specified by \a handler to handle a
       *  concrete class, derived from Object, that is explicitly identified by \a objectCode.\n
       *  A handler routine must be a member function of a Traverser-derived class. It must return
       *  void and expect a pointer to the concrete object as a first parameter, and can have one
       *  additional (optional) argument of arbitrary type. A handler routine must not have a
       *  default argument! If a handler routine is intended to remain overloadable, it should be
       *  declared virtual.\n
       *  For derived classes intended to provide new operations for known objects arranged in a
       *  scene graph, it is strongly recommended not to add new handler routines for those objects
       *  but to override the corresponding handler routine. In this context, we refer to 'known
       *  objects' as objects that are known by the Traverser base class and for which a
       *  corresponding handler routine is provided.\n
       *  However, \c addObjectHandler is indispensable if newly invented objects, and hence, 
       *  objects that are unknown by the Traverser base class, need to be considered for
       *  traversing.
       *  \note Any previously registered handler routine for the specified object code will be
       *  overridden by the new handler.
       *  \par Example:
       *  \code
       *    addObjectHandler( 
       *      OC_NEWGROUP   // object code for the new object NewGroup
       *                    // this must be provided by the author of NewGroup
       *      , &MyExclusiveTraverser::handleNewGroup // handler function  
       *    );    
       *  \endcode */
      template <typename T, typename U, typename V> 
      void addObjectHandler( unsigned int objectCode, void (T::*handler)(U*, V) );

      /*! \brief Override of the ViewState locking interface.
       *  \remarks This function is called from the framework to lock the current ViewState, if any,
       *  for writing.
       *  \sa setViewState, unlockViewState */
      NVSG_API virtual void lockViewState();

      /*! \brief Override of the ViewState unlocking interface.
       *  \remarks This function is called from the framework to unlock the current ViewState, if
       *  any, from writing.
       *  \sa setViewState, lockViewState */
      NVSG_API virtual void unlockViewState();

      /*! \brief Override of the traversal initiating interface.
       *  \param root The Node to start traversal at.
       *  \remarks This function is called from the framework after the Scene and the ViewState,
       *  if set, are locked. The actual traversal is done here.
       *  \sa preApply, postApply, lockViewState, unlockViewState */
      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

      /*! \brief Function for performing the object traversal.
       *  \param p The Object to traverse.
       *  \remarks This function is called by the framework to really traverse an object.
       *  \sa traverseObject */
      NVSG_API virtual void doTraverseObject( const nvsg::ObjectSharedPtr & p );

      /*! \brief Function for performing traversal of a Drawable.
       *  \param p The Drawable to traverse.
       *  \remarks This function is called by the framework to traverse a Drawable. */
      NVSG_API virtual void doTraverseDrawable( const nvsg::DrawableSharedPtr & p );

      /*! \brief Handler function for a ParallelCamera.
       *  \param camera A pointer to the write-locked ParallelCamera being traversed.
       *  \remarks This function is called from the framework whenever a ParallelCamera is
       *  encountered on traversal. The ParallelCamera \a camera is already write-locked.\n
       *  The base implementation traverses through all the head lights of this camera.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleMatrixCamera, handlePerspectiveCamera */
      NVSG_API virtual void handleParallelCamera( nvsg::ParallelCamera * camera );

      /*! \brief Handler function for a PerspectiveCamera.
       *  \param camera A pointer to the write-locked PerspectiveCamera being traversed.
       *  \remarks This function is called from the framework whenever a PerspectiveCamera is
       *  encountered on traversal. The PerspectiveCamera \a camera is already write-locked.\n
       *  The base implementation traverses through all the head lights of this camera.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleMatrixCamera, handleParallelCamera */
      NVSG_API virtual void handlePerspectiveCamera( nvsg::PerspectiveCamera * camera );

      /*! \brief Handler function for a MatrixCamera.
       *  \param camera A pointer to the write-locked MatrixCamera being traversed.
       *  \remarks This function is called from the framework whenever a MatrixCamera is
       *  encountered on traversal. The MatrixCamera \a camera is already read-locked.\n
       *  The base implementation traverses through all the head lights of this camera.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleParallelCamera, handlePerspectiveCamera */
      NVSG_API virtual void handleMatrixCamera( nvsg::MatrixCamera * camera );

      /*! \brief Handler function for a Group.
       *  \param group A pointer to the write-locked Group being traversed.
       *  \remarks This function is called from the framework whenever a Group is encountered on
       *  traversal. The Group \a group is already write-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup */
      NVSG_API virtual void handleGroup( nvsg::Group * group );

      /*! \brief Handler function for a FlipbookAnimation.
       *  \param anim A pointer to the write-locked FlipbookAnimation being traversed.
       *  \remarks This function is called from the framework whenever a FlipbookAnimation is
       *  encountered on traversal. The FlipbookAnimation \a anim is already write-locked.\n
       *  The base implementation first traverses the associated IndexAnimation, and then calls
       *  Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup, IndexAnimation */
      NVSG_API virtual void handleFlipbookAnimation( nvsg::FlipbookAnimation * anim );

      /*! \brief Handler function for a Transform.
       *  \param trafo A pointer to the write-locked Transform being traversed.
       *  \remarks This function is called from the framework whenever a Transform is encountered
       *  on traversal. The Transform \a trafo is already write-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup */
      NVSG_API virtual void handleTransform( nvsg::Transform * trafo );

      /*! \brief Handler function for an AnimatedTransform.
       *  \param trafo A pointer to the write-locked AnimatedTransform being traversed.
       *  \remarks This function is called from the framework whenever an AnimatedTransform is
       *  encountered on traversal. The AnimatedTransform \a trafo is already write-locked.\n
       *  The base implementation first traverses the associated TrafoAnimation, and then calls
       *  Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup, TrafoAnimation */
      NVSG_API virtual void handleAnimatedTransform( nvsg::AnimatedTransform * trafo );

      /*! \brief Handler function for an LOD.
       *  \param lod A pointer to the write-locked LOD being traversed.
       *  \remarks This function is called from the framework whenever an LOD is encountered on
       *  traversal. The LOD \a lod is already write-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup */
      NVSG_API virtual void handleLOD( nvsg::LOD * lod );

      /*! \brief Handler function for a Switch.
       *  \param swtch A pointer to the write-locked Switch being traversed.
       *  \remarks This function is called from the framework whenever a Switch is encountered on
       *  traversal. The Switch \a switch is already write-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup */
      NVSG_API virtual void handleSwitch( nvsg::Switch * swtch );

      /*! \brief Handler function for a Billboard.
       *  \param billboard A pointer to the write-locked Billboard being traversed.
       *  \remarks This function is called from the framework whenever a Billboard is encountered on
       *  traversal. The Billboard \a billboard is already write-locked.\n
       *  The base implementation just calls Traverser::traverseGroup().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGroup, handleGroup */
      NVSG_API virtual void handleBillboard( nvsg::Billboard * billboard );

      /*! \brief Handler function for a DirectedLight.
       *  \param light A pointer to the write-locked DirectedLight being traversed.
       *  \remarks This function is called from the framework whenever a DirectedLight is
       *  encountered on traversal. The DirectedLight \a light is already write-locked.\n
       *  The base implementation just calls traverseLight().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseLight */
      NVSG_API virtual void handleDirectedLight( nvsg::DirectedLight * light );

      /*! \brief Handler function for a PointLight.
       *  \param light A pointer to the write-locked PointLight being traversed.
       *  \remarks This function is called from the framework whenever a PointLight is encountered
       *  on traversal. The PointLight \a light is already write-locked.\n
       *  The base implementation just calls traverseLight().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseLight */
      NVSG_API virtual void handlePointLight( nvsg::PointLight * light );

      /*! \brief Handler function for a SpotLight.
       *  \param light A pointer to the write-locked SpotLight being traversed.
       *  \remarks This function is called from the framework whenever a SpotLight is encountered on
       *  traversal. The SpotLight \a light is already write-locked.\n
       *  The base implementation just calls traverseLight().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseLight */
      NVSG_API virtual void handleSpotLight( nvsg::SpotLight * light );

      /*! \brief Handler function for a GeoNode.
       *  \param gnode A pointer to the write-locked GeoNode being traversed.
       *  \remarks This function is called from the framework whenever a GeoNode is encountered on
       *  traversal. The GeoNode \a gnode is already write-locked.\n
       *  The base implementation just calls Traverser::traverseGeoNode().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseGeoNode */
      NVSG_API virtual void handleGeoNode( nvsg::GeoNode * gnode );

      /*! \brief Handler function for a RectPatches.
       *  \param patches A pointer to the read-locked RectPatches being traversed.
       *  \remarks This function is called from the framework whenever a RectPatches is encountered on
       *  traversal. The RectPatches \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleRectPatches( nvsg::RectPatches * patches );

      /*! \brief Handler function for a QuadPatches.
       *  \param patches A pointer to the read-locked QuadPatches being traversed.
       *  \remarks This function is called from the framework whenever a QuadPatches is encountered on
       *  traversal. The QuadPatches \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuadPatches( nvsg::QuadPatches * patches );

      /*! \brief Handler function for a QuadPatches4x4.
       *  \param patches A pointer to the read-locked QuadPatches4x4 being traversed.
       *  \remarks This function is called from the framework whenever a QuadPatches4x4 is encountered
       *  on traversal. The QuadPatches4x4 \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuadPatches4x4( nvsg::QuadPatches4x4 * patches );

      /*! \brief Handler function for a Quads.
       *  \param quads A pointer to the write-locked Quads being traversed.
       *  \remarks This function is called from the framework whenever a Quads is encountered on
       *  traversal. The Quads \a quads is already write-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuads( nvsg::Quads * quads );

      /*! \brief Handler function for a Triangles.
       *  \param triangles A pointer to the write-locked Triangles being traversed.
       *  \remarks This function is called from the framework whenever a Triangles is encountered on
       *  traversal. The Triangles \a quads is already write-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriangles( nvsg::Triangles * triangles );

      /*! \brief Handler function for a Lines.
       *  \param lines A pointer to the write-locked Lines being traversed.
       *  \remarks This function is called from the framework whenever a Lines is encountered on
       *  traversal. The Lines \a lines is already write-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleLines( nvsg::Lines * lines );

      /*! \brief Handler function for a Points.
       *  \param points A pointer to the write-locked Points being traversed.
       *  \remarks This function is called from the framework whenever a Points is encountered on
       *  traversal. The Points \a points is already write-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handlePoints( nvsg::Points * points );

      /*! \brief Handler function for a QuadsMeshes.
       *  \param meshes A pointer to the write-locked QuadsMeshes being traversed.
       *  \remarks This function is called from the framework whenever a QuadsMeshes is encountered
       *  on traversal. The QuadsMeshes \a meshes is already write-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuadMeshes( nvsg::QuadMeshes * meshes );

      /*! \brief Handler function for a QuadStrips.
       *  \param strips A pointer to the write-locked QuadStrips being traversed.
       *  \remarks This function is called from the framework whenever a QuadStrips is encountered
       *  on traversal. The QuadStrips \a strips is already write-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleQuadStrips( nvsg::QuadStrips * strips );

      /*! \brief Handler function for a TriFans.
       *  \param fans A pointer to the write-locked TriFans being traversed.
       *  \remarks This function is called from the framework whenever a TriFans is encountered on
       *  traversal. The TriFans \a fans is already write-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriFans( nvsg::TriFans * fans );

      /*! \brief Handler function for a TriPatches.
       *  \param patches A pointer to the read-locked TriPatches being traversed.
       *  \remarks This function is called from the framework whenever a TriPatches is encountered on
       *  traversal. The TriPatches \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriPatches( nvsg::TriPatches * patches );

      /*! \brief Handler function for a TriPatches4.
       *  \param patches A pointer to the read-locked TriPatches4 being traversed.
       *  \remarks This function is called from the framework whenever a TriPatches4 is encountered on
       *  traversal. The TriPatches4 \a patches is already read-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriPatches4( nvsg::TriPatches4 * patches );

      /*! \brief Handler function for a TriStrips.
       *  \param strips A pointer to the write-locked TriStrips being traversed.
       *  \remarks This function is called from the framework whenever a TriStrips is encountered on
       *  traversal. The TriStrips \a strips is already write-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleTriStrips( nvsg::TriStrips * strips );

      /*! \brief Handler function for a LineStrips.
       *  \param strips A pointer to the write-locked LineStrips being traversed.
       *  \remarks This function is called from the framework whenever a LineStrips is encountered
       *  on traversal. The LineStrips \a strips is already write-locked.\n
       *  The base implementation just calls traversePrimitiveSet().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitiveSet */
      NVSG_API virtual void handleLineStrips( nvsg::LineStrips * strips );

      /*! \brief Handler function for a Primitive
       *  \param primitive A pointer to the write-locked Primitive being traversed.
       *  \remarks This function is called from the framework whenever a Primitive is encountered on
       *  traversal. The Primitive \a primitive is already write-locked.\n
       *  The base implementation just calls Traverser::traversePrimitive().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traversePrimitive */
      NVSG_API virtual void handlePrimitive( nvsg::Primitive * primitive );

      /*! \brief Handler function for a Shape.
       *  \param shape A pointer to the write-locked Shape being traversed.
       *  \remarks This function is called from the framework whenever a Shape is encountered on
       *  traversal. The Shape \a shape is already write-locked.\n
       *  The base implementation just calls Traverser::traverseShape().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseShape */
      NVSG_API virtual void handleShape( nvsg::Shape * shape );

      /*! \brief Handler function for a StateSet.
       *  \param stateSet A pointer to the write-locked StateSet being traversed.
       *  \remarks This function is called from the framework whenever a StateSet is encountered on
       *  traversal. The StateSet \a stateSet is already write-locked.\n
       *  The base implementation just traverses the active StateVariant.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa StateVariant */
      NVSG_API virtual void handleStateSet( nvsg::StateSet * stateSet );

      /*! \brief Handler function for a StateVariant.
       *  \param variant A pointer to the write-locked StateVariant being traversed.
       *  \remarks This function is called from the framework whenever a StateVariant is encountered
       *  on traversal. The StateVariant \a variant is already write-locked.\n
       *  The base implementation just call Traverser::traverseStateVariant().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseStateVariant */
      NVSG_API virtual void handleStateVariant( nvsg::StateVariant * variant );

      /*! \brief Handler function for a StatePass.
       *  \param pass A pointer to the write-locked StatePass being traversed.
       *  \remarks This function is called from the framework whenever a StateSet is encountered on
       *  traversal. The StatePass \a pass is already write-locked.\n
       *  The base implementation just call Traverser::traverseStatePass().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseStatePass */
      NVSG_API virtual void handleStatePass( nvsg::StatePass * pass );

      /*! \brief Handler function for a CgFx.
       *  \param cgfx A pointer to the write-locked CgFx being traversed.
       *  \remarks This function is called from the framework whenever a CgFx is encountered on
       *  traversal. The CgFx \a cgfx is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleCgFx( nvsg::CgFx * cgfx );

      /*! \brief Handler function for a RTFx.
       *  \param rtfx A pointer to the write-locked RTFx being traversed.
       *  \remarks This function is called from the framework whenever a RTFx is encountered on
       *  traversal. The RTFx \a rtfx is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleRTFx( nvsg::RTFx * rtfx );

      NVSG_API virtual void handleRTFxProgram( nvsg::RTFxProgram * rtfxProgram );

      /*! \brief Handler function for an RTBufferAttribute.
       *  \param rtba A pointer to the read-locked RTBufferAttribute being traversed.
       *  \remarks This function is called from the framework whenever an RTBufferAttribute is encountered on
       *  traversal. The RTFx \a rtfx is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleRTBufferAttribute( nvsg::RTBufferAttribute * rtba );

      /*! \brief Handler function for a Material.
       *  \param material A pointer to the write-locked Material being traversed.
       *  \remarks This function is called from the framework whenever a Material is encountered on
       *  traversal. The Material \a material is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleMaterial( nvsg::Material * material );

      /*! \brief Handler function for a TextureAttribute.
       *  \param texAttrib A pointer to the write-locked TextureAttribute being traversed.
       *  \remarks This function is called from the framework whenever a TextureAttribute is
       *  encountered on traversal. The TextureAttribute \a texAttrib is already write-locked.\n
       *  The base implementation just calls Traverser::traverseTextureAttribute().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseTextureAttribute */
      NVSG_API virtual void handleTextureAttribute( nvsg::TextureAttribute * texAttrib );

      /*! \brief Handler function for a TextureAttributeItem.
       *  \param texAttribItem A pointer to the write-locked TextureAttributeItem being traversed.
       *  \remarks This function is called from the framework whenever a TextureAttributeItem is
       *  encountered on traversal. The TextureAttributeItem \a texAttribItem is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleTextureAttributeItem( nvsg::TextureAttributeItem * texAttribItem );

      /*! \brief Handler function for a AlphaTestAttribute.
      *  \param alphaTestAttrib A pointer to the write-locked AlphaTestAttribute being traversed.
      *  \remarks This function is called from the framework whenever a AlphaTestAttribute is
      *  encountered on traversal. The AlphaTestAttribute \a alphaTestAttrib is already write-locked.\n
      *  The base implementation just does nothing.
      *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it is
      *  recommended to always call the base class implementation in order to properly continue
      *  traversal. */
      NVSG_API virtual void handleAlphaTestAttribute( nvsg::AlphaTestAttribute * alphaTestAttrib );

      /*! \brief Handler function for a BlendAttribute.
       *  \param blendAttrib A pointer to the write-locked BlendAttribute being traversed.
       *  \remarks This function is called from the framework whenever a BlendAttribute is
       *  encountered on traversal. The BlendAttribute \a blendAttrib is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleBlendAttribute( nvsg::BlendAttribute * blendAttrib );

      /*! \brief Handler function for a DepthAttribute.
       *  \param depthAttrib A pointer to the write-locked DepthAttribute being traversed.
       *  \remarks This function is called from the framework whenever a DepthAttribute is
       *  encountered on traversal. The DepthAttribute \a depthAttrib is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleDepthAttribute( nvsg::DepthAttribute * depthAttrib );

      /*! \brief Handler function for a StencilAttribute.
       * \param stencilAttrib Specifies the StencilAttriburte to process.
       * \remarks This function is called from the framework whenever a StencilAttribute is
       * encountered on traversal. The StencilAttribute attribute will be write-locked by the
       * framework prior to call this handler function. The write lock will be released again,
       * after the function returns. The base implementation performs no operation. */
      NVSG_API virtual void handleStencilAttribute( nvsg::StencilAttribute * stencilAttrib );

      /*! \brief Handler function for a FaceAttribute.
       *  \param faceAttrib A pointer to the write-locked FaceAttribute being traversed.
       *  \remarks This function is called from the framework whenever a FaceAttribute is
       *  encountered on traversal. The FaceAttribute \a faceAttrib is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleFaceAttribute( nvsg::FaceAttribute * faceAttrib );

      /*! \brief Handler function for a LightingAttribute.
       *  \param laAttrib A pointer to the read-locked LightingAttribute being traversed.
       *  \remarks This function is called from the framework whenever a LightingAttribute is
       *  encountered on traversal. The LightingAttribute \a laAttrib is already read-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleLightingAttribute( nvsg::LightingAttribute * laAttrib );

      /*! \brief Handler function for a LineAttribute.
       *  \param lineAttrib A pointer to the write-locked LineAttribute being traversed.
       *  \remarks This function is called from the framework whenever a LineAttribute is
       *  encountered on traversal. The LineAttribute \a lineAttrib is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleLineAttribute( nvsg::LineAttribute * lineAttrib );

      /*! \brief Handler function for a PointAttribute.
       *  \param pointAttrib A pointer to the write-locked PointAttribute being traversed.
       *  \remarks This function is called from the framework whenever a PointAttribute is
       *  encountered on traversal. The PointAttribute \a pointAttrib is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handlePointAttribute( nvsg::PointAttribute * pointAttrib );

      /*! \brief Handler function for an UnlitColorAttribute.
       *  \param unlitColor A pointer to the write-locked UnlitColorAttribute being traversed.
       *  \remarks This function is called from the framework whenever an UnlitColorAttribute is
       *  encountered on traversal. The UnlitColorAttribute \a unlitColor is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleUnlitColorAttribute( nvsg::UnlitColorAttribute * unlitColor );

      /*! \brief Handler function for a MapArray.
       *  \param mapArray A pointer to the write-locked MapArray being traversed.
       *  \remarks This function is called from the framework whenever a MapArray is encountered on
       *  traversal. The MapArray \a mapArray is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleMapElement, handleMapMatrix */
      NVSG_API virtual void handleMapArray( nvsg::MapArray * mapArray );

      /*! \brief Handler function for a MapElement.
       *  \param mapElement A pointer to the write-locked MapElement being traversed.
       *  \remarks This function is called from the framework whenever a MapElement is
       *  encountered on traversal. The MapElement \a mapElement is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleMapArray, handleMapMatrix */
      NVSG_API virtual void handleMapElement( nvsg::MapElement * mapElement );

      /*! \brief Handler function for a MapMatrix.
       *  \param mapMatrix A pointer to the write-locked MapMatrix being traversed.
       *  \remarks This function is called from the framework whenever a MapMatrix is encountered on
       *  traversal. The MapMatrix \a mapMatrix is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa handleMapArray, handleMapElement */
      NVSG_API virtual void handleMapMatrix( nvsg::MapMatrix * mapMatrix );

      /*! \brief Handler function for a MapList.
       *  \param mapList A pointer to the write-locked MapList being traversed.
       *  \remarks This function is called from the framework whenever a MapList is encountered on
       *  traversal. The MapList \a mapList is already write-locked.\n
       *  The base implementation just calls Traverser::traverseMapList().
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseMapList */
      NVSG_API virtual void handleMapList( nvsg::MapList * mapList );

      /*! \brief Handler function for a VertexAttributeSet.
       *  \param vas A pointer to the write-locked VertexAttributeSet being traversed.
       *  \remarks This function is called from the framework whenever a VertexAttributeSet is
       *  encountered on traversal. The VertexAttributeSet \a vas is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleVertexAttributeSet( nvsg::VertexAttributeSet *vas );

      /*! \brief Handler function for an AnimatedVertexAttributeSet.
       *  \param avas A pointer to the write-locked AnimatedVertexAttributeSet being traversed.
       *  \remarks This function is called from the framework whenever an AnimatedVertexAttributeSet
       *  is encountered on traversal. The AnimatedVertexAttributeSet \a avas is already write-locked.\n
       *  The base implementation traverses each VertexAttributeAnimation of \a avas and holds
       *  the index of the currently traversed VertexAttributeAnimation in the protected
       *  member variable \c m_currentAttrib. */
      NVSG_API virtual void handleAnimatedVertexAttributeSet( nvsg::AnimatedVertexAttributeSet * avas );

      /*! \brief Handler function for a Skin.
       *  \param skin A pointer to the write-locked Skin being traversed.
       *  \remarks This function is called from the framework whenever a Skin is encountered on
       *  traversal. The Skin \a skin is already read-locked.\n
       *  The base implementation just does nothing. */
      NVSG_API virtual void handleSkin( nvsg::Skin * skin );

      /*! \brief Handle function for an Animation of indices.
       *  \param anim A pointer to the write-locked Animation being traversed.
       *  \remarks This function is called from the framework whenever an Animation of indices is
       *  encountered on traversal. The Animation \a anim is already write-locked.\n
       *  The base implementation just does nothing. */
      NVSG_API virtual void handleIndexAnimation( nvsg::Animation<unsigned int> * anim );

      /*! \brief Handle function for an Animation of Trafos
       *  \param anim A pointer to the write-locked Animation being traversed.
       *  \remarks This function is called from the framework whenever an Animation of Trafos is
       *  encountered on traversal. The Animation \a anim is already write-locked.\n
       *  The base implementation just does nothing. */
      NVSG_API virtual void handleTrafoAnimation( nvsg::Animation<nvmath::Trafo> * anim );

      /*! \brief Handle function for an Animation of VertexAttributes.
       *  \param anim A pointer to the write-locked Animation being traversed.
       *  \remarks This function is called from the framework whenever an Animation of
       *  VertexAttributes is encountered on traversal. Then Animation \a anim is already
       *  write-locked.\n
       *  The base implementation just does nothing. */
      NVSG_API virtual void handleVertexAttributeAnimation( nvsg::Animation<nvsg::VertexAttribute> * anim );

      /*! \brief Handler function for a FramedAnimationDescription<unsigned int>.
       *  \param anim A pointer to the write-locked FramedAnimationDescription<unsigned int> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  FramedAnimationDescription<unsigned int> is encountered on traversal. The
       *  FramedAnimationDescription<unsigned int> \a anim is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleFramedIndexAnimationDescription( nvsg::FramedAnimationDescription<unsigned int> * anim );

      /*! \brief Handler function for a FramedAnimationDescription<nvmath::Trafo>.
       *  \param trafo A pointer to the write-locked FramedAnimationDescription<nvmath::Trafo> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  FramedAnimationDescription<nvmath::Trafo> is encountered on traversal. The
       *  FramedAnimationDescription<nvmath::Trafo> \a trafo is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleFramedTrafoAnimationDescription( nvsg::FramedAnimationDescription<nvmath::Trafo> * trafo );

      /*! \brief Handler function for a FramedAnimationDescription<VertexAttribute>.
       *  \param p A pointer to the write-locked FramedAnimationDescription<VertexAttribute> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  FramedAnimationDescription<VertexAttribute> is encountered on traversal. The
       *  FramedAnimationDescription<VertexAttribute> \a p is already write-locked.\n
       *  The base implementation just does nothing
       *  \sa handleLinearInterpolatedVertexAttributeAnimationDescription */
      NVSG_API virtual void handleFramedVertexAttributeAnimationDescription( nvsg::FramedAnimationDescription<nvsg::VertexAttribute> * p );

      /*! \brief Handler function for a LinearInterpolatedAnimationDescription<nvmath::Trafo>.
       *  \param trafo A pointer to the write-locked LinearInterpolatedAnimationDescription<nvmath::Trafo> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  LinearInterpolatedAnimationDescription<nvmath::Trafo> is encountered on traversal. The
       *  LinearInterpolatedAnimationDescription<nvmath::Trafo> \a trafo is already write-locked.\n
       *  The base implementation just does nothing.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal. */
      NVSG_API virtual void handleLinearInterpolatedTrafoAnimationDescription( nvsg::LinearInterpolatedAnimationDescription<nvmath::Trafo> * trafo );

      /*! \brief Handler function for a LinearInterpolatedAnimationDescription<VertexAttribute>.
       *  \param p A pointer to the write-locked LinearInterpolatedAnimationDescription<VertexAttribute> being traversed.
       *  \remarks This function is called from the framework whenever a
       *  LinearInterpolatedAnimationDescription<VertexAttribute> is encountered on traversal. The
       *  LinearInterpolatedAnimationDescription<VertexAttribute> \a p is already write-locked.\n
       *  The base implementation just does nothing
       *  \sa handleFramedVertexAttributeAnimationDescription */
      NVSG_API virtual void handleLinearInterpolatedVertexAttributeAnimationDescription( nvsg::LinearInterpolatedAnimationDescription<nvsg::VertexAttribute> * p );

      /*! \brief Handler function for an RTFxSceneAttribute.
       *  \param rtfxsa A pointer to the read-locked RTFxSceneAttribute being traversed.
       *  \remarks This function is called from the framework whenever an RTFxSceneAttribute is encountered
       *  on traversal. The RTFxSceneAttribute \a rtfxsa is already read-locked.\n
       *  The base implementation just calls Traverser::traverseSceneAttribute().
       *  \note When this function is overridden by a traverser derived from SharedTraverser, it is
       *  recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa traverseSceneAttribute */
      NVSG_API virtual void handleRTFxSceneAttribute( nvsg::RTFxSceneAttribute * rtfxsa );

      /*! \brief Function for common handling of Camera classes.
       *  \param camera A pointer to the write-locked Camera being traversed.
       *  \remarks This function is called from the framework whenever a Camera is encountered
       *  on traversal. The Camera \a camera is already write-locked.\n
       *  The base implementation traverses the headlights of \a camera.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa LightSource */
      NVSG_API virtual void traverseCamera( nvsg::Camera * camera );

      /*! \brief Function for common handling of FrustumCamera classes.
       *  \param camera A pointer to the write-locked FrustumCamera being traversed.
       *  \remarks This function is called from the framework whenever a FrustumCamera is encountered
       *  on traversal. The FrustumCamera \a camera is already write-locked.\n
       *  The base implementation traverses the headlights of \a camera.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa LightSource */
      NVSG_API virtual void traverseFrustumCamera( nvsg::FrustumCamera * camera );

      /*! \brief Function for common handling of LightSource classes.
       *  \param light A pointer to the write-locked LightSource being traversed.
       *  \remarks This function is called from the framework whenever a LightSource is encountered
       *  on traversal. The LightSource \a light is already write-locked.\n
       *  The base implementation traverses the Animation associated with \a light, if there is one.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa TrafoAnimation */
      NVSG_API virtual void traverseLight( nvsg::LightSource * light );

      /*! \brief Function for common handling of PrimitiveSet classes.
       *  \param pset A pointer to the write-locked PrimitiveSet being traversed.
       *  \remarks This function is called from the framework whenever a PrimitiveSet is encountered
       *  on traversal. The PrimitiveSet \a pset is already write-locked.\n
       *  The base implementation first traverses the Skin, if there is one, and then the
       *  VertexAttributeSet of \a pset, if there is one.
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa VertexAttributeSet, Skin */
      NVSG_API virtual void traversePrimitiveSet( nvsg::PrimitiveSet * pset );

      /*! \brief Function for common handling of Primitive classes.
       *  \param p A pointer to the write-locked Primitive being traversed.
       *  \remarks This function is called from the framework whenever a Primitive is encountered
       *  on traversal. The Primitive \a p is already write-locked.\n
       *  The base implementation first traverses the Skin, if there is one, and then the
       *  VertexAttributeSet of \a p, if there is one. 
       *  \note When this function is overridden by a traverser derived from ExclusiveTraverser, it
       *  is recommended to always call the base class implementation in order to properly continue
       *  traversal.
       *  \sa VertexAttributeSet, Skin */
      NVSG_API virtual void traversePrimitive( nvsg::Primitive * p );

    protected:
      std::auto_ptr<nvsg::CameraWriteLock>    m_lockedCamera;
      std::auto_ptr<nvsg::SceneWriteLock>     m_lockedScene;
      std::auto_ptr<nvsg::ViewStateWriteLock> m_lockedViewState;

    private:
      bool                  m_treeModified;
  };

  //--- Inline functions ------------------

  inline void Traverser::postTraverseObject( const nvsg::Object * object )
  {
    NVSG_TRACE();
    NVSG_ASSERT( object );
  }

  inline void Traverser::setTraversalMask( unsigned int mask )
  {
    NVSG_TRACE();
    m_traversalMask = mask;
  }

  inline void Traverser::setTraversalMaskOverride( unsigned int mask )
  {
    NVSG_TRACE();
    m_traversalMaskOverride = mask;
  }

  inline unsigned int Traverser::getTraversalMaskOverride() const
  {
    NVSG_TRACE();
    return m_traversalMaskOverride;
  }

  inline unsigned int Traverser::getTraversalMask() const
  {
    NVSG_TRACE();
    return m_traversalMask;
  }

  inline unsigned int SharedTraverser::getTraversalMask() const
  {
    NVSG_TRACE();

    return m_lockedViewState.get() ? (*m_lockedViewState)->getTraversalMask() : Traverser::getTraversalMask();
  }

  inline unsigned int ExclusiveTraverser::getTraversalMask() const
  {
    NVSG_TRACE();

    return m_lockedViewState.get() ? (*m_lockedViewState)->getTraversalMask() : Traverser::getTraversalMask();
  }

  inline void Traverser::interruptTraversal()
  {
    NVSG_TRACE();
    m_interrupt = true;
  }

  inline bool Traverser::wasTraversalInterrupted()
  {
    NVSG_TRACE();
    return !!(m_traverserBits & NVSG_TRAVERSAL_INTERRUPTED);
  }

  inline void Traverser::traverseObject( const nvsg::ObjectSharedPtr & obj )
  {
    NVSG_TRACE();
    if ( m_interrupt )
    {
      // there was an interrupt request.
      // directly quit traversal
      m_traverserBits |= NVSG_TRAVERSAL_INTERRUPTED;
      return;
    }
    doTraverseObject( obj );
  }

  inline void Traverser::traverseLockedObject(const nvsg::Object * obj)
  {
    unsigned int oc = getObjectTraversalCode( obj );
    if ( (nvsg::OC_INVALID != oc) && 
         (( obj->getTraversalMask() | getTraversalMaskOverride() ) & getTraversalMask()) 
       )
    {
#if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
      void * __esp__0;
      void * __esp__1;
      __asm mov __esp__0, esp
#endif
      (this->*(void (Traverser::*)(const nvsg::Object*))m_mftbl[oc])(obj);
#if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
      __asm mov __esp__1, esp
      NVSG_ASSERT(__esp__1==__esp__0);
#endif
    }
  }

  inline unsigned int Traverser::getCurrentTextureUnit() const
  {
    return( m_currentTextureUnit );
  }

  template <typename T, typename U> 
  inline void SharedTraverser::addObjectHandler(unsigned int objectCode, void (T::*handler)(const U*))
  {
    NVSG_CTASSERT(SUPERSUBCLASS(SharedTraverser,T));
    m_mftbl.addEntry(objectCode, handler);
  }

  template <typename T, typename U, typename V> 
  inline void SharedTraverser::addObjectHandler(unsigned int objectCode, void (T::*handler)(const U*, V))
  {
    NVSG_CTASSERT(SUPERSUBCLASS(SharedTraverser,T));
    m_mftbl.addEntry(objectCode, handler);
  }

  inline void SharedTraverser::doTraverseObject( const nvsg::ObjectSharedPtr & p )
  {
    traverseLockedObject( nvsg::ObjectReadLock(p) );
  }

  template <typename T, typename U> 
  inline void ExclusiveTraverser::addObjectHandler(unsigned int objectCode, void (T::*handler)(U*))
  {
    NVSG_CTASSERT(SUPERSUBCLASS(ExclusiveTraverser,T));
    m_mftbl.addEntry(objectCode, handler);
  }

  template <typename T, typename U, typename V> 
  inline void ExclusiveTraverser::addObjectHandler(unsigned int objectCode, void (T::*handler)(U*, V))
  {
    NVSG_CTASSERT(SUPERSUBCLASS(ExclusiveTraverser,T));
    m_mftbl.addEntry(objectCode, handler);
  }

  inline void ExclusiveTraverser::doTraverseObject( const nvsg::ObjectSharedPtr & p )
  {
    nvsg::ObjectWriteLock lock(p);
    nvutil::Incarnation i = lock->getIncarnation();
    traverseLockedObject( lock );
    m_treeModified |= ( i != lock->getIncarnation() );
  }

  inline bool ExclusiveTraverser::getTreeModified( void ) const
  {
    return( m_treeModified );
  }

} // namespace nvtraverser

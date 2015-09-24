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
/** @file */

#include "nvsgcommon.h"
#include "nvmath/Planent.h"
#include "nvsg/ClipPlane.h"
#include "nvsg/Node.h"
#include "nvutil/HashGenerator.h"
#include "nvutil/SceniXIterator.h"

namespace nvsg
{
  /*! \brief Provides grouping of Node objects
    * \par Namespace: nvsg
    * \remarks
    * A Group is a special Node that provides grouping of all kinds of Node-derived objects.
    * Nodes that are grouped underneath a Group are referred to as children of the Group.
    */
  class Group : public Node
  {
    public:
      NVSG_API static GroupSharedPtr create();

    public:
      /*! \brief The container type of the Groups children */
      typedef nvutil::STLVector<NodeSharedPtr>                                    ChildrenContainer;

      /*! \brief The iterator over the ChildrenContainer */
      typedef nvutil::SceniXIterator<Group,ChildrenContainer::iterator>           ChildrenIterator;

      /*! \brief The const iterator over the ChildrenContainer */
      typedef nvutil::SceniXIterator<Group,ChildrenContainer::const_iterator>     ChildrenConstIterator;

      /*! \brief The container type of the Groups clip planes */
      typedef nvutil::STLVector<ClipPlaneSharedPtr>                               ClipPlaneContainer;

      /*! \brief The iterator over the ClipPlaneContainer */
      typedef nvutil::SceniXIterator<Group,ClipPlaneContainer::iterator>          ClipPlaneIterator;

      /*! \brief The const iterator over the ClipPlaneContainer */
      typedef nvutil::SceniXIterator<Group,ClipPlaneContainer::const_iterator>    ClipPlaneConstIterator;

      /*! \brief The type of the container to hold the Group’s light sources */
      typedef nvutil::STLVector<LightSourceSharedPtr>                             LightSourceContainer;

      /*! \brief Iterator type to be used with light source iteration */
      typedef nvutil::SceniXIterator<Group,LightSourceContainer::iterator>        LightSourceIterator;

      /*! \brief Iterator type to be used to iterate over light sources of a const Group */
      typedef nvutil::SceniXIterator<Group,LightSourceContainer::const_iterator>  LightSourceConstIterator;

    public:
      /*! \brief Get the number of children nodes in this Group
       *  \return The number of children nodes in this Group.
       *  \sa beginChildren, endChildren, addChild, insertChild, removeChild, replaceChild, clearChildren, findChild */
      NVSG_API unsigned int getNumberOfChildren() const;

      /*! \brief Get a const iterator to the first child in this Group.
       *  \return A const iterator to the first child in this Group.
       *  \sa getNumberOfChildren, endChildren, addChild, insertChild, removeChild, replaceChild, clearChildren, findChild */
      NVSG_API ChildrenConstIterator beginChildren() const;

      /*! \brief Get an iterator to the first child in this Group.
       *  \return An iterator to the first child in this Group.
       *  \sa getNumberOfChildren, endChildren, addChild, insertChild, removeChild, replaceChild, clearChildren, findChild */
      NVSG_API ChildrenIterator beginChildren();

      /*! \brief Get a const iterator that points just beyond the end of the children in this Group.
       *  \return A const iterator that points just beyond the end of the children in this Group.
       *  \sa getNumberOfChildren, beginChildren, addChild, insertChild, removeChild, replaceChild, clearChildren, findChild */
      NVSG_API ChildrenConstIterator endChildren() const;

      /*! \brief Get an iterator that points just beyond the end of the children in this Group.
       *  \return An iterator that points just beyond the end of the children in this Group.
       *  \sa getNumberOfChildren, beginChildren, addChild, insertChild, removeChild, replaceChild, clearChildren, findChild */
      NVSG_API ChildrenIterator endChildren();

      /*! \brief Add a child node at the end of the children in this Group.
       *  \param child The child node to add.
       *  \return An iterator that points to the position where \a child was added to the children.
       *  \sa getNumberOfChildren, beginChildren, endChildren, insertChild, removeChild, replaceChild, clearChildren, findChild */
      NVSG_API ChildrenIterator addChild( const NodeSharedPtr & child );

      /*! \brief Insert a child node at a specified position in this Group.
       *  \param gci An iterator that points to the position where \a child is inserted.
       *  \param child The child node to add.
       *  \return An iterator that points to the position where \a child was inserted.
       *  \sa getNumberOfChildren, beginChildren, endChildren, addChild, removeChild, replaceChild, clearChildren, findChild */
      NVSG_API ChildrenIterator insertChild( const ChildrenIterator & gci, const NodeSharedPtr & child );

      /*! \brief Remove all occurances of a specified Node from this Group.
       *  \param child The node to remove from this Group.
       *  \return \c true, if at least one Node has been removed from this Group, otherwise \c false.
       *  \note A Group might hold the same Node multiple times. This function removes all of them.
       *  \sa getNumberOfChildren, beginChildren, endChildren, addChild, insertChild, replaceChild, clearChildren, findChild */
      NVSG_API bool removeChild( const NodeSharedPtr & child );

      /*! \brief Remove a specified Node from this Group.
       *  \param childIterator An iterator that points to the child to be removed.
       *  \return An iterator pointing to the new location of the child that followed the Node removed by this
       *  function call, which is endChildren() if the operation removed the last child.
       *  \sa getNumberOfChildren, beginChildren, endChildren, addChild, insertChild, replaceChild, clearChildren, findChild */
      NVSG_API ChildrenIterator removeChild( const ChildrenIterator & childIterator );

      /*! \brief Replace all occurances of a specified Node in this Group by an other.
       *  \param newChild The new Node that replaces the old ones.
       *  \param oldChild The old Node to be replaced by the new one.
       *  \return \c true, if at least one Node has been replaced, otherwise \c false.
       *  \sa getNumberOfChildren, beginChildren, endChildren, addChild, insertChild, removeChild, clearChildren, findChild */
      NVSG_API bool replaceChild( const NodeSharedPtr & newChild, const NodeSharedPtr & oldChild );

      /*! \brief Replace a specified Node in this Group by an other.
       *  \param newChild The new Node that replaces the old one.
       *  \param oldChildIterator An iterator pointing to the Node to be replaced.
       *  \return \c true if the specified Node has been replaced, otherwise \c false.
       *  \sa getNumberOfChildren, beginChildren, endChildren, addChild, insertChild, removeChild, clearChildren, findChild */
      NVSG_API bool replaceChild( const NodeSharedPtr & newChild, ChildrenIterator & oldChildIterator );

      /*! \brief Remove all the children from this Group.
       *  \sa getNumberOfChildren, beginChildren, endChildren, addChild, insertChild, removeChild, replaceChild, findChild */
      NVSG_API void clearChildren();

      /*! \brief Find the first occurance of a specified Node in this Group.
       *  \param start A const iterator into the children in this Group, where the search is to start.
       *  \param node The Node to be found in this Group.
       *  \return A const iterator pointing to the first occurance of the specified node in this Group.
       *  \note To cound the multiplicity of a child in a Group, one could do something like that:
       *  \code
       *  unsigned int count = 0;
       *  for ( Group::ChildrenConstIterator gcci = group->findChild( group->beginChildren(), child )
       *      ; gcci != group->endChildren()
       *      ; gcci = group->findChild( ++gcci, child ) )
       *  {
       *    count++;
       *  }
       *  \endcode
       *  \sa getNumberOfChildren, beginChildren, endChildren, addChild, insertChild, removeChild, replaceChild, clearChildren */
      NVSG_API ChildrenConstIterator findChild( const ChildrenConstIterator & start, const NodeSharedPtr & node ) const;

      /*! \brief Find the first occurance of a specified Node in this Group.
       *  \param start An iterator into the children in this Group, where the search is to start.
       *  \param node The Node to be found in this Group.
       *  \return An iterator pointing to the first occurance of the specified node in this Group.
       *  \note To cound the multiplicity of a child in a Group, one could do something like that:
       *  \code
       *  unsigned int count = 0;
       *  for ( Group::ChildrenIterator gci = group->findChild( group->beginChildren(), child )
       *      ; gci != group->endChildren()
       *      ; gci = group->findChild( ++gci, child ) )
       *  {
       *    count++;
       *  }
       *  \endcode
       *  \sa getNumberOfChildren, beginChildren, endChildren, addChild, insertChild, removeChild, replaceChild, clearChildren */
      NVSG_API ChildrenIterator findChild( const ChildrenIterator & start, const NodeSharedPtr & node );

      /*! \brief Get the number of clip planes in this Group.
       *  \return The number of clip planes on this Group.
       *  \sa getNumberOfActiveClipPlanes, beginClipPlanes, endClipPlanes, addClipPlane, removeClipPlane, clearClipPlanes, findClipPlane */
      NVSG_API unsigned int getNumberOfClipPlanes() const;

      /*! \brief Get the number of active clip planes in this Group.
       *  \return The number of active clip planes in this Group.
       *  \sa getNumberOfClipPlanes, beginClipPlanes, endClipPlanes, addClipPlane, removeClipPlane, clearClipPlanes, findClipPlane */
      NVSG_API unsigned int getNumberOfActiveClipPlanes() const;

      /*! \brief Get a const iterator to the first clip plane in this Group.
       *  \return A const iterator to the first clip plane in this Group.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, endClipPlanes, addClipPlane, removeClipPlane, clearClipPlanes, findClipPlane */
      NVSG_API ClipPlaneConstIterator beginClipPlanes() const;

      /*! \brief Get an iterator to the first clip plane in this Group.
       *  \return An iterator to the first clip plane in this Group.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, endClipPlanes, addClipPlane, removeClipPlane, clearClipPlanes, findClipPlane */
      NVSG_API ClipPlaneIterator beginClipPlanes();

      /*! \brief Get a const iterator that points just beyond the end of the clip planes in this Group.
       *  \return A const iterator that points just beyond the end of the clip planes in this Group.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, beginClipPlanes, addClipPlane, removeClipPlane, clearClipPlanes, findClipPlane */
      NVSG_API ClipPlaneConstIterator endClipPlanes() const;

      /*! \brief Get an iterator that points just beyond the end of the clip planes in this Group.
       *  \return An iterator that points just beyond the end of the clip planes in this Group.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, beginClipPlanes, addClipPlane, removeClipPlane, clearClipPlanes, findClipPlane */
      NVSG_API ClipPlaneIterator endClipPlanes();

      /*! \brief Adds a user clipping plane to this Group.
       *  \param plane Specifies the ClipPlane to add
       *  \return An iterator that points to the position where \a plane was added.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, beginClipPlanes, endClipPlanes, removeClipPlane, clearClipPlanes, findClipPlane */
      NVSG_API ClipPlaneIterator addClipPlane( const ClipPlaneSharedPtr & plane );

      /*! \brief Remove a clip plane from this Group.
       *  \param plane The clip plane to remove from this Group.
       *  \return \c true, if the clip plane has been removed from this Group, otherwise \c false.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, beginClipPlanes, endClipPlanes, addClipPlane, clearClipPlanes, findClipPlane */
      NVSG_API bool removeClipPlane( const ClipPlaneSharedPtr & plane );

      /*! \brief Remove a clip plane from this Group.
       *  \param cpi An iterator to the clip plane to remove from this Group.
       *  \return An iterator pointing to the new location of the clip plane that followed the one removed by
       *  this function call, which is endClipPlanes() if the operation removed the last clip plane.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, beginClipPlanes, endClipPlanes, addClipPlane, clearClipPlanes, findClipPlane */
      NVSG_API ClipPlaneIterator removeClipPlane( const ClipPlaneIterator & cpi );

      /*! \brief Remove all clip planes from this Group.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, beginClipPlanes, endClipPlanes, addClipPlane, removeClipPlane, findClipPlane */
      NVSG_API void clearClipPlanes();

      /*  \brief Find a specified clip plane in this Group.
       *  \param plane The clip plane to find.
       *  \return A const iterator to the found clip plane in this Group.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, beginClipPlanes, endClipPlanes, addClipPlane, removeClipPlane, clearClipPlanes */
      NVSG_API ClipPlaneConstIterator findClipPlane( const ClipPlaneSharedPtr & plane ) const;

      /*  \brief Find a specified clip plane in this Group.
       *  \param plane The clip plane to find.
       *  \return An iterator to the found clip plane in this Group.
       *  \sa getNumberOfClipPlanes, getNumberOfActiveClipPlanes, beginClipPlanes, endClipPlanes, addClipPlane, removeClipPlane, clearClipPlanes */
      NVSG_API ClipPlaneIterator findClipPlane( const ClipPlaneSharedPtr & plane );


      /*! \brief Get the number of light sources in this Group.
       *  \return The number of light sources in this Group.
       *  \sa beginLightSources, endLightSources, addLightSource, removeLightSource, clearLightSources, findLightSource */
      NVSG_API unsigned int getNumberOfLightSources() const;

      /*! \brief Get a const iterator to the first light source in this Group.
       *  \return A const iterator to the first light source in this Group.
       *  \sa getNumberOfLightSources, endLightSources, addLightSource, removeLightSource, clearLightSources, findLightSource */
      NVSG_API LightSourceConstIterator beginLightSources() const;

      /*! \brief Get an iterator to the first light source in this Group.
       *  \return An iterator to the first light source in this Group.
       *  \sa getNumberOfLightSources, endLightSources, addLightSource, removeLightSource, clearLightSources, findLightSource */
      NVSG_API LightSourceIterator beginLightSources();

      /*! \brief Get a const iterator that points just beyond the end of the light source in this Group.
       *  \return A const iterator that points just beyond the end of the light source in this Group.
       *  \sa getNumberOfLightSources, beginLightSources, addLightSource, removeLightSource, clearLightSources, findLightSource */
      NVSG_API LightSourceConstIterator endLightSources() const;

      /*! \brief Get an iterator that points just beyond the end of the light source in this Group.
       *  \return An iterator that points just beyond the end of the light source in this Group.
       *  \sa getNumberOfLightSources, beginLightSources, addLightSource, removeLightSource, clearLightSources, findLightSource */
      NVSG_API LightSourceIterator endLightSources();

      /*! \brief Adds a light source to this Group.
       *  \param lightSource Specifies the light source to add
       *  \return An iterator that points to the position where \a lightSource was added.
       *  \sa getNumberOfLightSources, beginLightSources, endLightSources, removeLightSource, clearLightSources, findLightSource */
      NVSG_API LightSourceIterator addLightSource( const LightSourceSharedPtr & lightSource );

      /*! \brief Remove a light source from this Group.
       *  \param lightSource The light source to remove from this Group.
       *  \return \c true, if the light source has been removed from this Group, otherwise \c false.
       *  \sa getNumberOfLightSources, beginLightSources, endLightSources, addLightSource, clearLightSources, findLightSource */
      NVSG_API bool removeLightSource( const LightSourceSharedPtr & lightSource );

      /*! \brief Remove a light source from this Group.
       *  \param lsi An iterator to the light source to remove from this Group.
       *  \return An iterator pointing to the new location of the light source that followed the one removed by
       *  this function call, which is endLightSources() if the operation removed the last light source.
       *  \sa getNumberOfLightSources, beginLightSources, endLightSources, addLightSource, clearLightSources, findLightSource */
      NVSG_API LightSourceIterator removeLightSource( const LightSourceIterator & lsi );

      /*! \brief Remove all light sources from this Group.
       *  \sa getNumberOfLightSources, beginLightSources, endLightSources, addLightSource, removeLightSource, findLightSource */
      NVSG_API void clearLightSources();

      /*  \brief Find a specified light source in this Group.
       *  \param lightSource The light source to find.
       *  \return A const iterator to the found light source in this Group.
       *  \sa getNumberOfLightSources, beginLightSources, endLightSources, addLightSource, removeLightSource, clearLightSources */
      NVSG_API LightSourceConstIterator findLightSource( const LightSourceSharedPtr & lightSource ) const;

      /*  \brief Find a specified light source in this Group.
       *  \param lightSource The light source to find.
       *  \return An iterator to the found lightSource in this Group.
       *  \sa getNumberOfLightSources, beginLightSources, endLightSources, addLightSource, removeLightSource, clearLightSources */
      NVSG_API LightSourceIterator findLightSource( const LightSourceSharedPtr & lightSource );

      /*! \brief Overrides Object::isEquivalent.  
        * \param
        * p Pointer to the Object to test for equivalence with this Group object.
        * \param
        * ignoreNames Object names will be ignored while testing if this is \c true.
        * \param
        * deepCompare The function performs a deep-compare instead of a shallow compare if this is \c true.
        * \return
        * The function returns \c true if the Object pointed to by \a object is detected to be equivalent
        * to this Group object.
        * \remarks
        * The test will be performed considering the optional control parameters ignoreNames and deepCompare. 
        * If you omit these two, the function ignores the object names
        * and performs a shallow compare only.
        * \sa Object::isEquivalent
        */
      NVSG_API virtual bool isEquivalent(const Object * p, bool ignoreNames, bool deepCompare) const;

      /*! \brief Assigns new content from another Group object. 
        * \param
        * rhs Group object from which to assign the new content.
        * \return
        * A reference to this Group object.
        * \remarks
        * The assignment operator unreferences the old content before assigning the new content. The new
        * content will be a deep-copy of the right-hand-sided object's content.
        * \sa Group::clone
        */
      NVSG_API Group & operator=(const Group & rhs);

      REFLECTION_INFO_API( NVSG_API, Group );
    protected:
      friend struct nvutil::Holder<Group>;

      /*! \brief Default-constructs a Group object.
        */
      NVSG_API Group();

      /*! \brief Constructs a Group as a copy of another Group.
        * \param
        * rhs Group to serve as source for the newly constructed Group. 
        */
      NVSG_API Group( const Group& rhs );

      /*! \brief Destructs a Group object.
        */
      NVSG_API virtual ~Group();

      /*! \brief Called from the framework immediately before a child will be removed from the Group.
        * \param
        * position Zero-based position of the child that immediately will be removed from the Group.
        * \remarks
        * It is recommended, for Group-derived classes that need to keep track of child nodes removal, 
        * to override this function to perform necessary tasks immediately before a child node will be 
        * removed from the Group.   
        * \n\n
        * The function will be called with the zero-based index that references the child that
        * will be removed immediately after this call.
        * \sa Group::postRemoveChild
        */
      NVSG_API virtual void preRemoveChild(unsigned int position);

      /*! \brief Called from the framework immediately after a child has been removed from the Group.
        * \param
        * position Zero-based position that previously referenced the removed child node.
        * \remarks
        * It is recommended, for Group-derived classes that need to keep track of child nodes removal, 
        * to override this notifier to perform necessary tasks immediately after a child node has been 
        * removed from the Group.   
        * \n\n
        * The function will be called with the zero-based position that previously referenced the 
        * removed child. Note that this position can no longer be used to reference the child node, 
        * because the child node already has been removed from this Group.
        * \sa Group::preRemoveChild
        */
      NVSG_API virtual void postRemoveChild(unsigned int position);

      /*! \brief Called from the framework immediately before a child will be added to the Group.
        * \param
        * position Zero-based position where the child node will be added.
        * \remarks
        * It is recommended, for Group-derived classes that need to keep track of child nodes insertion,
        * to override this notifier to perform necessary tasks immediately before a child node will be 
        * added to the Group.   
        * \n\n
        * The function will be called with the zero-based position where the child node will be added.
        * Note that this position cannot yet be used to reference the child node, because the child
        * node has not been added to this Group at this point.
        * \sa Group::postAddChild
        */
      NVSG_API virtual void preAddChild(unsigned int position);

      /*! \brief Called from the framework immediately after a child was added to the Group.
        * \param
        * position Zero-based position where the child node was added.
        * \remarks
        * It is recommended, for Group-derived classes that need to keep track of child nodes insertion,
        * to override this notifier to perform necessary tasks immediately after a child node has been 
        * added to the Group.   
        * \n\n
        * The function will be called with the zero-based position where the child node has been added
        * immediately before this call.
        * \sa Group::preAddChild
        */
      NVSG_API virtual void postAddChild(unsigned int position);

      /*! \brief Called from the framework if re-calculation of the Group's bounding box is required.
       * \return
       * The function returns a nvmath::Box3f that represents the actual bounding box of this Group.
       * \remarks
       * The function calculates the bounding box by accumulating the bounding boxes of all 
       * available child nodes. 
       * \n\n
       * For Group-derived classes to specialize bounding box calculation, it is recommended to 
       * override this function. 
       * \sa nvmath::Box3f
       */
      NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;

      /*! \brief Called from the framework if re-calculation of the Group's bounding sphere is required.
        * \return
        * The function returns a nvmath::Sphere3f that represents the actual bounding sphere of this 
        * Group.
        * \remarks
        * The function calculates the bounding sphere by accumulating the bounding spheres of all 
        * available child nodes. 
        * \n\n
        * For Group-derived classes to specialize bounding sphere calculation, it is recommended to 
        * override this function. 
        * \sa nvmath::Sphere3f
        */
      NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere() const;

      /*! \brief Determines Object hints contained in the hierarchy
       * \param hints
       * Specifies a but mask to indicate the Object hints for which containment should be determined.
       * \return 
       * The function returns a bit field indicating all hints contained in the tree hierarchy
       * having this Group as root. The bit field returned is filtered by the input bit mask.
       * \remarks
       * The function override the base class implementation to trigger determination for all
       * children of the Group, as well as for all maintained LightSources.
       * \sa Object::determineHintsContainment
       */
      NVSG_API virtual unsigned int determineHintsContainment( unsigned int hints ) const;
      
      NVSG_API virtual bool determineTransparencyContainment() const;
      NVSG_API virtual bool determineShaderContainment() const; 
      NVSG_API virtual bool determineLightContainment() const; 
      NVSG_API virtual bool determineLODContainment() const; 
      NVSG_API virtual bool determineAnimationContainment() const;
      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

      NVSG_API virtual void incrementSwitchIncarnation() const;
      NVSG_API virtual const nvutil::Incarnation & querySwitchIncarnation() const;

    private:
      // The following copy* and remove* functions do not notify observers about changes
      // Functions calling these functions must ensure that the events are being passed up.
      void copyChildren( const ChildrenContainer & children );
      void copyClipPlanes( const ClipPlaneContainer & clipPlanes );
      void copyLightSources( const LightSourceContainer & lightSources );

      void removeChildren();
      void removeClipPlanes();
      void removeLights();

    private:
		NVSG_API ChildrenContainer::iterator doInsertChild( const ChildrenContainer::iterator & gcci, const NodeSharedPtr & child );
      NVSG_API ChildrenContainer::iterator doRemoveChild( const ChildrenContainer::iterator & cci );
      NVSG_API ChildrenContainer::iterator doReplaceChild( ChildrenContainer::iterator & cci, const NodeSharedPtr & newChild );
      NVSG_API ClipPlaneContainer::iterator doRemoveClipPlane( const ClipPlaneContainer::iterator & cpci );
      NVSG_API LightSourceContainer::iterator doRemoveLightSource( const LightSourceContainer::iterator & lsci );

    private:
      ChildrenContainer     m_children;
      ClipPlaneContainer    m_clipPlanes;
      LightSourceContainer  m_lightSources;

      mutable nvutil::Incarnation m_switchIncarnation; // tracks switch changes
  };

  inline unsigned int Group::getNumberOfChildren() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_children.size()) );
  }

  inline Group::ChildrenConstIterator Group::beginChildren() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( ChildrenConstIterator( m_children.begin() ) );
  }

  inline Group::ChildrenIterator Group::beginChildren()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( ChildrenIterator( m_children.begin() ) );
  }

  inline Group::ChildrenConstIterator Group::endChildren() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( ChildrenConstIterator( m_children.end() ) );
  }

  inline Group::ChildrenIterator Group::endChildren()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( ChildrenIterator( m_children.end() ) );
  }

  inline Group::ChildrenIterator Group::addChild( const NodeSharedPtr & child )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( ChildrenIterator( doInsertChild( m_children.end(), child ) ) );
  }

  inline Group::ChildrenConstIterator Group::findChild( const ChildrenConstIterator & start, const NodeSharedPtr & node ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( ChildrenConstIterator( find( start.m_iter, m_children.end(), node ) ) );
  }

  inline Group::ChildrenIterator Group::findChild( const ChildrenIterator & start, const NodeSharedPtr & node )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( ChildrenIterator( find( start.m_iter, m_children.end(), node ) ) );
  }

  inline unsigned int Group::getNumberOfClipPlanes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_clipPlanes.size()) );
  }

  inline Group::ClipPlaneConstIterator Group::beginClipPlanes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( ClipPlaneConstIterator( m_clipPlanes.begin() ) );
  }

  inline Group::ClipPlaneIterator Group::beginClipPlanes()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( ClipPlaneIterator( m_clipPlanes.begin() ) );
  }

  inline Group::ClipPlaneConstIterator Group::endClipPlanes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( ClipPlaneConstIterator( m_clipPlanes.end() ) );
  }

  inline Group::ClipPlaneIterator Group::endClipPlanes()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( ClipPlaneIterator( m_clipPlanes.end() ) );
  }

  inline Group::ClipPlaneConstIterator Group::findClipPlane( const ClipPlaneSharedPtr & plane ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( ClipPlaneConstIterator( find( m_clipPlanes.begin(), m_clipPlanes.end(), plane ) ) );
  }

  inline Group::ClipPlaneIterator Group::findClipPlane( const ClipPlaneSharedPtr & plane )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( ClipPlaneIterator( find( m_clipPlanes.begin(), m_clipPlanes.end(), plane ) ) );
  }

  inline unsigned int Group::getNumberOfLightSources() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_lightSources.size()) );
  }

  inline Group::LightSourceConstIterator Group::beginLightSources() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( LightSourceConstIterator( m_lightSources.begin() ) );
  }

  inline Group::LightSourceIterator Group::beginLightSources()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( LightSourceIterator( m_lightSources.begin() ) );
  }

  inline Group::LightSourceConstIterator Group::endLightSources() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( LightSourceConstIterator( m_lightSources.end() ) );
  }

  inline Group::LightSourceIterator Group::endLightSources()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( LightSourceIterator( m_lightSources.end() ) );
  }

  inline Group::LightSourceConstIterator Group::findLightSource( const LightSourceSharedPtr & lightSource ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( LightSourceConstIterator( find( m_lightSources.begin(), m_lightSources.end(), lightSource ) ) );
  }

  inline Group::LightSourceIterator Group::findLightSource( const LightSourceSharedPtr & lightSource )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( LightSourceIterator( find( m_lightSources.begin(), m_lightSources.end(), lightSource ) ) );
  }

  inline void Group::incrementSwitchIncarnation() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    nvutil::AutoLock lock(m_mutableLock); // secure exclusive write access to mutable members!
    m_switchIncarnation++;
  }

  inline const nvutil::Incarnation & Group::querySwitchIncarnation() const
  {
    return( m_switchIncarnation );
  }

} //  namespace nvsg

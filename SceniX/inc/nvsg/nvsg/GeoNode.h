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

#include "nvsgcommon.h" // commonly used stuff
#include "nvsg/Node.h" // base class definition
// additional dependencies
#include "nvsg/VertexAttributeSet.h"
#include "nvutil/HashGenerator.h"
#include "nvutil/SceniXIterator.h"
#include <vector>

namespace nvsg
{
  /*! \brief Leaf Node to maintain geometry.
   *  \par Namespace: nvsg
   *  \remarks
   *  A GeoNode represents a leaf Node containing Drawable objects associated to StateSets,
   *  where an arbitrary number of Drawable objects can be asssociated to a single StateSet.
   *  Such a compound we also refer to as a 'geometry'. StateSets as well as Drawables are 
   *  unique within a GeoNode.\n
   *  A GeoNode provides an API to add, replace, and remove geometry. It also provides an API
   *  to iterate through the contained StateSets and Drawables, as well as accessing the 
   *  contained StateSets and Drawables for further processing.
   *  \sa Drawable, Node, Scene, StateSet */
  class GeoNode : public Node
  {
    public:
      NVSG_API static GeoNodeSharedPtr create();

    public:
      /*! \brief The container type of the GeoNode's Drawables */
      typedef nvutil::STLVector<DrawableSharedPtr>                              DrawableContainer;

      /*! \brief The iterator over the DrawableContainer */
      typedef nvutil::SceniXIterator<GeoNode,DrawableContainer::iterator>       DrawableIterator;

      /*! \brief The const iterator over the DrawableContainer */
      typedef nvutil::SceniXIterator<GeoNode,DrawableContainer::const_iterator> DrawableConstIterator;

      /*! \brief A helper structure to hold a StateSet and the associated Drawables. */
      struct Geometry
      {
        Geometry() {}
        explicit Geometry( const StateSetSharedPtr & stateSet) : m_stateSet(stateSet) {}

        StateSetSharedPtr m_stateSet;
        DrawableContainer m_drawables;
      };

      /*! \brief The container type of the GeoNode's StateSets and Drawables */
      typedef nvutil::STLVector<Geometry> StateSetContainer;

      /*! \brief The iterator over the StateSetContainer */
      class StateSetIterator : public nvutil::SceniXIterator<GeoNode,StateSetContainer::iterator>
      {
        public:
          StateSetIterator()
          {
          }

          explicit StateSetIterator( const StateSetContainer::iterator & ssci )
            : nvutil::SceniXIterator<GeoNode,StateSetContainer::iterator>( ssci )
          {
          }

          const StateSetSharedPtr & operator*() const
          {
            return( m_iter->m_stateSet );
          }

          const StateSetSharedPtr * operator->() const
          {
            return( &m_iter->m_stateSet );
          }

          bool operator==( const StateSetIterator &ssci ) const
          {
            return( nvutil::SceniXIterator<GeoNode,StateSetContainer::iterator>::operator==( ssci ) );
          }

          bool operator!=( const StateSetIterator &ssci ) const
          {
            return( nvutil::SceniXIterator<GeoNode,StateSetContainer::iterator>::operator!=( ssci ) );
          }
      };

      /*! \brief The const iterator over the StateSetContainer */
      class StateSetConstIterator : public nvutil::SceniXIterator<GeoNode,StateSetContainer::const_iterator>
      {
        public:
          StateSetConstIterator()
          {
          }

          explicit StateSetConstIterator( const StateSetContainer::const_iterator & sscci )
            : nvutil::SceniXIterator<GeoNode,StateSetContainer::const_iterator>( sscci )
          {
          }

          // It's ok to get a ConstIterator out of an Iterator.
          StateSetConstIterator( const StateSetIterator & ssi )
            : nvutil::SceniXIterator<GeoNode,StateSetContainer::const_iterator>( ssi.m_iter )
          {
          }

          const StateSetSharedPtr & operator*() const
          {
            return( m_iter->m_stateSet );
          }

          const StateSetSharedPtr * operator->() const
          {
            return( &m_iter->m_stateSet );
          }

          bool operator==( const StateSetConstIterator &ssci ) const
          {
            return( nvutil::SceniXIterator<GeoNode,StateSetContainer::const_iterator>::operator==( ssci ) );
          }

          bool operator!=( const StateSetConstIterator &ssci ) const
          {
            return( nvutil::SceniXIterator<GeoNode,StateSetContainer::const_iterator>::operator!=( ssci ) );
          }
      };

    public:
      /*! \brief Get the number of StateSets in this GeoNode
       *  \return The number of StateSets in this GeoNode.
       *  \sa beginStateSets, endStateSets, replaceStateSet, removeStateSet, clearStateSets, findStateSet */
      NVSG_API unsigned int getNumberOfStateSets() const;

      /*! \brief Returns a const iterator to the GeoNode’s first StateSet.
       *  \return A const iterator to the GeoNode’s first StateSet.
       *  \sa getNumberOfStateSets, endStateSets, replaceStateSet, removeStateSet, clearStateSets, findStateSet */
      NVSG_API StateSetConstIterator beginStateSets() const;

      /*! \brief Returns an iterator to the GeoNode’s first StateSet.
       *  \return An iterator to the GeoNode’s first StateSet.
       *  \sa getNumberOfStateSets, endStateSets, replaceStateSet, removeStateSet, clearStateSets, findStateSet */
      NVSG_API StateSetIterator beginStateSets();

      /*! \brief Returns a const iterator that points past the end of the GeoNode’s StateSets.
       *  \return A const iterator that points past the end of the GeoNode’s StateSets. If
       *  GeoNode::beginHeadLights() == GeoNode::endHeadLights(), the GeoNode has not been assigned any StateSet.
       *  \sa getNumberOfStateSets, beginStateSets, replaceStateSet, removeStateSet, clearStateSets, findStateSet */
      NVSG_API StateSetConstIterator endStateSets() const;

      /*! \brief Returns an iterator that points past the end of the GeoNode’s StateSets.
       *  \return An iterator that points past the end of the GeoNode’s StateSets. If
       *  GeoNode::beginHeadLights() == GeoNode::endHeadLights(), the GeoNode has not been assigned any StateSet.
       *  \sa getNumberOfStateSets, beginStateSets, replaceStateSet, removeStateSet, clearStateSets, findStateSet */
      NVSG_API StateSetIterator endStateSets();

      /*! \brief Replace a StateSet in this GeoNode by an other.
       *  \param newStateSet The new StateSet to replace an other in this GeoNode.
       *  \param oldStateSet The old StateSet to be replaced in this GeoNode.
       *  \return \c true, if replacing \a oldStateSet by \a newState was successful, otherwise \c false.
       *  \note The replacement is successful if \a newStateSet and \a oldStateSet are not the same objects, and
       *  \a oldStateSet is part of this GeoNode. If \a newStateSet already was part of this GeoNode, the drawables
       *  in this GeoNode associated with \a oldStateSet are moved to be associated with \a newStateSet, and \a
       *  oldStateSet is removed from this GeoNode. Otherwise \a newStateSet just replaces \a oldStateSet, thus
       *  implicitly changing the association of the Drawables from \a oldStatSet to \a newStateSet.
       *  \sa getNumberOfStateSets, beginStateSets, endStateSet, removeStateSet, clearStateSets, findStateSet */
      NVSG_API bool replaceStateSet( const StateSetSharedPtr & newStateSet, const StateSetSharedPtr & oldStateSet );

      /*! \brief Replace a StateSet in this GeoNode by an other.
       *  \param newStateSet The new StateSet to replace an other in this GeoNode.
       *  \param oldSSI An iterator specifying the old StateSet to be replaced in this GeoNode.
       *  \return \c true, if replacing the StateSet specified by \a oldSSI by \a newState was successful,
       *  otherwise \c false.
       *  \note The replacement is successful if \a newStateSet and the StateSet specified by \a oldSSI are
       *  not the same objects, and \a oldSSI is a valid StateSetIteartor into this GeoNode. If \a newStateSet
       *  already was part of this GeoNode, the drawables in this GeoNode associated with the StateSet specified
       *  by \a oldSSI are moved to be associated with \a newStateSet, and the StateSet specified by \a oldSSI is
       *  removed from this GeoNode. Otherwise \a newStateSet just replaces the StateSet specified by \a oldSSI,
       *  thus implicitly changing the association of the Drawables from the StateSet specified by \a oldSSI to
       *  \a newStateSet.
       *  \sa getNumberOfStateSets, beginStateSets, endStateSet, removeStateSet, clearStateSets, findStateSet */
      NVSG_API bool replaceStateSet( const StateSetSharedPtr & newStateSet, const StateSetIterator & oldSSI );

      /*! \brief Replace all StateSets in this GeoNode with one single StateSet.
       *  \param newStateSet The StateSet to replace all other StateSets in this GeoNode.
       *  \sa getNumberOfStateSets, beginStateSets, endStateSet, removeStateSet, clearStateSets, findStateSet */
      NVSG_API void replaceAllStateSets( const StateSetSharedPtr & newStateSet );

      /*! \brief Remove a StateSet from this GeoNode.
       *  \param stateSet The StateSet to be removed from this GeoNode.
       *  \return \c true, if removing \a stateSet from this GeoNode was successful, otherwise \c false.
       *  \note The removal is successful if \a stateSet is part of this GeoNode. Any Drawables in this GeoNode
       *  associated with \a stateSet are also removed.
       *  \sa getNumberOfStateSets, beginStateSets, endStateSet, replaceStateSet, clearStateSets, findStateSet */
      NVSG_API bool removeStateSet( const StateSetSharedPtr & stateSet );

      /*! \brief Remove a StateSet from this GeoNode.
       *  \param ssi An iterator specifying the StateSet to be removed from this GeoNode.
       *  \return \c true, if removing the StateSet specified by \a ssi from this GeoNode was successful,
       *  otherwise \c false.
       *  \note The removal is successful if the StateSet specified by \a ssi is part of this GeoNode. Any
       *  Drawables in this GeoNode associated with the StateSet specified by \a ssi are also removed.
       *  \sa getNumberOfStateSets, beginStateSets, endStateSet, replaceStateSet, clearStateSets, findStateSet */
      NVSG_API bool removeStateSet( const StateSetIterator & ssi );

      /*! \brief Completely remove all StateSets and Drawables from this GeoNode.
       *  \sa getNumberOfStateSets, beginStateSets, endStateSet, replaceStateSet, removeStateSet, findStateSet */
      NVSG_API void clearStateSets();

      /*! \brief Find a StateSet in this GeoNode
       *  \param stateSet The StateSet to find in this GeoNode.
       *  \return A const iterator specifying \a stateSet, if it is part of this GeoNode, otherwise
       *  \a endStateSet().
       *  \sa getNumberOfStateSets, beginStateSets, endStateSet, replaceStateSet, removeStateSet, clearStateSets */
      NVSG_API StateSetConstIterator findStateSet( const StateSetSharedPtr & stateSet ) const;

      /*! \brief Find a StateSet in this GeoNode
       *  \param stateSet The StateSet to find in this GeoNode.
       *  \return An iterator specifying \a stateSet, if it is part of this GeoNode, otherwise \a endStateSet().
       *  \sa getNumberOfStateSets, beginStateSets, endStateSet, replaceStateSet, removeStateSet, clearStateSets */
      NVSG_API StateSetIterator findStateSet( const StateSetSharedPtr & stateSet );


      /*! \brief Get the total number of Drawables in this GeoNode.
       *  \return The total number of Drawables in this GeoNode.
       *  \sa beginDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API unsigned int getNumberOfDrawables() const;

      /*! \brief Get the number of Drawables associated with a specifed StateSet in this GeoNode.
       *  \param stateSet The StateSet to get the number of associated Drawables in this GeoNode.
       *  \return The number of Drawables associated with \a stateSet in this GeoNode.
       *  \sa beginDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API unsigned int getNumberOfDrawables( const StateSetSharedPtr & stateSet ) const;

      /*! \brief Get the number of Drawables associated with a specifed StateSet in this GeoNode.
       *  \param stateSet An iterator specifying the StateSet to get the number of associated Drawables in this GeoNode.
       *  \return The number of Drawables associated with the StateSet specified by \a stateSet in this GeoNode.
       *  \sa beginDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API unsigned int getNumberOfDrawables( const StateSetConstIterator & stateSet ) const;

      /*! \brief Returns a const iterator to the GeoNode’s first Drawable associated with a specified StateSet.
       *  \param stateSet The StateSet to get a const iterator to the first associated Drawable.
       *  \return A const iterator to the GeoNode’s first Drawable associated with the specified StateSet.
       *  \sa getNumberOfDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API DrawableConstIterator beginDrawables( const StateSetSharedPtr & stateSet ) const;

      /*! \brief Returns an iterator to the GeoNode’s first Drawable associated with a specified StateSet.
       *  \param stateSet The StateSet to get an iterator to the first associated Drawable.
       *  \return An iterator to the GeoNode’s first Drawable associated with the specified StateSet.
       *  \sa getNumberOfDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API DrawableIterator beginDrawables( const StateSetSharedPtr & stateSet );

      /*! \brief Returns a const iterator to the GeoNode’s first Drawable associated with a specified StateSet.
       *  \param stateSet A const iterator specifying the StateSet to get a const iterator to the first associated Drawable.
       *  \return A const iterator to the GeoNode’s first Drawable associated with the specified StateSet.
       *  \sa getNumberOfDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API DrawableConstIterator beginDrawables( const StateSetConstIterator & stateSet ) const;

      /*! \brief Returns an iterator to the GeoNode’s first Drawable associated with a specified StateSet.
       *  \param stateSet An iterator specifying the StateSet to get an iterator to the first associated Drawable.
       *  \return An iterator to the GeoNode’s first Drawable associated with the specified StateSet.
       *  \sa getNumberOfDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API DrawableIterator beginDrawables( const StateSetIterator & stateSet );

      /*! \brief Returns a const iterator that points past the end of the GeoNode’s Drawables associated with a specified StateSet.
       *  \param stateSet The StateSet to get a const iterator that points past the end of the associated Drawables.
       *  \return A const iterator that points past the end of to the GeoNode’s Drawables associated with the specified StateSet.
       *  \sa getNumberOfDrawables, beginDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API DrawableConstIterator endDrawables( const StateSetSharedPtr & stateSet ) const;

      /*! \brief Returns an iterator that points past the end of the GeoNode’s Drawables associated with a specified StateSet.
       *  \param stateSet The StateSet to get an iterator that points past the end of the associated Drawables.
       *  \return An iterator that points past the end of to the GeoNode’s Drawables associated with the specified StateSet.
       *  \sa getNumberOfDrawables, beginDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API DrawableIterator endDrawables( const StateSetSharedPtr & stateSet );

      /*! \brief Returns a const iterator that points past the end of the GeoNode’s Drawables associated with a specified StateSet.
       *  \param stateSet A const iterator specifying the StateSet to get a const iterator that points past the end of the associated Drawables.
       *  \return A const iterator that points past the end of to the GeoNode’s Drawables associated with the specified StateSet.
       *  \sa getNumberOfDrawables, beginDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API DrawableConstIterator endDrawables( const StateSetConstIterator & stateSet ) const;

      /*! \brief Returns an iterator that points past the end of the GeoNode’s Drawables associated with a specified StateSet.
       *  \param stateSet An iterator specifying the StateSet to get an iterator that points past the end of the associated Drawables.
       *  \return An iterator that points past the end of to the GeoNode’s Drawables associated with the specified StateSet.
       *  \sa getNumberOfDrawables, beginDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API DrawableIterator endDrawables( const StateSetIterator & stateSet );

      /*! \brief Add a Drawable to this GeoNode and associate it with a specified StateSet.
       *  \param stateSet The StateSet, the drawable is to associate with.
       *  \param drawable The Drawable to add.
       *  \return A pair of iterators specifying the position of the added Drawable in this GeoNode.
       *  \note If \a stateSet is not part of this GeoNode, it is added. If \a drawable already is part of this
       *  GeoNode, but associated with an other StateSet than \a stateSet, that association is released; if in 
       *  that case, there are no more Drawables associated with that other StateSet, that other StateSet is removed
       *  from this GeoNode. If \a drawable is not part of this GeoNode, it is added and associated with \a
       *  stateSet.
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API std::pair<StateSetIterator,DrawableIterator> addDrawable( const StateSetSharedPtr & stateSet, const DrawableSharedPtr & drawable );

      /*! \brief Add a Drawable to this GeoNode and associate it with a specified StateSet.
       *  \param stateSetIterator An iterator specifying the StateSet, the drawable is to associate with.
       *  \param drawable The Drawable to add.
       *  \return A pair of iterators specifying the position of the added Drawable in this GeoNode.
       *  \note If \a drawable already is part of this GeoNode, but associated with an other StateSet than that
       *  specified by \a stateSetIterator, that association is released; if in that case, there are no more
       *  Drawables associated with that other StateSet, that other StateSet is removed from this GeoNode. If \a
       *  drawable is not part of this GeoNode, it is added and associated with the StateSet specified by \a
       *  stateSetIterator.
       *  \note The behaviour is undefined, if \a stateSet is not a valid iterator over the StateSets in GeoNode, or
       *  equals \c endStateSets().
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, replaceDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API std::pair<StateSetIterator,DrawableIterator> addDrawable( const StateSetIterator & stateSetIterator, const DrawableSharedPtr & drawable );

      /*! \brief Replace a Drawable by an other in the set of Drawables associated with a specfied StateSet.
       *  \param newDrawable The new Drawable to replace an other.
       *  \param oldDrawable The Drawable to be replaced.
       *  \return \c true, if replacing the Drawable \a oldDrawable by \a newDrawable was successful, otherwise \c false.
       *  \note The replacement is successful if \a newDrawable and \a oldDrawable are not the same objects, and
       *  \a oldDrawable is part of this GeoNode.
       *  \a oldDrawable is removed from this GeoNode, and \a newDrawable is associated with the same StateSet
       *  \a oldDrawable was associated before. If \a newDrawable already was part of this GeoNode, but associated
       *  with an other StateSet, that association is released. If due to that release, there are no more Drawables
       *  associated with that other StateSet anymore, that other StateSet is removed from this GeoNode.
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, addDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API bool replaceDrawable( const DrawableSharedPtr & newDrawable, const DrawableSharedPtr & oldDrawable );

      /*! \brief Replace a Drawable by an other in the set of Drawables associated with a specfied StateSet.
       *  \param stateSetIterator An iterator specifying the StateSet, where \a oldDrawable is associated to.
       *  \param newDrawable The new Drawable to replace an other.
       *  \param oldDrawable The Drawable to be replaced.
       *  \return \c true, if replacing the Drawable \a oldDrawable by \a newDrawable was successful, otherwise \c false.
       *  \note The replacement is successful if \a newDrawable and \a oldDrawable are not the same objects, and
       *  the StateSet specified by \a stateSetIterator is part of this GeoNode, and \a oldDrawable is in this
       *  GeoNode associated with the StateSet specified by \a stateSetIterator.
       *  \a oldDrawable is removed from this GeoNode, and \a newDrawable is associated with the StateSet specified
       *  by \a stateSetIterator. If \a newDrawable already was part of this GeoNode, but associated with an other
       *  StateSet, that association is released. If due to that release, there are no more Drawables associated
       *  with that other StateSet anymore, that other StateSet is removed from this GeoNode.
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, addDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API bool replaceDrawable( const StateSetIterator & stateSetIterator, const DrawableSharedPtr & newDrawable, const DrawableSharedPtr & oldDrawable );

      /*! \brief Replace a Drawable by an other in the set of Drawables associated with a specfied StateSet.
       *  \param stateSetIterator An iterator specifying the StateSet, where \a oldDrawable is associated to.
       *  \param newDrawable The new Drawable to replace an other.
       *  \param oldDrawableIterator An iterator specifying the Drawable to be replaced.
       *  \return \c true, if replacing the Drawable specified by \a oldDrawable by \a newDrawable was successful,
       *  otherwise \c false.
       *  \note The replacement is successful if \a newDrawable and the Drawable specified by \a oldDrawableIterator
       *  are not the same objects, and the StateSet specified by \a stateSetIterator is part of this GeoNode, and
       *  the Drawable specified by \a oldDrawableIterator is in this GeoNode associated with the StateSet specified
       *  by \a stateSetIterator.
       *  The Drawable specified by \a oldDrawableIterator is removed from this GeoNode, and \a newDrawable is
       *  associated with the StateSet specified by \a stateSetIterator. If \a newDrawable already was part of this
       *  GeoNode, but associated with an other StateSet, that association is released. If due to that release,
       *  there are no more Drawables associated with that other StateSet anymore, that other StateSet is removed from this GeoNode.
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, addDrawable, removeDrawable, removeDrawables, findStateSet */
      NVSG_API bool replaceDrawable( const StateSetIterator & stateSetIterator, const DrawableSharedPtr & newDrawable, const DrawableIterator & oldDrawableIterator );

      /*! \brief Remove a Drawable from this GeoNode.
       *  \param drawable The Drawable to remove.
       *  \return \c true, if removing the Drawable \a drawable was successful, otherwise \c false.
       *  \note The removal is successful if \a drawable is part of this GeoNode.
       *  If the StateSet, \a drawable is associated with, has no other associated Drawables, it is also removed.
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawables, findStateSet */
      NVSG_API bool removeDrawable( const DrawableSharedPtr & drawable );

      /*! \brief Remove a Drawable associated with the specified StateSet from this GeoNode.
       *  \param stateSetIterator An iterator specifying the StateSet specifying the set of Drawables where the removal is to be done.
       *  \param drawable The Drawable to remove.
       *  \return \c true, if removing the Drawable \a drawable was successful, otherwise \c false.
       *  \note The removal is successful if the StateSet specified by \a stateSetIterator is part of this GeoNode,
       *  and \a drawable is associated with the StateSet specified by \a stateSetIterator.
       *  If the StateSet specified by \a stateSetIterator has no other associated Drawables, it is also removed.
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawables, findStateSet */
      NVSG_API bool removeDrawable( const StateSetIterator & stateSetIterator, const DrawableSharedPtr & drawable );

      /*! \brief Remove a Drawable from this GeoNode.
       *  \param stateSetIterator An iterator specifying the StateSet specifying the set of Drawables where the removal is to be done.
       *  \param drawableIterator An iterator specifying the Drawable to remove.
       *  \return \c true, if removing the Drawable specified by \a drawableIterator was successful, otherwise \c false.
       *  \note The removal is successful if the StateSet specified by \a stateSetIterator is part of this GeoNode,
       *  and the Drawable specified by \a drawableIterator is associated with the StateSet specified by \a
       *  stateSetIterator in this GeoNode.
       *  If the StateSet specified by \a stateSetIterator has no other associated Drawables, it is also removed.
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawables, findStateSet */
      NVSG_API bool removeDrawable( const StateSetIterator & stateSetIterator, const DrawableIterator & drawableIterator );

      /*! \brief Find a StateSet which is associated with the specified Drawable in this GeoNode.
       *  \param drawable The Drawable to find a StateSet in this GeoNode for.
       *  \return A const iterator to a StateSet in this GeoNode, that is associated in this GeoNode with \a drawable.
       *  \note If \a drawable is not associated with any StateSet in this GeoNode, the const iterator
       *  \c endStateSets() is returned.
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables */
      NVSG_API StateSetConstIterator findStateSet( const DrawableSharedPtr & drawable ) const;

      /*! \brief Find a StateSet which is associated with the specified Drawable in this GeoNode.
       *  \param drawable The Drawable to find a StateSet in this GeoNode for.
       *  \return An iterator to a StateSet in this GeoNode, that is associated in this GeoNode with \a drawable.
       *  \note If \a drawable is not associated with any StateSet in this GeoNode, the iterator \c endStateSets()
       *  is returned.
       *  \sa getNumberOfDrawables, beginDrawables, endDrawables, addDrawable, replaceDrawable, removeDrawable, removeDrawables */
      NVSG_API StateSetIterator findStateSet( const DrawableSharedPtr & drawable );


      /*! \brief Clear texture coordinates on all Drawable objects. */
      NVSG_API void clearTexCoords( unsigned int tu = 0 );

      /*! \brief Test for equivalence with an other GeoNode.
       *  \param p            A reference to the constant GeoNode to test for equivalence with.
       *  \param ignoreNames  Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare  Optional parameter to perform a deep comparison; default is \c false.
       *  \return \c true if the GeoNode \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as Node, and they have the same number of
       *  StateSet objects with the same number of Drawable objects assigned, they are each tested for
       *  equivalence. If \a deepCompare is false, they are considered to be equivalent if
       *  they are the same pointers. Otherwise a full equivalence test is performed.
       *  \note The behavior is undefined if \a p is not a GeoNode nor derived from one.
       *  \sa Node */
      NVSG_API virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant GeoNode to copy from
       *  \return A reference to the assigned GeoNode
       *  \remarks The assignment operator calls the assignment operator of Node and copies all
       *  geometries from rhs. On all StateSet and Drawable objects previously in \c this, the
       *  reference count is decremented.
       *  \sa Drawable, Node, StateSet */
      NVSG_API GeoNode & operator=(const GeoNode & rhs);

      REFLECTION_INFO_API( NVSG_API, GeoNode );
    protected:
      friend struct nvutil::Holder<GeoNode>;

      /*! \brief Default-constructs a GeoNode.
       */
      NVSG_API GeoNode();

      /*! \brief Constructs a GeoNode as a copy of another GeoNode.
       */
      NVSG_API GeoNode( const GeoNode& rhs );
 
      /*! \brief Destructs a GeoNode.
       */
      NVSG_API virtual ~GeoNode();

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      /*! \brief Interface to calculate the bounding box of this GeoNode.
       *  \return The bounding box of this GeoNode
       *  \remarks This function is called by the framework to determine the
       *  actual bounding box of this GeoNode.
       *  \sa getBoundingBox, invalidateBoundingVolumes */
      NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;

      /*! \brief Interface to calculate the bounding sphere of this GeoNode.
       *  \return A sphere that contains the complete GeoNode.
       *  \remarks This function is called by the framework to determine a sphere that completely
       *  contains the GeoNode.
       *  \sa getBoundingSphere, invalidateBoundingVolumes */
      NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere() const;

      /*! \brief Determine the transparency containment status.
       *  \return \c true, if this GeoNode holds any transparent geometry, otherwise \c false.
       *  \remarks This function is called by the framework to determine the transparency
       *  containment state of a Node when the transparency containment cache is marked as
       *  invalid.
       *  \sa Node */
      NVSG_API virtual bool determineTransparencyContainment() const;

      /*! \brief Determines the CgFx/RTFx containment status.
        * \return \c true, if this GeoNode holds at least one CgFx or RTFx, otherwise \c false. 
        * \remarks This overrides Node::determineShaderContainment, since a GeoNode can contain
        * a shader in at least one of its owned StateSets. */
      NVSG_API virtual bool determineShaderContainment() const;

      /*! \brief Determine the animation containment of this object.
       *  \return \c true, if at least on of the geometries contains an Animation, otherwise \c false. */
      NVSG_API virtual bool determineAnimationContainment() const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      // helper with copying geometries
      unsigned int copyGeometries(const StateSetContainer & from);

      unsigned int manageAddDrawable( const DrawableSharedPtr & drawable );
      unsigned int manageRemoveDrawable( const DrawableSharedPtr & drawable );

      unsigned int manageAddStateSet( const StateSetSharedPtr & stateSet );
      unsigned int manageRemoveStateSet( const StateSetSharedPtr & stateSet );

      std::pair<StateSetContainer::iterator,DrawableContainer::iterator> doAddDrawable( const StateSetContainer::iterator & ssciNew, const DrawableSharedPtr & drawable );
      bool doReplaceDrawable( const StateSetContainer::iterator & ssciOld, const DrawableContainer::iterator & dciOld, const DrawableSharedPtr & newDrawable );
      bool doReplaceStateSet( const StateSetContainer::iterator & ssciOld, const StateSetSharedPtr & newStateSet );
      bool doRemoveDrawable( const StateSetContainer::iterator & ssci, const DrawableContainer::iterator & dci );
      bool doRemoveStateSet( const StateSetContainer::iterator & ssci );

    private:
      StateSetContainer m_geometries;

      // just a cache to hold all drawables in this GeoNode, to speed up detecting if a Drawable is already part
      // of this GeoNode.
      nvutil::STLSet<DrawableWeakPtr> m_drawableCache;
  };

  /*! \relates GeoNode
   *  \brief Gnerates tangents and binormals for all geometries contained in the indicated GeoNode
   *  \param gn 
   *  Specifies the GeoNode for which to calculate tangents an binormals
   *  \param tc 
   *  Addresses the vertex attribute to hold the input texture coordinates used to calculate the tangent space. 
   *  By default, input coordinates are taken from the VertexAttributeSet::NVSG_TEXCOORD0. 
   *  \param tg 
   *  Addresses the vertex attribute where to output the calculated tangents. 
   *  By default tangents are written to VertexAttributeSet::NVSG_TANGENT, 
   *  which is aligned to the TANGENT binding semantic used by Cg for varying vertex shader input. 
   *  \param bn 
   *  Addresses the vertex attribute to output the calculated binormals. 
   *  By default binormals are written to VertexAttributeSet::NVSG_BINORMAL, 
   *  which is aligned to the BINORMAL binding semantic used by Cg for varying vertex shader input. 
   *  \param overwrite
   *  An optional flag to indicate whether or not to overwrite existing vertex data in the output vertex
   *  attributes \a tg, and \a bn. The default is to overwrite existing vertex data.
   *  \remarks 
   *  The function iterates through all geometries contained in the indicated GeoNode, and calculates
   *  tangents and binormals from the specified input 2D texture coordinates and vertex normals, 
   *  which are required to be defined already for the contained geometries. The calculated tangents
   *  and binormals are written to the specified output vertex attributes. If the specified output vertex 
   *  attributes already contain data, this data gets lost, if the \a overwrite flag is set. 
   *  If the \a overwrite flag is not set, tangents and binormals are only written to the indicated output
   *  vertex attributes, if these are empty at the time of calling. */  
  NVSG_API void generateTangentSpace( GeoNode * gn, 
                                      unsigned int tc = VertexAttributeSet::NVSG_TEXCOORD0,
                                      unsigned int tg = VertexAttributeSet::NVSG_TANGENT,
                                      unsigned int bn = VertexAttributeSet::NVSG_BINORMAL, 
                                      bool overwrite = true );

  /*! \relates GeoNode
   *  \brief Generates vertex normals 
   *  \param gn Specifies the GeoNode for which to calculate vertex normals.
   *  \param overwrite An optional flag to indicate whether or not to overwrite existing vertex normals. 
   *  \return \c true, if normals could be generated, otherwise \c false.
   *  The default is to overwrite existing vertex normals. */
  NVSG_API bool generateNormals( GeoNode * gn, bool overwrite = true );

  /*! \relates GeoNode
   *  \brief Generates 2D texture coordinates 
   *  \param gn 
   *  Specifies the GeoNode for which to calculate texture coordinates.
   *  \param type
   *  Desired texture coordinate type. Accepted are TCT_CYLINDRICAL, TCT_PLANAR, and TCT_SPHERICAL. 
   *  \param texcoords 
   *  Addresses the vertex attribute where to output the generated texture coords. 
   *  VertexAttributeSet::NVSG_TEXCOORD0 - VertexAttributeSet::NVSG_TEXCOORD7 are allowed identifiers.
   *  By default texture coords are written to VertexAttributeSet::NVSG_TEXCOORD0, 
   *  \param overwrite 
   *  An optional flag indicating whether or not to overwrite existing vertex data in the output
   *  vertex attribute \a tc. The default is to overwrite existing vertex data. */
  NVSG_API void generateTexCoords( GeoNode * gn, 
                                   TextureCoordType type,
                                   unsigned int texcoords = VertexAttributeSet::NVSG_TEXCOORD0,
                                   bool overwrite = true );

  // - - - - - - - - - - - - - - - - - - - - - - - - - 
  // inlines
  // - - - - - - - - - - - - - - - - - - - - - - - - - 

  inline unsigned int GeoNode::getNumberOfStateSets() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_geometries.size()) );
  }

  inline GeoNode::StateSetConstIterator GeoNode::beginStateSets() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( StateSetConstIterator( m_geometries.begin() ) );
  }

  inline GeoNode::StateSetIterator GeoNode::beginStateSets()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( StateSetIterator( m_geometries.begin() ) );
  }

  inline GeoNode::StateSetConstIterator GeoNode::endStateSets() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( StateSetConstIterator( m_geometries.end() ) );
  }

  inline GeoNode::StateSetIterator GeoNode::endStateSets()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( StateSetIterator( m_geometries.end() ) );
  }

  inline GeoNode::DrawableConstIterator GeoNode::beginDrawables( const StateSetConstIterator & stateSet ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( stateSet.m_iter != m_geometries.end() );

    return( DrawableConstIterator( stateSet.m_iter->m_drawables.begin() ) );
  }

  inline GeoNode::DrawableIterator GeoNode::beginDrawables( const StateSetIterator & stateSet )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( stateSet.m_iter != m_geometries.end() );

    return( DrawableIterator( stateSet.m_iter->m_drawables.begin() ) );
  }

  inline GeoNode::DrawableConstIterator GeoNode::endDrawables( const StateSetConstIterator & stateSet ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( stateSet.m_iter != m_geometries.end() );

    return( DrawableConstIterator( stateSet.m_iter->m_drawables.end() ) );
  }

  inline GeoNode::DrawableIterator GeoNode::endDrawables( const StateSetIterator & stateSet )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( stateSet.m_iter != m_geometries.end() );

    return( DrawableIterator( stateSet.m_iter->m_drawables.end() ) );
  }

} //  namespace nvsg

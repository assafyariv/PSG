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

#include "nvtraverser/OptimizeTraverser.h"

namespace nvtraverser
{
  /*! \brief Traverser that eliminates a number of Objects, if appropriate.
   *  \remarks There are a number of different object types that can be eliminated:\n
   *  - A Group, that is a child of a Group or a Transform, and that does not hold clip planes or
   *  lights, can be replaced by its children.
   *  - A Group, that is a child of an LOD or a Switch or is the root node of the tree, and that
   *  does not hold clip planes or lights, but exactly one child, can be replaced by that child.
   *  - An LOD, that holds exactly one child (and no ranges) can be replaced by that child.
   *  - A StateSet that holds no StateVariants can be removed.
   *  - A StateVariant that holds no StatePasses can be removed.
   *  \sa */
  class EliminateTraverser : public OptimizeTraverser
  {
    public:
      enum
      {
        ET_GROUP                  = BIT0,   //!< EliminateTarget Group: replace redundant Groups by their child/children.
        ET_INDEX_SET              = BIT1,   //!< EliminateTarget IndexSet: remove IndexSet that just enumerates from 0 to n-1.
        ET_LOD                    = BIT2,   //!< EliminateTarget LOD: replace redundant LODs by its child.
        ET_STATE_PASS             = BIT3,   //!< EliminateTarget StatePass: remove StatePasses without any StateAttributes.
        ET_STATE_SET              = BIT4,   //!< EliminateTarget StateSet: remove StateSets without any StateVariants.
        ET_STATE_VARIANT          = BIT5,   //!< EliminateTarget StateVariant: remove StateVariants with any StatePasses.
        ET_TEXTURE_ATTRIBUTE      = BIT6,   //!< EliminateTarget TextureAttribute: remove TextureAttributes without any TextureAttributeItems.
        ET_TEXTURE_ATTRIBUTE_ITEM = BIT7,   //!< EliminateTarget TextureAttributeItem: unbind TextureAttributeItems without a TextureHost.
        ET_ALL_TARGETS_MASK   = ( ET_GROUP | ET_INDEX_SET | ET_LOD | ET_STATE_PASS | ET_STATE_SET
                                | ET_STATE_VARIANT | ET_TEXTURE_ATTRIBUTE
                                | ET_TEXTURE_ATTRIBUTE_ITEM )
      } EliminateTarget;

    public:
      //! Constructor
      NVSG_API EliminateTraverser( void );

      /*! \brief Get the bitmask describing the targets to eliminate.
       *  \return A bitmask describing the targets to eliminate. */
      NVSG_API unsigned int getEliminateTargets() const;

      /*! \brief Set the bitmask describing the targets to eliminate.
       *  \param mask The bitmask describing the targets to eliminate. */
      NVSG_API void setEliminateTargets( unsigned int mask );

      REFLECTION_INFO_API( NVSG_API, EliminateTraverser );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( EliminateTargets );
      END_DECLARE_STATIC_PROPERTIES
    protected:
      //! Protected destructor to prevent instantiation of a EliminateTraverser on stack.
      NVSG_API virtual ~EliminateTraverser( void );

      //! If the root node is a Group with a single child, it is removed.
      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

      //! If the AnimatedTransform holds other Groups without lights or clip planes, they are replaced by their children.
      /** An AnimatedTransform is handled as a Transform. */
      NVSG_API virtual void handleAnimatedTransform( nvsg::AnimatedTransform *p );

      //! If the Billboard holds Groups without lights or clip planes, they are replaced by their children.
      NVSG_API virtual void handleBillboard( nvsg::Billboard *p );

      //! Traverse a FlipbookAnimation like a Group.
      NVSG_API virtual void handleFlipbookAnimation( nvsg::FlipbookAnimation * p );

      NVSG_API virtual void handleGeoNode( nvsg::GeoNode * p );

      //! If the Group holds other Groups without lights or clip planes, they are replaced by their children.
      NVSG_API virtual void handleGroup( nvsg::Group * p );

      //! If the LOD holds Groups with one child without lights or clip planes, they are replaced by their respective child.
      NVSG_API virtual void handleLOD( nvsg::LOD *p );

      NVSG_API virtual void handlePrimitive( nvsg::Primitive * p );

      NVSG_API virtual void handleStatePass( nvsg::StatePass * p );

      NVSG_API virtual void handleStateSet( nvsg::StateSet * p );

      NVSG_API virtual void handleStateVariant( nvsg::StateVariant * p );

      //! If the Switch holds Groups with one child without lights or clip planes, they are replaced by their respective child.
      NVSG_API virtual void handleSwitch( nvsg::Switch *p );

      NVSG_API virtual void handleTextureAttribute( nvsg::TextureAttribute * p );

      //! If the Transform holds Groups without lights or clip planes, they are replaced by their children.
      NVSG_API virtual void handleTransform( nvsg::Transform *p );
    private:
      void eliminateGroups( nvsg::Group *p );
      void eliminateSingleChildChildren( nvsg::Group *p, unsigned int objectCode );
      bool isOneChildCandidate( const nvsg::Group * p );

    private:
      unsigned int m_eliminateTargets;
  };

  inline unsigned int EliminateTraverser::getEliminateTargets() const
  {
    return( m_eliminateTargets );
  }

  inline void EliminateTraverser::setEliminateTargets( unsigned int mask )
  {
    if ( mask != m_eliminateTargets )
    {
      m_eliminateTargets = mask;
      notify( PID_EliminateTargets );
    }
  }
}

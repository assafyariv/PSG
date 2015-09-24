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

#include "nvsgcommon.h" // stuff commonly required
#include "nvsg/Group.h" // base class definition
#include "nvutil/HashGenerator.h"
// additional dependencies
#include <set>
#include <map>

namespace nvsg
{
  /*! \brief Switch node.
   *  \par Namespace: nvsg
   *  \remarks A Switch node is a special kind of grouping node 
   *  where one can define which children will be traversed and which will not (active/inactive children).\n
   *  The Switch node in this case is a special node. It can be thought of as a Dip Switch where 
   *  you can turn on and off the traversal of several children at the same time.\n\n 
   *  The primary method of controlling switches is through masks.  Masks 
   *  consist of a MaskKey and a SwitchMask describing the set of children 
   *  that will become in/active when this Mask is activated.  Any number of
   *  masks may be added to the switch.  Once a mask has been activated the
   *  individual children within that mask may also be manipulated using the older
   *  setActive/setInactive API, if desired.  Note that any changes to the active mask 
   *  through the set[In]Active interface will permanently change that mask - no temporary
   *  copy of each mask is created when it is set active.\n\n
   *  Every switch also contains a default mask (named DEFAULT_MASK_KEY).  The
   *  default mask is created when the switch node is constructed, and remains
   *  active if no other switch masks are activated.  This allows the switch
   *  to operate as it used to with the older API.
   *  
   *  \par Example - using Masks
   *  \code
   *
   *  mask: 0 - children (0, 2, 3)
   *  mask: 1 - children (0, 3)
   *  mask: 997 - children ( 1, 2 )
   *
   *                 sw 
   *                 |
   *     -----------------------
   *     |      |       |      |
   *     c0     c1      c2     c3
   *  
   *  [...]
   *  // create the masks
   *  Switch::MaskKey key0 = 0, key1 = 1, key997 = 997;
   *  Switch::SwitchMask mask;
   *
   *  mask.insert( 0 );
   *  mask.insert( 2 );
   *  mask.insert( 3 );
   *  sw->addMask( key0, mask );
   *
   *  mask.clear();
   *
   *  mask.insert( 0 );
   *  mask.insert( 3 );
   *  sw->addMask( key1, mask );
   *
   *  mask.clear();
   *
   *  mask.insert( 1 );
   *  mask.insert( 2 );
   *  sw->addMask( key997, mask );
   *
   *  sw->setActiveMask( key0 );  // mask with key = 0, children 0,2,3 active
   *                                                    the rest inactive
   *  sw->setActiveMask( key1 );  // mask with key = 1, children 0,3 active
   *                                                    the rest inactive
   *  [...]
   *  \endcode
   *
   *  \par Example - Individual Node (older) API
   *  \code
   *                 sw 
   *                 |
   *     -----------------------
   *     |      |       |      |
   *     c0     c1      c2     c3
   *  
   *  [...]
   *  sw->setInactive();  // set all children in this mask inactive
   *  sw->setActive(0);   // set child c0 active in this mask
   *  sw->setActive(3);   // set child c3 active in this mask
   *  [...]
   *  \endcode
   *  In this sample only C0 and C3 will be active and traversed.*/
  class Switch : public Group
  {
    public:
      NVSG_API static SwitchSharedPtr create();

    public:

      /*! \brief An unsigned integer identifier
       */
      typedef unsigned int      MaskKey;
      /*! \brief A container whose children are of type unsigned int.
       *  \remarks Add/remove elements using the standard c++ library container
       *   methods.
       */
      typedef std::set<unsigned int>  SwitchMask;
      /*! \brief A typedef helper to use when iterating the switches masks.
       *  \sa getSwitchMask, getMaskKey
       */
      typedef std::map<MaskKey,SwitchMask>::const_iterator MaskIterator;
      /*! \brief The key used for the default mask.  Cannot be added or removed,
       *   but can be modified and made active.
       */
      enum { DEFAULT_MASK_KEY = ~0 };

      /*! \brief Set all children of this Switch active. 
       *  \remarks Make all children within the current mask visible so all traversers traverse them.
       *  \sa setInactive, setActiveMask */
      NVSG_API void setActive();

      /*! \brief Make the specified child of the Switch active. 
       *  \param index Zero-based index of the child. Providing an invalid index will lead to 
       *  undefined behavior.
       *  \remarks This function makes the specified child visible within the current mask so all traversers traverse it.\n
       *  Currently active children will stay active within this mask.
       *  \sa setInactive, setActiveMask */
      NVSG_API void setActive(unsigned int index);

      /*! \brief Set all children of the Switch to inactive. 
       *  \remarks Make all children invisible within the current mask so no traverser traverses them.
       *  \sa setActive, setActiveMask, isActive */
      NVSG_API void setInactive();

      /*! \brief Make the specified child of the Switch inactive. 
       *  \param index Zero-based index of the child. Providing an invalid index will lead to 
       *  undefined behavior.
       *  \remarks This function makes the specified child invisible within the current mask so no traverser traverses it.\n
       *  Currently inactive children will stay inactive.
       *  \sa setInactive, setActiveMask, isActive */
      NVSG_API void setInactive(unsigned int index);

      /*! \brief Get the number of active children.
       *  \return Number of active children. 
       *  \remarks This function determines the number of active children within the current mask 
       *  under this Switch.
       *  \sa setActive, setInactive, setActiveMask, isActive */
      NVSG_API unsigned int getNumberOfActive() const;

      /*! \brief Get indices to active child nodes.
       *  \param indices Reference to a vector that will hold the returned indices. 
       *  \return The number of indices copied to /a indices.
       *  \remarks Copies the indices of all active child nodes within the current mask to the vector /a indices.\n
       *  The internal format a Switch uses for storing indices is unsigned int, which expands to
       *  32-bit for 32-bit environments and 64-bit for 64-bit environments. 
       *  \sa setActive, setInactive, isActive */
      NVSG_API unsigned int getActive(std::vector<unsigned int>& indices) const;

      /*! \brief Test if any of the children are active.
       *  \return This function returns true when at least one of the children is active. 
       *  \remarks This function can be used to test if this Switch has at least 
       *  one active child within the current mask.
       *  \sa setActive, setInactive, isActive */
      NVSG_API bool isActive() const;

      /*! \brief Test if the specified child is active.
       *  \param index Zero-based index of the child.
       *  \return This function returns true when the specified child is active within the current mask. 
       *  \remarks This function can be used to test if a specific child is active within the current mask.
       *  \sa setActive, setActiveMask, setInactive, isActive */
      NVSG_API bool isActive(unsigned int index) const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant Switch to copy from.
       *  \return A reference to the assigned Switch.
       *  \remarks The assignment operator performs a deep copy and calls 
       *  the assignment operator of Group.*/
      NVSG_API Switch & operator=(const Switch & rhs);

      /*! \brief Test for equivalence with an other Switch. 
       *  \param p A pointer to the Switch to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the Switch \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as a Group, they are equivalent
       *  if they have the same anti-aliasing state, stipple factor, stipple pattern, and width.
       *  \sa Group */
      NVSG_API virtual bool isEquivalent(const Object *p, bool ignoreNames, bool deepCompare) const;

      /*! \brief Get the number of masks in this switch.
       *  \return unsigned int number of masks.
       *  \remarks the number of masks will always be >= 1.
       *  \sa Group */
      NVSG_API unsigned int getNumberOfMasks() const;

      /*! \brief Get the first mask iterator.
       *  \return MaskIterator of the first in the list.
       *  \sa MaskIterator */
      NVSG_API MaskIterator getFirstMaskIterator() const;

      /*! \brief Get one past the last mask iterator.
       *  \return MaskIterator of one past the last in the list.
       *  \sa MaskIterator */
      NVSG_API MaskIterator getLastMaskIterator() const;

      /*! \brief Get the current mask iterator.
       *  \return MaskIterator of the current mask.
       *  \sa MaskIterator */
      NVSG_API MaskIterator getCurrentMaskIterator() const;

      /*! \brief Get the next mask iterator.
       *  \param it A MaskIterator obtained from a call to getFirstMaskIterator
       *  \return MaskIterator of one next in the list.
       *  \sa MaskIterator */
      NVSG_API MaskIterator getNextMaskIterator(MaskIterator it) const; 

      /*! \brief Extract the mask key from a MaskIterator.
       *  \param it A MaskIterator obtained from a call to getFirstMaskIterator
       *  \return MaskKey contained in the MaskIterator.
       *  \sa MaskIterator, MaskKey */
      NVSG_API MaskKey getMaskKey(MaskIterator it) const;

      /*! \brief Extract the switch mask from a MaskIterator.
       *  \param it A MaskIterator obtained from a call to getFirstMaskIterator
       *  \return SwitchMask contained in the MaskIterator.
       *  \sa MaskIterator, SwitchMask */
      NVSG_API const SwitchMask & getSwitchMask(MaskIterator it) const;

      /*! \brief Get the key of the active mask.
       *  \return MaskKey representing the active mask.
       *  \sa MaskKey */
      NVSG_API MaskKey getActiveMaskKey() const;

      /*! \brief Set the active mask to the one that was registered with the
       *   given MaskKey.
       *  \param k A MaskKey which was previously registered with addMask.
       *  \note If the given MaskKey is unknown by this Switch (was never added using addMask), 
       *  the call will be silently ignored, and the active mask will be unchanged.
       *  \sa MaskKey */
      NVSG_API void setActiveMaskKey( MaskKey k );

      /*! \brief Add a new mask to the set of masks for this switch.
       *  \param key A MaskKey to use to refer to this mask later.
       *  \param sm A SwitchMask containing the children to activate when this
       *   mask is active.
       *  \remarks All keys are considered unique, so adding a mask with the same key 
       *   as an existing mask will silently replace the existing mask.
       *  \sa removeMask, SwitchMask, MaskKey */
      NVSG_API void addMask( MaskKey key, const SwitchMask & sm );

      /*! \brief Remove a mask from the set of masks for this switch.
       *  \param key The MaskKey to remove.
       *  \return bool Signifying whether a mask with that key was found.
       *  \remarks You must not remove the mask identified by the DEFAULT_MASK_KEY.
       *  Doing so, would result in undefined behavior.
       *  \sa removeMask, SwitchMask, MaskKey */
      NVSG_API bool removeMask( MaskKey key );
      
      /*! \brief Retrieve the active SwitchMask of this switch.
      * \return The active SwitchMask. */
      NVSG_API const SwitchMask& getActiveSwitchMask() const;

      REFLECTION_INFO_API( NVSG_API, Switch );
    protected:
      friend struct nvutil::Holder<Switch>;

      /*! \brief Default-constructs a Switch. 
       */
      NVSG_API Switch();

      /*! \brief Constructs a Switch as a copy of another Switch.
      */
      NVSG_API Switch(const Switch &rhs);

      /*! \brief Destructs a Switch.
       */
      NVSG_API virtual ~Switch();

      /*! \brief An internal const typedef helper to use when iterating the switches masks.
       */
      typedef std::map<MaskKey,SwitchMask>::iterator NonConstMaskIterator;
      /*! \brief initializes internal data structures
       */

      NVSG_API virtual void init();

      /*! \brief Called from the framework immediately after a child has been removed from the Switch.
       *  \param index Zero-based position that previously has referenced the removed child node.
       *  \remarks This function takes care of updating the indices in the container that holds
       *  the information on the active children. Removing children from the switch may have influence 
       *  on the position of other children.  NOTE: The masks contained within the switch are updated 
       *  to reflect the child that has been lost.
       *  \sa Group::postRemoveChild */
      NVSG_API virtual void postRemoveChild(unsigned int index);

      /*! \brief Called from the framework immediately after a child was added to the Switch.
       *  \param index Zero-based position where the child node was added.
       *  \remarks This function takes care of updating the indices in the container that holds 
       *  the information on the active children. Adding children to the switch may have influence 
       *  on the position of other children.  NOTE: The masks contained within the switch are updated 
       *  to reflect the insertion of the child.
       *  \sa Group::postAddChild */
      NVSG_API virtual void postAddChild(unsigned int index);

      NVSG_API virtual void postActivateChild(unsigned int index);
      NVSG_API virtual void postDeactivateChild(unsigned int index);

      /*! \brief Calculates the bounding box of this Switch.
       *  \return The axis-aligned bounding box of this Switch.
       *  \remarks This function is called by the framework when re-calculation
       *  of the bounding box is required for this Switch. */
      NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;

      /*! \brief Called from the framework if re-calculation of the Group's bounding sphere 
       *  is required.
       *  \return The function returns a nvmath::Sphere3f that represents the actual bounding 
       *  sphere of this Switch. The bounding sphere encloses the active children of the Switch.
       *  \remarks The function calculates the bounding sphere by accumulating the bounding spheres 
       *  of all active child nodes.
       * \sa Group::calculateBoundingSphere, nvmath::Sphere3f */
      NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere( void ) const;

      NVSG_API virtual unsigned short determineHintsContainment( unsigned short hints ) const;
      NVSG_API virtual bool determineTransparencyContainment() const;
      NVSG_API virtual bool determineShaderContainment() const; 
      NVSG_API virtual bool determineLightContainment() const; 
      NVSG_API virtual bool determineLODContainment() const; 

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

      // Property framework
    public:
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( ActiveMaskKey );
          NVSG_API DECLARE_STATIC_PROPERTY( ActiveSwitchMask );
      END_DECLARE_STATIC_PROPERTIES      

    private:
      /** Index set (ordered) into the vector of children that's currently in use */
      std::map<MaskKey, SwitchMask>  m_masks;
      MaskKey                        m_activeMaskKey;

      //
      // Internal methods to get the activemask
      //
      inline const SwitchMask & activeMask() const 
      {
        NVSG_TRACE();
		    NVSG_ASSERT(m_masks.end()!=m_masks.find(m_activeMaskKey));
        return (*m_masks.find(m_activeMaskKey)).second;
      }

      inline SwitchMask & activeMask()
      {
        NVSG_TRACE();
		    NVSG_ASSERT(m_masks.end()!=m_masks.find(m_activeMaskKey));
		    return m_masks[ m_activeMaskKey ];
      }
	};

} //  namespace nvsg

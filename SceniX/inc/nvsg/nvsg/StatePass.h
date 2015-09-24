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
/** \file */

#include "nvsgcommon.h"
#include "nvsg/OwnedObject.h" // base class definition
#include "nvutil/BitMask.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Container class to hold a number of StateAttribute objects.
   *  \par Namespace: nvsg
   *  \remarks A StatePass holds a number of StateAttribute objects, that together define one
   *  rendering pass. A StatePass can be the child of one or more StateVariant objects.
   *  \sa StateAttribute, StateVariant */
  class StatePass : public OwnedObject<StateVariant>
  {
    public:
      /*! \brief The container type of the attributes */
      typedef nvutil::STLVector<StateAttributeSharedPtr>                            AttributeContainer;

      /*! \brief The iterator over the AttributeContainer */
      typedef nvutil::SceniXIterator<StatePass,AttributeContainer::iterator>        AttributeIterator;

      /*! \brief The const iterator over the AttributeContainer */
      typedef nvutil::SceniXIterator<StatePass,AttributeContainer::const_iterator>  AttributeConstIterator;

    public:
      NVSG_API static StatePassSharedPtr create();

    friend class StateVariant;

    public:
      /*! \brief Get the number of StateAttributes in this StatePass.
       *  \return The number of StateAttributes in this StatePass.
       *  \sa beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API unsigned int getNumberOfAttributes() const;

      /*! \brief Get a const iterator to the first attribute in this StatePass.
       *  \return A const iterator to the first attribute in this StatePass.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeConstIterator beginAttributes() const;

      /*! \brief Get an iterator to the first attribute in this StatePass.
       *  \return An iterator to the first attribute in this StatePass.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeIterator beginAttributes();

      /*! \brief Get a const iterator that points just beyond the end of the attribute in this StatePass.
       *  \return A const iterator that points just beyond the end of the attribute in this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeConstIterator endAttributes() const;

      /*! \brief Get an iterator that points just beyond the end of the attribute in this StatePass.
       *  \return An iterator that points just beyond the end of the attribute in this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeIterator endAttributes();

      /*! \brief Adds an attribute to this StatePass.
       *  \param attribute Specifies the attribute to add
       *  \return An iterator that points to the position where \a attribute was added.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeIterator addAttribute( const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from this StatePass.
       *  \param attribute The attribute to remove from this StatePass.
       *  \return \c true, if the attribute has been removed from this StatePass, otherwise \c false.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API bool removeAttribute( const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from this StatePass.
       *  \param sci An iterator to the attribute to remove from this StatePass.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeIterator removeAttribute( const AttributeIterator & sci );

      /*! \brief Remove an attribute specified by an object code from this StatePass.
       *  \param objectCode The object code of the attribute to remove.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeIterator removeAttributeByObjectCode( unsigned int objectCode );

      /*! \brief Remove all StateAttributes from this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, findAttribute */
      NVSG_API void clearAttributes();

      /*  \brief Find a specified attribute in this StatePass.
       *  \param attribute The attribute to find.
       *  \return A const iterator to the found attribute in this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API AttributeConstIterator findAttribute( const StateAttributeSharedPtr & attribute ) const;

      /*  \brief Find a specified attribute in this StatePass.
       *  \param attribute The attribute to find.
       *  \return An iterator to the found attribute in this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API AttributeIterator findAttribute( const StateAttributeSharedPtr & attribute );

      /*  \brief Find an attribute by object code in this StatePass.
       *  \param code The object code of the attribute to find.
       *  \return A const iterator to the found attribute in this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API AttributeConstIterator findAttribute( unsigned int code ) const;

      /*  \brief Find an attribute by object code in this StatePass.
       *  \param code The object code of the attribute to find.
       *  \return An iterator to the found attribute in this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API AttributeIterator findAttribute( unsigned int code );


      /*! \brief Assignment operator
       *  \param rhs A reference to the constant StatePass to copy from
       *  \return A reference to the assigned StatePass
       *  \remarks The assignment operator calls the assignment operator of Object. Then any
       *  StateAttribute attached to this StatePass is removed, and all StateAttribute objects
       *  of \a rhs are copied.
       *  \sa Object, StateAttribute */
      NVSG_API StatePass & operator=(const StatePass & rhs);

      /*! \brief Test for equivalence with an other StatePass.
       *  \param p A reference to the constant StatePass to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the StatePass \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as Object, they are equivalent if their
       *  StateAttribute objects are equivalent. If \a deepCompare is \c true, they are equivalent
       *  if for each StateAttribute in \c this there is an equivalent StateAttribute in \a p. If
       *  \a deepCompare is \c false, they are equivalent if each StateAttribute in \c this is also
       *  found in \c p.
       *  \note The behavior is undefined if \a p is not a StatePass nor derived from one.
       *  \sa Object, StateAttribute */
      NVSG_API virtual bool isEquivalent( const Object * p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, StatePass );
    protected:
      friend struct nvutil::Holder<StatePass>;

      /*! \brief Default-constructs a StatePass. 
       *  \remarks An empty StatePass is created. */
      NVSG_API StatePass();

      /*! \brief Constructs a StatePass as a copy from another StatePass. */ 
      NVSG_API StatePass( const StatePass& rhs );

      /*! \brief Destructs a StatePass. */
      NVSG_API virtual ~StatePass();

      /*! \brief Determines Object hints containment.
       * \param which
       * Specifies a bit mask indicating Object hints of interest.
       * Allowed bits are NVSG_HINT_DYNAMIC, NVSG_HINT_ALWAYS_VISIBLE, NVSG_HINT_ALWAYS_INVISIBLE,
       * NVSG_HINT_OVERLAY, NVSG_HINT_SHADOW_CASTER, NVSG_HINT_SHADOW_RECEIVER, or
       * a combination of these.
       * \return
       * The function returns a bit field indicating all requested Object hints contained 
       * in the subtree having this Object as root. 
       * \remarks
       * This function is called from the getContainedHints and containsHints entry points
       * if the hints containment state is flagged invalid at the time of calling. 
       * This implementation determines the hints contained in the all owned StateAttributes. */
      NVSG_API virtual unsigned int determineHintsContainment(unsigned int which) const;

      NVSG_API virtual bool determineTransparencyContainment() const;
      NVSG_API virtual bool determineShaderContainment() const;
      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      AttributeContainer::iterator doRemoveAttribute( const AttributeContainer::iterator & aci );
      void invalidateCaches( const StateAttributeSharedPtr & attribute, bool insert );

    private:
      AttributeContainer  m_attribs;
  };

  inline unsigned int StatePass::getNumberOfAttributes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_attribs.size()) );
  }

  inline StatePass::AttributeConstIterator StatePass::beginAttributes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( AttributeConstIterator( m_attribs.begin() ) );
  }

  inline StatePass::AttributeIterator StatePass::beginAttributes()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( AttributeIterator( m_attribs.begin() ) );
  }

  inline StatePass::AttributeConstIterator StatePass::endAttributes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( AttributeConstIterator( m_attribs.end() ) );
  }

  inline StatePass::AttributeIterator StatePass::endAttributes()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( AttributeIterator( m_attribs.end() ) );
  }

  inline StatePass::AttributeIterator StatePass::removeAttributeByObjectCode( unsigned int objectCode )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( removeAttribute( findAttribute( objectCode ) ) );
  }

  inline StatePass::AttributeConstIterator StatePass::findAttribute( const StateAttributeSharedPtr & attribute ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( AttributeConstIterator( find( m_attribs.begin(), m_attribs.end(), attribute ) ) );
  }

  inline StatePass::AttributeIterator StatePass::findAttribute( const StateAttributeSharedPtr & attribute )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( AttributeIterator( find( m_attribs.begin(), m_attribs.end(), attribute ) ) );
  }

} //  namespace nvsg

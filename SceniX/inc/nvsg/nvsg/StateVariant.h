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
#include "nvsg/OwnedObject.h"
#include "nvsg/StatePass.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  class StateSet;

  /*! \brief Container class to hold a number of StatePass objects.
   *  \par Namespace: nvsg
   *  \remarks A StateVariant holds a number of StatePass objects. Any geometries associated with
   *  this StateVariant are rendered with each of the StatePass objects, one after the other.\n
   *  A StateVariant can be the child of one or more StateSet objects.
   *  \sa StatePass, StateSet */
  class StateVariant : public OwnedObject<StateSet>
  {
    public:
      /*! \brief The container type of the passes */
      typedef nvutil::STLVector<StatePassSharedPtr>                               PassContainer;

      /*! \brief The iterator over the PassContainer */
      typedef nvutil::SceniXIterator<StateVariant,PassContainer::iterator>        PassIterator;

      /*! \brief The const iterator over the PassContainer */
      typedef nvutil::SceniXIterator<StateVariant,PassContainer::const_iterator>  PassConstIterator;

    public:
      NVSG_API static StateVariantSharedPtr create();

    friend class StateSet;

    public:
      /*! \brief Get the number of passes in this StateVariant.
       *  \return The number of passes in this StateVariant.
       *  \sa beginPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API unsigned int getNumberOfPasses() const;

      /*! \brief Get a const iterator to the first pass in this StateVariant.
       *  \return A const iterator to the first pass in this StateVariant.
       *  \sa getNumberOfPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API PassConstIterator beginPasses() const;

      /*! \brief Get an iterator to the first pass in this StateVariant.
       *  \return An iterator to the first pass in this StateVariant.
       *  \sa getNumberOfPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API PassIterator beginPasses();

      /*! \brief Get a const iterator that points just beyond the end of the pass in this StateVariant.
       *  \return A const iterator that points just beyond the end of the pass in this StateVariant.
       *  \sa getNumberOfPasses, beginPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API PassConstIterator endPasses() const;

      /*! \brief Get an iterator that points just beyond the end of the pass in this StateVariant.
       *  \return An iterator that points just beyond the end of the pass in this StateVariant.
       *  \sa getNumberOfPasses, beginPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API PassIterator endPasses();

      /*! \brief Adds a pass to this StateVariant.
       *  \param pass Specifies the pass to add
       *  \return An iterator that points to the position where \a pass was added.
       *  \sa getNumberOfPasses, beginPasses, endPasses, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API PassIterator addPass( const StatePassSharedPtr & pass );

      /*! \brief Insert a pass at a specified position in this StateVariant.
       *  \param spi An iterator that points to the position where \a pass is inserted.
       *  \param pass The pass to add.
       *  \return An iterator that points to the position where \a pass was inserted.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API PassIterator insertPass( const PassIterator & spi, const StatePassSharedPtr & pass );

      /*! \brief Remove a pass from this StateVariant.
       *  \param pass The pass to remove from this StateVariant.
       *  \return \c true, if the pass has been removed from this StateVariant, otherwise \c false.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, replacePass, clearPasses, findPass */
      NVSG_API bool removePass( const StatePassSharedPtr & pass );

      /*! \brief Remove a pass from this StateVariant.
       *  \param sci An iterator to the pass to remove from this StateVariant.
       *  \return An iterator pointing to the new location of the pass that followed the one removed by
       *  this function call, which is endPasses() if the operation removed the last pass.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, replacePass, clearPasses, findPass */
      NVSG_API PassIterator removePass( const PassIterator & sci );

      /*! \brief Replace one pass by an other in this StateVariant
       *  \param newPass The new pass that replaces the old ones.
       *  \param oldPass The old pass to be replaced by the new one.
       *  \return \c true, if \a oldPass has been replaced by \a newPass, otherwise \c false.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, findPass */
      NVSG_API bool replacePass( const StatePassSharedPtr & newPass, const StatePassSharedPtr & oldPass );

      /*! \brief Replace one pass by an other in this StateVariant
       *  \param newPass The new pass that replaces the old ones.
       *  \param oldSPI An iterator pointing to the pass to be replaced.
       *  \return \c true, if the pass specified by \a oldSPI has been replaced by \a newPass, otherwise \c false.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, findPass */
      NVSG_API bool replacePass( const StatePassSharedPtr & newPass, PassIterator & oldSPI );

      /*! \brief Remove all passes from this StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, replacePass, findPass */
      NVSG_API void clearPasses();

      /*  \brief Find a specified pass in this StateVariant.
       *  \param pass The pass to find.
       *  \return A const iterator to the found pass in this StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses */
      NVSG_API PassConstIterator findPass( const StatePassSharedPtr & pass ) const;

      /*  \brief Find a specified pass in this StateVariant.
       *  \param pass The pass to find.
       *  \return An iterator to the found pass in this StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses */
      NVSG_API PassIterator findPass( const StatePassSharedPtr & pass );


      /*! \brief Get the number of StateAttributes of the first StatePass in this StateVariant.
       *  \return The number of StateAttributes of the first StatePass in this StateVariant.
       *  \sa beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API unsigned int getNumberOfAttributes() const;

      /*! \brief Get a const iterator to the first attribute of the first StatePass in this StateVariant.
       *  \return A const iterator to the first attribute of the first StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeConstIterator beginAttributes() const;

      /*! \brief Get an iterator to the first attribute of the first StatePass in this StateVariant.
       *  \return An iterator to the first attribute of the first StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator beginAttributes();

      /*! \brief Get a const iterator that points just beyond the end of the attribute of the first StatePass in this StateVariant.
       *  \return A const iterator that points just beyond the end of the attribute of the first StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeConstIterator endAttributes() const;

      /*! \brief Get an iterator that points just beyond the end of the attribute of the first StatePass in this StateVariant.
       *  \return An iterator that points just beyond the end of the attribute of the first StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator endAttributes();

      /*! \brief Adds an attribute to the first StatePass in this StateVariant.
       *  \param attribute Specifies the attribute to add.
       *  \return An iterator that points to the position where \a attribute was added.
       *  \note If there is no StatePass in this StateVariant, one is created and the StateAttribute added to that.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator addAttribute( const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from the first StatePass in this StateVariant.
       *  \param attribute The attribute to remove.
       *  \return \c true, if the attribute has been removed, otherwise \c false.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API bool removeAttribute( const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from the first StatePass in this StateVariant.
       *  \param sci An iterator to the attribute to remove.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \note The behaviour is undefined if there is no StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator removeAttribute( const StatePass::AttributeIterator & sci );

      /*! \brief Remove an attribute specified by an object code from the first StatePass in this StateVariant.
       *  \param objectCode The object code of the attribute to remove.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator removeAttributeByObjectCode( unsigned int objectCode );

      /*! \brief Remove all StateAttributes from the first StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, findAttribute */
      NVSG_API void clearAttributes();

      /*  \brief Find a specified attribute in the first StatePass of this StateVariant.
       *  \param attribute The attribute to find.
       *  \return A const iterator to the found attribute.
       *  \note The behaviour is undefined if there is no StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeConstIterator findAttribute( const StateAttributeSharedPtr & attribute ) const;

      /*  \brief Find a specified attribute in the first StatePass of this StateVariant.
       *  \param attribute The attribute to find.
       *  \return An iterator to the found attribute in the first StatePass of this StateVariant.
       *  \note The behaviour is undefined if there is no StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeIterator findAttribute( const StateAttributeSharedPtr & attribute );

      /*  \brief Find an attribute by object code in the first StatePass of this StateVariant.
       *  \param code The object code of the attribute to find.
       *  \return A const iterator to the found attribute in the first SatePass of this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeConstIterator findAttribute( unsigned int code ) const;

      /*  \brief Find an attribute by object code in the first StatePass of this StateVariant.
       *  \param code The object code of the attribute to find.
       *  \return An iterator to the found attribute in the first StatePass of this StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeIterator findAttribute( unsigned int code );


      /*! \brief Get the number of StateAttributes in the specified StatePass in this StateVariant.
       *  \param spci Iterator to the StatePass to work on.
       *  \return The number of StateAttributes of the specified StatePass in this StateVariant.
       *  \sa beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API unsigned int getNumberOfAttributes( const PassConstIterator & spci ) const;

      /*! \brief Get a const iterator to the first attribute of the specified StatePass in this StateVariant.
       *  \param spci Iterator to the StatePass to work on.
       *  \return A const iterator to the first attribute of the specified StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeConstIterator beginAttributes( const PassConstIterator & spci ) const;

      /*! \brief Get an iterator to the first attribute of the specified StatePass in this StateVariant.
       *  \param spi Iterator to the StatePass to work on.
       *  \return An iterator to the first attribute of the specified StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator beginAttributes( const PassIterator & spi );

      /*! \brief Get a const iterator that points just beyond the end of the attribute of the specified StatePass in this StateVariant.
       *  \param spci Iterator to the StatePass to work on.
       *  \return A const iterator that points just beyond the end of the attribute of the first StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeConstIterator endAttributes( const PassConstIterator & spci ) const;

      /*! \brief Get an iterator that points just beyond the end of the attribute of the specified StatePass in this StateVariant.
       *  \param spi Iterator to the StatePass to work on.
       *  \return An iterator that points just beyond the end of the attribute of the specified StatePass in this StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator endAttributes( const PassIterator & spi );

      /*! \brief Adds an attribute to the specifed StatePass in this StateVariant.
       *  \param spi Iterator to the StatePass to work on.
       *  \param attribute Specifies the attribute to add.
       *  \return An iterator that points to the position where \a attribute was added.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator addAttribute( const PassIterator & spi, const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from the specifed StatePass in this StateVariant.
       *  \param spi Iterator to the StatePass to work on.
       *  \param attribute The attribute to remove.
       *  \return \c true, if the attribute has been removed, otherwise \c false.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API bool removeAttribute( const PassIterator & spi, const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from the specified StatePass in this StateVariant.
       *  \param spi Iterator to the StatePass to work on.
       *  \param sci An iterator to the attribute to remove.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator removeAttribute( const PassIterator & spi, const StatePass::AttributeIterator & sci );

      /*! \brief Remove an attribute specified by an object code from the specified StatePass in this StateVariant.
       *  \param spi Iterator to the StatePass to work on.
       *  \param objectCode The object code of the attribute to remove.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator removeAttributeByObjectCode( const PassIterator & spi, unsigned int objectCode );

      /*! \brief Remove all StateAttributes from the specified StatePass in this StateVariant.
       *  \param spi Iterator to the StatePass to work on.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, findAttribute */
      NVSG_API void clearAttributes( const PassIterator & spi );

      /*  \brief Find a specified attribute in the specified StatePass of this StateVariant.
       *  \param spci Iterator to the StatePass to work on.
       *  \param attribute The attribute to find.
       *  \return A const iterator to the found attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeConstIterator findAttribute( const PassConstIterator & spci, const StateAttributeSharedPtr & attribute ) const;

      /*  \brief Find a specified attribute in the specified StatePass of this StateVariant.
       *  \param spci Iterator to the StatePass to work on.
       *  \param attribute The attribute to find.
       *  \return An iterator to the found attribute in this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeIterator findAttribute( const PassIterator & spci, const StateAttributeSharedPtr & attribute );

      /*  \brief Find an attribute by object code in the specified StatePass of this StateVariant.
       *  \param spci Iterator to the StatePass to work on.
       *  \param code The object code of the attribute to find.
       *  \return A const iterator to the found attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeConstIterator findAttribute( const PassConstIterator & spci, unsigned int code ) const;

      /*  \brief Find an attribute by object code in the specified StatePass of this StateVariant.
       *  \param spci Iterator to the StatePass to work on.
       *  \param code The object code of the attribute to find.
       *  \return An iterator to the found attribute in this StatePass.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeIterator findAttribute( const PassIterator & spci, unsigned int code );


      /*! \brief Ask if this each StatePass of this StateVariant is transparent.
       *  \return \c true, if each StatePass of this StateVariant is transparent, otherwise \c false.
       *  \sa invalidateTransparencyContainment */
      NVSG_API bool isEachPassTransparent() const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant StateVariant to copy from
       *  \return A reference to the assigned StateVariant
       *  \remarks The assignment operator calls the assignment operator of Object. Then any
       *  StatePass attached to this StateVariant is removed, and all StatePass objects of \a rhs
       *  are copied.
       *  \sa StatePass */
      NVSG_API StateVariant & operator=( const StateVariant & rhs );

      /*! \brief Test for equivalence with an other StateVariant.
       *  \param p A reference to the constant StateVariant to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the StateVariant \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as Object, they are equivalent, if their
       *  StatePass objects are equivalent. If \a deepCompare is \c true, they are equivalent
       *  if the StatePass objects are pairwise equivalent. If \a deepCompare is \c false, they
       *  are equivalent, if they are pair-wise the same pointers.
       *  \note The behavior is undefined if \a p is not a StateVariant nor derived from one.
       *  \sa Object, StatePass */
      NVSG_API virtual bool isEquivalent( const Object * p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, StateVariant );
    protected:
      friend struct nvutil::Holder<StateVariant>;

      /*! \brief Default-constructs a StateVariant. 
       *  \remarks An empty StateVariant is created. */
      NVSG_API StateVariant();

      /*! \brief Constructs a StateVariant as a copy from another StateVariant. */
      NVSG_API StateVariant( const StateVariant& rhs );

      /*! \brief Destructs a StateVariant. */
      NVSG_API virtual ~StateVariant();

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
       * This implementation determines the hints contained in all owned StatePasses. */
      NVSG_API virtual unsigned int determineHintsContainment(unsigned int which) const;

      NVSG_API virtual bool determineShaderContainment() const;
      NVSG_API virtual bool determineTransparencyContainment() const;
      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      PassContainer::iterator doInsertPass( const PassContainer::iterator & pci, const StatePassSharedPtr & pass );
      void doReplacePass( PassContainer::iterator & pci, const StatePassSharedPtr & newPass );
      PassContainer::iterator doRemovePass( const PassContainer::iterator & pci );

    private:
      PassContainer  m_passes;
  };

  inline unsigned int StateVariant::getNumberOfPasses() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_passes.size()) );
  }

  inline StateVariant::PassConstIterator StateVariant::beginPasses() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( PassConstIterator( m_passes.begin() ) );
  }

  inline StateVariant::PassIterator StateVariant::beginPasses()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( PassIterator( m_passes.begin() ) );
  }

  inline StateVariant::PassConstIterator StateVariant::endPasses() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( PassConstIterator( m_passes.end() ) );
  }

  inline StateVariant::PassIterator StateVariant::endPasses()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( PassIterator( m_passes.end() ) );
  }

  inline StateVariant::PassConstIterator StateVariant::findPass( const StatePassSharedPtr & pass ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( PassConstIterator( find( m_passes.begin(), m_passes.end(), pass ) ) );
  }

  inline StateVariant::PassIterator StateVariant::findPass( const StatePassSharedPtr & pass )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( PassIterator( find( m_passes.begin(), m_passes.end(), pass ) ) );
  }


  inline unsigned int StateVariant::getNumberOfAttributes() const
  {
    return( getNumberOfAttributes( beginPasses() ) );
  }

  inline StatePass::AttributeConstIterator StateVariant::beginAttributes() const
  {
    return( beginAttributes( beginPasses() ) );
  }

  inline StatePass::AttributeIterator StateVariant::beginAttributes()
  {
    return( beginAttributes( beginPasses() ) );
  }

  inline StatePass::AttributeConstIterator StateVariant::endAttributes() const
  {
    return( endAttributes( beginPasses() ) );
  }

  inline StatePass::AttributeIterator StateVariant::endAttributes()
  {
    return( endAttributes( beginPasses() ) );
  }

  inline StatePass::AttributeIterator StateVariant::addAttribute( const StateAttributeSharedPtr & attribute )
  {
    return( addAttribute( beginPasses(), attribute ) );
  }

  inline bool StateVariant::removeAttribute( const StateAttributeSharedPtr & attribute )
  {
    return( removeAttribute( beginPasses(), attribute ) );
  }

  inline StatePass::AttributeIterator StateVariant::removeAttribute( const StatePass::AttributeIterator & sci )
  {
    return( removeAttribute( beginPasses(), sci ) );
  }

  inline StatePass::AttributeIterator StateVariant::removeAttributeByObjectCode( unsigned int objectCode )
  {
    return( removeAttributeByObjectCode( beginPasses(), objectCode ) );
  }

  inline void StateVariant::clearAttributes()
  {
    clearAttributes( beginPasses() );
  }

  inline StatePass::AttributeConstIterator StateVariant::findAttribute( const StateAttributeSharedPtr & attribute ) const
  {
    return( findAttribute( beginPasses(), attribute ) );
  }

  inline StatePass::AttributeIterator StateVariant::findAttribute( const StateAttributeSharedPtr & attribute )
  {
    return( findAttribute( beginPasses(), attribute ) );
  }

  inline StatePass::AttributeConstIterator StateVariant::findAttribute( unsigned int code ) const
  {
    return( findAttribute( beginPasses(), code ) );
  }

  inline StatePass::AttributeIterator StateVariant::findAttribute( unsigned int code )
  {
    return( findAttribute( beginPasses(), code ) );
  }


  inline StatePass::AttributeConstIterator StateVariant::beginAttributes( const PassConstIterator & spci ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( spci.m_iter != m_passes.end() );
    return( StatePassReadLock( *spci.m_iter )->beginAttributes() );
  }

  inline StatePass::AttributeIterator StateVariant::beginAttributes( const PassIterator & spi )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( spi.m_iter != m_passes.end() );
    return( StatePassWriteLock( *spi.m_iter )->beginAttributes() );
  }

  inline StatePass::AttributeConstIterator StateVariant::endAttributes( const PassConstIterator & spci ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( spci.m_iter != m_passes.end() );
    return( StatePassReadLock( *spci.m_iter )->endAttributes() );
  }

  inline StatePass::AttributeIterator StateVariant::endAttributes( const PassIterator & spi )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( spi.m_iter != m_passes.end() );
    return( StatePassWriteLock( *spi.m_iter )->endAttributes() );
  }

  inline StatePass::AttributeIterator StateVariant::removeAttribute( const PassIterator & spi, const StatePass::AttributeIterator & sci )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( spi.m_iter != m_passes.end() );
    return( StatePassWriteLock( *spi.m_iter )->removeAttribute( sci ) );
  }

  inline StatePass::AttributeIterator StateVariant::removeAttributeByObjectCode( const PassIterator & spi, unsigned int objectCode )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( spi.m_iter != m_passes.end() );
    return( StatePassWriteLock( *spi.m_iter )->removeAttributeByObjectCode( objectCode ) );
  }

  inline StatePass::AttributeConstIterator StateVariant::findAttribute( const PassConstIterator & spci, const StateAttributeSharedPtr & attribute ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( spci.m_iter != m_passes.end() );
    return( StatePassReadLock( *spci.m_iter )->findAttribute( attribute ) );
  }

  inline StatePass::AttributeIterator StateVariant::findAttribute( const PassIterator & spi, const StateAttributeSharedPtr & attribute )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( spi.m_iter != m_passes.end() );
    return( StatePassWriteLock( *spi.m_iter )->findAttribute( attribute ) );
  }

  inline StatePass::AttributeConstIterator StateVariant::findAttribute( const PassConstIterator & spci, unsigned int code ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( spci.m_iter != m_passes.end() );
    return( StatePassReadLock( *spci.m_iter )->findAttribute( code ) );
  }

  inline StatePass::AttributeIterator StateVariant::findAttribute( const PassIterator & spi, unsigned int code )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( spi.m_iter != m_passes.end() );
    return( StatePassWriteLock( *spi.m_iter )->findAttribute( code ) );
  }

} //  namespace nvsg

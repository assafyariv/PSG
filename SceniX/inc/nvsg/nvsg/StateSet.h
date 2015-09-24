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
#include "nvsg/StateVariant.h"
#include "nvutil/BitMask.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  class GeoNode;

  typedef unsigned int VariantKey; //!< Key to a specific StateVariant

  /*! \brief Container class to hold a number of StateVariant objects.
   *  \par Namespace: nvsg
   *  \remarks A StateSet holds a number of StateVariant objects and a key to select one of them.
   *  Only the selected StateVariant is used on standard traversal. That way, it is easy to switch
   *  between different StateVariant objects to use.
   *  \sa StateVariant */
  class StateSet : public OwnedObject<Node>
  {
    public:
      /*! \brief The container type of the StateVariants */
      typedef nvutil::STLMap<VariantKey,StateVariantSharedPtr>                  VariantContainer;

      /*! \brief The iterator over the VariantContainer */
      typedef nvutil::SceniXIterator<StateSet,VariantContainer::iterator>       VariantIterator;

      /*! \brief The const iterator over the VariantContainer */
      typedef nvutil::SceniXIterator<StateSet,VariantContainer::const_iterator> VariantConstIterator;

    public:
      NVSG_API static StateSetSharedPtr create();

    friend class Node;

    public:
      static const VariantKey INVALID_VARIANT_KEY = 0xFFFFFFFF;

      /*! \brief Get the number of StateVariants in this StateSet.
       *  \return The number of StateVariants in this StateSet.
       *  \sa beginVariants, endVariants, setVariant, removeVariant, clearVariants, findVariant */
      NVSG_API unsigned int getNumberOfVariants() const;

      /*! \brief Get a const iterator to the first StateVariant in this StateSet.
       *  \return A const iterator to the first StateVariant in this StateSet.
       *  \sa getNumberOfVariants, endVariants, setVariant, removeVariant, clearVariants, findVariant */
      NVSG_API VariantConstIterator beginVariants() const;

      /*! \brief Get an iterator to the first StateVariant in this StateSet.
       *  \return An iterator to the first StateVariant in this StateSet.
       *  \sa getNumberOfVariants, endVariants, setVariant, removeVariant, clearVariants, findVariant */
      NVSG_API VariantIterator beginVariants();

      /*! \brief Get a const iterator that points just beyond the end of the StateVariant in this StateSet.
       *  \return A const iterator that points just beyond the end of the StateVariant in this StateSet.
       *  \sa getNumberOfVariants, beginVariants, setVariant, removeVariant, clearVariants, findVariant */
      NVSG_API VariantConstIterator endVariants() const;

      /*! \brief Get an iterator that points just beyond the end of the StateVariant in this StateSet.
       *  \return An iterator that points just beyond the end of the StateVariant in this StateSet.
       *  \sa getNumberOfVariants, beginVariants, setVariant, removeVariant, clearVariants, findVariant */
      NVSG_API VariantIterator endVariants();

      /*! \brief Set a combination of VariantKey and StateVariant to this StateSet.
       *  \param key The VariantKey to associate \a variant with.
       *  \param variant Specifies the StateVariant to set.
       *  \return An iterator that points to the position where \a variant was added.
       *  \note If there was no StateVariant associated with the VariantKey \a key in this StateSet, it is added.
       *  If there already was such a combination, the variant associated with \a key is replaced by \a variant.
       *  \note The behavior is undefined if \a key is INVALID_VARIANT_KEY.
       *  \sa getNumberOfVariants, beginVariants, endVariants, removeVariant, clearVariants, findVariant */
      NVSG_API VariantIterator setVariant( VariantKey key, const StateVariantSharedPtr & variant );

      /*! \brief Remove a StateVariant from this StateSet.
       *  \param key The VariantKey to remove from this StateSet.
       *  \return \c true, if the VariantKey has been removed from this StateSet, otherwise \c false.
       *  \sa getNumberOfVariants, beginVariants, endVariants, setVariant, clearVariants, findVariant */
      NVSG_API bool removeVariant( VariantKey key );

      /*! \brief Remove a StateVariant from this StateSet.
       *  \param svi An iterator to the StateVariant to remove from this StateSet.
       *  \return An iterator pointing to the new location of the StateVariant that followed the one removed by
       *  this function call, which is endVariants() if the operation removed the last StateVariant.
       *  \sa getNumberOfVariants, beginVariants, endVariants, setVariant, clearVariants, findVariant */
      NVSG_API VariantIterator removeVariant( const VariantIterator & svi );

      /*! \brief Remove all StateVariants from this StateSet.
       *  \sa getNumberOfVariants, beginVariants, endVariants, setVariant, removeVariant, findVariant */
      NVSG_API void clearVariants();

      /*  \brief Find a specified StateVariant in this StateSet.
       *  \param key The VariantKey to find.
       *  \return A const iterator to the found StateVariant in this StateSet.
       *  \sa getNumberOfVariants, beginVariants, endVariants, setVariant, removeVariant, clearVariants */
      NVSG_API VariantConstIterator findVariant( VariantKey key ) const;

      /*  \brief Find a specified StateVariant in this StateSet.
       *  \param key The VariantKey to find.
       *  \return An iterator to the found StateVariant in this StateSet.
       *  \sa getNumberOfVariants, beginVariants, endVariants, setVariant, removeVariant, clearVariants */
      NVSG_API VariantIterator findVariant( VariantKey key );

      /*! \brief Get the currently active VariantKey.
       *  \return The currently active VariantKey.
       *  \remarks There is only one StateVariant active in a StateSet. Only that StateVariant is
       *  used in traversals. If there is no StateVariant active, the INVALID_VARIANT_KEY is
       *  returned.
       *  \sa VariantKey, getActiveVariant, getVariantKey, setActiveVariantKey */
      NVSG_API VariantKey getActiveVariantKey() const;

      /*! \brief Set the active VariantKey
       *  \param key The VariantKey to set active.
       *  \remarks There is only one StateVariant active in a StateSet. Only that StateVariant is
       *  used in traversals. Only a VariantKey that is part of the StateSet can be used.
       *  \sa VariantKey, getActiveVariant, getActiveVariantKey */
      NVSG_API void setActiveVariantKey( VariantKey key );


      /*! \brief Get the number of passes in the active StateVariant of this StateSet.
       *  \return The number of passes in the active StateVariant.
       *  \sa beginPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API unsigned int getNumberOfPasses() const;

      /*! \brief Get a const iterator to the first pass in the active StateVariant of this StateSet.
       *  \return A const iterator to the first pass in the active StateVariant.
       *  \note The behaviour is undefined, if there is no active StateVariant.
       *  \sa getNumberOfPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassConstIterator beginPasses() const;

      /*! \brief Get an iterator to the first pass in the active StateVariant of this StateSet.
       *  \return An iterator to the first pass in the active StateVariant.
       *  \note The behaviour is undefined, if there is no active StateVariant.
       *  \sa getNumberOfPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator beginPasses();

      /*! \brief Get a const iterator that points just beyond the end of the pass in the active StateVariant of this StateSet.
       *  \return A const iterator that points just beyond the end of the pass in the active StateVariant.
       *  \note The behaviour is undefined, if there is no active StateVariant.
       *  \sa getNumberOfPasses, beginPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassConstIterator endPasses() const;

      /*! \brief Get an iterator that points just beyond the end of the pass in the active StateVariant of this StateSet.
       *  \return An iterator that points just beyond the end of the pass in the active StateVariant.
       *  \note The behaviour is undefined, if there is no active StateVariant.
       *  \sa getNumberOfPasses, beginPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator endPasses();

      /*! \brief Adds a pass to the active StateVariant of this StateSet.
       *  \param pass Specifies the pass to add
       *  \return An iterator that points to the position where \a pass was added.
       *  \note If there is no StateVariant in this StateSet, a default with VariantKey 0 is created and the
       *  StatePass added to that.
       *  \sa getNumberOfPasses, beginPasses, endPasses, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator addPass( const StatePassSharedPtr & pass );

      /*! \brief Insert a pass at a specified position in the active StateVariant of this StateSet.
       *  \param spi An iterator that points to the position where \a pass is inserted.
       *  \param pass The pass to add.
       *  \return An iterator that points to the position where \a pass was inserted.
       *  \note The behaviour is undefined, if there is no active StateVariant, or \a spi is not an iterator over
       *  the StatePasses in that StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, removePass, replacePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator insertPass( const StateVariant::PassIterator & spi
                                                    , const StatePassSharedPtr & pass );

      /*! \brief Remove a pass from the active StateVariant of this StateSet.
       *  \param pass The pass to remove from the specified StateVariant.
       *  \return \c true, if the pass has been removed from the specified StateVariant, otherwise \c false.
       *  \note The behaviour is undefined, if there is no active StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, replacePass, clearPasses, findPass */
      NVSG_API bool removePass( const StatePassSharedPtr & pass );

      /*! \brief Remove a pass from the active StateVariant of this StateSet.
       *  \param spi An iterator to the pass to remove from the specified StateVariant.
       *  \return An iterator pointing to the new location of the pass that followed the one removed by
       *  this function call, which is endPasses() if the operation removed the last pass.
       *  \note The behaviour is undefined, if there is no active StateVariant, or \a spi it not an iterator over
       *  the StatePasses in that StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator removePass( const StateVariant::PassIterator & spi );

      /*! \brief Replace one pass by an other in the active StateVariant of this StateSet
       *  \param newPass The new pass that replaces the old ones.
       *  \param oldPass The old pass to be replaced by the new one.
       *  \return \c true, if \a oldPass has been replaced by \a newPass, otherwise \c false.
       *  \note The behaviour is undefined, if there is no active StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, findPass */
      NVSG_API bool replacePass( const StatePassSharedPtr & newPass, const StatePassSharedPtr & oldPass );

      /*! \brief Replace one pass by an other in the active StateVariant of this StateSet
       *  \param newPass The new pass that replaces the old ones.
       *  \param oldSPI An iterator pointing to the pass to be replaced.
       *  \return \c true, if the pass specified by \a oldSPI has been replaced by \a newPass, otherwise \c false.
       *  \note The behaviour is undefined, if there is no active StateVariant, or \a oldSPI is not an iterator over
       *  the StatePasses in that StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, findPass */
      NVSG_API bool replacePass( const StatePassSharedPtr & newPass, StateVariant::PassIterator & oldSPI );

      /*! \brief Remove all passes from the active StateVariant of this StateSet.
       *  \param key Optional VariantKey to specify the StateVariant to work on; default is 0.
       *  \note The behaviour is undefined, if there is no active StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, replacePass, findPass */
      NVSG_API void clearPasses();

      /*  \brief Find a specified pass in the active StateVariant of this StateSet.
       *  \param pass The pass to find.
       *  \return A const iterator to the found pass in the active StateVariant.
       *  \note The behaviour is undefined, if there is no active StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, replacePass */
      NVSG_API StateVariant::PassConstIterator findPass( const StatePassSharedPtr & pass ) const;

      /*  \brief Find a specified pass in the active StateVariant of this StateSet.
       *  \param pass The pass to find.
       *  \return An iterator to the found pass in the active StateVariant.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, replacePass */
      NVSG_API StateVariant::PassIterator findPass( const StatePassSharedPtr & pass );


      /*! \brief Get the number of passes in the specified StateVariant of this StateSet.
       *  \param svci Const iterator to the StateVariant to work on.
       *  \return The number of passes in the specified StateVariant.
       *  \sa beginPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API unsigned int getNumberOfPasses( const VariantConstIterator & svci ) const;

      /*! \brief Get a const iterator to the first pass in the specified StateVariant of this StateSet.
       *  \param svci Const iterator to the StateVariant to work on.
       *  \return A const iterator to the first pass in the specified StateVariant.
       *  \note The behaviour is undefined, if \a svci points past the end of the container of Passes.
       *  \sa getNumberOfPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassConstIterator beginPasses( const VariantConstIterator & svci ) const;

      /*! \brief Get an iterator to the first pass in the specified StateVariant of this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \return An iterator to the first pass in the specified StateVariant.
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, endPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator beginPasses( const VariantIterator & svi );

      /*! \brief Get a const iterator that points just beyond the end of the pass in the specified StateVariant of this StateSet.
       *  \param svci Const iterator to the StateVariant to work on.
       *  \return A const iterator that points just beyond the end of the pass in the specified StateVariant.
       *  \note The behaviour is undefined, if \a svci points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassConstIterator endPasses( const VariantConstIterator & svci ) const;

      /*! \brief Get an iterator that points just beyond the end of the pass in the specified StateVariant of this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \return An iterator that points just beyond the end of the pass in the specified StateVariant.
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, addPass, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator endPasses( const VariantIterator & svi );

      /*! \brief Adds a pass to the specified StateVariant of this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param pass Specifies the pass to add
       *  \return An iterator that points to the position where \a pass was added.
       *  \note If \a svi points past the end of the container of Passes, a new 
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, endPasses, insertPass, removePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator addPass( const VariantIterator & svi, const StatePassSharedPtr & pass );

      /*! \brief Insert a pass at a specified position in the specified StateVariant of this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi An iterator that points to the position where \a pass is inserted.
       *  \param pass The pass to add.
       *  \return An iterator that points to the position where \a pass was inserted.
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, removePass, replacePass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator insertPass( const VariantIterator & svi
                                                    , const StateVariant::PassIterator & spi
                                                    , const StatePassSharedPtr & pass );

      /*! \brief Remove a pass from the specified StateVariant of this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param pass The pass to remove from the specified StateVariant.
       *  \return \c true, if the pass has been removed from the specified StateVariant, otherwise \c false.
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, replacePass, clearPasses, findPass */
      NVSG_API bool removePass( const VariantIterator & svi, const StatePassSharedPtr & pass );

      /*! \brief Remove a pass from the StateVariant of this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi An iterator to the pass to remove from the specified StateVariant.
       *  \return An iterator pointing to the new location of the pass that followed the one removed by
       *  this function call, which is endPasses() if the operation removed the last pass.
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, replacePass, clearPasses, findPass */
      NVSG_API StateVariant::PassIterator removePass( const VariantIterator & svi, const StateVariant::PassIterator & spi );

      /*! \brief Replace one pass by an other in the specified StateVariant of this StateSet
       *  \param svi Iterator to the StateVariant to work on.
       *  \param newPass The new pass that replaces the old ones.
       *  \param oldPass The old pass to be replaced by the new one.
       *  \return \c true, if \a oldPass has been replaced by \a newPass, otherwise \c false.
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, findPass */
      NVSG_API bool replacePass( const VariantIterator & svi, const StatePassSharedPtr & newPass, const StatePassSharedPtr & oldPass );

      /*! \brief Replace one pass by an other in the specified StateVariant of this StateSet
       *  \param svi Iterator to the StateVariant to work on.
       *  \param newPass The new pass that replaces the old ones.
       *  \param oldSPI An iterator pointing to the pass to be replaced.
       *  \return \c true, if the pass specified by \a oldSPI has been replaced by \a newPass, otherwise \c false.
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, findPass */
      NVSG_API bool replacePass( const VariantIterator & svi, const StatePassSharedPtr & newPass, StateVariant::PassIterator & oldSPI );

      /*! \brief Remove all passes from the specified StateVariant of this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param key Optional VariantKey to specify the StateVariant to work on; default is 0.
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, replacePass, findPass */
      NVSG_API void clearPasses( const VariantIterator & svi );

      /*  \brief Find a specified pass in the specified StateVariant of this StateSet.
       *  \param svci Const iterator to the StateVariant to work on.
       *  \param pass The pass to find.
       *  \return A const iterator to the found pass in the specified StateVariant.
       *  \note The behaviour is undefined, if \a scvi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, replacePass */
      NVSG_API StateVariant::PassConstIterator findPass( const VariantConstIterator & svci, const StatePassSharedPtr & pass ) const;

      /*  \brief Find a specified pass in the specified StateVariant of this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param pass The pass to find.
       *  \return An iterator to the found pass in the specified StateVariant.
       *  \note The behaviour is undefined, if \a svi points past the end of the container of Passes.
       *  \sa getNumberOfPasses, beginPasses, endPasses, addPass, insertPass, removePass, clearPasses, replacePass */
      NVSG_API StateVariant::PassIterator findPass( const VariantIterator & svi, const StatePassSharedPtr & pass );


      /*! \brief Get the number of StateAttributes in the first StatePass of the current StateVariant in this StateSet.
       *  \return The number of StateAttributes in the first StatePass of the current StateVariant in this StateSet.
       *  \sa beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API unsigned int getNumberOfAttributes() const;

      /*! \brief Get a const iterator to the first attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \return A const iterator to the first attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \note The behaviour is undefined, if there is no StateVariant, or no StatePass in the current StateVariant.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeConstIterator beginAttributes() const;

      /*! \brief Get an iterator to the first attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \return An iterator to the first attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \note The behaviour is undefined, if there is no StateVariant, or no StatePass in the current StateVariant.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator beginAttributes();

      /*! \brief Get a const iterator that points just beyond the end of the attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \return A const iterator that points just beyond the end of the attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \note The behaviour is undefined, if there is no StateVariant, or no StatePass in the current StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeConstIterator endAttributes() const;

      /*! \brief Get an iterator that points just beyond the end of the attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \return An iterator that points just beyond the end of the attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \note The behaviour is undefined, if there is no StateVariant, or no StatePass in the current StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator endAttributes();

      /*! \brief Adds an attribute to the first StatePass of the current StateVariant in this StateSet.
       *  \param attribute Specifies the attribute to add.
       *  \return An iterator that points to the position where \a attribute was added.
       *  \note If there is not StateVariant, one is created and marked as the current one. If there is no StatePass
       *  in the current StateVariant, one is created and added to the current StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator addAttribute( const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from the first StatePass of the current StateVariant in this StateSet.
       *  \param attribute The attribute to remove.
       *  \return \c true, if the attribute has been removed from the first StatePass of the current StateVariant in
       *  this StateSet, otherwise \c false.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API bool removeAttribute( const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from the first StatePass of the current StateVariant in this StateSet.
       *  \param sci An iterator to the attribute to remove.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \note The behaviour is undefined if there is no StateVariant, or no StatePass in the current StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator removeAttribute( const StatePass::AttributeIterator & sci );

      /*! \brief Remove an attribute specified by an object code from the first StatePass of the current StateVariant in this StateVariant.
       *  \param objectCode The object code of the attribute to remove.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator removeAttributeByObjectCode( unsigned int objectCode );

      /*! \brief Remove all StateAttributes from the first StatePass of the current StateVariant in this StateSet.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, findAttribute */
      NVSG_API void clearAttributes();

      /*  \brief Find a specified attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \param attribute The attribute to find.
       *  \return A const iterator to the found attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \note The behaviour is undefined if there is no StateVariant, or no StatePass in the current StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeConstIterator findAttribute( const StateAttributeSharedPtr & attribute ) const;

      /*  \brief Find a specified attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \param attribute The attribute to find.
       *  \return An iterator to the found attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \note The behaviour is undefined if there is no StateVariant, or no StatePass in the current StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeIterator findAttribute( const StateAttributeSharedPtr & attribute );

      /*  \brief Find an attribute by object code in the first StatePass of the current StateVariant in this StateSet.
       *  \param code The object code of the attribute to find.
       *  \return A const iterator to the found attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \note The behaviour is undefined if there is no StateVariant, or no StatePass in the current StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeConstIterator findAttribute( unsigned int code ) const;

      /*  \brief Find an attribute by object code in the first StatePass of the current StateVariant in this StateSet.
       *  \param code The object code of the attribute to find.
       *  \return An iterator to the found attribute in the first StatePass of the current StateVariant in this StateSet.
       *  \note The behaviour is undefined if there is no StateVariant, or no StatePass in the current StateVariant.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeIterator findAttribute( unsigned int code );


      /*! \brief Get the number of StateAttributes in the specified StateVariant and StatePass in this StateSet.
       *  \param svci Const iterator to the StateVariant to work on.
       *  \param spci Const iterator to the StatePass in \a svci to work on.
       *  \return The number of StateAttributes in the specified StateVariant and StatePass in this StateSet.
       *  \sa beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API unsigned int getNumberOfAttributes( const VariantConstIterator & svci, const StateVariant::PassConstIterator & spci ) const;

      /*! \brief Get a const iterator to the first attribute in the specified StateVariant and StatePass in this StateSet.
       *  \param svci Const iterator to the StateVariant to work on.
       *  \param spci Const iterator to the StatePass in \a svci to work on.
       *  \return A const iterator to the first attribute in the specified StateVariant and StatePass in this StateSet.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeConstIterator beginAttributes( const VariantConstIterator & svci, const StateVariant::PassConstIterator & spci ) const;

      /*! \brief Get an iterator to the first attribute in the specified StateVariant and StatePass in this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi Iterator to the StatePass in \a svi to work on.
       *  \return An iterator to the first attribute in the specified StateVariant and StatePass in this StateSet.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator beginAttributes( const VariantIterator & svi, const StateVariant::PassIterator & spi );

      /*! \brief Get a const iterator that points just beyond the end of the attribute in the specified StateVariant and StatePass in this StateSet.
       *  \param svci Const iterator to the StateVariant to work on.
       *  \param spci Const iterator to the StatePass in \a svci to work on.
       *  \return A const iterator that points just beyond the end of the attribute in the specified StateVariant and StatePass in this StateSet.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeConstIterator endAttributes( const VariantConstIterator & svci, const StateVariant::PassConstIterator & spci ) const;

      /*! \brief Get an iterator that points just beyond the end of the attribute in the specified StateVariant and StatePass in this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi Iterator to the StatePass in \a svi to work on.
       *  \return An iterator that points just beyond the end of the attribute in the specified StateVariant and StatePass in this StateSet.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator endAttributes( const VariantIterator & svi, const StateVariant::PassIterator & spi );

      /*! \brief Adds an attribute to the specified StateVariant and StatePass in this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi Iterator to the StatePass in \a svi to work on.
       *  \param attribute Specifies the attribute to add.
       *  \return An iterator that points to the position where \a attribute was added.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, removeAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator addAttribute( const VariantIterator & svi, const StateVariant::PassIterator & spi, const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from the specified StateVariant and StatePass in this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi Iterator to the StatePass in \a svi to work on.
       *  \param attribute The attribute to remove.
       *  \return \c true, if the attribute has been removed from the specified StateVariant and StatePass in
       *  this StateSet, otherwise \c false.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API bool removeAttribute( const VariantIterator & svi, const StateVariant::PassIterator & spi, const StateAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from the specified StateVariant and StatePass in this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi Iterator to the StatePass in \a svi to work on.
       *  \param sci An iterator to the attribute to remove.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator removeAttribute( const VariantIterator & svi, const StateVariant::PassIterator & spi, const StatePass::AttributeIterator & sci );

      /*! \brief Remove an attribute specified by an object code from the specified StateVariant and StatePass in this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi Iterator to the StatePass to work on.
       *  \param objectCode The object code of the attribute to remove.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API StatePass::AttributeIterator removeAttributeByObjectCode( const VariantIterator & svi, const StateVariant::PassIterator & spi, unsigned int objectCode );

      /*! \brief Remove all StateAttributes from the specified StateVariant and StatePass in this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi Iterator to the StatePass in \a svi to work on.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, findAttribute */
      NVSG_API void clearAttributes( const VariantIterator & svi, const StateVariant::PassIterator & spi );

      /*  \brief Find a specified attribute in the specified StateVariant and StatePass in this StateSet.
       *  \param svci Const iterator to the StateVariant to work on.
       *  \param spci Const iterator to the StatePass in \a svci to work on.
       *  \param attribute The attribute to find.
       *  \return A const iterator to the found attribute in the specified StateVariant and StatePass in this StateSet.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeConstIterator findAttribute( const VariantConstIterator & svci, const StateVariant::PassConstIterator & spci, const StateAttributeSharedPtr & attribute ) const;

      /*  \brief Find a specified attribute in the specified StateVariant and StatePass in this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi Iterator to the StatePass in \a svi to work on.
       *  \param attribute The attribute to find.
       *  \return An iterator to the found attribute in the specified StateVariant and StatePass in this StateSet.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeIterator findAttribute( const VariantIterator & svi, const StateVariant::PassIterator & spi, const StateAttributeSharedPtr & attribute );

      /*  \brief Find an attribute by object code in the specified StateVariant and StatePass in this StateSet.
       *  \param svci Const iterator to the StateVariant to work on.
       *  \param spci Const iterator to the StatePass in \a svci to work on.
       *  \param code The object code of the attribute to find.
       *  \return A const iterator to the found attribute in the specified StateVariant and StatePass in this StateSet.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeConstIterator findAttribute( const VariantConstIterator & svci, const StateVariant::PassConstIterator & spci, unsigned int code ) const;

      /*  \brief Find an attribute by object code in the specified StateVariant and StatePass in this StateSet.
       *  \param svi Iterator to the StateVariant to work on.
       *  \param spi Iterator to the StatePass in \a svi to work on.
       *  \param code The object code of the attribute to find.
       *  \return An iterator to the found attribute in the specified StateVariant and StatePass in this StateSet.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API StatePass::AttributeIterator findAttribute( const VariantIterator & svi, const StateVariant::PassIterator & spi, unsigned int code );


      /*! \brief Assignment operator
       *  \param rhs A reference to the constant StateSet to copy from
       *  \return A reference to the assigned StateSet
       *  \remarks The assignment operator calls the assignment operator of Object. Then any
       *  StateVariant attached to this StateSet is removed, and all StateVariant objects of \a
       *  rhs are copied.
       *  \sa Object, StateVariant */
      NVSG_API StateSet & operator=(const StateSet & rhs);

      /*! \brief Test for equivalence with an other StateSet.
       *  \param p A reference to the constant StateSet to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the StateSet \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as Object, they are equivalent if they have
       *  the same number of StateVariant/VariantKey pairs and have pair-wise equal VariantKey values.
       *  If \a deepCompare is \c true, they are equivalent if the StateVariant objects associated
       *  with the same VariantKey values are pair-wise equivalent. If \a deepCompare is \c false,
       *  they are equivalent if they are pair-wise the same pointers.
       *  \note The behavior is undefined if \a p is not a StateSet nor derived from one.
       *  \sa Object, StateVariant, VariantKey */
      NVSG_API virtual bool isEquivalent( const Object * p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, StateSet );
    protected:
      friend struct nvutil::Holder<StateSet>;

      /*! \brief Default-constructs a StateSet. 
       *  \remarks An empty StateSet with an invalid key is created. */
      NVSG_API StateSet();

      /*! \brief Constructs a StateSet as a copy from another StateSet. */ 
      NVSG_API StateSet( const StateSet& rhs );

      /*! \brief Destructs a StateSet */ 
      NVSG_API virtual ~StateSet();

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
       * This implementation determines the hints contained in the currently active StateVariant. */
      NVSG_API virtual unsigned int determineHintsContainment(unsigned int which) const;

      NVSG_API virtual bool determineShaderContainment() const;
      NVSG_API virtual bool determineTransparencyContainment() const;
      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      NVSG_API virtual void notifyChange( const Subject * originator, unsigned int state = 0 ) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      VariantContainer::iterator doRemoveVariant( VariantContainer::iterator vci );

    private:
      VariantKey        m_variantKey;
      VariantContainer  m_variants;
  };

  /*! \brief Typedef for a map from a StateSet and a StateAttribute to a StateSet */
  typedef std::map<std::pair<nvsg::StateSetWeakPtr,nvsg::StateAttributeWeakPtr>,nvsg::StateSetSharedPtr>  StateSetReplacementMap;

  /*! \brief Get a StateSet, that references the same objects as a given StateSet and also holds a reference to the
   *  given StateAttribute.
   *  \param replacementStateSets A map from pairs of StateSet and StateAttribute to StateSets that holds results
   *  from previous replacement operations to reduce redundant object creation.
   *  \param sshIn The StateSet to get a replacement for.
   *  \param sahIn The StateAttribute to be part of the replacement StateSet.
   *  \return A StateSet that is exactly the same as \a sshIn, but also holds \a sahIn in every StatePass of the
   *  active StateVariant.
   *  \remarks With this function, you get a StateSet, that is exactly the same as the StateSet \a sshIn, but also
   *  holds the StateAttribute \a sahIn in every StatePass of the active StateVariant. Everything not in the active
   *  StateVariant, as well as all StateAttributes are just referenced, while the active StateVariant and all its
   *  StatePasses are newly created.\n
   *  As this might be a pretty complex operation, the newly created StateSet is stored in the StateSetReplacementMap
   *  \a replacementStateSets, using \a sshIn and \a sahIn as the key. If the same combination of StateSet and
   *  StateAttribute is used with this same map again, the already created StateSet from the map is returned. */
  StateSetSharedPtr getReplacementStateSet( StateSetReplacementMap & replacementStateSets
                                          , const StateSetSharedPtr & sshIn
                                          , const StateAttributeSharedPtr & sahIn );

#if defined( _DEBUG )
  inline bool find( std::map<VariantKey,StateVariantSharedPtr>::const_iterator it
                  , const std::map<VariantKey,StateVariantSharedPtr> &m )
  {
    for ( std::map<VariantKey,StateVariantSharedPtr>::const_iterator i = m.begin() ; i!=m.end() ; ++i )
    {
      if ( i == it )
      {
        return( true );
      }
    }
    return( false );
  }
#endif

  inline VariantKey StateSet::getActiveVariantKey() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_variantKey );
  }

  inline unsigned int StateSet::getNumberOfVariants() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_variants.size()) );
  }

  inline StateSet::VariantConstIterator StateSet::beginVariants() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( VariantConstIterator( m_variants.begin() ) );
  }

  inline StateSet::VariantIterator StateSet::beginVariants()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( VariantIterator( m_variants.begin() ) );
  }

  inline StateSet::VariantConstIterator StateSet::endVariants() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( VariantConstIterator( m_variants.end() ) );
  }

  inline StateSet::VariantIterator StateSet::endVariants()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( VariantIterator( m_variants.end() ) );
  }

  inline StateSet::VariantConstIterator StateSet::findVariant( VariantKey key ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( VariantConstIterator( m_variants.find( key ) ) );
  }

  inline StateSet::VariantIterator StateSet::findVariant( VariantKey key )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( VariantIterator( m_variants.find( key ) ) );
  }

  inline unsigned int StateSet::getNumberOfPasses() const
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( getNumberOfPasses( findVariant( m_variantKey ) ) );
  }

  inline StateVariant::PassConstIterator StateSet::beginPasses() const
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( beginPasses( findVariant( m_variantKey ) ) );
  }

  inline StateVariant::PassIterator StateSet::beginPasses()
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( beginPasses( findVariant( m_variantKey ) ) );
  }

  inline StateVariant::PassConstIterator StateSet::endPasses() const
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( endPasses( findVariant( m_variantKey ) ) );
  }

  inline StateVariant::PassIterator StateSet::endPasses()
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( endPasses( findVariant( m_variantKey ) ) );
  }

  inline StateVariant::PassIterator StateSet::insertPass( const StateVariant::PassIterator & spi, const StatePassSharedPtr & pass )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( insertPass( findVariant( m_variantKey ), spi, pass ) );
  }

  inline bool StateSet::replacePass( const StatePassSharedPtr & newPass, const StatePassSharedPtr & oldPass )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( replacePass( findVariant( m_variantKey ), newPass, oldPass ) );
  }

  inline bool StateSet::replacePass( const StatePassSharedPtr & newPass, StateVariant::PassIterator & oldSPI )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( replacePass( findVariant( m_variantKey ), newPass, oldSPI ) );
  }

  inline bool StateSet::removePass( const StatePassSharedPtr & pass )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( removePass( findVariant( m_variantKey ), pass ) );
  }

  inline StateVariant::PassIterator StateSet::removePass( const StateVariant::PassIterator & spi )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( removePass( findVariant( m_variantKey ), spi ) );
  }

  inline void StateSet::clearPasses()
  {
    return( clearPasses( findVariant( m_variantKey ) ) );
  }

  inline StateVariant::PassConstIterator StateSet::findPass( const StatePassSharedPtr & pass ) const
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( findPass( findVariant( m_variantKey ), pass ) );
  }

  inline StateVariant::PassIterator StateSet::findPass( const StatePassSharedPtr & pass )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    return( findPass( findVariant( m_variantKey ), pass ) );
  }


  inline unsigned int StateSet::getNumberOfPasses( const VariantConstIterator & svci ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    if ( svci.m_iter != m_variants.end() )
    {
      return( StateVariantReadLock( svci.m_iter->second )->getNumberOfPasses() );
    }
    return( 0 );
  }

  inline StateVariant::PassConstIterator StateSet::beginPasses( const VariantConstIterator & svci ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( svci.m_iter != m_variants.end() );
    return( StateVariantReadLock( svci.m_iter->second )->beginPasses() );
  }

  inline StateVariant::PassIterator StateSet::beginPasses( const VariantIterator & svi )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi.m_iter->second )->beginPasses() );
  }

  inline StateVariant::PassConstIterator StateSet::endPasses( const VariantConstIterator & svci ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( svci.m_iter != m_variants.end() );
    return( StateVariantReadLock( svci.m_iter->second )->endPasses() );
  }

  inline StateVariant::PassIterator StateSet::endPasses( const VariantIterator & svi )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi.m_iter->second )->endPasses() );
  }

  inline StateVariant::PassIterator StateSet::addPass( const VariantIterator & svi, const StatePassSharedPtr & pass )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi->second )->addPass( pass ) );
  }

  inline StateVariant::PassIterator StateSet::insertPass( const VariantIterator & svi, const StateVariant::PassIterator & spi, const StatePassSharedPtr & pass )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi->second )->insertPass( spi, pass ) );
  }

  inline bool StateSet::replacePass( const VariantIterator & svi, const StatePassSharedPtr & newPass, const StatePassSharedPtr & oldPass )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi.m_iter->second )->replacePass( newPass, oldPass ) );
  }

  inline bool StateSet::replacePass( const VariantIterator & svi, const StatePassSharedPtr & newPass, StateVariant::PassIterator & oldSPI )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi.m_iter->second )->replacePass( newPass, oldSPI ) );
  }

  inline bool StateSet::removePass( const VariantIterator & svi, const StatePassSharedPtr & pass )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi.m_iter->second )->removePass( pass ) );
  }

  inline StateVariant::PassIterator StateSet::removePass( const VariantIterator & svi, const StateVariant::PassIterator & spi )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi.m_iter->second )->removePass( spi ) );
  }

  inline void StateSet::clearPasses( const VariantIterator & svi )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi.m_iter->second )->clearPasses() );
  }

  inline StateVariant::PassConstIterator StateSet::findPass( const VariantConstIterator & svci, const StatePassSharedPtr & pass ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( svci.m_iter != m_variants.end() );
    return( StateVariantReadLock( svci.m_iter->second )->findPass( pass ) );
  }

  inline StateVariant::PassIterator StateSet::findPass( const VariantIterator & svi, const StatePassSharedPtr & pass )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi.m_iter->second )->findPass( pass ) );
  }


  inline StatePass::AttributeConstIterator StateSet::beginAttributes() const
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantReadLock( findVariant( m_variantKey )->second )->beginAttributes() );
  }

  inline StatePass::AttributeIterator StateSet::beginAttributes()
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantWriteLock( findVariant( m_variantKey )->second )->beginAttributes() );
  }

  inline StatePass::AttributeConstIterator StateSet::endAttributes() const
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantReadLock( findVariant( m_variantKey )->second )->endAttributes() );
  }

  inline StatePass::AttributeIterator StateSet::endAttributes()
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantWriteLock( findVariant( m_variantKey )->second )->endAttributes() );
  }

  inline StatePass::AttributeIterator StateSet::removeAttribute( const StatePass::AttributeIterator & sci )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantWriteLock( findVariant( m_variantKey )->second )->removeAttribute( sci ) );
  }

  inline StatePass::AttributeIterator StateSet::removeAttributeByObjectCode( unsigned int objectCode )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantWriteLock( findVariant( m_variantKey )->second )->removeAttributeByObjectCode( objectCode ) );
  }

  inline StatePass::AttributeConstIterator StateSet::findAttribute( const StateAttributeSharedPtr & attribute ) const
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantReadLock( findVariant( m_variantKey )->second )->findAttribute( attribute ) );
  }

  inline StatePass::AttributeIterator StateSet::findAttribute( const StateAttributeSharedPtr & attribute )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantWriteLock( findVariant( m_variantKey )->second )->findAttribute( attribute ) );
  }

  inline StatePass::AttributeConstIterator StateSet::findAttribute( unsigned int code ) const
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantReadLock( findVariant( m_variantKey )->second )->findAttribute( code ) );
  }

  inline StatePass::AttributeIterator StateSet::findAttribute( unsigned int code )
  {
    NVSG_ASSERT( m_variantKey != INVALID_VARIANT_KEY );
    NVSG_ASSERT( findVariant( m_variantKey ) != endVariants() );
    return( StateVariantWriteLock( findVariant( m_variantKey )->second )->findAttribute( code ) );
  }


  inline StatePass::AttributeConstIterator StateSet::beginAttributes( const VariantConstIterator & svci, const StateVariant::PassConstIterator & spci ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( svci.m_iter != m_variants.end() );
    return( StateVariantReadLock( svci->second )->beginAttributes( spci ) );
  }

  inline StatePass::AttributeIterator StateSet::beginAttributes( const VariantIterator & svi, const StateVariant::PassIterator & spi )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi->second )->beginAttributes( spi ) );
  }

  inline StatePass::AttributeConstIterator StateSet::endAttributes( const VariantConstIterator & svci, const StateVariant::PassConstIterator & spci ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( svci.m_iter != m_variants.end() );
    return( StateVariantReadLock( svci->second )->endAttributes( spci ) );
  }

  inline StatePass::AttributeIterator StateSet::endAttributes( const VariantIterator & svi, const StateVariant::PassIterator & spi )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi->second )->endAttributes( spi ) );
  }

  inline StatePass::AttributeIterator StateSet::addAttribute( const VariantIterator & svi, const StateVariant::PassIterator & spi, const StateAttributeSharedPtr & attribute )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi->second )->addAttribute( spi, attribute ) );
  }

  inline StatePass::AttributeIterator StateSet::removeAttribute( const VariantIterator & svi, const StateVariant::PassIterator & spi, const StatePass::AttributeIterator & sci )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi->second )->removeAttribute( spi, sci ) );
  }

  inline StatePass::AttributeIterator StateSet::removeAttributeByObjectCode( const VariantIterator & svi, const StateVariant::PassIterator & spi, unsigned int objectCode )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi->second )->removeAttributeByObjectCode( spi, objectCode ) );
  }

  inline StatePass::AttributeConstIterator StateSet::findAttribute( const VariantConstIterator & svci, const StateVariant::PassConstIterator & spci, const StateAttributeSharedPtr & attribute ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( svci.m_iter != m_variants.end() );
    return( StateVariantReadLock( svci->second )->findAttribute( spci, attribute ) );
  }

  inline StatePass::AttributeIterator StateSet::findAttribute( const VariantIterator & svi, const StateVariant::PassIterator & spi, const StateAttributeSharedPtr & attribute )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi->second )->findAttribute( spi, attribute ) );
  }

  inline StatePass::AttributeConstIterator StateSet::findAttribute( const VariantConstIterator & svci, const StateVariant::PassConstIterator & spci, unsigned int code ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( svci.m_iter != m_variants.end() );
    return( StateVariantReadLock( svci->second )->findAttribute( spci, code ) );
  }

  inline StatePass::AttributeIterator StateSet::findAttribute( const VariantIterator & svi, const StateVariant::PassIterator & spi, unsigned int code )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( svi.m_iter != m_variants.end() );
    return( StateVariantWriteLock( svi->second )->findAttribute( spi, code ) );
  }

} //  namespace nvsg

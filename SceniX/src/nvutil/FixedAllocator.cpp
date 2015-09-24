#include "pch.h"
#include <nvutil/Allocator.h>

namespace nvutil 
{
	Chunk::Chunk (unsigned int blocksize)
	{
		m_chunkSize = blocksize * Chunk::numBlocks;
		m_blockSize = blocksize;
		init ();
	}
	FixedAllocator::FixedAllocator ()
	{
		m_blockSize = 4096;
	}
	FixedAllocator::~FixedAllocator ()
	{
	}

	
	void *FixedAllocator::alloc (void)
	{
		void *theReturnAddress = NULL;



		// Try to allocate on the available chunks.
		bool allocated = false;
		std::set<Chunk *>::iterator its = m_availableChunks.begin ();
		while (its!=m_availableChunks.end())
		{

			if ((*its)->blocksAvailable())
			{
				theReturnAddress = (*its)->alloc ();
				allocated = true;
				if ((*its)->blocksAvailable() == 0)
				{
					m_availableChunks.erase (its);
				}
				break;
			}
			its++;
		}

		if (!allocated)
		{
			// Create a new chunk.
			nvutil::Chunk *ck = new Chunk (m_blockSize);
			theReturnAddress = ck->alloc ();
			this->m_chunks.insert (std::pair<void*,Chunk*>(theReturnAddress,ck));
			if (ck->blocksAvailable ())
			{
				this->m_availableChunks.insert (ck);
			}

			// Allocate.

			// Push new chunk to m_availableChunks if aviaalbe for later use.
		}
		return theReturnAddress;
	}
	void FixedAllocator::dealloc ( void *addr)
	{
		std::map<void *, Chunk *>::iterator its = m_chunks.find (addr);//begin ();
		if (its != m_chunks.end())
		{
			((*its).second)->dealloc (addr);
			// if availble and not on m_aviablehunks.. then add it to avialable chunks.
			std::set<Chunk*>::iterator itav = m_availableChunks.find ((*its).second);
			if (itav != m_availableChunks.end())
				m_availableChunks.insert ((*its).second);

		}

	}
}
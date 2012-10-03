	/********************************************************************
	*																	*
	*							Includes								*
	*																	*
	********************************************************************/

	#include "i_memory.h"

	/********************************************************************
	*																	*
	*							Internals								*
	*																	*
	********************************************************************/

	PRIVATE MEMORY MemMgr;	// Memory manager; linkage restricted to memory library

	/********************************************************************************
	*																				*
	*								MemInit											*
	*																				*
	********************************************************************************/	

	// Purpose:	Initializes a memory manager
	// Input:   No input
	// Return:  A code indicating the results of the initialization 

	PUBLIC RETCODE MemInit (CONFIG * config)
	{
		Dword PoolSize;	// Size to allot for memory frame

		assert(config);	// Verify that config points to valid memory

		PoolSize = ALIGNED(config->PoolSize);	// Get requested size of memory frame and align it

		// Check for meaningless configurations
		ASSERT_MSG(PoolSize != 0,"Invalid pool size: no operation","MemInit");

		MemMgr.Pool = (PMEMBLOCK) malloc (PoolSize);
		// Allocate memory for manager object and pool

		if (MemMgr.Pool == NULL)	// Ascertain that malloc succeeded
		{
			MESSAGE_BOX("Unable to allocate memory","MemInit");
			return RETCODE_FAILURE;	// Return failure
		}

		MemMgr.FrameBytes = PoolSize;	// Load up amount of frame memory
		MemMgr.FreeBytes  = PoolSize;	// Load up amount of available memory

		MemMgr.Pool->Type = MEMTYPE_UNUSED;				// Prepare pool for use
		MemMgr.Pool->Size = PoolSize - sizeof(MEMBLOCK);// Set pool amount available

		MemMgr.Pool->Prev = MemMgr.Pool;// Loop pool backward
		MemMgr.Pool->Next = MemMgr.Pool;// Loop pool forward

		MemInsertAmongUnusedBlocks (MemMgr.Pool);	// Initialize pool

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemTerm											*
	*																				*
	********************************************************************************/	

	// Purpose:	Deinitializes a memory manager
	// Input:   Optional name of a log file
	// Return:  A code indicating the results of the termination

	PUBLIC RETCODE MemTerm (char const * LogFile)
	{		
		if (LogFile != NULL)	// User requests diagnostics
		{
			PMEMBLOCK MemBlock = MemMgr.Pool;	// Refer to first memory block

			FILE * fpLog;	// File used to log debug info

			char Pattern[9] = {0};	// Buffer used to retrieve memory patterns

			fpLog = fopen (LogFile, "wt");	// Create a log file

			if (fpLog != NULL)	// Log diagnostics if file creation was successful
			{
				// Output used entry/byte information
				fprintf (fpLog, "%d unfreed entries\n", MemMgr.numUsed);
				fprintf (fpLog, "%d unfreed bytes\n",   MemMgr.FrameBytes - MemMgr.FreeBytes);

				if (MemMgr.numUsed != 0) do {	// If memory is unfreed, list instances
					if (MemBlock->Type == MEMTYPE_USED)	// Check if memory block is not freed
					{
						// Retrieve memory pattern
						MemGetPattern (&MemBlock[1], Pattern);

						if (Pattern[0] == '\0')	// Check for unlabeled blocks
							fprintf (fpLog, "No pattern\n");// Print message

						else fprintf (fpLog, "Entry: %s\n", Pattern);	// Print label

						fprintf (fpLog, "Bytes used: %u\n", MemBlock->Size);// Print memory consumption

						Pattern[0] = '\0';	// Reset to prepare next block
					}

					MemBlock = MemBlock->Next;	// Go to next block in memory chain
				} while (MemBlock != MemMgr.Pool);	// Loop through all blocks

				fclose (fpLog);	// Close the log file
			}
		}

		free (MemMgr.Pool);	// Deallocate memory pool

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemAlloc										*
	*																				*
	********************************************************************************/	
	
	// Purpose:	Used to allocate memory of a given size to a particular datum
	// Input:   Pointer to context to attach memory to, block size, and options
	// Return:  A memory datum indicating private memory information

	PUBLIC RETCODE MemAlloc (void ** memory, Dword numBytes, FLAGS Options)
	{
		assert(memory);	// Verify that memory points to valid memory

		// Verify meaningful allocation request
		ASSERT_MSG(numBytes != 0,"No operation: MemAlloc failed","Error");

		numBytes = ALIGNED(numBytes);	// Align requested amount

		*memory = MemGrabBlock (numBytes, Options);	// Allocate requested amount of memory

		return *memory != NULL;	// Return allocation result
	}

	/********************************************************************************
	*																				*
	*								MemRealloc										*
	*																				*
	********************************************************************************/	
	
	// Purpose:	Used to reallocate a memory variable
	// Input:	Pointer to context to reallocate (may be NULL), block size (may be 0), and options
	// Return:	A code indicating the results of the allocation

	PUBLIC RETCODE MemRealloc (void ** memory, Dword numBytes, FLAGS Options)
	{
		PMEMBLOCK MemBlock;	// Owner memory block

		assert(memory);	// Verify that memory points to valid memory

		numBytes = ALIGNED(numBytes);	// Align requested amount

		// If no context exists, perform initial allocation
		if (*memory == NULL) return MemAlloc (memory, numBytes, Options);

		// If byte count is zero, free memory
		if (numBytes == 0) return MemFree (memory);

		MemBlock = (PMEMBLOCK) ((Pbyte) *memory - sizeof(MEMBLOCK));// Refer to memory's owning block

 		if (numBytes < MemBlock->Size) return MemReduceBlock (MemBlock, numBytes);	// Attempt to reduce block

		else if (numBytes > MemBlock->Size)	*memory = MemGrowBlock (MemBlock, numBytes, Options);	// Attempt to grow block

		return *memory != NULL;	// Return allocation result
	}

	/********************************************************************************
	*																				*
	*								MemFree											*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to release memory of a particular datum
	// Input:   Pointer to context to release from
	// Return:  A code indicating the results of the release

	PUBLIC RETCODE MemFree (void ** memory)
	{
		assert(memory);	// Verify that memory points to valid memory

		// Verify that attempt is made to free unreleased memory
		ASSERT_MSG(*memory != NULL,"Attempt to free invalid memory: MemFree failed","Error");

		MemReleaseBlock (*memory);	// Deallocate memory

		*memory = NULL;	// Invalidate context

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemGetPattern									*
	*																				*
	********************************************************************************/

	// Purpose:	Used to retrieve a pattern used to identify memory
	// Input:   Memory variable associated with pattern, and a buffer to load
	// Return:  A code indicating the results of getting the pattern

	PUBLIC RETCODE MemGetPattern (void * memory, char Pattern[])
	{
		PMEMBLOCK MemBlock;	// Owner memory block

		int index;	// Loop variable

		assert(memory), assert(Pattern);// Verify that memory and Pattern point to valid memory

		MemBlock = (PMEMBLOCK) ((Pbyte) memory - sizeof(MEMBLOCK));	// Refer to memory's owning block

		for (index = 0; index < 8; ++index)	// Loop through pattern
		{
			Pattern[index] = MemBlock->Pattern[index];	// Get byte

			if (MemBlock->Pattern[index] == '\0') break;// Terminate pattern on null byte
		}

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemSetPattern									*
	*																				*
	********************************************************************************/

	// Purpose:	Used to associate a pattern to a memory variable
	// Input:   Memory variable to associate with pattern, and pattern to assign
	// Return:  A code indicating the results of setting the pattern

	PUBLIC RETCODE MemSetPattern (void * memory, char Pattern[])
	{
		PMEMBLOCK MemBlock;	// Owner memory block

		int index;	// Loop variable

		assert(memory), assert(Pattern);// Verify that memory and Pattern point to valid memory

		MemBlock = (PMEMBLOCK)((Pbyte) memory - sizeof(MEMBLOCK));	// Refer to memory's owning block

		for (index = 0; index < 8; ++index)	// Loop through pattern
		{
			MemBlock->Pattern[index] = Pattern[index];	// Set byte

			if (MemBlock->Pattern[index] == '\0') break;// Terminate pattern on null byte
		}

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemGetSize										*
	*																				*
	********************************************************************************/

	// Purpose:	Used to retrieve the size of a memory variable
	// Input:   Memory variable to obtain size of
	// Return:  Amount of bytes alloted to memory variable (includes padding)

	PUBLIC long MemGetSize (void * memory)
	{
		PMEMBLOCK MemBlock;	// Owner memory block

		assert(memory);	// Verify that memory points to valid memory

		MemBlock = (PMEMBLOCK)((Pbyte) memory - sizeof(MEMBLOCK));	// Refer to memory's owning block

		return MemBlock->Size;	// Return size associated with memory block
	}

	/********************************************************************************
	*																				*
	*								MemGetDiagnostics								*
	*																				*
	********************************************************************************/

	// Purpose:	Used to acquire memory diagnostics
	// Input:   Pointer to a diagnostics structure
	// Return:  A code indicating the results of acquiring diagnostics

	PUBLIC RETCODE MemGetDiagnostics (MEMDATA * MemData)
	{
		assert(MemData);// Verify that MemData points to valid memory

		MemData->BytesUsed = MemMgr.FrameBytes - MemMgr.FreeBytes;	// Acquire used byte count
		MemData->BytesFree = MemMgr.FreeBytes;						// Acquire free byte count

		MemData->numUnused = MemMgr.numUnused;	// Acquire unused block count
		MemData->numUsed   = MemMgr.numUsed;	// Acquire used block count

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemGrabBlock									*
	*																				*
	********************************************************************************/

	// Purpose:	Used to grab a memory block for use by outside data
	// Input:	Byte size of block to grab, and option data
	// Return:	A pointer to a block of memory

	PRIVATE void * MemGrabBlock (Dword numBytes, FLAGS Options)
	{
		PMEMBLOCK MemBlock = MemMgr.Cache;	// Current memory block

		Dword NumBlocks = MemMgr.numUnused;	// Low-level alias for number of unused blocks in manager

		_asm {	// Scan pool for adequately large memory block
			mov esi, dword ptr [MemBlock];	// Store address of next block
			mov eax, dword ptr [numBytes];	// Store size of request
			mov ecx, dword ptr [NumBlocks];	// Store block count
$findMem:	cmp dword ptr [esi].O_SIZE, eax;// Check whether previous block is at least of requested capacity
			jae $memFound;	// If block is adequate, break out of loop
		 	mov esi, dword ptr [esi].O_TNEXT;	// Advance to next block
			dec ecx;// Reduce search count
			jnz $findMem;	// Iterate again
			xor esi, esi;	// If no entry found, set block to NULL
$memFound:	mov dword ptr [MemBlock], esi;	// Load pointer to memory block
		}

		if (MemBlock == NULL) return NULL;	// Return null block if search failed

		MemExtractUnusedBlock (MemBlock);	// Detach memory block from unused block section

		MemReduceBlock (MemBlock, numBytes);// Pare memory block down to necessary byte size

		// DEBUG: Allot memory for the memory block
		assert(MemAllotBytes (MemBlock->Size + sizeof(MEMBLOCK)) == RETCODE_SUCCESS);

		++MemMgr.numUsed;	// Document addition of used block

		// Check for request to zero memory
		if (GETFLAGS(Options,M_ZERO)) _asm {	// Zero-fill all bytes in data chunk
			cld;// Make string operations perform forward 
			xor	eax, eax;	// Set fill value to zero
			mov	esi, dword ptr [MemBlock];	// Set fill context to MemBlock
			mov ecx, dword ptr [esi].O_SIZE;// Set fill count to numBytes
			lea edi, dword ptr [esi].O_DATA;// Set fill region to MemBlock's data section
			shr	ecx, 2;	// Adjust fill count from bytes to dwords
			rep stosd;	// Blast dwords to MemBlock's data section
		}

		MemBlock->Pattern[0] = '\0';// Zero out byte for pattern usage

		// Verify that a used block is being given to the user
		ASSERT_MSG(MemBlock->Type == MEMTYPE_USED,"Invalid type: MemGrabBlock failed","Error");

		return &MemBlock[1];// Return pointer to data chunk
	}

	/********************************************************************************
	*																				*
	*								MemReleaseBlock									*
	*																				*
	********************************************************************************/

	// Purpose:	Used to release a memory block used by outside data
	// Input:	A data block to release
	// Return:	A code indicating the results of the block release operation

	PRIVATE RETCODE MemReleaseBlock (void * data)
	{
		PMEMBLOCK MemBlock = (PMEMBLOCK) ((Pbyte) data - sizeof(MEMBLOCK));	// Current memory block

		// Verify that a used block is to be released
		ASSERT_MSG(MemBlock->Type == MEMTYPE_USED,"Invalid type: MemReleaseBlock failed","Error");

		// DEBUG: Collect memory from the memory block
		assert(MemCollectBytes (MemBlock->Size + sizeof(MEMBLOCK)) == RETCODE_SUCCESS);

		--MemMgr.numUsed;	// Document removal of used block

		MemInsertAmongUnusedBlocks (MemBlock);	// Attach memory block to unused block section

		MemBlock->Type = MEMTYPE_UNUSED;// Document memory block's removal

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemAllotBytes									*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:	Used to allot bytes of memory from the memory pool
	// Input:   A number of bytes to allot
	// Return:	A code indicating the results of the byte allotment

	PRIVATE RETCODE MemAllotBytes (Dword numBytes)
	{
		if (numBytes == 0)	// Check for meaningless allotments
		{
			MESSAGE_BOX("No operation: MemAllotBytes failed","Error");
			return RETCODE_FAILURE;	// Return failure
		}

		if (MemMgr.FreeBytes < numBytes)	// Check whether allotment may be processed
		{
			MESSAGE_BOX("Out of memory: MemAllotBytes failed","Error");
			return RETCODE_FAILURE;	// Return failure
		}

		MemMgr.FreeBytes -= numBytes;	// Decrement count of free bytes

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemCollectBytes									*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:	Used to collect alloted bytes of memory into the memory pool
	// Input:   A number of bytes to collect
	// Return:  A code indicating the results of the byte collection

	PRIVATE RETCODE MemCollectBytes (Dword numBytes)
	{
		if (numBytes == 0)	// Check for meaningless collections
		{
			MESSAGE_BOX("No operation: MemCollectBytes failed","Error");
			return RETCODE_FAILURE;	// Return failure
		}

		if (MemMgr.FreeBytes + numBytes > MemMgr.FrameBytes)	// Check whether collection may be processed
		{
			MESSAGE_BOX("Overflow: MemCollectBytes failed","Error");
			return RETCODE_FAILURE;	// Return failure
		}

		MemMgr.FreeBytes += numBytes;	// Increment count of free bytes

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemExtractUnusedBlock							*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to extract a block from the unused blocks
	// Input:	Pointer to a memory block
	// Return:	A code indicating the results of the extraction

	PRIVATE RETCODE MemExtractUnusedBlock (MEMBLOCK * MemBlock)
	{
		// Verify that valid type of memory is being extracted
		ASSERT_MSG(MemBlock->Type == MEMTYPE_UNUSED,"Invalid type: MemExtractUnusedBlock failed","Error");
		// Verify that unused blocks exist
		ASSERT_MSG(MemMgr.numUnused != 0,"No unused blocks: MemExtractUnusedBlock failed","Error");

		if (MemBlock == MemMgr.Cache)	// Check whether cache is to be extracted
		{
			if (MemMgr.numUnused == 1)	// Check whether extraction will empty cache
				MemMgr.Cache = NULL;	// Nullify cache

			else MemMgr.Cache = MemBlock->tNext;// Reassign cache
		}

		MemBlock->tPrev->tNext = MemBlock->tNext;	// Remove memory block's typed back reference
		MemBlock->tNext->tPrev = MemBlock->tPrev;	// Remove memory block's typed forward reference
		
		--MemMgr.numUnused;	// Decrease unused block count

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemInsertAmongUnusedBlocks						*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to place a block within the unused blocks
	// Input:	Pointer to a memory block
	// Return:	A code indicating the results of the insertion

	PRIVATE RETCODE MemInsertAmongUnusedBlocks (MEMBLOCK * MemBlock)
	{
		if (MemMgr.numUnused == 0)	// Check whether cache is empty
		{
			MemMgr.Cache = MemBlock;	// If so, load cache

			MemBlock->tPrev = MemMgr.Cache;	// Refer memory block back to cache
			MemBlock->tNext = MemMgr.Cache;	// Refer memory block forward to cache
		}

		else	// Check for other cases
		{
			if (MemAdjoinBlocks (MemBlock) != M_DISJOINT)	// Check whether block may adjoin to memory
				return RETCODE_SUCCESS;	// Return trivial success

			MemBlock->tPrev = MemMgr.Cache;			// Refer memory block back
			MemBlock->tNext = MemMgr.Cache->tNext;	// Refer memory block forward
		}

		MemBlock->tPrev->tNext = MemBlock;	// Refer memory block's previous unused block ahead to memory block
		MemBlock->tNext->tPrev = MemBlock;	// Refer memory block's next unused block back to memory block
						
		++MemMgr.numUnused;// Increment unused block count

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemAdjoinBlocks									*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to adjoin memory
	// Input:	Address of pointer to a memory block
	// Return:	Amount of non-adjoinments

	PRIVATE long MemAdjoinBlocks (MEMBLOCK * MemBlock)
	{
		PMEMBLOCK PrevBlock;// Pointer to previous memory block
		PMEMBLOCK NextBlock;// Pointer to next memory block

		long Ends = 0;	// Counter for memory block's unjoined ends

		PrevBlock = MemBlock->Prev;	// Retrieve previous block in chain
		NextBlock = MemBlock->Next;	// Retrieve next block in chain

		if (NextBlock->Type == MEMTYPE_UNUSED && NextBlock > MemBlock)	// Check whether memory block may join with upper memory block
			MemJoinToUpperBlock (MemBlock, NextBlock);	// Attach memory to upper block

		else ++Ends;// Increment unjoined end counter

		if (PrevBlock->Type == MEMTYPE_UNUSED && PrevBlock < MemBlock)	// Check whether lower memory block may join with memory block
			MemJoinToLowerBlock (MemBlock, PrevBlock, Ends == 0);	// Attach memory to lower block

		else ++Ends;// Increment unjoined end counter

		return Ends;// Return number of unjoined ends
	}

	/********************************************************************************
	*																				*
	*								MemReduceBlock									*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to shrink a memory block
	// Input:	Pointer to a memory block, and size to reduce to
	// Return:	A code indicating the results of the reduction

	PRIVATE RETCODE MemReduceBlock (MEMBLOCK * MemBlock, Dword numBytes)
	{
		Dword Padding = MemBlock->Size - numBytes;	// Padding between block size and request

		PMEMBLOCK NewBlock;	// New block to create after padding

		MemBlock->Size = numBytes;		// Set alloted block's size
		MemBlock->Type = MEMTYPE_USED;	// Set alloted block's type

		if (Padding >= sizeof(MEMBLOCK))	// Check whether room exists for a new data block
		{
			NewBlock = (PMEMBLOCK) PTR_OFF(&MemBlock[1],numBytes);	// Grab a new block at the beginning of the padding

			NewBlock->Size = Padding - sizeof(MEMBLOCK);// Set size of new memory block
			NewBlock->Type = MEMTYPE_UNUSED;			// Set type of new memory block

			NewBlock->Prev = MemBlock;		// Bind new block back
			NewBlock->Next = MemBlock->Next;// Bind new block ahead

			NewBlock->Prev->Next = NewBlock;// Bind previous block ahead to new block
			NewBlock->Next->Prev = NewBlock;// Bind next block back to new block

			MemInsertAmongUnusedBlocks (NewBlock);	// Attach new block to unused block section
		}

		else MemBlock->Size += Padding;	// Accumulate padding

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemGrowBlock									*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to enlarge a memory block
	// Input:	Pointer to a memory block, size to grow to, and options
	// Return:	Pointer to memory location of enlarged block

	PRIVATE void * MemGrowBlock (MEMBLOCK * MemBlock, Dword numBytes, FLAGS Options)
	{
		Dword MemSize = MemBlock->Size;	// Cached value of current block size

		Dword Diff = numBytes - MemSize;// Difference to make up between block size and request

		PMEMBLOCK Prev = MemBlock->Prev;// Previous block in chain
		PMEMBLOCK Next = MemBlock->Next;// Next block in chain

		void * Datum, * MemOff;	// Newly allocated block; offset into memory

		BOOL IsNew = FALSE;	// Indicates that block was relocated

		if (Next->Type == MEMTYPE_UNUSED && Next > MemBlock && Next->Size + sizeof(MEMBLOCK) >= Diff)	// Check whether upper block may be coalesced
		{
			MemAbsorbUpperBlock (MemBlock, Next);	// Attach memory to upper block

			MemReduceBlock (MemBlock, numBytes);// Reduce block to the minimum necessary size

			// DEBUG: Allot new memory for the memory block
			assert(MemAllotBytes (MemBlock->Size - MemSize) == RETCODE_SUCCESS);

			Datum = &MemBlock[1];	// Memory remains intact
		}

		else	// Obtain a new block
		{
			if (Prev->Type == MEMTYPE_UNUSED && Prev < MemBlock && Prev->Size + sizeof(MEMBLOCK) >= Diff)	// Check whether lower block may be coalesced
			{
				MemAbsorbLowerBlock (MemBlock, Prev);	// Attach memory to lower block

				MemReduceBlock (Prev, numBytes);// Reduce block to the minimum necessary size

				// DEBUG: Allot new memory for the memory block
				assert(MemAllotBytes (Prev->Size - MemSize) == RETCODE_SUCCESS);

				Datum = &Prev[1];	// Shuffle memory backward
			}

			else	// Obtain a block elsewhere in memory
			{
				Datum = MemGrabBlock (numBytes, 0);	// Grab new memory block

				if (Datum == NULL) return NULL;	// Fail if unable to find a new block

				IsNew = TRUE;	// Indicate that a new block was generated
			}
			
			MemOff = &MemBlock[1];	// Refer to source to copy over

			_asm {	// Copy all bytes over
				cld;// Make string operation perform forward 
				mov ecx, dword ptr [MemSize];	// Set copy-over count
				mov esi, dword ptr [MemOff];// Set copy-over source
				mov edi, dword ptr [Datum];	// Set copy-over destination
				shr	ecx, 2;	// Adjust copy-over count from bytes to dwords
				rep movsd;	// Copy-over dwords into data section
			}
		}

		if (GETFLAGS(Options,M_ZERO))	// Check for request to zero new memory
		{
			MemOff = PTR_OFF(Datum,MemSize);// Compute offset into the new portion of memory
		
			_asm {	// Zero-fill all bytes in data chunk
				cld;// Make string operation perform forward 
				xor	eax, eax;	// Set fill value to zero
				mov ecx, dword ptr [Diff];	// Set fill count to numBytes
				mov edi, dword ptr [MemOff];// Set fill region to MemBlock's data section
				shr	ecx, 2;	// Adjust fill count from bytes to dwords
				rep stosd;	// Blast dwords to MemBlock's data section
			}
		}

		if (IsNew)	// Check whether block was completely reallocated
		{
			MemSetPattern (Datum, (char *) MemBlock->Pattern);	// Preserve original pattern

			MemReleaseBlock (&MemBlock[1]);	// Release original memory
		}

		return Datum;	// Return allocated datum
	}

	/********************************************************************************
	*																				*
	*								MemJoinToUpperBlock								*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to join a memory block to an upper block in the chain
	// Input:	Pointer to a memory block, and pointer to upper block
	// Return:	A code indicating the results of the join

	PRIVATE RETCODE MemJoinToUpperBlock (MEMBLOCK * MemBlock, MEMBLOCK * NextBlock)
	{
		MemBlock->Size += NextBlock->Size + sizeof(MEMBLOCK);	// Accumulate next block's size

		if (MemMgr.numUnused == 1)	// Check whether a single node is to be joined to
		{
			MemBlock->tPrev = MemBlock;	// Refer memory block back to self
			MemBlock->tNext = MemBlock;	// Refer memory block forward to self
		}

		else	// Check for other cases
		{
			MemBlock->tPrev = NextBlock->tPrev;	// Refer memory block back to next block's previous typed block	
			MemBlock->tNext = NextBlock->tNext;	// Refer memory block forward to next block's next typed block

			MemBlock->tPrev->tNext = MemBlock;	// Refer memory block's typed previous block ahead to memory block
			MemBlock->tNext->tPrev = MemBlock;	// Refer memory block's typed next block back to memory block
		}
		
		MemBlock->Next		 = NextBlock->Next;	// Link memory block ahead to block beyond next block
		MemBlock->Next->Prev = MemBlock;		// Link next block back to memory block

		MemMgr.Cache = MemBlock;// Restart cache

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemJoinToLowerBlock								*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to join a memory block to a lower block in the chain
	// Input:	Pointer to a memory block, pointer to lower block, and a boolean indicating whether memory has already joined
	// Return:	A code indicating the results of the join

	PRIVATE RETCODE MemJoinToLowerBlock (MEMBLOCK * MemBlock, MEMBLOCK * PrevBlock, BOOL Join)
	{
		PrevBlock->Size += MemBlock->Size + sizeof(MEMBLOCK);	// Accumulate memory block's size

		if (Join == TRUE)	// Check whether memory block has been joined ahead
		{
			--MemMgr.numUnused;	// Document adjoinment of PrevBlock, MemBlock, and NextBlock

			if (MemMgr.numUnused > 1)	// Check whether more than one unused node remains
			{
				MemBlock->tPrev->tNext = MemBlock->tNext;	// Bind memory block's previous block forward
				MemBlock->tNext->tPrev = MemBlock->tPrev;	// Bind memory block's next block back
			}
		}

		PrevBlock->Next		  = MemBlock->Next;	// Link previous block ahead to data block
		PrevBlock->Next->Prev = PrevBlock;		// Link data block back to previous block

		MemMgr.Cache = PrevBlock;	// Restart cache

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemAbsorbUpperBlock								*
	*																				*
	********************************************************************************/	

	// Purpose:	Used to absorb an upper block in the chain
	// Input:	Pointer to a memory block, and pointer to upper block
	// Return:	A code indicating the results of the absorption

	PRIVATE RETCODE MemAbsorbUpperBlock (MEMBLOCK * MemBlock, MEMBLOCK * NextBlock)
	{
		MemExtractUnusedBlock (NextBlock);	// Decouple upper block from free blocks

		MemBlock->Size += NextBlock->Size + sizeof(MEMBLOCK);	// Accumulate next block's size

		MemBlock->Next		 = NextBlock->Next;	// Link memory block ahead to block beyond next block
		MemBlock->Next->Prev = MemBlock;		// Link next block back to memory block

		return RETCODE_SUCCESS;	// Return success
	}

	/********************************************************************************
	*																				*
	*								MemAbsorbLowerBlock								*
	*																				*
	********************************************************************************/

	// Purpose:	Used to absorb a lower block in the chain
	// Input:	Pointer to a memory block, and pointer to lower block
	// Return:	A code indicating the results of the absorption

	PRIVATE RETCODE MemAbsorbLowerBlock (MEMBLOCK * MemBlock, MEMBLOCK * PrevBlock)
	{
		MemExtractUnusedBlock (PrevBlock);	// Decouple lower block from free blocks

		PrevBlock->Size += MemBlock->Size + sizeof(MEMBLOCK);	// Accumulate memory block's size

		PrevBlock->Next		  = MemBlock->Next;	// Link previous block ahead to data block
		PrevBlock->Next->Prev = PrevBlock;		// Link data block back to previous block

		PrevBlock->Type = MEMTYPE_USED;	// Put new block into use

		MemSetPattern (&PrevBlock[1], (char *) MemBlock->Pattern);	// Preserve original pattern

		return RETCODE_SUCCESS;	// Return success
	}

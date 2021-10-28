/*
 * Copyright (c) 2018-2021 The Forge Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/

#if _WIN32
#pragma warning( disable: 4702 4996 4456 4244 4800 4324 4005 )
#endif

#include "RendererConfig.h"

#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "../../ThirdParty/tinyimageformat/tinyimageformat_base.h"
#include "../../ThirdParty/tinyimageformat/tinyimageformat_query.h"
#include "../../ThirdParty/tinyimageformat/tinyimageformat_bits.h"
#include "../../ThirdParty/tinyimageformat/tinyimageformat_apis.h"

#include "IRenderer.h"
#include "IResourceLoader.h"
#include "../Interfaces/ILog.h"
#include "../Interfaces/IThread.h"

#if defined(__ANDROID__) && defined(VULKAN)
#include <shaderc/shaderc.h>
#endif

#if defined(GLES)
#include "OpenGLES/GLESContextCreator.h"
#endif

#include "../Interfaces/IMemory.h"

#ifdef NX64
#include "../ThirdParty/OpenSource/murmurhash3/MurmurHash3_32.h"
#endif

struct SubresourceDataDesc
{
	uint64_t mSrcOffset;
	uint32_t mMipLevel;
	uint32_t mArrayLayer;
#if defined(DIRECT3D11) || defined(METAL) || defined(VULKAN)
	uint32_t mRowPitch;
	uint32_t mSlicePitch;
#endif
};

#define MIP_REDUCE(s, mip) (std::max(1u, (uint32_t)((s) >> (mip))))

enum
{
	MAPPED_RANGE_FLAG_UNMAP_BUFFER = (1 << 0),
	MAPPED_RANGE_FLAG_TEMP_BUFFER = (1 << 1),
};

extern RendererApi gSelectedRendererApi;

/************************************************************************/
/************************************************************************/

// Xbox, Orbis, Prospero, iOS have unified memory
// so we dont need a command buffer to upload linear data
// A simple memcpy suffices since the GPU memory is marked as CPU write combine
#if defined(XBOX) || defined(ORBIS) || defined(PROSPERO) || defined(TARGET_APPLE_ARM64)
#define UMA 1
#else
#define UMA 0
#endif

#define MAX_FRAMES 3U

ResourceLoaderDesc gDefaultResourceLoaderDesc = { 8ull << 20, 2 };
/************************************************************************/
// Surface Utils
/************************************************************************/
#if defined(VULKAN)
static inline ResourceState util_determine_resource_start_state(bool uav)
{
	if (uav)
		return RESOURCE_STATE_UNORDERED_ACCESS;
	else
		return RESOURCE_STATE_SHADER_RESOURCE;
}
#endif

/************************************************************************/
// Surface Utils
/************************************************************************/
static inline bool util_get_surface_info(
	uint32_t width,
	uint32_t height,
	TinyImageFormat fmt,
	uint32_t* outNumBytes,
	uint32_t* outRowBytes,
	uint32_t* outNumRows)
{
	uint64_t numBytes = 0;
	uint64_t rowBytes = 0;
	uint64_t numRows = 0;

	uint32_t bpp = TinyImageFormat_BitSizeOfBlock(fmt);
	bool compressed = TinyImageFormat_IsCompressed(fmt);
	bool planar = TinyImageFormat_IsPlanar(fmt);
	// #TODO
	bool packed = false;

	if (compressed)
	{
		uint32_t blockWidth = TinyImageFormat_WidthOfBlock(fmt);
		uint32_t blockHeight = TinyImageFormat_HeightOfBlock(fmt);
		uint32_t numBlocksWide = 0;
		uint32_t numBlocksHigh = 0;
		if (width > 0)
		{
			numBlocksWide = std::max(1U, (width + (blockWidth - 1)) / blockWidth);
		}
		if (height > 0)
		{
			numBlocksHigh = std::max(1u, (height + (blockHeight - 1)) / blockHeight);
		}

		rowBytes = numBlocksWide * (bpp >> 3);
		numRows = numBlocksHigh;
		numBytes = rowBytes * numBlocksHigh;
	}
	else if (packed) //-V547
	{
		LOGF(eERROR, "Not implemented");
		return false;
		//rowBytes = ((uint64_t(width) + 1u) >> 1) * bpe;
		//numRows = uint64_t(height);
		//numBytes = rowBytes * height;
	}
	else if (planar)
	{
		uint32_t numOfPlanes = TinyImageFormat_NumOfPlanes(fmt);

		for (uint32_t i = 0; i < numOfPlanes; ++i)
		{
			numBytes += TinyImageFormat_PlaneWidth(fmt, i, width) * TinyImageFormat_PlaneHeight(fmt, i, height) * TinyImageFormat_PlaneSizeOfBlock(fmt, i);
		}

		numRows = 1;
		rowBytes = numBytes;
	}
	else
	{
		if (!bpp)
			return false;

		rowBytes = (uint64_t(width) * bpp + 7u) / 8u; // round up to nearest byte
		numRows = uint64_t(height);
		numBytes = rowBytes * height;
	}

	if (numBytes > UINT32_MAX || rowBytes > UINT32_MAX || numRows > UINT32_MAX) //-V560
		return false;

	if (outNumBytes)
	{
		*outNumBytes = (uint32_t)numBytes;
	}
	if (outRowBytes)
	{
		*outRowBytes = (uint32_t)rowBytes;
	}
	if (outNumRows)
	{
		*outNumRows = (uint32_t)numRows;
	}

	return true;
}

static inline uint32_t util_get_surface_size(
	TinyImageFormat format,
	uint32_t width, uint32_t height, uint32_t depth,
	uint32_t rowStride, uint32_t sliceStride,
	uint32_t baseMipLevel, uint32_t mipLevels,
	uint32_t baseArrayLayer, uint32_t arrayLayers)
{
	uint32_t requiredSize = 0;
	for (uint32_t s = baseArrayLayer; s < baseArrayLayer + arrayLayers; ++s)
	{
		uint32_t w = width;
		uint32_t h = height;
		uint32_t d = depth;

		for (uint32_t m = baseMipLevel; m < baseMipLevel + mipLevels; ++m)
		{
			uint32_t rowBytes = 0;
			uint32_t numRows = 0;

			if (!util_get_surface_info(w, h, format, NULL, &rowBytes, &numRows))
			{
				return 0;
			}

			requiredSize += KRG::Math::RoundUpToNearestMultiple32(d * KRG::Math::RoundUpToNearestMultiple32(rowBytes, rowStride) * numRows, sliceStride);

			w = w >> 1;
			h = h >> 1;
			d = d >> 1;
			if (w == 0)
			{
				w = 1;
			}
			if (h == 0)
			{
				h = 1;
			}
			if (d == 0)
			{
				d = 1;
			}
		}
	}

	return requiredSize;
}

static inline ResourceState util_determine_resource_start_state(const BufferDesc* pDesc)
{
	// Host visible (Upload Heap)
	if (pDesc->mMemoryUsage == RESOURCE_MEMORY_USAGE_CPU_ONLY || pDesc->mMemoryUsage == RESOURCE_MEMORY_USAGE_CPU_TO_GPU)
	{
		return RESOURCE_STATE_GENERIC_READ;
	}
	// Device Local (Default Heap)
	else if (pDesc->mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY)
	{
		DescriptorType usage = (DescriptorType)pDesc->mDescriptors;

		// Try to limit number of states used overall to avoid sync complexities
		if (usage & DESCRIPTOR_TYPE_RW_BUFFER)
			return RESOURCE_STATE_UNORDERED_ACCESS;
		if ((usage & DESCRIPTOR_TYPE_VERTEX_BUFFER) || (usage & DESCRIPTOR_TYPE_UNIFORM_BUFFER))
			return RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		if (usage & DESCRIPTOR_TYPE_INDEX_BUFFER)
			return RESOURCE_STATE_INDEX_BUFFER;
		if ((usage & DESCRIPTOR_TYPE_BUFFER))
			return RESOURCE_STATE_SHADER_RESOURCE;

		return RESOURCE_STATE_COMMON;
	}
	// Host Cached (Readback Heap)
	else
	{
		return RESOURCE_STATE_COPY_DEST;
	}
}

#define F16_EXPONENT_BITS 0x1F
#define F16_EXPONENT_SHIFT 10
#define F16_EXPONENT_BIAS 15
#define F16_MANTISSA_BITS 0x3ff
#define F16_MANTISSA_SHIFT (23 - F16_EXPONENT_SHIFT)
#define F16_MAX_EXPONENT (F16_EXPONENT_BITS << F16_EXPONENT_SHIFT)

static inline uint16_t util_float_to_half(float val)
{
	uint32_t f32 = (*(uint32_t*)&val);
	uint16_t f16 = 0;
	/* Decode IEEE 754 little-endian 32-bit floating-point value */
	int sign = (f32 >> 16) & 0x8000;
	/* Map exponent to the range [-127,128] */
	int exponent = ((f32 >> 23) & 0xff) - 127;
	int mantissa = f32 & 0x007fffff;
	if (exponent == 128)
	{ /* Infinity or NaN */
		f16 = (uint16_t)(sign | F16_MAX_EXPONENT);
		if (mantissa)
			f16 |= (mantissa & F16_MANTISSA_BITS);
	}
	else if (exponent > 15)
	{ /* Overflow - flush to Infinity */
		f16 = (unsigned short)(sign | F16_MAX_EXPONENT);
	}
	else if (exponent > -15)
	{ /* Representable value */
		exponent += F16_EXPONENT_BIAS;
		mantissa >>= F16_MANTISSA_SHIFT;
		f16 = (unsigned short)(sign | exponent << F16_EXPONENT_SHIFT | mantissa);
	}
	else
	{
		f16 = (unsigned short)sign;
	}
	return f16;
}

static inline void util_pack_float2_to_half2(uint32_t count, uint32_t stride, uint32_t offset, const uint8_t* src, uint8_t* dst)
{
	struct f2
	{
		float x;
		float y;
	};
	f2* f = (f2*)src;
	for (uint32_t e = 0; e < count; ++e)
	{
		*(uint32_t*)(dst + e * sizeof(uint32_t) + offset) =
			((util_float_to_half(f[e].x) & 0x0000FFFF) | ((util_float_to_half(f[e].y) << 16) & 0xFFFF0000));
	}
}

static inline uint32_t util_float2_to_unorm2x16(const float* v)
{
    uint32_t x = (uint32_t) round( KRG::Math::Clamp( v[0], 0.0f, 1.0f ) * 65535.0f );
    uint32_t y = (uint32_t) round( KRG::Math::Clamp( v[1], 0.0f, 1.0f ) * 65535.0f );
	return ((uint32_t)0x0000FFFF & x) | ((y << 16) & (uint32_t)0xFFFF0000);
}

#define OCT_WRAP(v, w) ((1.0f - abs((w))) * ((v) >= 0.0f ? 1.0f : -1.0f))

static inline void util_pack_float3_direction_to_half2(uint32_t count, uint32_t stride, uint32_t offset, const uint8_t* src, uint8_t* dst)
{
	struct f3
	{
		float x;
		float y;
		float z;
	};
	for (uint32_t e = 0; e < count; ++e)
	{
		f3    f = *(f3*)(src + e * stride);
		float absLength = (abs(f.x) + abs(f.y) + abs(f.z));
		f3    enc = {};
		if (absLength)
		{
			enc.x = f.x / absLength;
			enc.y = f.y / absLength;
			enc.z = f.z / absLength;
			if (enc.z < 0)
			{
				float oldX = enc.x;
				enc.x = OCT_WRAP(enc.x, enc.y);
				enc.y = OCT_WRAP(enc.y, oldX);
			}
			enc.x = enc.x * 0.5f + 0.5f;
			enc.y = enc.y * 0.5f + 0.5f;
			*(uint32_t*)(dst + e * sizeof(uint32_t) + offset) = util_float2_to_unorm2x16(&enc.x);
		}
		else
		{
			*(uint32_t*)(dst + e * sizeof(uint32_t) + offset) = 0;
		}
	}
}
typedef struct TextureUpdateDescInternal
{
	Texture*          pTexture;
	MappedMemoryRange mRange;
	uint32_t          mBaseMipLevel;
	uint32_t          mMipLevels;
	uint32_t          mBaseArrayLayer;
	uint32_t          mLayerCount;
	bool              mMipsAfterSlice;
} TextureUpdateDescInternal;

typedef struct CopyResourceSet
{
	Fence*   pFence;
	Cmd*     pCmd;
	CmdPool* pCmdPool;
	Buffer*  mBuffer;
	uint64_t mAllocatedSpace;

	/// Buffers created in case we ran out of space in the original staging buffer
	/// Will be cleaned up after the fence for this set is complete
	eastl::vector<Buffer*> mTempBuffers;

	Semaphore* pCopyCompletedSemaphore;
} CopyResourceSet;

//Synchronization?
typedef struct CopyEngine
{
	Queue*           pQueue;
	CopyResourceSet* resourceSets;
	uint64_t         bufferSize;
	uint32_t         bufferCount;
	/// Node index in linked GPU mode, Renderer index in unlinked mode
	uint32_t         nodeIndex;
	bool             isRecording;
	Semaphore*       pLastCompletedSemaphore;

	/// For reading back GPU generated textures, we need to ensure writes have completed before performing the copy.
	eastl::vector<Semaphore*> mWaitSemaphores;

} CopyEngine;

typedef enum UpdateRequestType
{
	UPDATE_REQUEST_UPDATE_BUFFER,
	UPDATE_REQUEST_UPDATE_TEXTURE,
	UPDATE_REQUEST_BUFFER_BARRIER,
	UPDATE_REQUEST_TEXTURE_BARRIER,
	UPDATE_REQUEST_COPY_TEXTURE,
	UPDATE_REQUEST_INVALID,
} UpdateRequestType;

typedef enum UploadFunctionResult
{
	UPLOAD_FUNCTION_RESULT_COMPLETED,
	UPLOAD_FUNCTION_RESULT_STAGING_BUFFER_FULL,
	UPLOAD_FUNCTION_RESULT_INVALID_REQUEST
} UploadFunctionResult;

struct UpdateRequest
{
	UpdateRequest(const BufferUpdateDesc& buffer): mType(UPDATE_REQUEST_UPDATE_BUFFER), bufUpdateDesc(buffer) {}
	UpdateRequest(const TextureUpdateDescInternal& texture): mType(UPDATE_REQUEST_UPDATE_TEXTURE), texUpdateDesc(texture) {}
	UpdateRequest(const BufferBarrier& barrier): mType(UPDATE_REQUEST_BUFFER_BARRIER), bufferBarrier(barrier) {}
	UpdateRequest(const TextureBarrier& barrier): mType(UPDATE_REQUEST_TEXTURE_BARRIER), textureBarrier(barrier) {}
	UpdateRequest(const TextureCopyDesc& texture): mType(UPDATE_REQUEST_COPY_TEXTURE), texCopyDesc(texture) {}

	UpdateRequestType mType = UPDATE_REQUEST_INVALID;
	uint64_t          mWaitIndex = 0;
	Buffer*           pUploadBuffer = NULL;
	union
	{
		BufferUpdateDesc          bufUpdateDesc;
		TextureUpdateDescInternal texUpdateDesc;
		BufferBarrier             bufferBarrier;
		TextureBarrier            textureBarrier;
		TextureCopyDesc           texCopyDesc;
	};
};

struct ResourceLoader
{
	Renderer* ppRenderers[MAX_MULTIPLE_GPUS];
	uint32_t mGpuCount;

	ResourceLoaderDesc mDesc;

	volatile int mRun;
	ThreadDesc   mThreadDesc;
	ThreadHandle mThread;

	Mutex                        mQueueMutex;
	ConditionVariable            mQueueCond;
	Mutex                        mTokenMutex;
	ConditionVariable            mTokenCond;
	eastl::vector<UpdateRequest> mRequestQueue[MAX_MULTIPLE_GPUS];

	tfrg_atomic64_t mTokenCompleted;
	tfrg_atomic64_t mTokenSubmitted;
	tfrg_atomic64_t mTokenCounter;

	Mutex mSemaphoreMutex;

	SyncToken mCurrentTokenState[MAX_FRAMES];

	CopyEngine pCopyEngines[MAX_MULTIPLE_GPUS];
	uint32_t   mNextSet;
	uint32_t   mSubmittedSets;

#if defined(NX64)
	ThreadTypeNX mThreadType;
	void*        mThreadStackPtr;
#endif
};

static ResourceLoader* pResourceLoader = NULL;

static uint32_t util_get_texture_row_alignment(Renderer* pRenderer)
{
	return std::max(1u, pRenderer->pActiveGpuSettings->mUploadBufferTextureRowAlignment);
}

static uint32_t util_get_texture_subresource_alignment(Renderer* pRenderer, TinyImageFormat fmt = TinyImageFormat_UNDEFINED)
{
	uint32_t blockSize = std::max(1u, TinyImageFormat_BitSizeOfBlock(fmt) >> 3);
	uint32_t alignment = KRG::Math::RoundUpToNearestMultiple32(pRenderer->pActiveGpuSettings->mUploadBufferTextureAlignment, blockSize);
	return KRG::Math::RoundUpToNearestMultiple32(alignment, util_get_texture_row_alignment(pRenderer));
}
/************************************************************************/
// Internal Functions
/************************************************************************/
/// Return a new staging buffer
static MappedMemoryRange allocateUploadMemory(Renderer* pRenderer, uint64_t memoryRequirement, uint32_t alignment)
{
	Buffer* buffer;
#if defined(DIRECT3D11)
	if (gSelectedRendererApi == RENDERER_API_D3D11)
	{
		// There is no such thing as staging buffer in D3D11
		// To keep code paths unified in update functions, we allocate space for a dummy buffer and the system memory for pCpuMappedAddress
		buffer = (Buffer*)tf_memalign(alignof(Buffer), sizeof(Buffer) + (size_t)memoryRequirement);
		*buffer = {};
		buffer->pCpuMappedAddress = buffer + 1;
		buffer->mSize = memoryRequirement;
	}
	else
#endif
#if defined(GLES)
		if (gSelectedRendererApi == RENDERER_API_GLES)
	{
		// There is no such thing as staging buffer in D3D11
		// To keep code paths unified in update functions, we allocate space for a dummy buffer and the system memory for pCpuMappedAddress
		buffer = (Buffer*)tf_memalign(alignof(Buffer), sizeof(Buffer) + (size_t)memoryRequirement);
		*buffer = {};
		buffer->pCpuMappedAddress = buffer + 1;
		buffer->mSize = memoryRequirement;
	}
	else
#endif
	{
		//LOGF(LogLevel::eINFO, "Allocating temporary staging buffer. Required allocation size of %llu is larger than the staging buffer capacity of %llu", memoryRequirement, size);
		buffer = {};
		BufferDesc bufferDesc = {};
		bufferDesc.mSize = memoryRequirement;
		bufferDesc.mAlignment = alignment;
		bufferDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_ONLY;
		bufferDesc.mFlags = BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
		bufferDesc.mNodeIndex = pRenderer->mUnlinkedRendererIndex;
		addBuffer(pRenderer, &bufferDesc, &buffer);
	}
	return { (uint8_t*)buffer->pCpuMappedAddress, buffer, 0, memoryRequirement };
}

static void setupCopyEngine(Renderer* pRenderer, CopyEngine* pCopyEngine, uint32_t nodeIndex, uint64_t size, uint32_t bufferCount)
{
	QueueDesc desc = { QUEUE_TYPE_TRANSFER, QUEUE_FLAG_NONE, QUEUE_PRIORITY_NORMAL, nodeIndex };
	addQueue(pRenderer, &desc, &pCopyEngine->pQueue);

	const uint64_t maxBlockSize = 32;
	size = std::max(size, maxBlockSize);

	pCopyEngine->resourceSets = (CopyResourceSet*)tf_malloc(sizeof(CopyResourceSet) * bufferCount);
	for (uint32_t i = 0; i < bufferCount; ++i)
	{
		tf_placement_new<CopyResourceSet>(pCopyEngine->resourceSets + i);

		CopyResourceSet& resourceSet = pCopyEngine->resourceSets[i];
#if defined(DIRECT3D11)
		if (gSelectedRendererApi != RENDERER_API_D3D11)
#endif
			addFence(pRenderer, &resourceSet.pFence);

		CmdPoolDesc cmdPoolDesc = {};
		cmdPoolDesc.pQueue = pCopyEngine->pQueue;
		addCmdPool(pRenderer, &cmdPoolDesc, &resourceSet.pCmdPool);

		CmdDesc cmdDesc = {};
		cmdDesc.pPool = resourceSet.pCmdPool;
		addCmd(pRenderer, &cmdDesc, &resourceSet.pCmd);

		addSemaphore(pRenderer, &resourceSet.pCopyCompletedSemaphore);

		resourceSet.mBuffer = allocateUploadMemory(pRenderer, size, util_get_texture_subresource_alignment(pRenderer)).pBuffer;
	}

	pCopyEngine->bufferSize = size;
	pCopyEngine->bufferCount = bufferCount;
	pCopyEngine->nodeIndex = nodeIndex;
	pCopyEngine->isRecording = false;
	pCopyEngine->pLastCompletedSemaphore = NULL;
}

static void cleanupCopyEngine(Renderer* pRenderer, CopyEngine* pCopyEngine)
{
	for (uint32_t i = 0; i < pCopyEngine->bufferCount; ++i)
	{
		CopyResourceSet& resourceSet = pCopyEngine->resourceSets[i];
		removeBuffer(pRenderer, resourceSet.mBuffer);

		removeSemaphore(pRenderer, resourceSet.pCopyCompletedSemaphore);

		removeCmd(pRenderer, resourceSet.pCmd);
		removeCmdPool(pRenderer, resourceSet.pCmdPool);
#if defined(DIRECT3D11)
		if (gSelectedRendererApi != RENDERER_API_D3D11)
#endif
			removeFence(pRenderer, resourceSet.pFence);

		if (!resourceSet.mTempBuffers.empty())
			LOGF(eINFO, "Was not cleaned up %d", i);
		for (Buffer*& buffer : resourceSet.mTempBuffers)
		{
			removeBuffer(pRenderer, buffer);
		}
		pCopyEngine->resourceSets[i].mTempBuffers.set_capacity(0);
	}

	tf_free(pCopyEngine->resourceSets);

	removeQueue(pRenderer, pCopyEngine->pQueue);
}

static bool waitCopyEngineSet(Renderer* pRenderer, CopyEngine* pCopyEngine, size_t activeSet, bool wait)
{
	ASSERT(!pCopyEngine->isRecording);
	CopyResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
	bool             completed = true;
#if defined(DIRECT3D11)
	if (gSelectedRendererApi != RENDERER_API_D3D11)
	{
#endif
		FenceStatus status;
		getFenceStatus(pRenderer, resourceSet.pFence, &status);
		completed = status != FENCE_STATUS_INCOMPLETE;
		if (wait && !completed)
		{
			waitForFences(pRenderer, 1, &resourceSet.pFence);
		}
#if defined(DIRECT3D11)
	}
	else
	{
		UNREF_PARAM(pRenderer);
		UNREF_PARAM(pCopyEngine);
		UNREF_PARAM(activeSet);
	}
#endif
	return completed;
}

static void resetCopyEngineSet(Renderer* pRenderer, CopyEngine* pCopyEngine, size_t activeSet)
{
	ASSERT(!pCopyEngine->isRecording);
	pCopyEngine->resourceSets[activeSet].mAllocatedSpace = 0;
	pCopyEngine->isRecording = false;

	for (Buffer*& buffer : pCopyEngine->resourceSets[activeSet].mTempBuffers)
	{
		removeBuffer(pRenderer, buffer);
	}
	pCopyEngine->resourceSets[activeSet].mTempBuffers.clear();
}

static Cmd* acquireCmd(CopyEngine* pCopyEngine, size_t activeSet)
{
	CopyResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
	if (!pCopyEngine->isRecording)
	{
		resetCmdPool(pResourceLoader->ppRenderers[pCopyEngine->nodeIndex], resourceSet.pCmdPool);
		beginCmd(resourceSet.pCmd);
		pCopyEngine->isRecording = true;
	}
	return resourceSet.pCmd;
}

static void streamerFlush(CopyEngine* pCopyEngine, size_t activeSet)
{
	if (pCopyEngine->isRecording)
	{
		CopyResourceSet& resourceSet = pCopyEngine->resourceSets[activeSet];
		endCmd(resourceSet.pCmd);
		QueueSubmitDesc submitDesc = {};
		submitDesc.mCmdCount = 1;
		submitDesc.ppCmds = &resourceSet.pCmd;
		submitDesc.mSignalSemaphoreCount = 1;
		submitDesc.ppSignalSemaphores = &resourceSet.pCopyCompletedSemaphore;
		submitDesc.pSignalFence = resourceSet.pFence;
		if (!pCopyEngine->mWaitSemaphores.empty())
		{
			submitDesc.mWaitSemaphoreCount = (uint32_t)pCopyEngine->mWaitSemaphores.size();
			submitDesc.ppWaitSemaphores = &pCopyEngine->mWaitSemaphores[0];
			pCopyEngine->mWaitSemaphores.clear();
		}
		{
			queueSubmit(pCopyEngine->pQueue, &submitDesc);
		}
		pCopyEngine->isRecording = false;
	}
}

/// Return memory from pre-allocated staging buffer or create a temporary buffer if the streamer ran out of memory
static MappedMemoryRange allocateStagingMemory(uint64_t memoryRequirement, uint32_t alignment, uint32_t nodeIndex)
{
	CopyEngine* pCopyEngine = &pResourceLoader->pCopyEngines[nodeIndex];

	uint64_t offset = pCopyEngine->resourceSets[pResourceLoader->mNextSet].mAllocatedSpace;
	if (alignment != 0)
	{
		offset = KRG::Math::RoundUpToNearestMultiple64(offset, alignment);
	}

	CopyResourceSet* pResourceSet = &pCopyEngine->resourceSets[pResourceLoader->mNextSet];
	uint64_t         size = (uint64_t)pResourceSet->mBuffer->mSize;
	bool             memoryAvailable = (offset < size) && (memoryRequirement <= size - offset);
	if (memoryAvailable && pResourceSet->mBuffer->pCpuMappedAddress)
	{
		Buffer* buffer = pResourceSet->mBuffer;
		ASSERT(buffer->pCpuMappedAddress);
		uint8_t* pDstData = (uint8_t*)buffer->pCpuMappedAddress + offset;
		pCopyEngine->resourceSets[pResourceLoader->mNextSet].mAllocatedSpace = offset + memoryRequirement;
		return { pDstData, buffer, offset, memoryRequirement };
	}
	else
	{
		if (pCopyEngine->bufferSize < memoryRequirement)
		{
			MappedMemoryRange range = allocateUploadMemory(pResourceLoader->ppRenderers[nodeIndex], memoryRequirement, alignment);
			//LOGF(LogLevel::eINFO, "Allocating temporary staging buffer. Required allocation size of %llu is larger than the staging buffer capacity of %llu", memoryRequirement, size);
			pResourceSet->mTempBuffers.emplace_back(range.pBuffer);
			return range;
		}
	}

	MappedMemoryRange range = allocateUploadMemory(pResourceLoader->ppRenderers[nodeIndex], memoryRequirement, alignment);
	//LOGF(LogLevel::eINFO, "Allocating temporary staging buffer. Required allocation size of %llu is larger than the staging buffer capacity of %llu", memoryRequirement, size);
	pResourceSet->mTempBuffers.emplace_back(range.pBuffer);
	return range;
}

static void freeAllUploadMemory()
{
	for (size_t i = 0; i < MAX_MULTIPLE_GPUS; ++i)
	{
		for (UpdateRequest& request : pResourceLoader->mRequestQueue[i])
		{
			if (request.pUploadBuffer)
			{
				removeBuffer(pResourceLoader->ppRenderers[i], request.pUploadBuffer);
			}
		}
	}
}

static UploadFunctionResult
	updateTexture(Renderer* pRenderer, CopyEngine* pCopyEngine, size_t activeSet, const TextureUpdateDescInternal& texUpdateDesc)
{
	// When this call comes from updateResource, staging buffer data is already filled
	// All that is left to do is record and execute the Copy commands
	bool                  dataAlreadyFilled = texUpdateDesc.mRange.pBuffer ? true : false;
	Texture*              texture = texUpdateDesc.pTexture;
	const TinyImageFormat fmt = (TinyImageFormat)texture->mFormat;
	Cmd*                  cmd = acquireCmd(pCopyEngine, activeSet);

	ASSERT(pCopyEngine->pQueue->mNodeIndex == texUpdateDesc.pTexture->mNodeIndex);

	const uint32_t sliceAlignment = util_get_texture_subresource_alignment(pRenderer, fmt);
	const uint32_t rowAlignment = util_get_texture_row_alignment(pRenderer);
	const uint64_t requiredSize = util_get_surface_size(
		fmt, texture->mWidth, texture->mHeight, texture->mDepth, rowAlignment, sliceAlignment, texUpdateDesc.mBaseMipLevel,
		texUpdateDesc.mMipLevels, texUpdateDesc.mBaseArrayLayer, texUpdateDesc.mLayerCount);

#if defined(VULKAN)
	TextureBarrier barrier;
	if (gSelectedRendererApi == RENDERER_API_VULKAN)
	{
		barrier = { texture, RESOURCE_STATE_UNDEFINED, RESOURCE_STATE_COPY_DEST };
		cmdResourceBarrier(cmd, 0, NULL, 1, &barrier, 0, NULL);
	}
#endif

	MappedMemoryRange upload = dataAlreadyFilled ? texUpdateDesc.mRange : allocateStagingMemory(requiredSize, sliceAlignment, texture->mNodeIndex);
	uint64_t          offset = 0;

	// #TODO: Investigate - fsRead crashes if we pass the upload buffer mapped address. Allocating temporary buffer as a workaround. Does NX support loading from disk to GPU shared memory?
#ifdef NX64
	void* nxTempBuffer = NULL;
	if (!dataAlreadyFilled)
	{
		size_t remainingBytes = fsGetStreamFileSize(&stream) - fsGetStreamSeekPosition(&stream);
		nxTempBuffer = tf_malloc(remainingBytes);
		ssize_t bytesRead = fsReadFromStream(&stream, nxTempBuffer, remainingBytes);
		if (bytesRead != remainingBytes)
		{
			fsCloseStream(&stream);
			tf_free(nxTempBuffer);
			return UPLOAD_FUNCTION_RESULT_INVALID_REQUEST;
		}

		fsCloseStream(&stream);
		fsOpenStreamFromMemory(nxTempBuffer, remainingBytes, FM_READ_BINARY, true, &stream);
	}
#endif

	if (!upload.pData)
	{
		return UPLOAD_FUNCTION_RESULT_STAGING_BUFFER_FULL;
	}

	uint32_t firstStart = texUpdateDesc.mMipsAfterSlice ? texUpdateDesc.mBaseMipLevel : texUpdateDesc.mBaseArrayLayer;
	uint32_t firstEnd = texUpdateDesc.mMipsAfterSlice ? (texUpdateDesc.mBaseMipLevel + texUpdateDesc.mMipLevels)
													  : (texUpdateDesc.mBaseArrayLayer + texUpdateDesc.mLayerCount);
	uint32_t secondStart = texUpdateDesc.mMipsAfterSlice ? texUpdateDesc.mBaseArrayLayer : texUpdateDesc.mBaseMipLevel;
	uint32_t secondEnd = texUpdateDesc.mMipsAfterSlice ? (texUpdateDesc.mBaseArrayLayer + texUpdateDesc.mLayerCount)
													   : (texUpdateDesc.mBaseMipLevel + texUpdateDesc.mMipLevels);

	for (uint32_t p = 0; p < 1; ++p)
	{
		for (uint32_t j = firstStart; j < firstEnd; ++j)
		{
			for (uint32_t i = secondStart; i < secondEnd; ++i)
			{
				uint32_t mip = texUpdateDesc.mMipsAfterSlice ? j : i;
				uint32_t layer = texUpdateDesc.mMipsAfterSlice ? i : j;

				uint32_t w = MIP_REDUCE(texture->mWidth, mip);
				uint32_t h = MIP_REDUCE(texture->mHeight, mip);
				uint32_t d = MIP_REDUCE(texture->mDepth, mip);

				uint32_t numBytes = 0;
				uint32_t rowBytes = 0;
				uint32_t numRows = 0;

				bool ret = util_get_surface_info(w, h, fmt, &numBytes, &rowBytes, &numRows);
				if (!ret)
				{
					return UPLOAD_FUNCTION_RESULT_INVALID_REQUEST;
				}

				uint32_t subRowPitch = KRG::Math::RoundUpToNearestMultiple32(rowBytes, rowAlignment);
				uint32_t subSlicePitch = KRG::Math::RoundUpToNearestMultiple32(subRowPitch * numRows, sliceAlignment);
				uint32_t subNumRows = numRows;
				uint32_t subDepth = d;
				uint32_t subRowSize = rowBytes;
				uint8_t* data = upload.pData + offset;

				//if (!dataAlreadyFilled)
				//{
				//	for (uint32_t z = 0; z < subDepth; ++z)
				//	{
				//		uint8_t* dstData = data + subSlicePitch * z;
				//		for (uint32_t r = 0; r < subNumRows; ++r)
				//		{
				//			ssize_t bytesRead = fsReadFromStream(&stream, dstData + r * subRowPitch, subRowSize);
				//			if (bytesRead != subRowSize)
				//			{
				//				return UPLOAD_FUNCTION_RESULT_INVALID_REQUEST;
				//			}
				//		}
				//	}
				//}
				SubresourceDataDesc subresourceDesc = {};
				subresourceDesc.mArrayLayer = layer;
				subresourceDesc.mMipLevel = mip;
				subresourceDesc.mSrcOffset = upload.mOffset + offset;
#if defined(DIRECT3D11) || defined(METAL) || defined(VULKAN)
				subresourceDesc.mRowPitch = subRowPitch;
				subresourceDesc.mSlicePitch = subSlicePitch;
#endif
				cmdUpdateSubresource(cmd, texture, upload.pBuffer, &subresourceDesc);
				offset += subDepth * subSlicePitch;
			}
		}
	}

#if defined(VULKAN)
	if (gSelectedRendererApi == RENDERER_API_VULKAN)
	{
		barrier = { texture, RESOURCE_STATE_COPY_DEST, RESOURCE_STATE_SHADER_RESOURCE };
		cmdResourceBarrier(cmd, 0, NULL, 1, &barrier, 0, NULL);
	}
#endif

	return UPLOAD_FUNCTION_RESULT_COMPLETED;
}

static UploadFunctionResult
	updateBuffer(Renderer* pRenderer, CopyEngine* pCopyEngine, size_t activeSet, const BufferUpdateDesc& bufUpdateDesc)
{
	ASSERT(pCopyEngine->pQueue->mNodeIndex == bufUpdateDesc.pBuffer->mNodeIndex);
	Buffer* pBuffer = bufUpdateDesc.pBuffer;
	ASSERT(pBuffer->mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY || pBuffer->mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_TO_CPU);

	Cmd* pCmd = acquireCmd(pCopyEngine, activeSet);

	MappedMemoryRange range = bufUpdateDesc.mInternal.mMappedRange;
	cmdUpdateBuffer(pCmd, pBuffer, bufUpdateDesc.mDstOffset, range.pBuffer, range.mOffset, range.mSize);

	return UPLOAD_FUNCTION_RESULT_COMPLETED;
}

static UploadFunctionResult copyTexture(Renderer* pRenderer, CopyEngine* pCopyEngine, size_t activeSet, TextureCopyDesc& pTextureCopy)
{
	Texture* texture = pTextureCopy.pTexture;
	const TinyImageFormat fmt = (TinyImageFormat)texture->mFormat;

	Cmd* cmd = acquireCmd(pCopyEngine, activeSet);

	if (pTextureCopy.pWaitSemaphore)
		pCopyEngine->mWaitSemaphores.push_back(pTextureCopy.pWaitSemaphore);

#if defined(VULKAN)
	if (gSelectedRendererApi == RENDERER_API_VULKAN)
	{
		TextureBarrier barrier = { texture, pTextureCopy.mTextureState, RESOURCE_STATE_COPY_SOURCE };
		barrier.mAcquire = 1;
		barrier.mQueueType = pTextureCopy.mQueueType;
		cmdResourceBarrier(cmd, 0, NULL, 1, &barrier, 0, NULL);
	}
#endif
#if defined(DIRECT3D12)
	if (gSelectedRendererApi == RENDERER_API_D3D12)
	{
		TextureBarrier barrier = { texture, pTextureCopy.mTextureState, RESOURCE_STATE_COPY_SOURCE };
		barrier.mAcquire = 1;
		barrier.mQueueType = pTextureCopy.mQueueType;
		cmdResourceBarrier(cmd, 0, NULL, 1, &barrier, 0, NULL);
	}
#endif

	uint32_t numBytes = 0;
	uint32_t rowBytes = 0;
	uint32_t numRows = 0;

	bool ret = util_get_surface_info(texture->mWidth, texture->mHeight, fmt, &numBytes, &rowBytes, &numRows);
	if (!ret)
	{
		return UPLOAD_FUNCTION_RESULT_INVALID_REQUEST;
	}

	SubresourceDataDesc subresourceDesc = {};
	subresourceDesc.mArrayLayer = pTextureCopy.mTextureArrayLayer;
	subresourceDesc.mMipLevel = pTextureCopy.mTextureMipLevel;
	subresourceDesc.mSrcOffset = pTextureCopy.mBufferOffset;
#if defined(DIRECT3D11) || defined(METAL) || defined(VULKAN)
	const uint32_t sliceAlignment = util_get_texture_subresource_alignment(pRenderer, fmt);
	const uint32_t rowAlignment = util_get_texture_row_alignment(pRenderer);
	uint32_t subRowPitch = KRG::Math::RoundUpToNearestMultiple32(rowBytes, rowAlignment);
	uint32_t subSlicePitch = KRG::Math::RoundUpToNearestMultiple32(subRowPitch * numRows, sliceAlignment);
	subresourceDesc.mRowPitch = subRowPitch;
	subresourceDesc.mSlicePitch = subSlicePitch;
#endif
	cmdCopySubresource(cmd, pTextureCopy.pBuffer, pTextureCopy.pTexture, &subresourceDesc);
#if defined(DIRECT3D12)
	if (gSelectedRendererApi == RENDERER_API_D3D12)
	{
		TextureBarrier barrier = { texture, RESOURCE_STATE_COPY_SOURCE, pTextureCopy.mTextureState };
		barrier.mRelease = 1;
		barrier.mQueueType = pTextureCopy.mQueueType;
		cmdResourceBarrier(cmd, 0, NULL, 1, &barrier, 0, NULL);
	}
#endif

	return UPLOAD_FUNCTION_RESULT_COMPLETED;

}
/************************************************************************/
// Internal Resource Loader Implementation
/************************************************************************/
static bool areTasksAvailable(ResourceLoader* pLoader)
{
	for (size_t i = 0; i < MAX_MULTIPLE_GPUS; ++i)
	{
		if (!pLoader->mRequestQueue[i].empty())
		{
			return true;
		}
	}

	return false;
}

static void streamerThreadFunc(void* pThreadData)
{
	ResourceLoader* pLoader = (ResourceLoader*)pThreadData;
	ASSERT(pLoader);

#if defined(GLES)
	GLContext localContext;
	if (gSelectedRendererApi == RENDERER_API_GLES)
	{
		if (!pLoader->mDesc.mSingleThreaded)
			initGLContext(pLoader->ppRenderers[0]->mGLES.pConfig, &localContext, pLoader->ppRenderers[0]->mGLES.pContext);
	}
#endif

	SyncToken maxToken = {};

	while (pLoader->mRun)
	{
		acquireMutex(&pLoader->mQueueMutex);

		// Check for pending tokens
		// Safe to use mTokenCounter as we are inside critical section
		bool allTokensSignaled = (pLoader->mTokenCompleted == tfrg_atomic64_load_relaxed(&pLoader->mTokenCounter));

		while (!areTasksAvailable(pLoader) && allTokensSignaled && pLoader->mRun)
		{
			// No waiting if not running dedicated resource loader thread.
			if (pLoader->mDesc.mSingleThreaded)
			{
				return;
			}
			// Sleep until someone adds an update request to the queue
			waitConditionVariable(&pLoader->mQueueCond, &pLoader->mQueueMutex, TIMEOUT_INFINITE);
		}

		releaseMutex(&pLoader->mQueueMutex);

		pLoader->mNextSet = (pLoader->mNextSet + 1) % pLoader->mDesc.mBufferCount;
		for (uint32_t nodeIndex = 0; nodeIndex < pLoader->mGpuCount; ++nodeIndex)
		{
			waitCopyEngineSet(pLoader->ppRenderers[nodeIndex], &pLoader->pCopyEngines[nodeIndex], pLoader->mNextSet, true);
			resetCopyEngineSet(pLoader->ppRenderers[nodeIndex], &pLoader->pCopyEngines[nodeIndex], pLoader->mNextSet);
		}

		// Signal pending tokens from previous frames
		acquireMutex(&pLoader->mTokenMutex);
		tfrg_atomic64_store_release(&pLoader->mTokenCompleted, pLoader->mCurrentTokenState[pLoader->mNextSet]);
		releaseMutex(&pLoader->mTokenMutex);
		wakeAllConditionVariable(&pLoader->mTokenCond);

		uint64_t completionMask = 0;

		for (uint32_t nodeIndex = 0; nodeIndex < pLoader->mGpuCount; ++nodeIndex)
		{
			acquireMutex(&pLoader->mQueueMutex);

			eastl::vector<UpdateRequest>& requestQueue = pLoader->mRequestQueue[nodeIndex];
			CopyEngine&                   copyEngine = pLoader->pCopyEngines[nodeIndex];

			if (!requestQueue.size())
			{
				releaseMutex(&pLoader->mQueueMutex);
				continue;
			}

			eastl::vector<UpdateRequest> activeQueue;
			eastl::swap(requestQueue, activeQueue);
			releaseMutex(&pLoader->mQueueMutex);

			Renderer* pRenderer = pLoader->ppRenderers[nodeIndex];
			size_t requestCount = activeQueue.size();
			SyncToken maxNodeToken = {};

			for (size_t j = 0; j < requestCount; ++j)
			{
				UpdateRequest updateState = activeQueue[j];

				UploadFunctionResult result = UPLOAD_FUNCTION_RESULT_COMPLETED;
				switch (updateState.mType)
				{
					case UPDATE_REQUEST_UPDATE_BUFFER:
						result = updateBuffer(pRenderer, &copyEngine, pLoader->mNextSet, updateState.bufUpdateDesc);
						break;
					case UPDATE_REQUEST_UPDATE_TEXTURE:
						result = updateTexture(pRenderer, &copyEngine, pLoader->mNextSet, updateState.texUpdateDesc);
						break;
					case UPDATE_REQUEST_BUFFER_BARRIER:
						cmdResourceBarrier(acquireCmd(&copyEngine, pLoader->mNextSet), 1, &updateState.bufferBarrier, 0, NULL, 0, NULL);
						result = UPLOAD_FUNCTION_RESULT_COMPLETED;
						break;
					case UPDATE_REQUEST_TEXTURE_BARRIER:
						cmdResourceBarrier(acquireCmd(&copyEngine, pLoader->mNextSet), 0, NULL, 1, &updateState.textureBarrier, 0, NULL);
						result = UPLOAD_FUNCTION_RESULT_COMPLETED;
						break;
					case UPDATE_REQUEST_COPY_TEXTURE:
						result = copyTexture(pRenderer, &copyEngine, pLoader->mNextSet, updateState.texCopyDesc);
						break;
					case UPDATE_REQUEST_INVALID: break;
				}

				if (updateState.pUploadBuffer)
				{
					CopyResourceSet& resourceSet = copyEngine.resourceSets[pLoader->mNextSet];
					resourceSet.mTempBuffers.push_back(updateState.pUploadBuffer);
				}

				bool completed = result == UPLOAD_FUNCTION_RESULT_COMPLETED || result == UPLOAD_FUNCTION_RESULT_INVALID_REQUEST;

				completionMask |= (uint64_t)completed << nodeIndex;

				if (updateState.mWaitIndex && completed)
				{
					ASSERT(maxNodeToken < updateState.mWaitIndex);
					maxNodeToken = updateState.mWaitIndex;
				}

				ASSERT(result != UPLOAD_FUNCTION_RESULT_STAGING_BUFFER_FULL);
			}
			maxToken = std::max(maxToken, maxNodeToken);
		}

		if (completionMask != 0)
		{
			for (uint32_t nodeIndex = 0; nodeIndex < pLoader->mGpuCount; ++nodeIndex)
			{
				if (completionMask & ((uint64_t)1 << nodeIndex))
				{
					streamerFlush(&pLoader->pCopyEngines[nodeIndex], pLoader->mNextSet);
					acquireMutex(&pLoader->mSemaphoreMutex);
					pLoader->pCopyEngines[nodeIndex].pLastCompletedSemaphore =
						pLoader->pCopyEngines[nodeIndex].resourceSets[pLoader->mNextSet].pCopyCompletedSemaphore;
					releaseMutex(&pLoader->mSemaphoreMutex);
				}
			}

		}

		SyncToken nextToken = std::max(maxToken, getLastTokenCompleted());
		pLoader->mCurrentTokenState[pLoader->mNextSet] = nextToken;

		// Signal submitted tokens
		acquireMutex(&pLoader->mTokenMutex);
		tfrg_atomic64_store_release(&pLoader->mTokenSubmitted, pLoader->mCurrentTokenState[pLoader->mNextSet]);
		releaseMutex(&pLoader->mTokenMutex);
		wakeAllConditionVariable(&pLoader->mTokenCond);

		if (pResourceLoader->mDesc.mSingleThreaded)
		{
			return;
		}
	}

	for (uint32_t nodeIndex = 0; nodeIndex < pLoader->mGpuCount; ++nodeIndex)
	{
		streamerFlush(&pLoader->pCopyEngines[nodeIndex], pLoader->mNextSet);
#if defined(DIRECT3D11)
		if (gSelectedRendererApi != RENDERER_API_D3D11)
#endif
#if defined(GLES)
			if (gSelectedRendererApi != RENDERER_API_GLES)
#endif
			{
				waitQueueIdle(pLoader->pCopyEngines[nodeIndex].pQueue);
			}
		cleanupCopyEngine(pLoader->ppRenderers[nodeIndex], &pLoader->pCopyEngines[nodeIndex]);
	}

	freeAllUploadMemory();

#if defined(GLES)
	if (gSelectedRendererApi == RENDERER_API_GLES)
	{
		if (!pResourceLoader->mDesc.mSingleThreaded)
			removeGLContext(&localContext);
	}
#endif
}

static void addResourceLoader(Renderer** ppRenderers, uint32_t rendererCount, ResourceLoaderDesc* pDesc, ResourceLoader** ppLoader)
{
	ASSERT(rendererCount > 0);
	ASSERT(rendererCount <= MAX_MULTIPLE_GPUS);
	ResourceLoader* pLoader = tf_new(ResourceLoader);

	uint32_t gpuCount = rendererCount;
	if (ppRenderers[0]->mGpuMode != GPU_MODE_UNLINKED)
	{
		ASSERT(rendererCount == 1);
		gpuCount = ppRenderers[0]->mLinkedNodeCount;
	}

	pLoader->mGpuCount = gpuCount;

	for (uint32_t i = 0; i < gpuCount; ++i)
	{
		ASSERT(rendererCount == 1 || ppRenderers[i]->mGpuMode == GPU_MODE_UNLINKED);
		// Replicate single renderer in linked mode, for uniform handling of linked and unlinked multi gpu.
		pLoader->ppRenderers[i] = (rendererCount > 1) ? ppRenderers[i] : ppRenderers[0];
	}

	pLoader->mRun = true;    //-V601
	pLoader->mDesc = pDesc ? *pDesc : gDefaultResourceLoaderDesc;

	initMutex(&pLoader->mQueueMutex);
	initMutex(&pLoader->mTokenMutex);
	initConditionVariable(&pLoader->mQueueCond);
	initConditionVariable(&pLoader->mTokenCond);
	initMutex(&pLoader->mSemaphoreMutex);

	pLoader->mTokenCounter = 0;
	pLoader->mTokenCompleted = 0;
	pLoader->mTokenSubmitted = 0;

	for (uint32_t i = 0; i < gpuCount; ++i)
	{
		setupCopyEngine(pLoader->ppRenderers[i], &pLoader->pCopyEngines[i], i, pLoader->mDesc.mBufferSize, pLoader->mDesc.mBufferCount);
	}

	pLoader->mThreadDesc.pFunc = streamerThreadFunc;
	pLoader->mThreadDesc.pData = pLoader;

#if defined(NX64)
	pLoader->mThreadDesc.pThreadStack = aligned_alloc(THREAD_STACK_ALIGNMENT_NX, ALIGNED_THREAD_STACK_SIZE_NX);
	pLoader->mThreadDesc.hThread = &pLoader->mThreadType;
	pLoader->mThreadDesc.pThreadName = "ResourceLoaderTask";
	pLoader->mThreadDesc.preferredCore = 1;
#endif

#if defined(DIRECT3D11)
	if (gSelectedRendererApi == RENDERER_API_D3D11)
		pLoader->mDesc.mSingleThreaded = true;
#endif

	// Create dedicated resource loader thread.
	if (!pLoader->mDesc.mSingleThreaded)
	{
		initThread(&pLoader->mThreadDesc, &pLoader->mThread);
	}

	*ppLoader = pLoader;
}

static void removeResourceLoader(ResourceLoader* pLoader)
{
	pLoader->mRun = false;    //-V601

	if (pLoader->mDesc.mSingleThreaded)
	{
		streamerThreadFunc(pLoader);
	}
	else
	{
		wakeOneConditionVariable(&pLoader->mQueueCond);
		destroyThread(pLoader->mThread);
	}

	destroyConditionVariable(&pLoader->mQueueCond);
	destroyConditionVariable(&pLoader->mTokenCond);
	destroyMutex(&pLoader->mQueueMutex);
	destroyMutex(&pLoader->mTokenMutex);
	destroyMutex(&pLoader->mSemaphoreMutex);

	tf_delete(pLoader);
}

static void queueBufferUpdate(ResourceLoader* pLoader, BufferUpdateDesc* pBufferUpdate, SyncToken* token)
{
	uint32_t nodeIndex = pBufferUpdate->pBuffer->mNodeIndex;
	acquireMutex(&pLoader->mQueueMutex);

	SyncToken t = tfrg_atomic64_add_relaxed(&pLoader->mTokenCounter, 1) + 1;

	pLoader->mRequestQueue[nodeIndex].emplace_back(UpdateRequest(*pBufferUpdate));
	pLoader->mRequestQueue[nodeIndex].back().mWaitIndex = t;
	pLoader->mRequestQueue[nodeIndex].back().pUploadBuffer = (pBufferUpdate->mInternal.mMappedRange.mFlags & MAPPED_RANGE_FLAG_TEMP_BUFFER)
																 ? pBufferUpdate->mInternal.mMappedRange.pBuffer
																 : NULL;
	releaseMutex(&pLoader->mQueueMutex);
	wakeOneConditionVariable(&pLoader->mQueueCond);
	if (token)
		*token = std::max(t, *token);
}

static void queueTextureUpdate(ResourceLoader* pLoader, TextureUpdateDescInternal* pTextureUpdate, SyncToken* token)
{
	ASSERT(pTextureUpdate->mRange.pBuffer);

	uint32_t nodeIndex = pTextureUpdate->pTexture->mNodeIndex;
	acquireMutex(&pLoader->mQueueMutex);

	SyncToken t = tfrg_atomic64_add_relaxed(&pLoader->mTokenCounter, 1) + 1;

	pLoader->mRequestQueue[nodeIndex].emplace_back(UpdateRequest(*pTextureUpdate));
	pLoader->mRequestQueue[nodeIndex].back().mWaitIndex = t;
	pLoader->mRequestQueue[nodeIndex].back().pUploadBuffer =
		(pTextureUpdate->mRange.mFlags & MAPPED_RANGE_FLAG_TEMP_BUFFER) ? pTextureUpdate->mRange.pBuffer : NULL;
	releaseMutex(&pLoader->mQueueMutex);
	wakeOneConditionVariable(&pLoader->mQueueCond);
	if (token)
		*token = std::max(t, *token);
}

#if defined(VULKAN)
static void queueBufferBarrier(ResourceLoader* pLoader, Buffer* pBuffer, ResourceState state, SyncToken* token)
{
	uint32_t nodeIndex = pBuffer->mNodeIndex;
	acquireMutex(&pLoader->mQueueMutex);

	SyncToken t = tfrg_atomic64_add_relaxed(&pLoader->mTokenCounter, 1) + 1;

	pLoader->mRequestQueue[nodeIndex].emplace_back(UpdateRequest{ BufferBarrier{ pBuffer, RESOURCE_STATE_UNDEFINED, state } });
	pLoader->mRequestQueue[nodeIndex].back().mWaitIndex = t;
	releaseMutex(&pLoader->mQueueMutex);
	wakeOneConditionVariable(&pLoader->mQueueCond);
	if (token)
		*token = max(t, *token);
}

static void queueTextureBarrier(ResourceLoader* pLoader, Texture* pTexture, ResourceState state, SyncToken* token)
{
	uint32_t nodeIndex = pTexture->mNodeIndex;
	acquireMutex(&pLoader->mQueueMutex);

	SyncToken t = tfrg_atomic64_add_relaxed(&pLoader->mTokenCounter, 1) + 1;

	pLoader->mRequestQueue[nodeIndex].emplace_back(UpdateRequest{ TextureBarrier{ pTexture, RESOURCE_STATE_UNDEFINED, state } });
	pLoader->mRequestQueue[nodeIndex].back().mWaitIndex = t;
	releaseMutex(&pLoader->mQueueMutex);
	wakeOneConditionVariable(&pLoader->mQueueCond);
	if (token)
		*token = max(t, *token);
}
#endif

static void queueTextureCopy(ResourceLoader* pLoader, TextureCopyDesc* pTextureCopy, SyncToken* token)
{
	ASSERT(pTextureCopy->pTexture->mNodeIndex == pTextureCopy->pBuffer->mNodeIndex);
	uint32_t nodeIndex = pTextureCopy->pTexture->mNodeIndex;
	acquireMutex(&pLoader->mQueueMutex);

	SyncToken t = tfrg_atomic64_add_relaxed(&pLoader->mTokenCounter, 1) + 1;

	pLoader->mRequestQueue[nodeIndex].emplace_back(UpdateRequest(*pTextureCopy));
	pLoader->mRequestQueue[nodeIndex].back().mWaitIndex = t;
	releaseMutex(&pLoader->mQueueMutex);
	wakeOneConditionVariable(&pLoader->mQueueCond);
	if (token)
		*token = std::max(t, *token);
}

static void waitForToken(ResourceLoader* pLoader, const SyncToken* token)
{
	if (pLoader->mDesc.mSingleThreaded)
	{
		return;
	}
	acquireMutex(&pLoader->mTokenMutex);
	while (!isTokenCompleted(token))
	{
		waitConditionVariable(&pLoader->mTokenCond, &pLoader->mTokenMutex, TIMEOUT_INFINITE);
	}
	releaseMutex(&pLoader->mTokenMutex);
}

static void waitForTokenSubmitted(ResourceLoader* pLoader, const SyncToken* token)
{
	if (pLoader->mDesc.mSingleThreaded)
	{
		return;
	}
	acquireMutex(&pLoader->mTokenMutex);
	while (!isTokenSubmitted(token))
	{
		waitConditionVariable(&pLoader->mTokenCond, &pLoader->mTokenMutex, TIMEOUT_INFINITE);
	}
	releaseMutex(&pLoader->mTokenMutex);
}
/************************************************************************/
// Resource Loader Interface Implementation
/************************************************************************/
void initResourceLoaderInterface(Renderer* pRenderer, ResourceLoaderDesc* pDesc) { addResourceLoader(&pRenderer, 1, pDesc, &pResourceLoader); }

void exitResourceLoaderInterface(Renderer* pRenderer) 
{
	removeResourceLoader(pResourceLoader); 
}

void initResourceLoaderInterface(Renderer** ppRenderers, uint32_t rendererCount, ResourceLoaderDesc* pDesc)
{
	addResourceLoader(ppRenderers, rendererCount, pDesc, &pResourceLoader);
}

void exitResourceLoaderInterface(Renderer** pRenderers, uint32_t rendererCount)
{
	removeResourceLoader(pResourceLoader);
}

void addResource(BufferLoadDesc* pBufferDesc, SyncToken* token)
{
	uint64_t stagingBufferSize = pResourceLoader->pCopyEngines[0].bufferSize;
	bool     update = pBufferDesc->pData || pBufferDesc->mForceReset;

	ASSERT(stagingBufferSize > 0);
	if (RESOURCE_MEMORY_USAGE_GPU_ONLY == pBufferDesc->mDesc.mMemoryUsage && !pBufferDesc->mDesc.mStartState && (!update || UMA))
	{
		pBufferDesc->mDesc.mStartState = util_determine_resource_start_state(&pBufferDesc->mDesc);
		LOGF(
			eWARNING, "Buffer start state not provided. Determined the start state as (%u) based on the provided BufferDesc",
			(uint32_t)pBufferDesc->mDesc.mStartState);
	}

	if (pBufferDesc->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY && update && !UMA)
	{
		pBufferDesc->mDesc.mStartState = RESOURCE_STATE_COMMON;
	}
	addBuffer(pResourceLoader->ppRenderers[pBufferDesc->mDesc.mNodeIndex], &pBufferDesc->mDesc, pBufferDesc->ppBuffer);

	if (update)
	{
		if (!UMA && pBufferDesc->mDesc.mSize > stagingBufferSize && pBufferDesc->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY)
		{
			// The data is too large for a single staging buffer copy, so perform it in stages.

			// Save the data parameter so we can restore it later.
			const void* data = pBufferDesc->pData;

			BufferUpdateDesc updateDesc = {};
			updateDesc.pBuffer = *pBufferDesc->ppBuffer;
			for (uint64_t offset = 0; offset < pBufferDesc->mDesc.mSize; offset += stagingBufferSize)
			{
				size_t chunkSize = (size_t) std::min(stagingBufferSize, pBufferDesc->mDesc.mSize - offset);
				updateDesc.mSize = chunkSize;
				updateDesc.mDstOffset = offset;
				beginUpdateResource(&updateDesc);
				if (pBufferDesc->mForceReset)
				{
					memset(updateDesc.pMappedData, 0, chunkSize);
				}
				else
				{
					ASSERT(data);
					memcpy(updateDesc.pMappedData, (char*)data + offset, chunkSize);    //-V769
				}
				endUpdateResource(&updateDesc, token);
			}
		}
		else
		{
			BufferUpdateDesc updateDesc = {};
			updateDesc.pBuffer = *pBufferDesc->ppBuffer;
			beginUpdateResource(&updateDesc);
			if (pBufferDesc->mForceReset)
			{
				memset(updateDesc.pMappedData, 0, (size_t)pBufferDesc->mDesc.mSize);
			}
			else
			{
				ASSERT(!pBufferDesc->mDesc.mSize || pBufferDesc->pData);
				if (pBufferDesc->pData)
					memcpy(updateDesc.pMappedData, pBufferDesc->pData, (size_t)pBufferDesc->mDesc.mSize);
			}
			endUpdateResource(&updateDesc, token);
		}
	}
	else
	{
		// Transition GPU buffer to desired state for Vulkan since all Vulkan resources are created in undefined state
#if defined(VULKAN)
		if (gSelectedRendererApi == RENDERER_API_VULKAN && pBufferDesc->mDesc.mMemoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY &&
			// Check whether this is required (user specified a state other than undefined / common)
			(pBufferDesc->mDesc.mStartState != RESOURCE_STATE_UNDEFINED && pBufferDesc->mDesc.mStartState != RESOURCE_STATE_COMMON))
			queueBufferBarrier(pResourceLoader, *pBufferDesc->ppBuffer, pBufferDesc->mDesc.mStartState, token);
#endif
	}
}

void beginUpdateResource(BufferUpdateDesc* pBufferUpdate)
{
	Buffer* pBuffer = pBufferUpdate->pBuffer;
	ASSERT(pBuffer);

	uint64_t size = pBufferUpdate->mSize > 0 ? pBufferUpdate->mSize : (pBufferUpdate->pBuffer->mSize - pBufferUpdate->mDstOffset);
	ASSERT(pBufferUpdate->mDstOffset + size <= pBuffer->mSize);

	ResourceMemoryUsage memoryUsage = (ResourceMemoryUsage)pBufferUpdate->pBuffer->mMemoryUsage;
	if (UMA || memoryUsage != RESOURCE_MEMORY_USAGE_GPU_ONLY)
	{
		bool map = !pBuffer->pCpuMappedAddress;
		if (map)
		{
			mapBuffer(pResourceLoader->ppRenderers[pBuffer->mNodeIndex], pBuffer, NULL);
		}

		pBufferUpdate->mInternal.mMappedRange = { (uint8_t*)pBuffer->pCpuMappedAddress + pBufferUpdate->mDstOffset, pBuffer };
		pBufferUpdate->pMappedData = pBufferUpdate->mInternal.mMappedRange.pData;
		pBufferUpdate->mInternal.mMappedRange.mFlags = map ? MAPPED_RANGE_FLAG_UNMAP_BUFFER : 0;
	}
	else
	{
		// We need to use a staging buffer.
		MappedMemoryRange range = allocateUploadMemory(pResourceLoader->ppRenderers[pBuffer->mNodeIndex], size, RESOURCE_BUFFER_ALIGNMENT);
		pBufferUpdate->pMappedData = range.pData;

		pBufferUpdate->mInternal.mMappedRange = range;
		pBufferUpdate->mInternal.mMappedRange.mFlags = MAPPED_RANGE_FLAG_TEMP_BUFFER;
	}
}

void endUpdateResource(BufferUpdateDesc* pBufferUpdate, SyncToken* token)
{
	if (pBufferUpdate->mInternal.mMappedRange.mFlags & MAPPED_RANGE_FLAG_UNMAP_BUFFER)
	{
		unmapBuffer(pResourceLoader->ppRenderers[pBufferUpdate->pBuffer->mNodeIndex], pBufferUpdate->pBuffer);
	}

	ResourceMemoryUsage memoryUsage = (ResourceMemoryUsage)pBufferUpdate->pBuffer->mMemoryUsage;
	if (!UMA && memoryUsage == RESOURCE_MEMORY_USAGE_GPU_ONLY)
	{
		queueBufferUpdate(pResourceLoader, pBufferUpdate, token);
	}

	// Restore the state to before the beginUpdateResource call.
	pBufferUpdate->pMappedData = NULL;
	pBufferUpdate->mInternal = {};
	if (pResourceLoader->mDesc.mSingleThreaded)
	{
		streamerThreadFunc(pResourceLoader);
	}
}

void beginUpdateResource(TextureUpdateDesc* pTextureUpdate)
{
	const Texture*        texture = pTextureUpdate->pTexture;
	const TinyImageFormat fmt = (TinyImageFormat)texture->mFormat;
	Renderer*             pRenderer = pResourceLoader->ppRenderers[texture->mNodeIndex];
	const uint32_t        alignment = util_get_texture_subresource_alignment(pRenderer, fmt);

	bool success = util_get_surface_info(
		MIP_REDUCE(texture->mWidth, pTextureUpdate->mMipLevel), MIP_REDUCE(texture->mHeight, pTextureUpdate->mMipLevel), fmt,
		&pTextureUpdate->mSrcSliceStride, &pTextureUpdate->mSrcRowStride, &pTextureUpdate->mRowCount);
	ASSERT(success);
	UNREF_PARAM(success);

	pTextureUpdate->mDstRowStride = KRG::Math::RoundUpToNearestMultiple32(pTextureUpdate->mSrcRowStride, util_get_texture_row_alignment(pRenderer));
	pTextureUpdate->mDstSliceStride = KRG::Math::RoundUpToNearestMultiple32(pTextureUpdate->mDstRowStride * pTextureUpdate->mRowCount, alignment);

	const ssize_t requiredSize = KRG::Math::RoundUpToNearestMultiple32(
		MIP_REDUCE(texture->mDepth, pTextureUpdate->mMipLevel) * pTextureUpdate->mDstRowStride * pTextureUpdate->mRowCount, alignment);

	// We need to use a staging buffer.
	pTextureUpdate->mInternal.mMappedRange = allocateUploadMemory(pRenderer, requiredSize, alignment);
	pTextureUpdate->mInternal.mMappedRange.mFlags = MAPPED_RANGE_FLAG_TEMP_BUFFER;
	pTextureUpdate->pMappedData = pTextureUpdate->mInternal.mMappedRange.pData;
}

void endUpdateResource(TextureUpdateDesc* pTextureUpdate, SyncToken* token)
{
	TextureUpdateDescInternal desc = {};
	desc.pTexture = pTextureUpdate->pTexture;
	desc.mRange = pTextureUpdate->mInternal.mMappedRange;
	desc.mBaseMipLevel = pTextureUpdate->mMipLevel;
	desc.mMipLevels = 1;
	desc.mBaseArrayLayer = pTextureUpdate->mArrayLayer;
	desc.mLayerCount = 1;
	queueTextureUpdate(pResourceLoader, &desc, token);

	// Restore the state to before the beginUpdateResource call.
	pTextureUpdate->pMappedData = NULL;
	pTextureUpdate->mInternal = {};
	if (pResourceLoader->mDesc.mSingleThreaded)
	{
		streamerThreadFunc(pResourceLoader);
	}
}

void copyResource(TextureCopyDesc* pTextureDesc, SyncToken* token)
{
	queueTextureCopy(pResourceLoader, pTextureDesc, token);
	if (pResourceLoader->mDesc.mSingleThreaded)
	{
		streamerThreadFunc(pResourceLoader);
	}
}

SyncToken getLastTokenCompleted() { return tfrg_atomic64_load_acquire(&pResourceLoader->mTokenCompleted); }

bool isTokenCompleted(const SyncToken* token) { return *token <= tfrg_atomic64_load_acquire(&pResourceLoader->mTokenCompleted); }

void waitForToken(const SyncToken* token) { waitForToken(pResourceLoader, token); }

SyncToken getLastTokenSubmitted() { return tfrg_atomic64_load_acquire(&pResourceLoader->mTokenSubmitted); }

bool isTokenSubmitted(const SyncToken* token) { return *token <= tfrg_atomic64_load_acquire(&pResourceLoader->mTokenSubmitted); }

void waitForTokenSubmitted(const SyncToken* token) { waitForTokenSubmitted(pResourceLoader, token); }

bool allResourceLoadsCompleted()
{
	SyncToken token = tfrg_atomic64_load_relaxed(&pResourceLoader->mTokenCounter);
	return token <= tfrg_atomic64_load_acquire(&pResourceLoader->mTokenCompleted);
}

void waitForAllResourceLoads()
{
	SyncToken token = tfrg_atomic64_load_relaxed(&pResourceLoader->mTokenCounter);
	waitForToken(pResourceLoader, &token);
}

bool isResourceLoaderSingleThreaded()
{
	ASSERT(pResourceLoader);
	return pResourceLoader->mDesc.mSingleThreaded;
}

Semaphore* getLastSemaphoreCompleted(uint32_t nodeIndex)
{
	acquireMutex(&pResourceLoader->mSemaphoreMutex);
	Semaphore* sem = pResourceLoader->pCopyEngines[nodeIndex].pLastCompletedSemaphore;
	releaseMutex(&pResourceLoader->mSemaphoreMutex);
	return sem;
}
/************************************************************************/
/************************************************************************/

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

#pragma once

#include "RendererConfig.h"

#include "System/Core/Math/Matrix.h"
#include "../Core/Atomics.h"
#include "IRenderer.h"

typedef struct MappedMemoryRange
{
	uint8_t* pData;
	Buffer*  pBuffer;
	uint64_t mOffset;
	uint64_t mSize;
	uint32_t mFlags;
} MappedMemoryRange;

// MARK: - Resource Loading

typedef struct BufferLoadDesc
{
	Buffer**    ppBuffer;
	const void* pData;
	BufferDesc  mDesc;
	/// Force Reset buffer to NULL
	bool mForceReset;
} BufferLoadDesc;

typedef struct VirtualTexturePageInfo
{
	uint32_t pageAlive;
	uint32_t TexID;
	uint32_t mipLevel;
	uint32_t padding1;
} VirtualTexturePageInfo;

typedef struct BufferUpdateDesc
{
	Buffer*  pBuffer;
	uint64_t mDstOffset;
	uint64_t mSize;

	/// To be filled by the caller
	/// Example:
	/// BufferUpdateDesc update = { pBuffer, bufferDstOffset };
	/// beginUpdateResource(&update);
	/// ParticleVertex* vertices = (ParticleVertex*)update.pMappedData;
	///   for (uint32_t i = 0; i < particleCount; ++i)
	///	    vertices[i] = { rand() };
	/// endUpdateResource(&update, &token);
	void* pMappedData;

	/// Internal
	struct
	{
		MappedMemoryRange mMappedRange;
	} mInternal;
} BufferUpdateDesc;

/// #NOTE: Only use for procedural textures which are created on CPU (noise textures, font texture, ...)
typedef struct TextureUpdateDesc
{
	Texture* pTexture;
	uint32_t mMipLevel;
	uint32_t mArrayLayer;

	/// To be filled by the caller
	/// Example:
	/// BufferUpdateDesc update = { pTexture, 2, 1 };
	/// beginUpdateResource(&update);
	/// Row by row copy is required if mDstRowStride > mSrcRowStride. Single memcpy will work if mDstRowStride == mSrcRowStride
	/// 2D
	/// for (uint32_t r = 0; r < update.mRowCount; ++r)
	///     memcpy(update.pMappedData + r * update.mDstRowStride, srcPixels + r * update.mSrcRowStride, update.mSrcRowStride);
	/// 3D
	/// for (uint32_t z = 0; z < depth; ++z)
	/// {
	///     uint8_t* dstData = update.pMappedData + update.mDstSliceStride * z;
	///     uint8_t* srcData = srcPixels + update.mSrcSliceStride * z;
	///     for (uint32_t r = 0; r < update.mRowCount; ++r)
	///         memcpy(dstData + r * update.mDstRowStride, srcData + r * update.mSrcRowStride, update.mSrcRowStride);
	/// }
	/// endUpdateResource(&update, &token);
	uint8_t* pMappedData;
	/// Size of each row in destination including padding - Needs to be respected otherwise texture data will be corrupted if dst row stride is not the same as src row stride
	uint32_t mDstRowStride;
	/// Number of rows in this slice of the texture
	uint32_t mRowCount;
	/// Src row stride for convenience (mRowCount * width * texture format size)
	uint32_t mSrcRowStride;
	/// Size of each slice in destination including padding - Use for offsetting dst data updating 3D textures
	uint32_t mDstSliceStride;
	/// Size of each slice in src - Use for offsetting src data when updating 3D textures
	uint32_t mSrcSliceStride;

	/// Internal
	struct
	{
		MappedMemoryRange mMappedRange;
	} mInternal;
} TextureUpdateDesc;

typedef struct TextureCopyDesc
{
	Texture* pTexture;
	Buffer* pBuffer;
	/// Semaphore to synchronize graphics/compute operations that write to the texture with the texture -> buffer copy.
	Semaphore* pWaitSemaphore;
	uint32_t mTextureMipLevel;
	uint32_t mTextureArrayLayer;
	/// Current texture state.
	ResourceState mTextureState;
	/// Queue the texture is copied from.
	QueueType mQueueType;
	uint64_t mBufferOffset;
} TextureCopyDesc;

typedef enum ShaderStageLoadFlags
{
	SHADER_STAGE_LOAD_FLAG_NONE = 0x0,
	/// D3D12 only - Enable passing primitive id to pixel shader
	SHADER_STAGE_LOAD_FLAG_ENABLE_PS_PRIMITIVEID = 0x1,
    /// Creates VR multisample variant of the shader
    SHADER_STAGE_LOAD_FLAG_ENABLE_VR_MULTIVIEW = 0x2,
} ShaderStageLoadFlags;
MAKE_ENUM_FLAG(uint32_t, ShaderStageLoadFlags);

typedef struct ShaderStageLoadDesc
{    //-V802 : Very user-facing struct, and order is highly important to convenience
	const char*          pFileName;
	ShaderMacro*         pMacros;
	uint32_t             mMacroCount;
	const char*          pEntryPointName;
	ShaderStageLoadFlags mFlags;
} ShaderStageLoadDesc;

typedef struct ShaderLoadDesc
{
	ShaderStageLoadDesc   mStages[SHADER_STAGE_COUNT];
	ShaderTarget          mTarget;
	const ShaderConstant* pConstants;
	uint32_t              mConstantCount;
} ShaderLoadDesc;

typedef struct PipelineCacheLoadDesc
{
	const char*        pFileName;
	const char*        pFilePassword;
	PipelineCacheFlags mFlags;
} PipelineCacheLoadDesc;

typedef struct PipelineCacheSaveDesc
{
	const char* pFileName;
	const char* pFilePassword;
} PipelineCacheSaveDesc;

typedef uint64_t SyncToken;

typedef struct ResourceLoaderDesc
{
	uint64_t mBufferSize;
	uint32_t mBufferCount;
	bool     mSingleThreaded;
} ResourceLoaderDesc;

extern ResourceLoaderDesc gDefaultResourceLoaderDesc;

// MARK: - Resource Loader Functions

void initResourceLoaderInterface(Renderer* pRenderer, ResourceLoaderDesc* pDesc = nullptr);
void exitResourceLoaderInterface(Renderer* pRenderer);

/// Multiple Renderer (unlinked GPU) variants. The Resource Loader must be shared between Renderers.
void initResourceLoaderInterface(Renderer** ppRenderers, uint32_t rendererCount, ResourceLoaderDesc* pDesc = nullptr);
void exitResourceLoaderInterface(Renderer** ppRenderers, uint32_t rendererCount);

// MARK: updateResource

void beginUpdateResource(BufferUpdateDesc* pBufferDesc);
void beginUpdateResource(TextureUpdateDesc* pTextureDesc);
void endUpdateResource(BufferUpdateDesc* pBuffer, SyncToken* token);
void endUpdateResource(TextureUpdateDesc* pTexture, SyncToken* token);

/// Copies data from GPU to the CPU, typically for transferring it to another GPU in unlinked mode.
/// For optimal use, the amount of data to transfer should be minimized as much as possible and applications should
/// provide additional graphics/compute work that the GPU can execute alongside the copy.
void copyResource(TextureCopyDesc* pTextureDesc, SyncToken* token);

// MARK: Waiting for Loads

/// Returns whether all submitted resource loads and updates have been completed.
bool allResourceLoadsCompleted();

/// Blocks the calling thread until allResourceLoadsCompleted() returns true.
/// Note that if more resource loads or updates are submitted from a different thread while
/// while the calling thread is blocked, those loads or updates are not guaranteed to have
/// completed when this function returns.
void waitForAllResourceLoads();

/// Returns wheter the resourceloader is single threaded or not
bool isResourceLoaderSingleThreaded();

/// A SyncToken is an array of monotonically increasing integers.
/// getLastTokenCompleted() returns the last value for which
/// isTokenCompleted(token) is guaranteed to return true.
SyncToken getLastTokenCompleted();
bool      isTokenCompleted(const SyncToken* token);
void      waitForToken(const SyncToken* token);

/// Allows clients to synchronize with the submission of copy commands (as opposed to their completion).
/// This can reduce the wait time for clients but requires using the Semaphore from getLastSemaphoreCompleted() in a wait
/// operation in a submit that uses the textures just updated.
SyncToken getLastTokenSubmitted();
bool      isTokenSubmitted(const SyncToken* token);
void      waitForTokenSubmitted(const SyncToken* token);

/// Return the semaphore for the last copy operation of a specific GPU.
/// Could be NULL if no operations have been executed.
Semaphore* getLastSemaphoreCompleted(uint32_t nodeIndex);

/// Save/Load pipeline cache from disk
void loadPipelineCache(Renderer* pRenderer, const PipelineCacheLoadDesc* pDesc, PipelineCache** ppPipelineCache);
void savePipelineCache(Renderer* pRenderer, PipelineCache* pPipelineCache, PipelineCacheSaveDesc* pDesc);

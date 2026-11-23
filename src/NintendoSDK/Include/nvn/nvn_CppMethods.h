// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/NVN/nvn_CppMethods.h
//
// Trimmed to the methods NX64NVNShaders.cpp and NX64NVNRenderStates.cpp inline. Each one forwards to
// its pfncpp_nvn* pointer; the builders return *this.

#pragma once

namespace nvn {

// line 447
inline NVNboolean Program::Initialize(nvn::Device * device) {
    return pfncpp_nvnProgramInitialize(this, device);
}

// line 452
inline void Program::Finalize() {
    pfncpp_nvnProgramFinalize(this);
}

// line 462
inline NVNboolean Program::SetShaders(int count, const nvn::ShaderData * stageData) {
    return pfncpp_nvnProgramSetShaders(this, count, stageData);
}

// line 467
inline nvn::MemoryPoolBuilder & MemoryPoolBuilder::SetDevice(nvn::Device * device) {
    pfncpp_nvnMemoryPoolBuilderSetDevice(this, device);
    return *this;
}

// line 473
inline nvn::MemoryPoolBuilder & MemoryPoolBuilder::SetDefaults() {
    pfncpp_nvnMemoryPoolBuilderSetDefaults(this);
    return *this;
}

// line 479
inline nvn::MemoryPoolBuilder & MemoryPoolBuilder::SetStorage(void * memory, size_t size) {
    pfncpp_nvnMemoryPoolBuilderSetStorage(this, memory, size);
    return *this;
}

// line 485
inline nvn::MemoryPoolBuilder & MemoryPoolBuilder::SetFlags(nvn::MemoryPoolFlags flags) {
    pfncpp_nvnMemoryPoolBuilderSetFlags(this, flags);
    return *this;
}

// line 511
inline NVNboolean MemoryPool::Initialize(const nvn::MemoryPoolBuilder * builder) {
    return pfncpp_nvnMemoryPoolInitialize(this, builder);
}

// line 521
inline void MemoryPool::Finalize() {
    pfncpp_nvnMemoryPoolFinalize(this);
}

// line 641
inline nvn::BufferBuilder & BufferBuilder::SetDevice(nvn::Device * device) {
    pfncpp_nvnBufferBuilderSetDevice(this, device);
    return *this;
}

// line 647
inline nvn::BufferBuilder & BufferBuilder::SetDefaults() {
    pfncpp_nvnBufferBuilderSetDefaults(this);
    return *this;
}

// line 653
inline nvn::BufferBuilder & BufferBuilder::SetStorage(nvn::MemoryPool * pool, ptrdiff_t offset, size_t size) {
    pfncpp_nvnBufferBuilderSetStorage(this, pool, offset, size);
    return *this;
}

// line 679
inline NVNboolean Buffer::Initialize(const nvn::BufferBuilder * builder) {
    return pfncpp_nvnBufferInitialize(this, builder);
}

// line 689
inline void Buffer::Finalize() {
    pfncpp_nvnBufferFinalize(this);
}

// line 699
inline nvn::BufferAddress Buffer::GetAddress() const {
    return pfncpp_nvnBufferGetAddress(this);
}

// line 719
inline ptrdiff_t Buffer::GetMemoryOffset() const {
    return pfncpp_nvnBufferGetMemoryOffset(this);
}

// line 1726
inline nvn::DepthStencilState & DepthStencilState::SetDepthTestEnable(NVNboolean enable) {
    pfncpp_nvnDepthStencilStateSetDepthTestEnable(this, enable);
    return *this;
}

// line 1732
inline nvn::DepthStencilState & DepthStencilState::SetDepthWriteEnable(NVNboolean enable) {
    pfncpp_nvnDepthStencilStateSetDepthWriteEnable(this, enable);
    return *this;
}

// line 1738
inline nvn::DepthStencilState & DepthStencilState::SetDepthFunc(nvn::DepthFunc func) {
    pfncpp_nvnDepthStencilStateSetDepthFunc(this, func);
    return *this;
}

// line 1744
inline nvn::DepthStencilState & DepthStencilState::SetStencilTestEnable(NVNboolean enable) {
    pfncpp_nvnDepthStencilStateSetStencilTestEnable(this, enable);
    return *this;
}

// line 1750
inline nvn::DepthStencilState & DepthStencilState::SetStencilFunc(nvn::Face faces, nvn::StencilFunc func) {
    pfncpp_nvnDepthStencilStateSetStencilFunc(this, faces, func);
    return *this;
}

// line 1756
inline nvn::DepthStencilState & DepthStencilState::SetStencilOp(nvn::Face faces, nvn::StencilOp fail, nvn::StencilOp depthFail, nvn::StencilOp depthPass) {
    pfncpp_nvnDepthStencilStateSetStencilOp(this, faces, fail, depthFail, depthPass);
    return *this;
}

// line 1792
inline nvn::VertexAttribState & VertexAttribState::SetDefaults() {
    pfncpp_nvnVertexAttribStateSetDefaults(this);
    return *this;
}

// line 1798
inline nvn::VertexAttribState & VertexAttribState::SetFormat(nvn::Format format, ptrdiff_t relativeOffset) {
    pfncpp_nvnVertexAttribStateSetFormat(this, format, relativeOffset);
    return *this;
}

// line 1804
inline nvn::VertexAttribState & VertexAttribState::SetStreamIndex(int streamIndex) {
    pfncpp_nvnVertexAttribStateSetStreamIndex(this, streamIndex);
    return *this;
}

// line 332
inline void Queue::Finish() {
    pfncpp_nvnQueueFinish(this);
}

// nvn_CppMethods.h:618
inline void SamplerPool::RegisterSampler(int id, const nvn::Sampler * sampler) const {
    pfncpp_nvnSamplerPoolRegisterSampler(this, id, sampler);
}

// nvn_CppMethods.h:337, 437
inline void Queue::PresentTexture(nvn::Window * window, int textureIndex) {
    pfncpp_nvnQueuePresentTexture(this, window, textureIndex);
}

inline void Window::SetCrop(int x, int y, int w, int h) {
    pfncpp_nvnWindowSetCrop(this, x, y, w, h);
}

// nvn_CppMethods.h:324, 1918, 2635
inline void Queue::SubmitCommands(int numCommands, const nvn::CommandHandle * handles) {
    pfncpp_nvnQueueSubmitCommands(this, numCommands, handles);
}

inline nvn::CommandHandle CommandBuffer::EndRecording() {
    return pfncpp_nvnCommandBufferEndRecording(this);
}

inline void CommandBuffer::SignalEvent(const nvn::Event * event, nvn::EventSignalMode mode, nvn::EventSignalLocation location, nvn::EventSignalFlags flags, uint32_t value) {
    pfncpp_nvnCommandBufferSignalEvent(this, event, mode, location, flags, value);
}

// nvn_CppMethods.h:2420
inline void CommandBuffer::WaitSync(const nvn::Sync * sync) {
    pfncpp_nvnCommandBufferWaitSync(this, sync);
}

// nvn_CppMethods.h:2128, 2143, 2640
inline void CommandBuffer::ClearColor(int index, const float * color, nvn::ClearColorMask mask) {
    pfncpp_nvnCommandBufferClearColor(this, index, color, mask);
}

inline void CommandBuffer::ClearDepthStencil(float depthValue, NVNboolean depthMask, int stencilValue, int stencilMask) {
    pfncpp_nvnCommandBufferClearDepthStencil(this, depthValue, depthMask, stencilValue, stencilMask);
}

inline void CommandBuffer::SetStencilCullCriteria(nvn::StencilFunc func, int ref, int mask) {
    pfncpp_nvnCommandBufferSetStencilCullCriteria(this, func, ref, mask);
}

// nvn_CppMethods.h:966, 978, 1002, 1068
inline nvn::TextureView & TextureView::SetDefaults() {
    pfncpp_nvnTextureViewSetDefaults(this);
    return *this;
}

inline nvn::TextureView & TextureView::SetLayers(int baseLayer, int numLayers) {
    pfncpp_nvnTextureViewSetLayers(this, baseLayer, numLayers);
    return *this;
}

inline nvn::TextureView & TextureView::SetTarget(nvn::TextureTarget target) {
    pfncpp_nvnTextureViewSetTarget(this, target);
    return *this;
}

// nvn_CppMethods.h:1053
inline void Texture::Finalize() {
    pfncpp_nvnTextureFinalize(this);
}

inline ptrdiff_t Texture::GetViewOffset(const nvn::TextureView * textureView) const {
    return pfncpp_nvnTextureGetViewOffset(this, textureView);
}

// Texture::GetStride / FlushTexels (nvn_CppMethods.h)
inline ptrdiff_t Texture::GetStride() const {
    return pfncpp_nvnTextureGetStride(this);
}

inline void Texture::FlushTexels(const nvn::TextureView * textureView, const nvn::CopyRegion * region) const {
    pfncpp_nvnTextureFlushTexels(this, textureView, region);
}

// CommandBuffer::ReportCounter / ResetCounter (line 2358)
inline void CommandBuffer::ReportCounter(nvn::CounterType counter, nvn::BufferAddress buffer) {
    pfncpp_nvnCommandBufferReportCounter(this, counter, buffer);
}

inline void CommandBuffer::ResetCounter(nvn::CounterType counter) {
    pfncpp_nvnCommandBufferResetCounter(this, counter);
}

// line 541
inline nvn::BufferAddress MemoryPool::GetBufferAddress() const {
    return pfncpp_nvnMemoryPoolGetBufferAddress(this);
}

// line 79
inline nvn::TextureHandle Device::GetTextureHandle(int textureID, int samplerID) const {
    return pfncpp_nvnDeviceGetTextureHandle(this, textureID, samplerID);
}

// line 1191
inline nvn::SamplerBuilder & SamplerBuilder::SetDevice(nvn::Device * device) {
    pfncpp_nvnSamplerBuilderSetDevice(this, device);
    return *this;
}

// line 1197
inline nvn::SamplerBuilder & SamplerBuilder::SetDefaults() {
    pfncpp_nvnSamplerBuilderSetDefaults(this);
    return *this;
}

// line 1203
inline nvn::SamplerBuilder & SamplerBuilder::SetMinMagFilter(nvn::MinFilter min, nvn::MagFilter mag) {
    pfncpp_nvnSamplerBuilderSetMinMagFilter(this, min, mag);
    return *this;
}

// line 1209
inline nvn::SamplerBuilder & SamplerBuilder::SetWrapMode(nvn::WrapMode s, nvn::WrapMode t, nvn::WrapMode r) {
    pfncpp_nvnSamplerBuilderSetWrapMode(this, s, t, r);
    return *this;
}

// line 1221
inline nvn::SamplerBuilder & SamplerBuilder::SetLodBias(float bias) {
    pfncpp_nvnSamplerBuilderSetLodBias(this, bias);
    return *this;
}

// line 1263
inline nvn::SamplerBuilder & SamplerBuilder::SetLodSnap(float lodSnap) {
    pfncpp_nvnSamplerBuilderSetLodSnap(this, lodSnap);
    return *this;
}

// line 1329
inline NVNboolean Sampler::Initialize(const nvn::SamplerBuilder * builder) {
    return pfncpp_nvnSamplerInitialize(this, builder);
}

// line 1404
inline nvn::BlendState & BlendState::SetDefaults() {
    pfncpp_nvnBlendStateSetDefaults(this);
    return *this;
}

// line 1416
inline nvn::BlendState & BlendState::SetBlendFunc(nvn::BlendFunc srcFunc, nvn::BlendFunc dstFunc, nvn::BlendFunc srcFuncAlpha, nvn::BlendFunc dstFuncAlpha) {
    pfncpp_nvnBlendStateSetBlendFunc(this, srcFunc, dstFunc, srcFuncAlpha, dstFuncAlpha);
    return *this;
}

// line 1422
inline nvn::BlendState & BlendState::SetBlendEquation(nvn::BlendEquation modeRGB, nvn::BlendEquation modeAlpha) {
    pfncpp_nvnBlendStateSetBlendEquation(this, modeRGB, modeAlpha);
    return *this;
}

// line 1487
inline nvn::ColorState & ColorState::SetDefaults() {
    pfncpp_nvnColorStateSetDefaults(this);
    return *this;
}

// line 1493
inline nvn::ColorState & ColorState::SetBlendEnable(int index, NVNboolean enable) {
    pfncpp_nvnColorStateSetBlendEnable(this, index, enable);
    return *this;
}

// line 1526
inline nvn::ChannelMaskState & ChannelMaskState::SetDefaults() {
    pfncpp_nvnChannelMaskStateSetDefaults(this);
    return *this;
}

// line 1532
inline nvn::ChannelMaskState & ChannelMaskState::SetChannelMask(int index, NVNboolean r, NVNboolean g, NVNboolean b, NVNboolean a) {
    pfncpp_nvnChannelMaskStateSetChannelMask(this, index, r, g, b, a);
    return *this;
}

// line 1670
inline nvn::PolygonState & PolygonState::SetDefaults() {
    pfncpp_nvnPolygonStateSetDefaults(this);
    return *this;
}

// line 1676
inline nvn::PolygonState & PolygonState::SetCullFace(nvn::Face face) {
    pfncpp_nvnPolygonStateSetCullFace(this, face);
    return *this;
}

// line 1682
inline nvn::PolygonState & PolygonState::SetFrontFace(nvn::FrontFace face) {
    pfncpp_nvnPolygonStateSetFrontFace(this, face);
    return *this;
}

// line 1688
inline nvn::PolygonState & PolygonState::SetPolygonMode(nvn::PolygonMode polygonMode) {
    pfncpp_nvnPolygonStateSetPolygonMode(this, polygonMode);
    return *this;
}

// line 1820
inline nvn::VertexStreamState & VertexStreamState::SetDefaults() {
    pfncpp_nvnVertexStreamStateSetDefaults(this);
    return *this;
}

// line 1933
inline void CommandBuffer::BindBlendState(const nvn::BlendState * blend) {
    pfncpp_nvnCommandBufferBindBlendState(this, blend);
}

// line 1938
inline void CommandBuffer::BindChannelMaskState(const nvn::ChannelMaskState * channelMask) {
    pfncpp_nvnCommandBufferBindChannelMaskState(this, channelMask);
}

// line 1943
inline void CommandBuffer::BindColorState(const nvn::ColorState * color) {
    pfncpp_nvnCommandBufferBindColorState(this, color);
}

// line 1953
inline void CommandBuffer::BindPolygonState(const nvn::PolygonState * polygon) {
    pfncpp_nvnCommandBufferBindPolygonState(this, polygon);
}

// line 1958
inline void CommandBuffer::BindDepthStencilState(const nvn::DepthStencilState * depthStencil) {
    pfncpp_nvnCommandBufferBindDepthStencilState(this, depthStencil);
}

// line 1963
inline void CommandBuffer::BindVertexAttribState(int numAttribs, const nvn::VertexAttribState * attribs) {
    pfncpp_nvnCommandBufferBindVertexAttribState(this, numAttribs, attribs);
}

// line 1968
inline void CommandBuffer::BindVertexStreamState(int numStreams, const nvn::VertexStreamState * streams) {
    pfncpp_nvnCommandBufferBindVertexStreamState(this, numStreams, streams);
}

// line 1973
inline void CommandBuffer::BindProgram(const nvn::Program * program, nvn::ShaderStageBits stages) {
    pfncpp_nvnCommandBufferBindProgram(this, program, stages);
}

// line 1983
inline void CommandBuffer::BindVertexBuffers(int first, int count, const nvn::BufferRange * buffers) {
    pfncpp_nvnCommandBufferBindVertexBuffers(this, first, count, buffers);
}

// line 2018
inline void CommandBuffer::BindTexture(nvn::ShaderStage stage, int index, nvn::TextureHandle texture) {
    pfncpp_nvnCommandBufferBindTexture(this, stage, index, texture);
}

// line 2158
inline void CommandBuffer::SetViewport(int x, int y, int w, int h) {
    pfncpp_nvnCommandBufferSetViewport(this, x, y, w, h);
}

// line 2183
inline void CommandBuffer::SetDepthRange(float n, float f) {
    pfncpp_nvnCommandBufferSetDepthRange(this, n, f);
}

// line 2248
inline void CommandBuffer::SetStencilRef(nvn::Face faces, int ref) {
    pfncpp_nvnCommandBufferSetStencilRef(this, faces, ref);
}

// line 2253
inline void CommandBuffer::SetBlendColor(const float * blendColor) {
    pfncpp_nvnCommandBufferSetBlendColor(this, blendColor);
}

// line 2348
inline void CommandBuffer::UpdateUniformBuffer(nvn::BufferAddress buffer, size_t bufferSize, ptrdiff_t updateOffset, size_t updateSize, const void * data) {
    pfncpp_nvnCommandBufferUpdateUniformBuffer(this, buffer, bufferSize, updateOffset, updateSize, data);
}

// line 2413
inline void CommandBuffer::Barrier(nvn::BarrierBits barrier) {
    pfncpp_nvnCommandBufferBarrier(this, barrier);
}

// line 531
inline void MemoryPool::FlushMappedRange(ptrdiff_t offset, size_t size) const {
    pfncpp_nvnMemoryPoolFlushMappedRange(this, offset, size);
}

// line 2083
inline void CommandBuffer::DrawArrays(nvn::DrawPrimitive mode, int first, int count) {
    pfncpp_nvnCommandBufferDrawArrays(this, mode, first, count);
}

// line 2093
inline void CommandBuffer::DrawElementsBaseVertex(nvn::DrawPrimitive mode, nvn::IndexType type, int count, nvn::BufferAddress indexBuffer, int baseVertex) {
    pfncpp_nvnCommandBufferDrawElementsBaseVertex(this, mode, type, count, indexBuffer, baseVertex);
}

// line 2378
inline void CommandBuffer::SetRenderTargets(int numColors, const nvn::Texture *const * colors, const nvn::TextureView *const * colorViews, const nvn::Texture * depthStencil, const nvn::TextureView * depthStencilView) {
    pfncpp_nvnCommandBufferSetRenderTargets(this, numColors, colors, colorViews, depthStencil, depthStencilView);
}

// line 144
inline uint64_t Device::GetTimestampInNanoseconds(const nvn::CounterData * counterData) const {
    return pfncpp_nvnDeviceGetTimestampInNanoseconds(this, counterData);
}

// line 1853
inline void CommandBuffer::Finalize() {
    pfncpp_nvnCommandBufferFinalize(this);
}

// line 1848
inline NVNboolean CommandBuffer::Initialize(nvn::Device * device) {
    return pfncpp_nvnCommandBufferInitialize(this, device);
}

// line 1863
inline void CommandBuffer::SetMemoryCallback(nvn::CommandBufferMemoryCallbackFunc callback) {
    pfncpp_nvnCommandBufferSetMemoryCallback(this, callback);
}

// line 1873
inline void CommandBuffer::AddCommandMemory(const nvn::MemoryPool * pool, ptrdiff_t offset, size_t size) {
    pfncpp_nvnCommandBufferAddCommandMemory(this, pool, offset, size);
}

// line 1878
inline void CommandBuffer::AddControlMemory(void * memory, size_t size) {
    pfncpp_nvnCommandBufferAddControlMemory(this, memory, size);
}

// line 2620
inline uint32_t Event::GetValue() const {
    return pfncpp_nvnEventGetValue(this);
}

// line 1720
inline nvn::DepthStencilState & DepthStencilState::SetDefaults() {
    pfncpp_nvnDepthStencilStateSetDefaults(this);
    return *this;
}

// line 734
inline nvn::TextureBuilder & TextureBuilder::SetDevice(nvn::Device * device) {
    pfncpp_nvnTextureBuilderSetDevice(this, device);
    return *this;
}

// line 740
inline nvn::TextureBuilder & TextureBuilder::SetDefaults() {
    pfncpp_nvnTextureBuilderSetDefaults(this);
    return *this;
}

// line 746
inline nvn::TextureBuilder & TextureBuilder::SetFlags(nvn::TextureFlags flags) {
    pfncpp_nvnTextureBuilderSetFlags(this, flags);
    return *this;
}

// line 752
inline nvn::TextureBuilder & TextureBuilder::SetTarget(nvn::TextureTarget target) {
    pfncpp_nvnTextureBuilderSetTarget(this, target);
    return *this;
}

// line 800
inline nvn::TextureBuilder & TextureBuilder::SetFormat(nvn::Format format) {
    pfncpp_nvnTextureBuilderSetFormat(this, format);
    return *this;
}

// line 812
inline nvn::TextureBuilder & TextureBuilder::SetSwizzle(nvn::TextureSwizzle r, nvn::TextureSwizzle g, nvn::TextureSwizzle b, nvn::TextureSwizzle a) {
    pfncpp_nvnTextureBuilderSetSwizzle(this, r, g, b, a);
    return *this;
}

// line 788
inline nvn::TextureBuilder & TextureBuilder::SetSize3D(int width, int height, int depth) {
    pfncpp_nvnTextureBuilderSetSize3D(this, width, height, depth);
    return *this;
}

// line 794
inline nvn::TextureBuilder & TextureBuilder::SetLevels(int numLevels) {
    pfncpp_nvnTextureBuilderSetLevels(this, numLevels);
    return *this;
}

// line 852
inline nvn::TextureBuilder & TextureBuilder::SetStride(ptrdiff_t stride) {
    pfncpp_nvnTextureBuilderSetStride(this, stride);
    return *this;
}

// line 824
inline size_t TextureBuilder::GetStorageSize() const {
    return pfncpp_nvnTextureBuilderGetStorageSize(this);
}

// line 829
inline size_t TextureBuilder::GetStorageAlignment() const {
    return pfncpp_nvnTextureBuilderGetStorageAlignment(this);
}

// line 834
inline nvn::TextureBuilder & TextureBuilder::SetStorage(nvn::MemoryPool * pool, ptrdiff_t offset) {
    pfncpp_nvnTextureBuilderSetStorage(this, pool, offset);
    return *this;
}

// line 1041
inline NVNboolean Texture::Initialize(const nvn::TextureBuilder * builder) {
    return pfncpp_nvnTextureInitialize(this, builder);
}

// line 576
inline void TexturePool::RegisterTexture(int id, const nvn::Texture * texture, const nvn::TextureView * view) const {
    pfncpp_nvnTexturePoolRegisterTexture(this, id, texture, view);
}

// line 347
inline nvn::WindowBuilder & WindowBuilder::SetDevice(nvn::Device * device) {
    pfncpp_nvnWindowBuilderSetDevice(this, device);
    return *this;
}

// line 353
inline nvn::WindowBuilder & WindowBuilder::SetDefaults() {
    pfncpp_nvnWindowBuilderSetDefaults(this);
    return *this;
}

// line 359
inline nvn::WindowBuilder & WindowBuilder::SetNativeWindow(NVNnativeWindow nativeWindow) {
    pfncpp_nvnWindowBuilderSetNativeWindow(this, nativeWindow);
    return *this;
}

// line 365
inline nvn::WindowBuilder & WindowBuilder::SetTextures(int numTextures, nvn::Texture *const * textures) {
    pfncpp_nvnWindowBuilderSetTextures(this, numTextures, textures);
    return *this;
}

// line 371
inline nvn::WindowBuilder & WindowBuilder::SetPresentInterval(int presentInterval) {
    pfncpp_nvnWindowBuilderSetPresentInterval(this, presentInterval);
    return *this;
}

// line 402
inline NVNboolean Window::Initialize(const nvn::WindowBuilder * builder) {
    return pfncpp_nvnWindowInitialize(this, builder);
}

// line 2563
inline NVNboolean Sync::Initialize(nvn::Device * device) {
    return pfncpp_nvnSyncInitialize(this, device);
}

// line 214
inline nvn::QueueBuilder & QueueBuilder::SetDevice(nvn::Device * device) {
    pfncpp_nvnQueueBuilderSetDevice(this, device);
    return *this;
}

// line 220
inline nvn::QueueBuilder & QueueBuilder::SetDefaults() {
    pfncpp_nvnQueueBuilderSetDefaults(this);
    return *this;
}

// line 238
inline nvn::QueueBuilder & QueueBuilder::SetComputeMemorySize(size_t size) {
    pfncpp_nvnQueueBuilderSetComputeMemorySize(this, size);
    return *this;
}

// line 232
inline nvn::QueueBuilder & QueueBuilder::SetCommandMemorySize(size_t size) {
    pfncpp_nvnQueueBuilderSetCommandMemorySize(this, size);
    return *this;
}

// line 261
inline nvn::QueueBuilder & QueueBuilder::SetCommandFlushThreshold(size_t threshold) {
    pfncpp_nvnQueueBuilderSetCommandFlushThreshold(this, threshold);
    return *this;
}

// line 250
inline size_t QueueBuilder::GetQueueMemorySize() const {
    return pfncpp_nvnQueueBuilderGetQueueMemorySize(this);
}

// line 255
inline nvn::QueueBuilder & QueueBuilder::SetQueueMemory(void * memory, size_t size) {
    pfncpp_nvnQueueBuilderSetQueueMemory(this, memory, size);
    return *this;
}

// line 307
inline NVNboolean Queue::Initialize(const nvn::QueueBuilder * builder) {
    return pfncpp_nvnQueueInitialize(this, builder);
}

// line 561
inline NVNboolean TexturePool::Initialize(const nvn::MemoryPool * memoryPool, ptrdiff_t offset, int numDescriptors) {
    return pfncpp_nvnTexturePoolInitialize(this, memoryPool, offset, numDescriptors);
}

// line 601
inline NVNboolean SamplerPool::Initialize(const nvn::MemoryPool * memoryPool, ptrdiff_t offset, int numDescriptors) {
    return pfncpp_nvnSamplerPoolInitialize(this, memoryPool, offset, numDescriptors);
}

// line 2318
inline void CommandBuffer::CopyTextureToTexture(const nvn::Texture * srcTexture, const nvn::TextureView * srcView, const nvn::CopyRegion * srcRegion, const nvn::Texture * dstTexture, const nvn::TextureView * dstView, const nvn::CopyRegion * dstRegion, nvn::CopyFlags flags) {
    pfncpp_nvnCommandBufferCopyTextureToTexture(this, srcTexture, srcView, srcRegion, dstTexture, dstView, dstRegion, flags);
}

// line 417
inline nvn::WindowAcquireTextureResult Window::AcquireTexture(nvn::Sync * textureAvailableSync, int * textureIndex) {
    return pfncpp_nvnWindowAcquireTexture(this, textureAvailableSync, textureIndex);
}

// line 1913
inline void CommandBuffer::BeginRecording() {
    pfncpp_nvnCommandBufferBeginRecording(this);
}

// line 1988
inline void CommandBuffer::BindUniformBuffer(nvn::ShaderStage stage, int index, nvn::BufferAddress buffer, size_t size) {
    pfncpp_nvnCommandBufferBindUniformBuffer(this, stage, index, buffer, size);
}

// line 2428
inline void CommandBuffer::SetTexturePool(const nvn::TexturePool * texturePool) {
    pfncpp_nvnCommandBufferSetTexturePool(this, texturePool);
}

// line 2433
inline void CommandBuffer::SetSamplerPool(const nvn::SamplerPool * samplerPool) {
    pfncpp_nvnCommandBufferSetSamplerPool(this, samplerPool);
}

// line 571
inline void TexturePool::Finalize() {
    pfncpp_nvnTexturePoolFinalize(this);
}

// line 611
inline void SamplerPool::Finalize() {
    pfncpp_nvnSamplerPoolFinalize(this);
}

// line 2615
inline void Event::Finalize() {
    pfncpp_nvnEventFinalize(this);
}

// line 29
inline void DeviceBuilder::SetDefaults() {
    pfncpp_nvnDeviceBuilderSetDefaults(this);
}

// line 34
inline void DeviceBuilder::SetFlags(nvn::DeviceFlagBits flags) {
    pfncpp_nvnDeviceBuilderSetFlags(this, flags);
}

// line 44
inline NVNboolean Device::Initialize(const nvn::DeviceBuilder * builder) {
    return pfncpp_nvnDeviceInitialize(this, builder);
}

// line 104
inline void Device::SetWindowOriginMode(nvn::WindowOriginMode windowOriginMode) {
    pfncpp_nvnDeviceSetWindowOriginMode(this, windowOriginMode);
}

// line 109
inline void Device::SetDepthMode(nvn::DepthMode depthMode) {
    pfncpp_nvnDeviceSetDepthMode(this, depthMode);
}

// line 2593
inline nvn::EventBuilder & EventBuilder::SetDefaults() {
    pfncpp_nvnEventBuilderSetDefaults(this);
    return *this;
}

// line 2599
inline nvn::EventBuilder & EventBuilder::SetStorage(const nvn::MemoryPool * pool, int64_t offset) {
    pfncpp_nvnEventBuilderSetStorage(this, pool, offset);
    return *this;
}

// line 2610
inline NVNboolean Event::Initialize(const nvn::EventBuilder * builder) {
    return pfncpp_nvnEventInitialize(this, builder);
}

// line 2625
inline void Event::Signal(nvn::EventSignalMode mode, uint32_t value) {
    pfncpp_nvnEventSignal(this, mode, value);
}

// line 1826
inline nvn::VertexStreamState & VertexStreamState::SetStride(ptrdiff_t stride) {
    pfncpp_nvnVertexStreamStateSetStride(this, stride);
    return *this;
}

}  // namespace nvn

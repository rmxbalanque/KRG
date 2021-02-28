#include "RenderDevice_DX11.h"
#include "RenderTypes_DX11.h"
#include "TextureLoader_Win32.h"
#include "System/Render/RenderDevice/RenderDefaultResources.h"
#include "System/Render/RenderDevice/RenderDeviceSettings.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

#define KRG_ENABLE_RENDERDEVICE_DEBUG 0

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        static bool CreateDepthStencilState( ID3D11Device* pDevice, bool enable, bool writeEnable, ID3D11DepthStencilState** ppState )
        {
            D3D11_DEPTH_STENCIL_DESC desc;
            Memory::MemsetZero( &desc, sizeof( desc ) );

            desc.DepthEnable = enable;
            desc.DepthWriteMask = writeEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

            desc.StencilEnable = false;
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

            desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

            desc.BackFace = desc.FrontFace;
            return( SUCCEEDED( pDevice->CreateDepthStencilState( &desc, ppState ) ) );
        }

        //-------------------------------------------------------------------------

        RenderDevice::RenderDevice()
            : m_resolution( 0 )
            , m_refreshRate( 0 )
            , m_fullscreen( false )
            , m_pDevice( nullptr )
            , m_pSwapChain( nullptr )
            , m_pDepthStencilView( nullptr )
        {
            m_pRenderTargetView = nullptr;
        }

        RenderDevice::~RenderDevice()
        {
            KRG_ASSERT( RenderContext::s_pDepthTestingReadOnly == nullptr );
            KRG_ASSERT( RenderContext::s_pDepthTestingOff == nullptr );
            KRG_ASSERT( RenderContext::s_pDepthTestingOn == nullptr );
            KRG_ASSERT( !m_immediateContext.IsValid() );

            KRG_ASSERT( m_pRenderTargetView == nullptr );
            KRG_ASSERT( m_pDepthStencilView == nullptr );
            KRG_ASSERT( m_pSwapChain == nullptr );
            KRG_ASSERT( m_pDevice == nullptr );
        }

        bool RenderDevice::IsInitialized() const
        {
            return m_pDevice != nullptr;
        }

        bool RenderDevice::Initialize()
        {
            m_resolution = Int2( Settings::g_resolutionX, Settings::g_resolutionY );
            m_refreshRate = Settings::g_refreshRate;
            m_fullscreen = Settings::g_fullscreen;

            CreateDeviceAndSwapchain();
            CreateRenderTargets();
            CreateDepthStencilView();
            CreateDefaultDepthStencilStates();

            // Set OM default state
            m_immediateContext.m_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
            m_immediateContext.m_pDeviceContext->OMSetDepthStencilState( RenderContext::s_pDepthTestingOn, 0 );

            DefaultResources::Initialize( this );

            return true;
        }

        void RenderDevice::Shutdown()
        {
            DefaultResources::Shutdown( this );

            m_immediateContext.m_pDeviceContext->ClearState();
            m_immediateContext.m_pDeviceContext->Flush();

            DestroyDefaultDepthStencilStates();
            DestroyDepthStencilView();
            DestroyRenderTargets();
            DestroyDeviceAndSwapchain();
        }

        //-------------------------------------------------------------------------

        bool RenderDevice::CreateDeviceAndSwapchain()
        {
            DXGI_SWAP_CHAIN_DESC swapChainDesc;
            KRG::Memory::MemsetZero( &swapChainDesc, sizeof( swapChainDesc ) );

            // Set buffer dimensions and format
            swapChainDesc.BufferCount = 2;
            swapChainDesc.BufferDesc.Width = m_resolution.x;
            swapChainDesc.BufferDesc.Height = m_resolution.y;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc.BufferDesc.RefreshRate.Numerator = m_refreshRate;
            swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            swapChainDesc.OutputWindow = GetActiveWindow();
            swapChainDesc.Windowed = !m_fullscreen;

            // Set debug flags on D3D device in debug build
            UINT flags = 0;
            #if KRG_ENABLE_RENDERDEVICE_DEBUG
            flags |= D3D10_CREATE_DEVICE_DEBUG;
            #endif

            // Setup D3D feature levels
            D3D_FEATURE_LEVEL featureLevelsRequested[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
            D3D_FEATURE_LEVEL featureLevelSupported;
            U32 const numLevelsRequested = 2;

            // Create the D3D device
            HRESULT result = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevelsRequested, numLevelsRequested, D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, &featureLevelSupported, &m_immediateContext.m_pDeviceContext );
            if ( FAILED( result ) )
            {
                if ( m_immediateContext.m_pDeviceContext != nullptr )
                {
                    m_immediateContext.m_pDeviceContext->Release();
                    m_immediateContext.m_pDeviceContext = nullptr;
                }

                if ( m_pSwapChain != nullptr )
                {
                    m_pSwapChain->Release();
                    m_pSwapChain = nullptr;
                }

                KRG_LOG_ERROR( "RenderingSystem", "Device Creation Failed" );
                return false;
            }

            return true;
        }

        void RenderDevice::DestroyDeviceAndSwapchain()
        {

            if ( m_pSwapChain != nullptr )
            {
                m_pSwapChain->Release();
                m_pSwapChain = nullptr;
            }

            if ( m_immediateContext.m_pDeviceContext != nullptr )
            {
                m_immediateContext.m_pDeviceContext->Flush();
                m_immediateContext.m_pDeviceContext->Release();
                m_immediateContext.m_pDeviceContext = nullptr;
            }

            if ( m_pDevice != nullptr )
            {
                #if KRG_ENABLE_RENDERDEVICE_DEBUG
                ID3D11Debug* pDebug = nullptr;
                m_pDevice->QueryInterface( IID_PPV_ARGS( &pDebug ) );
                #endif

                m_pDevice->Release();
                m_pDevice = nullptr;

                #if KRG_ENABLE_RENDERDEVICE_DEBUG
                if ( pDebug != nullptr )
                {
                    //pDebug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL );
                    pDebug->Release();
                }
                #endif
            }
        }

        bool RenderDevice::CreateRenderTargets()
        {
            ID3D11Texture2D* pBackBuffer;
            if ( FAILED( m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*) &pBackBuffer ) ) )
            {
                KRG_LOG_ERROR( "RenderingSystem", "Failed to get back buffer texture resource" );
                return false;
            }

            D3D11_TEXTURE2D_DESC textureDesc;
            pBackBuffer->GetDesc( &textureDesc );

            // Create primary render target
            //-------------------------------------------------------------------------

            HRESULT result = m_pDevice->CreateRenderTargetView( pBackBuffer, nullptr, &m_pRenderTargetView );
            pBackBuffer->Release();
            if ( FAILED( result ) )
            {
                KRG_LOG_ERROR( "RenderingSystem", "Failed to create render target" );
                return false;
            }

            return true;
        }

        void RenderDevice::DestroyRenderTargets()
        {
            ID3D11RenderTargetView* nullViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { 0 };
            m_immediateContext.m_pDeviceContext->OMSetRenderTargets( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullViews, nullptr );

            if ( m_pRenderTargetView != nullptr )
            {
                m_pRenderTargetView->Release();
                m_pRenderTargetView = nullptr;
            }
        }

        bool RenderDevice::CreateDepthStencilView()
        {
            // Create depth stencil texture
            D3D11_TEXTURE2D_DESC descDepth;
            descDepth.Width = m_resolution.x;
            descDepth.Height = m_resolution.y;
            descDepth.MipLevels = 1;
            descDepth.ArraySize = 1;
            descDepth.Format = DXGI_FORMAT_D32_FLOAT;
            descDepth.SampleDesc.Count = 1;
            descDepth.SampleDesc.Quality = 0;
            descDepth.Usage = D3D11_USAGE_DEFAULT;
            descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            descDepth.CPUAccessFlags = 0;
            descDepth.MiscFlags = 0;

            ID3D11Texture2D* pDepthStencil = nullptr;
            if ( FAILED( m_pDevice->CreateTexture2D( &descDepth, nullptr, &pDepthStencil ) ) )
            {
                KRG_LOG_ERROR( "RenderingSystem", "Depth stencil creation failed" );
                return false;
            }

            // Create the depth stencil view
            D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
            descDSV.Format = descDepth.Format;
            descDSV.Flags = 0;
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            descDSV.Texture2D.MipSlice = 0;

            auto result = m_pDevice->CreateDepthStencilView( pDepthStencil, &descDSV, &m_pDepthStencilView );
            pDepthStencil->Release();

            if ( FAILED( result ) )
            {
                KRG_LOG_ERROR( "RenderingSystem", "Depth stencil resource view creation failed" );
                return false;
            }

            return true;
        }

        void RenderDevice::DestroyDepthStencilView()
        {
            m_immediateContext.m_pDeviceContext->OMSetDepthStencilState( nullptr, 0 );

            if ( m_pDepthStencilView != nullptr )
            {
                m_pDepthStencilView->Release();
                m_pDepthStencilView = nullptr;
            }
        }

        bool RenderDevice::CreateDefaultDepthStencilStates()
        {
            // Create the three depth testing modes
            if ( !CreateDepthStencilState( m_pDevice, true, true, &RenderContext::s_pDepthTestingOn ) )
            {
                KRG_LOG_ERROR( "RenderingSystem", "Depth stencil state creation failed" );
                return false;
            }

            if ( !CreateDepthStencilState( m_pDevice, false, false, &RenderContext::s_pDepthTestingOff ) )
            {
                KRG_LOG_ERROR( "RenderingSystem", "Depth stencil state creation failed" );
                return false;
            }

            if ( !CreateDepthStencilState( m_pDevice, true, false, &RenderContext::s_pDepthTestingReadOnly ) )
            {
                KRG_LOG_ERROR( "RenderingSystem", "Depth stencil state creation failed" );
                return false;
            }

            return true;
        }

        void RenderDevice::DestroyDefaultDepthStencilStates()
        {
            if ( RenderContext::s_pDepthTestingOn != nullptr )
            {
                RenderContext::s_pDepthTestingOn->Release();
                RenderContext::s_pDepthTestingOn = nullptr;
            }

            if ( RenderContext::s_pDepthTestingOff != nullptr )
            {
                RenderContext::s_pDepthTestingOff->Release();
                RenderContext::s_pDepthTestingOff = nullptr;
            }

            if ( RenderContext::s_pDepthTestingReadOnly != nullptr )
            {
                RenderContext::s_pDepthTestingReadOnly->Release();
                RenderContext::s_pDepthTestingReadOnly = nullptr;
            }
        }

        //-------------------------------------------------------------------------

        void RenderDevice::PresentFrame()
        {
            KRG_PROFILE_FUNCTION_RENDER();

            static Float4 const backgroundColor = Colors::LightGray.ToFloat4();

            KRG_ASSERT( IsInitialized() );

            // Show rendered frame
            m_pSwapChain->Present( 0, 0 );

            // Clear render targets and depth stencil
            m_immediateContext.m_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
            m_immediateContext.m_pDeviceContext->ClearRenderTargetView( m_pRenderTargetView, (F32*) &backgroundColor );
            m_immediateContext.m_pDeviceContext->ClearDepthStencilView( m_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );
        }

        void RenderDevice::ResizeRenderTargets( Int2 const& dimensions )
        {
            KRG_ASSERT( dimensions.x > 0 && dimensions.y > 0 );
            m_resolution = dimensions;

            // Release render target and depth stencil
            DestroyDepthStencilView();
            DestroyRenderTargets();
            m_immediateContext.m_pDeviceContext->Flush();

            // Create new render targets
            if ( FAILED( m_pSwapChain->ResizeBuffers( 2, m_resolution.x, m_resolution.y, DXGI_FORMAT_UNKNOWN, 0 ) ) )
            {
                KRG_LOG_ERROR( "RenderingSystem", "Failed to resize swap chain buffers" );
                KRG_HALT();
            }

            if ( !CreateRenderTargets() || !CreateDepthStencilView() )
            {
                KRG_LOG_ERROR( "RenderingSystem", "Failed to create render targets/depth stencil view" );
                KRG_HALT();
            }

            m_immediateContext.m_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
        }

        //-------------------------------------------------------------------------

        void RenderDevice::CreateShader( Shader& shader )
        {
            KRG_ASSERT( IsInitialized() && !shader.IsValid() );

            if ( shader.GetPipelineStage() == PipelineStage::Vertex )
            {
                ID3D11VertexShader* pVertexShader;
                if ( SUCCEEDED( m_pDevice->CreateVertexShader( &shader.m_byteCode[0], shader.m_byteCode.size(), nullptr, &pVertexShader ) ) )
                {
                    shader.m_shaderHandle.m_pHandle = pVertexShader;
                    shader.m_shaderHandle.m_type = ResourceHandle::Type::Shader;
                }
            }
            else if ( shader.GetPipelineStage() == PipelineStage::Pixel )
            {
                ID3D11PixelShader* pPixelShader;
                if ( SUCCEEDED( m_pDevice->CreatePixelShader( &shader.m_byteCode[0], shader.m_byteCode.size(), nullptr, &pPixelShader ) ) )
                {
                    shader.m_shaderHandle.m_pHandle = pPixelShader;
                    shader.m_shaderHandle.m_type = ResourceHandle::Type::Shader;
                }
            }
            else if ( shader.GetPipelineStage() == PipelineStage::Geometry )
            {
                ID3D11GeometryShader* pGeometryShader;
                if ( SUCCEEDED( m_pDevice->CreateGeometryShader( &shader.m_byteCode[0], shader.m_byteCode.size(), nullptr, &pGeometryShader ) ) )
                {
                    shader.m_shaderHandle.m_pHandle = pGeometryShader;
                    shader.m_shaderHandle.m_type = ResourceHandle::Type::Shader;
                }
            }
            else //  Hull / Compute / etc...
            {
                KRG_UNIMPLEMENTED_FUNCTION();
            }

            // Create buffers const for shader
            for ( auto& cbuffer : shader.m_cbuffers )
            {
                CreateBuffer( cbuffer );
                KRG_ASSERT( cbuffer.IsValid() );
            }

            KRG_ASSERT( shader.IsValid() );
        }

        void RenderDevice::DestroyShader( Shader& shader )
        {
            KRG_ASSERT( IsInitialized() && shader.IsValid() && shader.GetPipelineStage() != PipelineStage::None );

            if ( shader.GetPipelineStage() == PipelineStage::Vertex )
            {
                ( (ID3D11VertexShader*) shader.m_shaderHandle.m_pHandle )->Release();
            }
            else if ( shader.GetPipelineStage() == PipelineStage::Pixel )
            {
                ( (ID3D11PixelShader*) shader.m_shaderHandle.m_pHandle )->Release();
            }
            else if ( shader.GetPipelineStage() == PipelineStage::Geometry )
            {
                ( (ID3D11GeometryShader*) shader.m_shaderHandle.m_pHandle )->Release();
            }
            else if ( shader.GetPipelineStage() == PipelineStage::Hull )
            {
                ( (ID3D11HullShader*) shader.m_shaderHandle.m_pHandle )->Release();
            }
            else if ( shader.GetPipelineStage() == PipelineStage::Compute )
            {
                ( (ID3D11ComputeShader*) shader.m_shaderHandle.m_pHandle )->Release();
            }

            shader.m_shaderHandle.Reset();

            for ( auto& cbuffer : shader.m_cbuffers )
            {
                DestroyBuffer( cbuffer );
            }
            shader.m_cbuffers.clear();
        }

        //-------------------------------------------------------------------------

        void RenderDevice::CreateBuffer( RenderBuffer& buffer, void const* pInitializationData )
        {
            KRG_ASSERT( IsInitialized() && !buffer.IsValid() );

            D3D11_BUFFER_DESC bufferDesc;
            KRG::Memory::MemsetZero( &bufferDesc, sizeof( D3D11_BUFFER_DESC ) );
            bufferDesc.ByteWidth = buffer.m_byteSize;
            bufferDesc.StructureByteStride = buffer.m_byteStride;

            switch ( buffer.m_usage )
            {
                case RenderBuffer::Usage::CPU_and_GPU:
                bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
                bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                break;

                case RenderBuffer::Usage::GPU_only:
                bufferDesc.Usage = D3D11_USAGE_DEFAULT;
                break;

                default:
                bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
                break;
            }

            switch ( buffer.m_type )
            {
                case RenderBuffer::Type::Constant:
                bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                break;

                case RenderBuffer::Type::Vertex:
                bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                break;

                case RenderBuffer::Type::Index:
                bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                KRG_ASSERT( buffer.m_byteStride == 2 || buffer.m_byteStride == 4 ); // only 16/32 bit indices support
                break;

                default:
                KRG_HALT();
            }

            // If there is initialization data set it
            D3D11_SUBRESOURCE_DATA initData;
            if ( pInitializationData != nullptr )
            {
                KRG::Memory::MemsetZero( &initData, sizeof( D3D11_SUBRESOURCE_DATA ) );
                initData.pSysMem = pInitializationData;
            }

            // Create and store buffer
            m_pDevice->CreateBuffer( &bufferDesc, pInitializationData == nullptr ? nullptr : &initData, (ID3D11Buffer**) &buffer.m_resourceHandle.m_pHandle );
            buffer.m_resourceHandle.m_type = ResourceHandle::Type::Buffer;
            KRG_ASSERT( buffer.IsValid() );
        }

        void RenderDevice::ResizeBuffer( RenderBuffer& buffer, U32 newSize )
        {
            KRG_ASSERT( buffer.IsValid() && newSize % buffer.m_byteStride == 0 );

            // Release D3D buffer
            ( (ID3D11Buffer*) buffer.m_resourceHandle.m_pHandle )->Release();
            buffer.m_resourceHandle.m_pHandle = nullptr;
            buffer.m_byteSize = newSize;
            CreateBuffer( buffer );
        }

        void RenderDevice::DestroyBuffer( RenderBuffer& buffer )
        {
            KRG_ASSERT( IsInitialized() );

            if ( buffer.IsValid() )
            {
                ( (ID3D11Buffer*) buffer.m_resourceHandle.m_pHandle )->Release();
                buffer.m_resourceHandle.Reset();
                buffer = RenderBuffer();
            }
        }

        //-------------------------------------------------------------------------

        namespace DX11
        {
            static char const* const g_semanticNames[] = { "POSITION", "NORMAL", "TANGENT", "BINORMAL", "COLOR", "TEXCOORD", "BLENDINDICES", "BLENDWEIGHTS" };

            static char const* GetNameForSemantic( DataSemantic semantic )
            {
                KRG_ASSERT( semantic < DataSemantic::None );

                switch ( semantic )
                {
                    case DataSemantic::Position: return g_semanticNames[0]; break;
                    case DataSemantic::Normal: return g_semanticNames[1]; break;
                    case DataSemantic::Tangent: return g_semanticNames[2]; break;
                    case DataSemantic::BiTangent: return g_semanticNames[3]; break;
                    case DataSemantic::Color: return g_semanticNames[4]; break;
                    case DataSemantic::TexCoord: return g_semanticNames[5]; break;
                    case DataSemantic::BlendIndex: return g_semanticNames[6]; break;
                    case DataSemantic::BlendWeight: return g_semanticNames[7]; break;
                }

                return nullptr;
            }

            static DXGI_FORMAT GetDXGIFormat( DataTypeFormat format )
            {
                switch ( format )
                {
                    case DataTypeFormat::UInt_R8: return DXGI_FORMAT_R8_UINT; break;
                    case DataTypeFormat::UInt_R8G8: return DXGI_FORMAT_R8G8_UINT; break;
                    case DataTypeFormat::UInt_R8G8B8A8: return DXGI_FORMAT_R8G8B8A8_UINT; break;

                    case DataTypeFormat::UNorm_R8: return DXGI_FORMAT_R8_UNORM; break;
                    case DataTypeFormat::UNorm_R8G8: return DXGI_FORMAT_R8G8_UNORM; break;
                    case DataTypeFormat::UNorm_R8G8B8A8: return DXGI_FORMAT_R8G8B8A8_UNORM; break;

                    case DataTypeFormat::UInt_R32: return DXGI_FORMAT_R32_UINT; break;
                    case DataTypeFormat::UInt_R32G32: return DXGI_FORMAT_R32G32_UINT; break;
                    case DataTypeFormat::UInt_R32G32B32: return DXGI_FORMAT_R32G32B32_UINT; break;
                    case DataTypeFormat::UInt_R32G32B32A32: return DXGI_FORMAT_R32G32B32A32_UINT; break;
                   
                    case DataTypeFormat::SInt_R32: return DXGI_FORMAT_R32_SINT; break;
                    case DataTypeFormat::SInt_R32G32: return DXGI_FORMAT_R32G32_SINT; break;
                    case DataTypeFormat::SInt_R32G32B32: return DXGI_FORMAT_R32G32B32_SINT; break;
                    case DataTypeFormat::SInt_R32G32B32A32: return DXGI_FORMAT_R32G32B32A32_SINT; break;

                    case DataTypeFormat::Float_R32: return DXGI_FORMAT_R32_FLOAT; break;
                    case DataTypeFormat::Float_R32G32: return DXGI_FORMAT_R32G32_FLOAT; break;
                    case DataTypeFormat::Float_R32G32B32: return DXGI_FORMAT_R32G32B32_FLOAT; break;
                    case DataTypeFormat::Float_R32G32B32A32: return DXGI_FORMAT_R32G32B32A32_FLOAT; break;

                    default:
                    KRG_HALT();
                    break;
                }

                return DXGI_FORMAT_UNKNOWN;
            }
        }

        void RenderDevice::CreateShaderInputBinding( VertexShader const& shader, VertexLayoutDescriptor const& vertexLayoutDesc, ResourceHandle& inputBinding )
        {
            KRG_ASSERT( IsInitialized() && shader.GetPipelineStage() == PipelineStage::Vertex && vertexLayoutDesc.IsValid() );

            bool bindingSucceeded = true;
            TVector<D3D11_INPUT_ELEMENT_DESC> inputLayout;
            auto const& shaderVertexLayoutDesc = shader.GetVertexLayoutDesc();
            for ( auto const& shaderVertexElementDesc : shaderVertexLayoutDesc.m_elementDescriptors )
            {
                bool inputBound = false;
                for ( auto const& vertexElement : vertexLayoutDesc.m_elementDescriptors )
                {
                    if ( shaderVertexElementDesc.m_semantic == vertexElement.m_semantic && shaderVertexElementDesc.m_semanticIndex == vertexElement.m_semanticIndex )
                    {
                        D3D11_INPUT_ELEMENT_DESC elementDesc;
                        elementDesc.SemanticName = DX11::GetNameForSemantic( shaderVertexElementDesc.m_semantic );
                        elementDesc.SemanticIndex = shaderVertexElementDesc.m_semanticIndex;
                        elementDesc.Format = DX11::GetDXGIFormat( shaderVertexElementDesc.m_format );
                        elementDesc.InputSlot = 0;
                        elementDesc.AlignedByteOffset = vertexElement.m_offset;
                        elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                        elementDesc.InstanceDataStepRate = 0;
                        inputLayout.push_back( elementDesc );

                        inputBound = true;
                        break;
                    }
                }

                if ( !inputBound )
                {
                    bindingSucceeded = false;
                    break;
                }
            }

            // Try to create Input Layout
            ID3D11InputLayout* pInputLayout = nullptr;
            if ( bindingSucceeded )
            {
                m_pDevice->CreateInputLayout( &inputLayout[0], (UINT) inputLayout.size(), &shader.m_byteCode[0], shader.m_byteCode.size(), &pInputLayout );
                inputBinding.m_pHandle = pInputLayout;
                inputBinding.m_type = ResourceHandle::Type::ShaderInputBinding;
            }
        }

        void RenderDevice::DestroyShaderInputBinding( ResourceHandle& inputBinding )
        {
            KRG_ASSERT( IsInitialized() && inputBinding.IsValid() );
            ( (ID3D11InputLayout*) inputBinding.m_pHandle )->Release();
            inputBinding.Reset();
        }

        //-------------------------------------------------------------------------

        void RenderDevice::CreateRasterizerState( RasterizerState& state )
        {
            KRG_ASSERT( IsInitialized() && !state.IsValid() );

            D3D11_RASTERIZER_DESC rdesc;
            KRG::Memory::MemsetZero( &rdesc, sizeof( D3D11_RASTERIZER_DESC ) );
            rdesc.FrontCounterClockwise = true;
            rdesc.DepthClipEnable = true;

            switch ( state.m_cullMode )
            {
                case CullMode::None: rdesc.CullMode = D3D11_CULL_NONE; break;
                case CullMode::FrontFace: rdesc.CullMode = D3D11_CULL_FRONT; break;
                case CullMode::BackFace: rdesc.CullMode = D3D11_CULL_BACK; break;
            }

            switch ( state.m_fillMode )
            {
                case FillMode::Solid: rdesc.FillMode = D3D11_FILL_SOLID; break;
                case FillMode::Wireframe: rdesc.FillMode = D3D11_FILL_WIREFRAME; break;
            }

            rdesc.ScissorEnable = state.m_scissorCulling;
            auto result = m_pDevice->CreateRasterizerState( &rdesc, (ID3D11RasterizerState**) &state.m_resourceHandle.m_pHandle );
            state.m_resourceHandle.m_type = ResourceHandle::Type::RasterizerState;
            KRG_ASSERT( SUCCEEDED( result ) );
        }

        void RenderDevice::DestroyRasterizerState( RasterizerState& state )
        {
            KRG_ASSERT( IsInitialized() );
            KRG_ASSERT( state.IsValid() );
            ( (ID3D11RasterizerState*) state.m_resourceHandle.m_pHandle )->Release();
            state.m_resourceHandle.Reset();
        }

        //-------------------------------------------------------------------------

        namespace DX11
        {
            static D3D11_BLEND GetBlendValue( BlendValue value )
            {
                switch ( value )
                {
                    case BlendValue::Zero: return D3D11_BLEND_ZERO; break;
                    case BlendValue::One: return D3D11_BLEND_ONE; break;
                    case BlendValue::SourceColor: return D3D11_BLEND_SRC_COLOR; break;
                    case BlendValue::InverseSourceColor: return D3D11_BLEND_INV_SRC_COLOR; break;
                    case BlendValue::SourceAlpha: return D3D11_BLEND_SRC_ALPHA; break;
                    case BlendValue::InverseSourceAlpha: return D3D11_BLEND_INV_SRC_ALPHA; break;
                    case BlendValue::DestinationAlpha: return D3D11_BLEND_DEST_ALPHA; break;
                    case BlendValue::InverseDestinationAlpha: return D3D11_BLEND_INV_DEST_ALPHA; break;
                    case BlendValue::DestinationColor: return D3D11_BLEND_DEST_COLOR; break;
                    case BlendValue::InverseDestinationColor: return D3D11_BLEND_INV_DEST_COLOR; break;
                    case BlendValue::SourceAlphaSaturated: return D3D11_BLEND_SRC_ALPHA_SAT; break;
                    case BlendValue::BlendFactor: return D3D11_BLEND_BLEND_FACTOR; break;
                    case BlendValue::InverseBlendFactor: return D3D11_BLEND_INV_BLEND_FACTOR; break;
                    case BlendValue::Source1Color: return D3D11_BLEND_SRC1_COLOR; break;
                    case BlendValue::InverseSource1Color: return D3D11_BLEND_INV_SRC1_COLOR; break;
                    case BlendValue::Source1Alpha: return D3D11_BLEND_SRC1_ALPHA; break;
                    case BlendValue::InverseSource1Alpha: return D3D11_BLEND_INV_SRC1_ALPHA; break;
                }

                return D3D11_BLEND_ZERO;
            }

            static D3D11_BLEND_OP GetBlendOp( BlendOp op )
            {
                switch ( op )
                {
                    case BlendOp::Add: return D3D11_BLEND_OP_ADD; break;
                    case BlendOp::SourceMinusDestination: return D3D11_BLEND_OP_SUBTRACT; break;
                    case BlendOp::DestinationMinusSource: return D3D11_BLEND_OP_REV_SUBTRACT; break;
                    case BlendOp::Min: return D3D11_BLEND_OP_MIN; break;
                    case BlendOp::Max: return D3D11_BLEND_OP_MAX; break;
                }

                return D3D11_BLEND_OP_ADD;
            }
        }

        void RenderDevice::CreateBlendState( BlendState& state )
        {
            KRG_ASSERT( IsInitialized() );

            D3D11_BLEND_DESC blendDesc;
            KRG::Memory::MemsetZero( &blendDesc, sizeof( D3D11_BLEND_DESC ) );

            blendDesc.AlphaToCoverageEnable = false;
            blendDesc.IndependentBlendEnable = true;

            blendDesc.RenderTarget[0].BlendEnable = state.m_blendEnable;
            blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            blendDesc.RenderTarget[0].SrcBlend = DX11::GetBlendValue( state.m_srcValue );
            blendDesc.RenderTarget[0].DestBlend = DX11::GetBlendValue( state.m_dstValue );
            blendDesc.RenderTarget[0].BlendOp = DX11::GetBlendOp( state.m_blendOp );
            blendDesc.RenderTarget[0].SrcBlendAlpha = DX11::GetBlendValue( state.m_srcAlphaValue );
            blendDesc.RenderTarget[0].DestBlendAlpha = DX11::GetBlendValue( state.m_dstAlphaValue );
            blendDesc.RenderTarget[0].BlendOpAlpha = DX11::GetBlendOp( state.m_blendOpAlpha );

            for ( auto i = 1; i < 8; i++ )
            {
                blendDesc.RenderTarget[i].BlendEnable = false;
                blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
                blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
                blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
                blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
                blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
                blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
                blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            }

            m_pDevice->CreateBlendState( &blendDesc, (ID3D11BlendState**) &state.m_resourceHandle.m_pHandle );
            state.m_resourceHandle.m_type = ResourceHandle::Type::BlendState;
            KRG_ASSERT( state.IsValid() );
        }

        void RenderDevice::DestroyBlendState( BlendState& state )
        {
            KRG_ASSERT( IsInitialized() && state.IsValid() );
            ( (ID3D11BlendState*) state.m_resourceHandle.m_pHandle )->Release();
            state.m_resourceHandle.Reset();
        }

        //-------------------------------------------------------------------------

        void RenderDevice::CreateTexture( Texture& texture )
        {
            switch ( texture.m_format )
            {
                case TextureFormat::Raw : CreateRawTexture( texture ); break;
                case TextureFormat::DDS : CreateDDSTexture( texture ); break;
            }
        }

        void RenderDevice::CreateRawTexture( Texture& texture )
        {
            KRG_ASSERT( IsInitialized() );

            D3D11_TEXTURE2D_DESC texDesc;
            Memory::MemsetZero( &texDesc, sizeof( texDesc ) );
            texDesc.Width = texture.m_dimensions.x;
            texDesc.Height = texture.m_dimensions.y;
            texDesc.MipLevels = 1;
            texDesc.ArraySize = 1;
            texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            texDesc.SampleDesc.Count = 1;
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.CPUAccessFlags = 0;

            // Create texture
            auto pTextureSRV = KRG::New<DX11::TextureSRV>();

            ID3D11Texture2D* pTexture2D = nullptr;
            D3D11_SUBRESOURCE_DATA subResource;
            subResource.pSysMem = texture.m_rawData.data();
            subResource.SysMemPitch = texDesc.Width * 4;
            subResource.SysMemSlicePitch = 0;
            if ( m_pDevice->CreateTexture2D( &texDesc, &subResource, &pTexture2D ) != S_OK )
            {
                KRG::Delete<DX11::TextureSRV>( pTextureSRV );
                return;
            }
            pTextureSRV->m_pTextureResource = pTexture2D;

            // Create texture view
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            Memory::MemsetZero( &srvDesc, sizeof( srvDesc ) );
            srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
            srvDesc.Texture2D.MostDetailedMip = 0;
            if ( m_pDevice->CreateShaderResourceView( pTexture2D, &srvDesc, &pTextureSRV->m_pTextureSRV ) != S_OK )
            {
                KRG::Delete<DX11::TextureSRV>( pTextureSRV );
                return;
            }

            texture.m_resourceHandle.m_pHandle = pTextureSRV;
            texture.m_resourceHandle.m_type = ResourceHandle::Type::Texture;
        }

        void RenderDevice::CreateDDSTexture( Texture& texture )
        {
            KRG_ASSERT( IsInitialized() );

            auto pTextureSRV = KRG::New<DX11::TextureSRV>();
            auto result = DirectX::CreateDDSTextureFromMemory( m_pDevice, m_immediateContext.m_pDeviceContext, texture.m_rawData.data(), texture.m_rawData.size(), &pTextureSRV->m_pTextureResource, &pTextureSRV->m_pTextureSRV );
            if ( result != S_OK )
            {
                KRG::Delete<DX11::TextureSRV>( pTextureSRV );
            }

            texture.m_resourceHandle.m_pHandle = pTextureSRV;
            texture.m_resourceHandle.m_type = ResourceHandle::Type::Texture;
        }

        void RenderDevice::DestroyTexture( Texture& texture )
        {
            KRG_ASSERT( IsInitialized() && texture.IsValid() );

            auto pTextureSRV = ( DX11::TextureSRV* ) texture.m_resourceHandle.m_pHandle;
            pTextureSRV->m_pTextureSRV->Release();
            pTextureSRV->m_pTextureResource->Release();

            KRG::Delete<DX11::TextureSRV>( ( DX11::TextureSRV*& ) texture.m_resourceHandle.m_pHandle );
            texture.m_resourceHandle.Reset();
        }

        //-------------------------------------------------------------------------

        namespace DX11
        {
            D3D11_FILTER GetTextureFilter( TextureFiltering filtering )
            {
                switch ( filtering )
                {
                    case TextureFiltering::MinMagMipPoint: return D3D11_FILTER_MIN_MAG_MIP_POINT; break;
                    case TextureFiltering::MinMagPointMipLinear: return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR; break;
                    case TextureFiltering::MinPointMagLinearMipPoint: return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT; break;
                    case TextureFiltering::MinPointMagMipLinear: return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR; break;
                    case TextureFiltering::MinLinearMagMipPoint: return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT; break;
                    case TextureFiltering::MinLinearMagPointMipLinear: return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR; break;
                    case TextureFiltering::MinMagLinearMipPoint: return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; break;
                    case TextureFiltering::MinMagMipLinear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
                    case TextureFiltering::Anisotropic: return D3D11_FILTER_ANISOTROPIC; break;
                    case TextureFiltering::ComparisonMinMagMipPoint: return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; break;
                    case TextureFiltering::ComparisonMinMagPointMipLinear: return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR; break;
                    case TextureFiltering::ComparisonMinPointMagLinearMipPoint: return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT; break;
                    case TextureFiltering::ComparisonMinPointMagMipLinear: return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR; break;
                    case TextureFiltering::ComparisonMinLinearMagMipPoint: return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT; break;
                    case TextureFiltering::ComparisonMinLinearMagPointMipLinear: return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR; break;
                    case TextureFiltering::ComparisonMinMagLinearMipPoint: return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; break;
                    case TextureFiltering::ComparisonMinMagMipLinear: return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; break;
                    case TextureFiltering::ComparisonAnisotropic: return D3D11_FILTER_COMPARISON_ANISOTROPIC; break;
                    case TextureFiltering::MinimumMinMagMipPoint: return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT; break;
                    case TextureFiltering::MinimumMinMagPointMipLinear: return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR; break;
                    case TextureFiltering::MinimumMinPointMagLinearMipPoint: return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT; break;
                    case TextureFiltering::MinimumMinPointMagMipLinear: return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR; break;
                    case TextureFiltering::MinimumMinLinearMagMipPoint: return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT; break;
                    case TextureFiltering::MinimumMinLinearMagPointMipLinear:return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR; break;
                    case TextureFiltering::MinimumMinMagLinearMipPoint:  return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT; break;
                    case TextureFiltering::MinimumMinMagMipLinear: return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR; break;
                    case TextureFiltering::MinimumAnisotropic: return D3D11_FILTER_MINIMUM_ANISOTROPIC; break;
                    case TextureFiltering::MaximumMinMagMipPoint: return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT; break;
                    case TextureFiltering::MaximumMinMagPointMipLinear: return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR; break;
                    case TextureFiltering::MaximumMinPointMagLinearMipPoint: return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT; break;
                    case TextureFiltering::MaximumMinPointMagMipLinear: return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR; break;
                    case TextureFiltering::MaximumMinLinearMagMipPoint: return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT; break;
                    case TextureFiltering::MaximumMinLinearMagPointMipLinear: return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR; break;
                    case TextureFiltering::MaximumMinMagLinearMipPoint: return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT; break;
                    case TextureFiltering::MaximumMinMagMipLinear: return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR; break;
                    case TextureFiltering::MaximumAnisotropic: return D3D11_FILTER_MAXIMUM_ANISOTROPIC; break;
                }

                return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            }

            D3D11_TEXTURE_ADDRESS_MODE GetTextureAddressMode( TextureAddressMode mode )
            {
                switch ( mode )
                {
                    case TextureAddressMode::Wrap: return D3D11_TEXTURE_ADDRESS_WRAP; break;
                    case TextureAddressMode::Mirror: return D3D11_TEXTURE_ADDRESS_MIRROR; break;
                    case TextureAddressMode::Clamp: return D3D11_TEXTURE_ADDRESS_CLAMP; break;
                    case TextureAddressMode::Border: return D3D11_TEXTURE_ADDRESS_BORDER; break;
                }

                return D3D11_TEXTURE_ADDRESS_WRAP;
            }
        }

        void RenderDevice::CreateSamplerState( SamplerState& state )
        {
            KRG_ASSERT( IsInitialized() );

            D3D11_SAMPLER_DESC samplerDesc;
            KRG::Memory::MemsetZero( &samplerDesc, sizeof( D3D11_SAMPLER_DESC ) );
            samplerDesc.Filter = DX11::GetTextureFilter( TextureFiltering::Anisotropic );
            samplerDesc.AddressU = DX11::GetTextureAddressMode( state.m_addressModeU );
            samplerDesc.AddressV = DX11::GetTextureAddressMode( state.m_addressModeV );
            samplerDesc.AddressW = DX11::GetTextureAddressMode( state.m_addressModeW );
            samplerDesc.MipLODBias = state.m_LODBias;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
            samplerDesc.MinLOD = state.m_minLOD;
            samplerDesc.MaxLOD = state.m_maxLOD;
            samplerDesc.MaxAnisotropy = 16;

            m_pDevice->CreateSamplerState( &samplerDesc, (ID3D11SamplerState**) &state.m_resourceHandle.m_pHandle );
            state.m_resourceHandle.m_type = ResourceHandle::Type::SamplerState;
        }

        void RenderDevice::DestroySamplerState( SamplerState& state )
        {
            KRG_ASSERT( IsInitialized() );
            KRG_ASSERT( state.IsValid() );
            ( (ID3D11SamplerState*) state.m_resourceHandle.m_pHandle )->Release();
            state.m_resourceHandle.Reset();
        }
    }
}
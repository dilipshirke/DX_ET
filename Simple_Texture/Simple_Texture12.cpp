#include "stdafx.h"
#include "Simple_Texture12.h"

using namespace SF2;
using namespace SF2::D3D12;

namespace EnablingTests
{
    namespace D3D12
    {
		//const uint CONSTANT_BUFFER_SIZE_IN_BYTES = 16 * sizeof(float4);
		void Simple_Texture::RegisterCases(TestGroup* group)
        {
            group->Description = L"Simple_Texture\n";
			group->AddCase(new Simple_Texture(Parameters{}));           
        }

        Simple_Texture::Simple_Texture(Parameters parameters) :
        parameters(parameters)
		//constantBufferColor(0.0, 1.0, 0.0, 0.5)
        {
			SetPreferredRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
			SetOptimizedClearValueForRenderTargets(float4(1.0f, 1.0f, 1.0f, 1.0f));			
        }

        void Simple_Texture::SetupRootSignatures()
        {
            graphicsRootSignature = Device->GetRootSignatureFactory()				
				//->AddParameterAsConstantBufferView(0)
				->AddParameterAsDescriptorTable(DescriptorRange(DescriptorRangeType::ShaderResourceView, 1), ShaderVisibility::Pixel)
				->AddParameterAsDescriptorTable(DescriptorRange(DescriptorRangeType::Sampler, 1), ShaderVisibility::Pixel)
                ->AllowInputAssemblerInputLayout()
                ->Create();
        }

        void Simple_Texture::SetupPipelineStateObjects()
        {	

            graphicsPipelineState = Device->GetGraphicsPipelineStateFactory()
				
                ->SetVertexShader(Device->GetShaderFactory()
                    ->SetHLSLFromResource(L"Simple_Texture12.hlsl")
                    ->SetEntrypoint("VSMain")
                    ->SetTarget(Shader::Vertex)
                    ->Compile())

                ->SetPixelShader(Device->GetShaderFactory()
                    ->SetHLSLFromResource(L"Simple_Texture12.hlsl")
                    ->SetEntrypoint("PSMain")
                    ->SetTarget(Shader::Pixel)
                    ->Compile())

                ->SetRenderTargetFormat(RenderTargetFormat())				
				->GetInputLayout()
				->AddElements([](InputElementContainer* inputElementContainer)
				{
					inputElementContainer->Add()
						->SetSemanticName("Position")
						->SetSemanticIndex(0)
						->SetFormat(DXGI_FORMAT_R32G32B32A32_FLOAT)
						->SetAppendAlignedElement();

					inputElementContainer->Add()
						->SetSemanticName("Texcoord")
						->SetSemanticIndex(0)
						->SetFormat(DXGI_FORMAT_R32G32_FLOAT)
						->SetAppendAlignedElement();
				})
				->Done()
				
				->SetPrimitiveTopologyType(PrimitiveTopologyType::Triangle)
				->SetRootSignature(graphicsRootSignature)
				->Create();
        }

        void Simple_Texture::SetupCommandObjects()
        {
            defaultCommandQueue = Device->GetDefaultCommandQueue();
            graphicsCommandAllocator = Device->CreateCommandAllocator(CommandListType::Direct);
            graphicsCommandList = Device->CreateCommandList(CommandListType::Direct, graphicsCommandAllocator);
            graphicsCommandList->Close();
        }

        void Simple_Texture::SetupResources2()
        {
		}

        void Simple_Texture::SetupDescriptorHeaps()
        {            
        }

        void Simple_Texture::SetupViewsAndSamplers()
        {            
        }

        void Simple_Texture::SetupUploadHeaps()
        {
			// Create Upload Heap
			uploadHeap = Device->GetCommittedResourceFactory()
				->SetDimension(ResourceDimension::Buffer)
				->SetWidth(16 * 1024 * 1024)
				->SetInitialResourceState(ResourceState::GenericRead)
				->SetHeapProperties(HeapProperties(HeapType::Upload))
				->Create();

			// Upload Heap Manager
			UploadHeapManager manager(uploadHeap);

			// Upload Vertex Data
			std::vector<float4> vertices(
				{
					float4(0.0f, 0.5f, 0.0f, 1.0f),
					float4(0.5f, -0.5f, 0.0f, 1.0f),
					float4(-0.5f, -0.5f, 0.0f, 1.0f),
				});			

			vertexBufferHeap = Device->GetCommittedResourceFactory()
				->SetDimension(ResourceDimension::Buffer)
				->SetWidth(4 * sizeof(float4))
				->SetHeapProperties(HeapProperties(HeapType::Upload))
				->SetInitialResourceState(ResourceState::GenericRead)
				->Create();			

			auto map = vertexBufferHeap->Map();
			std::copy(vertices.begin(), vertices.end(), (float4*)map->GetData());
			map->Unmap();
			
			// Upload Contand Data
			//constantBufferRegion = manager.ReserveSpaceForConstantBuffer(CONSTANT_BUFFER_SIZE_IN_BYTES);
			//auto constantBufferMapping = constantBufferRegion->GetTypedMapping<float4>();
			//constantBufferMapping(0) = constantBufferColor;
			//constantBufferView = ConstantBufferView(uploadHeap, CONSTANT_BUFFER_SIZE_IN_BYTES, constantBufferRegion->GetOffset());
			
			
			// Texture sample using image data			
			DirectX::ScratchImage image;
			CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			texture2DData.reset(new Texture2DData<byte4>(RenderX(), RenderY()));			
			LOGINFO(L"Warning: Simple Texture12 test - Loading Image!");
			if (DirectX::LoadFromWICFile(L"C:\\Users\\dshirke\\Source\\Repos\\ET_1\\EnablingTests-master\\Tests\\D3D11\\Benchmarks\\BenchSobel\\BenchSobel.bmp", 
				DirectX::WIC_FLAGS_FORCE_RGB, nullptr, image) != S_OK)

				throw std::exception("Failed to load image!");
			
			shaderResource = Device->GetCommittedResourceFactory()
				->SetDimension(ResourceDimension::Texture2D)
				->SetWidth((uint)image.GetMetadata().width)
				->SetHeight((uint)image.GetMetadata().height)
				->SetFormat(image.GetMetadata().format)
				->SetInitialResourceState(ResourceState::Common)
				->SetHeapProperties(HeapProperties(HeapType::Default))
				->Create();
			
			shaderResourceRegion = manager.ReserveSpaceForTexture(shaderResource);
			auto shaderResourceMapping = shaderResourceRegion->GetTypedMapping<byte4>();
			Subresource<byte4> imgView(image);
			shaderResourceMapping.CopyFrom(imgView);
			

			
			/*
			// Texture sample using gradient data
			texture2DData.reset(new Texture2DData<byte4>(RenderX(), RenderY()));
			texture2DData->Sub(0).FillWithGradient(byte4(0, 255, 0, 255), byte4(255, 0, 255, 0));

			shaderResource = Device->GetCommittedResourceFactory()
				->SetDimension(ResourceDimension::Texture2D)
				->SetWidth(RenderX())
				->SetHeight(RenderY())
				->SetFormat(RenderTargetFormat())
				->SetInitialResourceState(ResourceState::Common)
				->SetHeapProperties(HeapProperties(HeapType::Default))
				->Create();

			shaderResourceRegion = manager.ReserveSpaceForTexture(shaderResource);		
			auto shaderResourceMapping = shaderResourceRegion->GetTypedMapping<byte4>();
			shaderResourceMapping.CopyFrom(texture2DData->Sub(0));
			*/
			
			
			
			shaderResourceDescriptorHeap = Device->GetDescriptorHeapFactory()
				->SetType(DescriptorHeapType::ConstantBufferViewShaderResourceViewUnorderedAccessView)
				->SetNumberOfDescriptors(1)
				->SetShaderVisible()
				->Create();


			Device->CreateShaderResourceView(ShaderResourceView().SetResource(shaderResource), shaderResourceDescriptorHeap->GetCPUDescriptorHandle(0));

			// Sampler heap
			samplerDescriptorHeap = Device->GetDescriptorHeapFactory()
				->SetType(DescriptorHeapType::Sampler)
				->SetNumberOfDescriptors(1)
				->SetShaderVisible()
				->Create();

			Device->CreateSampler(
				Sampler().SetFilter(FilterType::Linear, FilterType::Linear, FilterType::Linear),
				samplerDescriptorHeap->GetCPUDescriptorHandle(0));
			
			graphicsCommandList = Device->CreateCommandList(CommandListType::Direct, graphicsCommandAllocator);
			graphicsCommandList->Close();
			
        }

        void Simple_Texture::RecordFrame(uint frame, uint workerId)
        {
            graphicsCommandList->Reset(graphicsCommandAllocator, graphicsPipelineState);			
			
			graphicsCommandList->ResourceBarrier(Barrier::Transition(shaderResource, ResourceState::Common, ResourceState::CopyDestination));
			graphicsCommandList->GetCopyTextureRegionHelper()
				->SetDestinationResource(shaderResource)
				->SetSourceResource(uploadHeap)
				->SetSourceSubresourceAsPlacedFootprint(shaderResourceRegion->GetCopyableLayout().layouts[0])
				->CopyTextureRegion();
			graphicsCommandList->ResourceBarrier(Barrier::Transition(shaderResource, ResourceState::CopyDestination, ResourceState::PixelShaderResource));
			
			graphicsCommandList->SetGraphicsRootSignature(graphicsRootSignature);
			graphicsCommandList->SetDescriptorHeaps({ shaderResourceDescriptorHeap, samplerDescriptorHeap });
			//broken at next line
			graphicsCommandList->SetGraphicsRootDescriptorTable(0, shaderResourceDescriptorHeap->GetGPUDescriptorHandle(0));
			graphicsCommandList->SetGraphicsRootDescriptorTable(1, samplerDescriptorHeap->GetGPUDescriptorHandle(0));

			graphicsCommandList->SetVertexBuffer(VertexBufferView(vertexBufferHeap, sizeof(float4), 4 * sizeof(float4)));
			//graphicsCommandList->SetGraphicsRootConstantBufferView(0, constantBufferView);
			
			
			graphicsCommandList->SetPrimitiveTopology(PrimitiveTopology::Trianglestrip);
			graphicsCommandList->SetViewport(RenderX(), RenderY());
			graphicsCommandList->SetScissorRect(Rect<int>(0, 0, RenderX(), RenderY()));
			graphicsCommandList->SetRenderTarget(GetCurrentFrameRenderTargetView());			
			graphicsCommandList->ClearRenderTargetView(GetCurrentFrameRenderTargetView(), GetOptimizedClearValueForRenderTargets());
			graphicsCommandList->DrawInstanced(3, 1, 0, 0);
			graphicsCommandList->Close();
        }

        void Simple_Texture::ExecuteFrame(uint frame)
        {
            defaultCommandQueue->ExecuteCommandList(graphicsCommandList);
        }

        bool Simple_Texture::IsOutputValid()
        {
			bool result = true;
			return result;
        }

        std::wstring Simple_Texture::GetTestCaseShortName()
        {
            // [AR-BEFORE-COMMIT] Short test name - identifies test case
            std::wstringstream wss;
            //wss << parameters.a << L" "
            //    << parameters.b << L" "
            //    << parameters.c;

            return wss.str();
        }

        std::wstring Simple_Texture::GetTestCaseDescription()
        {
            // [AR-BEFORE-COMMIT] Long test name - similar to Short name but add some description
            std::wstringstream wss;
            //wss << L"Test creates resources with following dimensions: "
            //    << L"W:" << parameters.a << L" "
            //    << L"H:" << parameters.b << L" "
            //    << L"D:" << parameters.c;

            return wss.str();
        }
    }
}

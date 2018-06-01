#pragma once

#include "D3D12Test.h"

namespace EnablingTests
{
    namespace D3D12
    {
        class Simple_Texture : public D3D12MultithreadedTest
        {
        public:			
            struct Parameters
            {                
            };

            static void RegisterCases(TestGroup* group);

            Simple_Texture(Parameters parameters);

            void SetupRootSignatures() override;
            void SetupPipelineStateObjects() override;
            void SetupCommandObjects() override;
            void SetupResources2() override;
            void SetupDescriptorHeaps() override;
            void SetupViewsAndSamplers() override;
            void SetupUploadHeaps() override;

            void RecordFrame(uint frame, uint workerId) override;
            void ExecuteFrame(uint frame) override;

            bool IsOutputValid() override;

            std::wstring GetTestCaseShortName() override;
            std::wstring GetTestCaseDescription() override;

        private:		
            Parameters parameters;
			const float4 constantBufferColor;

            SF2::D3D12::CommandQueue* defaultCommandQueue;
            SF2::D3D12::CommandAllocator* graphicsCommandAllocator;
            SF2::D3D12::GraphicsCommandList* graphicsCommandList;
            SF2::D3D12::RootSignature* graphicsRootSignature;
            SF2::D3D12::PipelineState* graphicsPipelineState;

            SF2::D3D12::DescriptorHeap* shaderResourceDescriptorHeap;
			SF2::D3D12::DescriptorHeap* samplerDescriptorHeap;
			SF2::D3D12::DescriptorHeap* constantBufferDescriptorHeap;

            SF2::D3D12::Resource* uploadHeap;
            SF2::D3D12::Resource* shaderResource;
			SF2::D3D12::Resource* vertexBufferHeap;
			SF2::D3D12::ConstantBufferView constantBufferView;

			std::shared_ptr<SF2::D3D12::UploadHeapRegion> vertexBufferRegion;
			std::shared_ptr<SF2::D3D12::UploadHeapRegion> shaderResourceTexture2DRegion;
			std::shared_ptr<SF2::D3D12::UploadHeapRegion> shaderResourceRegion;
			std::shared_ptr<SF2::D3D12::UploadHeapRegion> constantBufferRegion;
			
			std::unique_ptr<SF2::Texture2DData<byte4>> texture2DData;
            std::shared_ptr<SF2::D3D12::VertexBuffer> vertexBuffer;
            //std::shared_ptr<SF2::Subresource<byte4>> texture2DData; 

			SF2::D3D12::ShaderResourceView shaderResourceView;
			SF2::D3D12::Sampler sampler;
        };
    }
}

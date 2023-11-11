#include "network.h"

Network::Network() : session(nullptr) {
    Ort::SessionOptions session_options;

    OrtTensorRTProviderOptions options{};
    options.device_id = 0; // code the device id needed
    options.trt_max_workspace_size = 2ull * 1024 * 1024 * 1024; // same as default yet prevents warnings in console
    options.trt_max_partition_iterations = 1000;                // same as default yet prevents warnings in console
    options.trt_min_subgraph_size = 1;                          // same as default yet prevents warnings in console
    options.trt_engine_cache_enable = 1;
    options.trt_engine_cache_path = ".";
    session_options.AppendExecutionProvider_TensorRT(options);

    int device_id = 0;
    Ort::ThrowOnError(OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, device_id));
    
    auto model_path = "models/model.onnx";
    session = Ort::Session(env, model_path, session_options);
    forward(InputPlanes(112)); 
}

std::vector<float*> Network::forward(InputPlanes planes) {
    auto input_name0 = session.GetInputNameAllocated(0, allocator);
    auto output_name0 = session.GetOutputNameAllocated(0, allocator);
    auto output_name1 = session.GetOutputNameAllocated(1, allocator);
    auto output_name2 = session.GetOutputNameAllocated(2, allocator);

    std::vector<const char*> input_names = { input_name0.get() };
    std::vector<const char*> output_names = { output_name0.get(), output_name1.get(), output_name2.get() };

	std::vector<int64_t> input_shape = { 1, kInputPlanes, 8, 8 };
	std::vector<float> input_data(input_shape[1] * input_shape[2] * input_shape[3]);
    for (int i = 0; i < 112; i++) {
        for (int j = 0; j < 64; j++) {
            if (planes[i].mask & (1ULL << j)) {
                input_data[i * 64 + j] = 1; 
            }
        }
    }

    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info,
        input_data.data(),
        input_data.size(),
        input_shape.data(),
        input_shape.size()
    );

    std::vector<Ort::Value> output_tensors = session.Run(
        Ort::RunOptions{ nullptr },
        input_names.data(),
        &input_tensor,
        input_names.size(),
        output_names.data(),
        output_names.size());
    
    return { output_tensors[0].GetTensorMutableData<float>(), output_tensors[1].GetTensorMutableData<float>() };
}
#pragma once

#include "encoder.h"

#include <onnxruntime_cxx_api.h>

class Network {
private:
	Ort::Env env;
	Ort::Session session;
	Ort::AllocatorWithDefaultOptions allocator;
	std::vector<const char*> input_names;
	std::vector<const char*> output_names;

public:
	Network();
	float* forward(InputPlanes planes); 
};
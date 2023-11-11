cmake_minimum_required(VERSION 3.13)

project(ONNX_Runtime_Inference VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

add_executable(main main.cc search.cc node.cc encoder.cc network.cc position.cc tables.cc types.cc)
target_include_directories(main PRIVATE ${ONNX_RUNTIME_SESSION_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${ONNX_RUNTIME_LIB})
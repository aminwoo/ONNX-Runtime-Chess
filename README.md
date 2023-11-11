# ONNX-Runtime-Chess

## Building Application

```
cmake -B build
cmake --build build --config Release --parallel
```

## Building TensorRT Docker Image

```
cd docker
docker build -t onnxruntime-trt -f Dockerfile.tensorrt .
```

## Running in Docker Container

```
docker run --gpus all -it -v $(pwd):/mnt onnxruntime-trt
```
# ONNX-Runtime-Chess

## Building Application

```
cmake -B build
cmake --build build --config Release --parallel
```

Once built, the application should be in /build/src. 

## Building TensorRT Docker Image

```
cd docker
docker build -t onnxruntime-trt -f Dockerfile.tensorrt .
```

## Running in Docker Container

```
docker run --gpus all -it -v $(pwd):/mnt onnxruntime-trt
```
#pragma once

#include "position.h"
#include "types.h"

// All input planes are 64 value vectors, every element of which is either
// 0 or some value, unique for the plane. Therefore, input is defined as
// a bitmask showing where to set the value, and the value itself.
struct InputPlane {
    InputPlane() = default;
    void SetAll() { mask = ~0ull; }
    void Fill(float val) {
        SetAll();
        value = val;
    }
    std::uint64_t mask = 0ull;
    float value = 1.0f;
};
using InputPlanes = std::vector<InputPlane>;

const int kInputPlanes = 112;
constexpr int kMoveHistory = 8;
constexpr int kPlanesPerBoard = 13;
constexpr int kAuxPlaneBase = kPlanesPerBoard * kMoveHistory;

// Encodes the last position in history for the neural network request.
InputPlanes EncodePositionForNN(History& history);


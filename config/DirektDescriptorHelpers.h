#pragma once
#include "config/DirektDescriptors.h"

namespace DirektDSP
{

// Heap-wrap any descriptor into a Node (needed for recursive containers)
template <typename T>
Node node (T&& desc)
{
    return std::make_unique<NodeDescriptor> (std::forward<T> (desc));
}

} // namespace DirektDSP

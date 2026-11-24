#pragma once

#include "TaggedBlock.h"
#include <array>
#include <optional>

PSAPI_NAMESPACE_BEGIN

struct ShapeFillTaggedBlock : TaggedBlock
{
	std::optional<std::array<double, 3>> rgb;
	void read(File& document, const FileHeader& header, uint64_t offset, Signature signature,
	          Enum::TaggedBlockKey key, uint16_t padding);
};

PSAPI_NAMESPACE_END

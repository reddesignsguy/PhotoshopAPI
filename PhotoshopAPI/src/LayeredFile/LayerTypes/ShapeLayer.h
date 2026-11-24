#pragma once

#include "Macros.h"
#include "ImageLayer.h"
#include "Core/TaggedBlocks/ShapeFillTaggedBlock.h"
#include "LayeredFile/concepts.h"

PSAPI_NAMESPACE_BEGIN

/// A fill layer with a vector outline and optional cached image channels.
template <typename T>
	requires concepts::bit_depth<T>
struct ShapeLayer : ImageLayer<T>
{
	ShapeLayer(const LayerRecord& record, ChannelImageData& channels, const FileHeader& header)
		: ImageLayer<T>(record, channels, header),
		  has_pixel_bounds(record.m_Right > record.m_Left && record.m_Bottom > record.m_Top) {}

	// Layer dimensions can come from a mask even when the color channels are empty.
	bool has_pixel_bounds;
	std::optional<std::array<double, 3>> solid_fill_rgb;
};

extern template struct ShapeLayer<bpp8_t>;
extern template struct ShapeLayer<bpp16_t>;
extern template struct ShapeLayer<bpp32_t>;

PSAPI_NAMESPACE_END

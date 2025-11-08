#pragma once

#include "Macros.h"
#include "Layer.h"
#include "LayeredFile/concepts.h"

PSAPI_NAMESPACE_BEGIN

/// This struct holds no data, we just use it to identify its type
template <typename T>
	requires concepts::bit_depth<T>
struct ShapeLayer : Layer<T>
{
	ShapeLayer(){ std::cout << "making shape layer" << std::endl;} ;
	
	ShapeLayer(const LayerRecord& layer_record, ChannelImageData& channel_image_data, const FileHeader& header) : Layer<T>(layer_record, channel_image_data, header)
	{
		std::cout << "making shape layer w/ channel image data" << std::endl;
	}
};

extern template struct ShapeLayer<bpp8_t>;
extern template struct ShapeLayer<bpp16_t>;
extern template struct ShapeLayer<bpp32_t>;

PSAPI_NAMESPACE_END

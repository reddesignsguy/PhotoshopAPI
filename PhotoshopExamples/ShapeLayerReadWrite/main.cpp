/*
Example of loading a PhotoshopFile and extracting the image data, modifying this and then replacing it again
*/

#include "PhotoshopAPI.h"

#include <algorithm>
#include <execution>
#include <cmath>
#include <cstdint>
#include <vector>
#include <unordered_map>


// sRGB to linear conversion
float srgbToLinear(uint8_t srgb)
{
	float normalized = srgb / 255.0f;
	if (normalized <= 0.04045f)
		return normalized / 12.92f;
	else
		return std::pow((normalized + 0.055f) / 1.055f, 2.4f);
}

int main(int argc, char* argv[])
{
//		// In this case we already know the bit depth but otherwise one could use the PhotoshopFile.m_Header.m_Depth
//		// variable on the PhotoshopFile to figure it out programmatically. This would need to be done using the 
//		// "extended" read signature shown in the ExtendedSignature example.
//		LayeredFile<bpp8_t> layeredFile = LayeredFile<bpp8_t>::read("ImageData.psb");
//
//		// We could also use find_layer() on the LayeredFile but this way we directly get the appropriate type.
//		// Keep in mind this can return nullptr!
//		auto imageLayerPtr = find_layer_as<bpp8_t, ImageLayer>("Blue_Lagoon/Blue_Lagoon.exr", layeredFile);
//
//		// Now we can grab all channels (we could also use just grab a single channel)
//		auto channels = imageLayerPtr->get_image_data();
//
//		// Now we do our modifications. In this example we apply a sRGB -> linear operation
//		for (auto& [_, value] : channels)
//		{
//			std::for_each(std::execution::par, value.begin(), value.end(), [&](uint8_t& pixelValue) 
//				{
//					pixelValue = static_cast<uint8_t>(std::round(srgbToLinear(pixelValue) * 255.0f));
//				});
//		}
//
//	// Finally we can set the image data to the channel again and save out our file
//	imageLayerPtr->set_image_data(std::move(channels));
//	LayeredFile<bpp8_t>::write(std::move(layeredFile), "ModifiedImageData.psb");


	using namespace NAMESPACE_PSAPI;
	std::cout << "entered main method" << std::endl; 
	// In this case we already know the bit depth but otherwise one could use the PhotoshopFile.m_Header.m_Depth // variable on the PhotoshopFile to figure it out programmatically. This would need to be done using the 
	// "extended" read signature shown in the ExtendedSignature example.
	std::cout << "got arguments: " << argc << std::endl;
	std::string psd = "VectorLayer.psd";
	if (argc > 0)
	{
	psd = argv[0];
	}

	std::string layerName = "vexeo";
	if (argc > 1) {
	layerName = argv[1];
	}

	std::cout << "parsed arguments" << std::endl; 
	LayeredFile<bpp8_t> layeredFile = LayeredFile<bpp8_t>::read("VectorLayer.psd");
	std::cout << "made layeredfile" << std::endl; 
	
	// We could also use find_layer() on the LayeredFile but this way we directly get the appropriate type.
	// Keep in mind this can return nullptr!
	auto imageLayerPtr = find_layer_as<bpp8_t, ImageLayer>(layerName, layeredFile);

	std::cout << "tried to get image layer ptr" << std::endl; 
	// auto shapeLayerPtr = find_layer_as<bpp8_t, ShapeLayer>(layerName, layeredFile);
	std::cout << "tried to get shape layer ptr" << std::endl; 
	
	if (imageLayerPtr)
	{
		std::cout << "Found an image layer named " + layerName +  "!" << std::endl;
	}

//	if (shapeLayerPtr)
//	{
//		std::cout << "Found an shape layer named " + layerName +  "!" << std::endl;
//	}
	// Now we can grab all channels (we could also use just grab a single channel)
	//auto channels = imageLayerPtr->get_image_data();
//	auto r = imageLayerPtr->get_channel(0);
//	auto g = imageLayerPtr->get_channel(1);
//	auto b = imageLayerPtr->get_channel(2);
//	std::unordered_map<int, std::vector<bpp8_t>> channels;
//
//	channels[0] = r;
//	channels[1] = g;
//	channels[2] = b;
//	
//      // Now we do our modifications. In this example we apply a sRGB -> linear operation
//	for (auto& [_, value] : channels)
//	{
//		std::for_each(std::execution::par, value.begin(), value.end(), [&](uint8_t& pixelValue) 
//			{
//				pixelValue = static_cast<uint8_t>(125.0f);
//			});
//	}

	// Finally we can set the image data to the channel again and save out our file
//	imageLayerPtr->set_image_data(std::move(channels));
	std::cout << "trying to write" << std::endl; 
	LayeredFile<bpp8_t>::write(std::move(layeredFile), "Result.psd");
}

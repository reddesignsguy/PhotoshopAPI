#include "doctest.h"
#include "LayeredFile/LayeredFile.h"
#include "LayeredFile/LayerTypes/ImageLayer.h"

#include <fstream>
#include <numeric>

using namespace NAMESPACE_PSAPI;

namespace {
using Bytes = std::vector<uint8_t>;
void number(Bytes& out, uint64_t value, size_t count)
{
    for (size_t i = count; i > 0; --i) out.push_back(static_cast<uint8_t>(value >> ((i - 1) * 8)));
}
void append(Bytes& out, const Bytes& bytes) { out.insert(out.end(), bytes.begin(), bytes.end()); }
void rectangle(Bytes& out, int top, int left, int bottom, int right)
{
    for (auto value : {top, left, bottom, right}) number(out, value, 4);
}

// Encode the PSD fields independently of the library writer, which does not
// support writing the two mask rectangles together.
Bytes combinedMaskDocument(bool parameters, bool reverseChannels)
{
    Bytes mask;
    rectangle(mask, 10, 20, 12, 24);
    number(mask, 255, 1);
    number(mask, parameters ? 0x18 : 0x08, 1);
    number(mask, 3, 1); // Real mask: relative to layer and disabled.
    number(mask, 0, 1);
    rectangle(mask, -2, -1, 1, 1);
    if (parameters) {
        number(mask, 0x0f, 1); // Parameters follow the real mask header in Photoshop files.
        number(mask, 123, 1);
        number(mask, 0x4004000000000000ULL, 8); // User feather: 2.5.
        number(mask, 231, 1);
        number(mask, 0x4012000000000000ULL, 8); // Vector feather: 4.5.
    }
    while (mask.size() % 4) mask.push_back(0);
    Bytes extra;
    number(extra, mask.size(), 4);
    append(extra, mask);
    number(extra, 0, 4); // Blending ranges.
    append(extra, {4, 'b', 'o', 't', 'h', 0, 0, 0});
    append(extra, {'8','B','I','M','v','m','s','k'});
    number(extra, 8, 4);
    number(extra, 3, 4); // Vector mask version.
    number(extra, 0, 4); // Flags; empty path.

    Bytes record;
    rectangle(record, 0, 0, 4, 4);
    number(record, 5, 2);
    std::vector<int> ids{0, 1, 2, -2, -3};
    if (reverseChannels) std::swap(ids[3], ids[4]);
    Bytes pixels;
    for (int id : ids) {
        const size_t width = id == -3 ? 2 : 4;
        const size_t height = id == -2 ? 2 : id == -3 ? 3 : 4;
        number(record, id, 2);
        number(record, 2 + 4 * height, 4);
        number(pixels, 1, 2); // RLE compression.
        for (size_t row = 0; row < height; ++row) number(pixels, 2, 2);
        for (size_t row = 0; row < height; ++row) {
            number(pixels, 257 - width, 1);
            number(pixels, id == -2 ? 77 : id == -3 ? 155 : 42, 1);
        }
    }
    append(record, {'8','B','I','M','n','o','r','m',255,0,0,0});
    number(record, extra.size(), 4);
    append(record, extra);
    Bytes layers;
    number(layers, 1, 2);
    append(layers, record);
    append(layers, pixels);
    if (layers.size() % 2) layers.push_back(0);
    Bytes section;
    number(section, layers.size(), 4);
    append(section, layers);
    number(section, 0, 4); // Global mask.
    Bytes document{'8','B','P','S',0,1,0,0,0,0,0,0};
    number(document, 3, 2);
    number(document, 32, 4);
    number(document, 32, 4);
    number(document, 8, 2);
    number(document, 3, 2);
    number(document, 0, 4); // Color mode data.
    number(document, 0, 4); // Image resources.
    number(document, section.size(), 4);
    append(document, section);
    number(document, 0, 2);
    document.insert(document.end(), 32 * 32 * 3, 0);
    return document;
}
}

TEST_CASE("Read combined raster and vector masks")
{
    for (bool parameters : {false, true}) {
        for (bool reverseChannels : {false, true}) {
            CAPTURE(parameters);
            CAPTURE(reverseChannels);
            const auto bytes = combinedMaskDocument(parameters, reverseChannels);
            const auto path = std::filesystem::temp_directory_path() / "psapi-combined-masks.psd";
            struct Cleanup {
                std::filesystem::path path;
                ~Cleanup() { std::error_code error; std::filesystem::remove(path, error); }
            } cleanup{path};
            {
                std::ofstream out(path, std::ios::binary);
                out.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
                REQUIRE(out.good());
            }
            auto file = LayeredFile<bpp8_t>::read(path);
            auto layer = find_layer_as<bpp8_t, ImageLayer>("both", file);
            REQUIRE(layer);
            REQUIRE(layer->has_mask());
            CHECK(layer->get_mask() == std::vector<uint8_t>(6, 155));
            CHECK(layer->mask_width() == 2);
            CHECK(layer->mask_height() == 3);
            CHECK(layer->mask_position().x == 0);
            CHECK(layer->mask_position().y == -0.5);
            CHECK(layer->mask_disabled());
            CHECK(layer->mask_relative_to_layer());
            CHECK(layer->mask_default_color() == 0);
            CHECK(layer->mask_density() == (parameters ? std::optional<uint8_t>(123) : std::nullopt));
            CHECK(layer->mask_feather() == (parameters ? std::optional<double>(2.5) : std::nullopt));
            REQUIRE(layer->m_vectorMask);
            const auto image = layer->get_image_data();
            // The public image API includes the raster mask under normalized ID -2.
            REQUIRE(image.size() == 4);
            CHECK(image.at(-2) == std::vector<uint8_t>(6, 155));
            CHECK(image.count(-3) == 0);
            for (int id : {0, 1, 2}) CHECK(image.at(id) == std::vector<uint8_t>(16, 42));
        }
    }
}

TEST_CASE("Read Photoshop raster and vector mask fixture")
{
    auto file = LayeredFile<bpp8_t>::read("documents/Masks/RasterAndVector.psd");
    auto layer = find_layer_as<bpp8_t, ImageLayer>("targets/Folder 1/Layer 3", file);
    REQUIRE(layer);
    REQUIRE(layer->m_vectorMask);
    REQUIRE(layer->has_mask());
    CHECK(layer->mask_width() == 82);
    CHECK(layer->mask_height() == 89);
    const auto mask = layer->get_mask();
    CHECK(mask.size() == 82 * 89);
    // Independently decoded from the fixture's PackBits rows.
    CHECK(std::accumulate(mask.begin(), mask.end(), uint64_t{0}) == 891309);
    CHECK(layer->mask_position().x == 342);
    CHECK(layer->mask_position().y == 124.5);
    CHECK(layer->mask_density() == std::optional<uint8_t>(255));
    CHECK(layer->mask_feather() == std::optional<double>(0.0));
    const auto image = layer->get_image_data();
    CHECK(image.count(-3) == 0);
    CHECK(image.at(-2) == mask);
    for (int id : {-1, 0, 1, 2}) CHECK(image.at(id).size() == 67 * 48);
}

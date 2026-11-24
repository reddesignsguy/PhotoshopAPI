#include "ShapeFillTaggedBlock.h"
#include "Core/FileIO/Read.h"
#include "Core/Struct/DescriptorStructure.h"

PSAPI_NAMESPACE_BEGIN

void ShapeFillTaggedBlock::read(File& document, const FileHeader& header, uint64_t offset,
                                Signature signature, Enum::TaggedBlockKey key, uint16_t padding)
{
	TaggedBlock::read(document, header, offset, signature, key, padding);
	const auto end = document.get_offset();
	const auto start = offset + 12; // signature, key, and 32-bit length
	if (end < start + (key == Enum::TaggedBlockKey::vecStrokeContentData ? 8 : 4)) return;
	document.setOffset(start);
	const bool solid = key == Enum::TaggedBlockKey::adjSolidColor ||
	                   Signature(ReadBinaryData<uint32_t>(document)) == Signature("SoCo");
	if (solid && ReadBinaryData<uint32_t>(document) == 16)
	{
		Descriptors::Descriptor fill;
		fill.read(document);
		if (fill.contains("Clr "))
		{
			const auto* color = fill.at<Descriptors::Descriptor>("Clr ");
			if (color->contains("Rd  ") && color->contains("Grn ") && color->contains("Bl  "))
				rgb = {color->at<double>("Rd  "), color->at<double>("Grn "), color->at<double>("Bl  ")};
		}
	}
	document.setOffset(end);
}

PSAPI_NAMESPACE_END

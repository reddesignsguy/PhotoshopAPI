#include "Core/TaggedBlocks/VectorMaskTaggedBlock.h"
#include "Core/FileIO/LengthMarkers.h"

// using this function assumes that a vector mask tagged block does indeed exist at the current document position
void PhotoshopAPI::VectorMaskTaggedBlock::read(File& document, const FileHeader& header, const uint64_t offset, const Enum::TaggedBlockKey key, const Signature signature, const uint16_t padding)
{
	m_Key = key;
	m_Offset = offset;
	m_Signature = signature;
	
	const auto length = ReadBinaryData<uint32_t>(document);
	const auto start = document.get_offset();
	m_Length = length;
	if (length < 8)
		throw std::runtime_error("Vector mask is missing its version and flags");

	m_version = ReadBinaryData<uint32_t>(document);
	m_flag = ReadBinaryData<uint32_t>(document);
	m_pathResourceData = std::make_unique<PathResourceData>();
	const auto records = (length - 8) / 26;
	for (uint32_t i = 0; i < records; ++i)
		m_pathResourceData->read(document, padding);

	document.setOffset(start + RoundUpToMultiple<uint64_t>(length, padding));
}

void PhotoshopAPI::VectorMaskTaggedBlock::write(File& document, const FileHeader& header, ProgressCallback& callback, const uint16_t padding)
{
	WriteBinaryData<uint32_t>(document, Signature("8BIM").m_Value);
	WriteBinaryData<uint32_t>(document, Signature(m_Key == Enum::TaggedBlockKey::vecMaskSettingsCS6 ? "vsms" : "vmsk").m_Value);
	Impl::ScopedLengthBlock<uint32_t> len_block(document, 4u);
	WriteBinaryData<uint32_t>(document, m_version);
	WriteBinaryData<uint32_t>(document, m_flag);
	
	m_pathResourceData->write(document);

}

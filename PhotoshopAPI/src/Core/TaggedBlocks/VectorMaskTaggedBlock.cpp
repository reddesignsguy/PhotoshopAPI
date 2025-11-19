#include "Core/TaggedBlocks/VectorMaskTaggedBlock.h"
#include "Core/FileIO/LengthMarkers.h"

// using this function assumes that a vector mask tagged block does indeed exist at the current document position
void PhotoshopAPI::VectorMaskTaggedBlock::read(File& document, const FileHeader& header, const uint64_t offset, const Enum::TaggedBlockKey key, const Signature signature, const uint16_t padding)
{
	m_Key = key;
	m_Offset = offset;
	m_Signature = signature;
	
	m_Length = ReadBinaryData<uint32_t>(document);
	auto len_offset = document.get_offset();

	m_version = ReadBinaryData<uint32_t>(document);
	std::cout << m_version << " was the version" << std::endl;
	m_flag = ReadBinaryData<uint32_t>(document); // TODO: Read this properly!!

	std::cout << m_flag << " was the flag" << std::endl;
	// read all the records
	m_pathResourceData = std::make_unique<PathResourceData>();
	uint32_t lengthBeforePathRecords = 8; // is the length of the version and flag as seen in the above 2 lines
	int numRecords = std::visit([&](auto &&length) {
		int tmp = static_cast<int>((length - lengthBeforePathRecords) / 26); 
		std::cout << length << " was the total length of the section" << std::endl;
		std::cout << "length - 8 = " << length - lengthBeforePathRecords << std::endl;
		std::cout << "(length - 8) / 26 = " << tmp << std::endl;
		return tmp;
							 }, m_Length);

	std::cout << numRecords << " path resource records found!" << std::endl;
	for (int i = 0; i < numRecords; i ++)
	{
		m_pathResourceData->read(document, padding);
	}

	// Manually skip to the end as this section may be padded
	document.setOffset(len_offset + std::get<uint32_t>(m_Length));
}

void PhotoshopAPI::VectorMaskTaggedBlock::write(File& document, const FileHeader& header, ProgressCallback& callback, const uint16_t padding)
{
	WriteBinaryData<uint32_t>(document, Signature("8BIM").m_Value);
	WriteBinaryData<uint32_t>(document, Signature("vmsk").m_Value);
	Impl::ScopedLengthBlock<uint32_t> len_block(document, 4u);
	WriteBinaryData<uint32_t>(document, m_version);
	WriteBinaryData<uint32_t>(document, m_flag);
	
	// the meat of vector masks	
	for (auto& resource : m_pathResourceData->m_records)
	{
		// TODO: IMPLEMENT ME
		// resource.write(document);
	}

	// verify 1st block is pathfill
	// second block is a subpath length record, clipboard record, or initial fill record
	// subpath length records that says there are x knot records after this
	// verify that there are x knot records after this
	// we must ensure that all knot records have a "parent" subpath length record
}

 #include "VectorMaskTaggedBlock.h"

void VectorMaskUtils::readPathResources(File& document)
{
	// verify 1st block is pathfill
	// second block is a subpath length record, clipboard record, or initial fill record
	// subpath length records that says there are x knot records after this
	// verify that there are x knot records after this
	// we must ensure that all knot records have a "parent" subpath length record
	auto flags = ReadBinaryData<uint32_t>(document);
	m_IsLocked = flags & 128u;	// Check if the flags at bit 7 are true
}

// Read the next 26 bytes (assumes that we have space)
void VectorMaskUtils::readPathResource(File& document)
{
//			ClosedSubpathLengthRecord        = 0,
//			ClosedSubpathBezierKnotLinked    = 1,
//			ClosedSubpathBezierKnotUnlinked  = 2,
//			OpenSubpathLengthRecord          = 3,
//			OpenSubpathBezierKnotLinked      = 4,
//			OpenSubpathBezierKnotUnlinked    = 5,
//			PathFillRuleRecord               = 6,
//			ClipboardRecord                  = 7,
//			InitialFillRuleRecord            = 8
	// Get record type from first 2 bytes
	uint16_t type = ReadBinaryData<uint16_t>(document);
	switch (type)
	{
		// Create a factory that takes in the type and spits out a constructor
		case PathRecord::Selector::PathFillRuleRecord 
	};
	// TODO: Address this concern from spec: For Windows, you should swap the bytes before accessing it as a short.)
	// Based on record type.. branch out	
}

void VectorMaskUtils::parsePathResourceType(File& document)
{

}



void VectorMaskTaggedBlock::read(File& document, const FileHeader& header, const uint64_t offset, const Enum::TaggedBlockKey key, const Signature signature, const uint16_t padding)
{
	m_Key = key;
	m_Offset = offset;
	m_Signature = signature;

	m_Length = ReadBinaryData<uint32_t>(document);
	auto len_offset = document.get_offset();

	m_version = ReadBinaryData<uint32_t>(document);
	m_flag = ReadBinaryData<uint32_t>(document); // TODO: Read this properly!!!
	
	

	if (m_Version != 4 || descriptorVersion != 16)
	{
		PSAPI_LOG_ERROR("PlacedLayerData", "Unknown version or descriptor version encountered. Version: %d. Descriptor Version: %d. Expected 4 and 16 for these respectively", m_Version, descriptorVersion);
	}

	m_Descriptor = std::make_unique<Descriptors::Descriptor>();
	m_Descriptor->read(document);
	// Manually skip to the end as this section may be padded
	document.setOffset(len_offset + std::get<uint32_t>(m_Length));
}

void VectorMaskTaggedBlock::write(File& document, const FileHeader& header, ProgressCallback& callback, const uint16_t padding)
{
	WriteBinaryData<uint32_t>(document, Signature("8BIM").m_Value);
	WriteBinaryData<uint32_t>(document, Signature("vmsk").m_Value);
	Impl::ScopedLengthBlock<uint32_t> len_block(document, 4u);
	WriteBinaryData<uint32_t>(document, m_version);
	WriteBinaryData<uint32_t>(document, m_flag);
	
	// the meat of vector masks	
	for (auto& resource : m_pathResources)
	{
		resource.write(document);
	}

	// verify 1st block is pathfill
	// second block is a subpath length record, clipboard record, or initial fill record
	// subpath length records that says there are x knot records after this
	// verify that there are x knot records after this
	// we must ensure that all knot records have a "parent" subpath length record
}

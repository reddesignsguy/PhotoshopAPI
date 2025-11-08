#include "VectorMaskTaggedBlock.h"


void VectorMaskTaggedBlock::write(File& document, const FileHeader& header, ProgressCallback& callback, const uint16_t padding = 1u) override
{
	WriteBinaryData<uint32_t>(document, Signature("8BIM").m_Value);
	WriteBinaryData<uint32_t>(document, Signature(m_VectorMaskKey).m_Value);
	Impl::ScopedLengthBlock<uint32_t> len_block(document, 4u);
	// write length	
	for (auto& resource : m_pathResources)
	{
		resource.write(document);
	}
	
//	// The regular lnk2 block is 8 bytes in psb while lnkE is always 4 bytes.
//	if (is_linked_externally)
//	{
//		Impl::ScopedLengthBlock<uint32_t> len_block(document, 4u);
//		for (auto& item : m_LayerData)
//		{
//			item.write(document);
//		}
//	}
//	else
//	{
//		Impl::ScopedLengthBlock<Impl::VariadicSize<uint32_t, uint64_t>> len_block(document, header, 4u);
//		for (auto& item : m_LayerData)
//		{
//			item.write(document);
//		}
//	}
}

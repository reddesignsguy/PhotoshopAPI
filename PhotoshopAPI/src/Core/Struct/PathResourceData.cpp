#include "Core/Struct/PathResourceData.h"

PSAPI_NAMESPACE_BEGIN

// PathRecord::PathRecord(Selector selector) // TODO rename to BasePathRecord
// 	{
// 		m_selector = static_cast<uint16_t>(selector);
// 	}	
// 		
//void PathRecord::write(File& document) const
//{
//
//};

//		template <typename T>
//		void insertData(T data)
//		{
//			std::memcpy(m_selector.data() + m_dataOffset, &data, sizeof(T));
//			m_dataOffset += sizeof(T);
//		}
//		
//		// Byte-level info:
//		// Each record is 26 bytes. Selector is 2 bytes so the rest of the data is 24
//		uint16_t m_selector;
//		std::array<uint8_t, 24> m_data = {};

// using Point2D = Geometry::Point2D<float>;

// TODO: We can migrate these for the user facing interface
//BezierKnotRecord::BezierKnotRecord(bool closed,
//			 bool linked,
//			 Point2D precedingControlPoint,
//			 Point2D anchorPoint,
//			 Point2D leavingControlPoint)
//				: PathRecord(closed
//					? (linked ? ClosedSubpathBezierKnotLinked: ClosedSubpathBezierKnotUnlinked)
//					: (linked ? OpenSubpathBezierKnotLinked: OpenSubpathBezierKnotUnlinked))
//	{
//		
//
//		insertData(static_cast<uint32_t>(precedingControlPoint.y * (1 << m_binaryPoint)));
//		insertData(static_cast<uint32_t>(precedingControlPoint.x * (1 << m_binaryPoint)));
//
//		insertData(static_cast<uint32_t>(anchorPoint.y * (1 << m_binaryPoint)));
//		insertData(static_cast<uint32_t>(anchorPoint.x * (1 << m_binaryPoint)));
//
//
//		insertData(static_cast<uint32_t>(leavingControlPoint.y * (1 << m_binaryPoint)));
//		insertData(static_cast<uint32_t>(leavingControlPoint.x * (1 << m_binaryPoint)));	
//	}
//
//
//SubpathLengthRecord::SubpathLengthRecord(bool closed, int numPoints)
//				: PathRecord(closed ? ClosedSubpathLengthRecord : OpenSubpathLengthRecord)
//{
//	insertData(static_cast<uint16_t>(numPoints));
//}
void skipBytes(File& document, int skipNumBytes) // TODO: make this a util
{
	for (int i = 0; i < skipNumBytes; i++)
	{
		ReadBinaryData<uint8_t>(document);
	}
}

void PathResourceData::read(File& document, const uint8_t padding)
{
	std::cout << "reading path resource" << std::endl;
	uint16_t selector = ReadBinaryData<uint16_t>(document);
	std::shared_ptr<IPathRecord> record;
	if (selector == 6) // path fill rule record
	{
		std::cout << "path fill beizer knot" << std::endl;
		record = std::make_shared<PathFillRecord>();
		skipBytes(document, 24);
	}
	else if (selector == 0 || selector == 3) //  subpath length record (open or closed)
	{
		std::cout << "subpath length record" << std::endl;
		uint16_t length = ReadBinaryData<uint16_t>(document);
		bool closed = selector == 0; 
		record = std::make_shared<SubpathLengthRecord>(closed, length);
		skipBytes(document, 22);
	}
	else if (selector == 1 || selector == 2 || selector == 4 || selector == 5) // subpath bezier knot record (closed, open, linked, and unlinked)
	{
		// TODO: Forgot that y-component is first (apatriawan)
		// Change !!
		Point2D preceding;
	       	preceding.y = ReadBinaryData<uint32_t>(document);
	       	preceding.x = ReadBinaryData<uint32_t>(document);
		double tmpX = ((preceding.x << 3) >> 3) / (double(1 << 24));
		double tmpY = ((preceding.y << 3) >> 3) / (double(1 << 24));

		std::cout << "preceding coords: " << preceding.x << ", "<< preceding.y << std::endl;
		std::cout << "preceding coords: " << tmpX << ", "<< tmpY << std::endl;
		Point2D anchor;
	       	anchor.y = ReadBinaryData<uint32_t>(document);
	       	anchor.x = ReadBinaryData<uint32_t>(document);
		std::cout << "anchor coords: " << anchor.x << ", "<< anchor.y << std::endl;

		Point2D leaving;
	       	leaving.y = ReadBinaryData<uint32_t>(document);
	       	leaving.x = ReadBinaryData<uint32_t>(document);
		std::cout << "leaving coords: " << leaving.x << ", "<< leaving.y << std::endl;

		switch (selector)
		{
			case 1:
				std::cout << "Closed and linked beizer knot" << std::endl;
				record = std::make_shared<BezierKnotRecord>(true, true,   preceding, anchor, leaving);
				break;
			case 2:
				std::cout << "Closed and unlinked beizer knot" << std::endl;
				record = std::make_shared<BezierKnotRecord>(true, false,  preceding, anchor, leaving);
				break;
			case 4:
				std::cout << "Open and linked beizer knot" << std::endl;
				record =std::make_shared<BezierKnotRecord>(false, true,  preceding, anchor, leaving);
				break;
			case 5:
				std::cout << "Open and unlinked beizer knot" << std::endl;
				record = std::make_shared<BezierKnotRecord>(false, false, preceding, anchor, leaving);
				break;
		}
	}
	else if (selector == 7) // clipboard record
	{
		std::cout << "clipboard fill record" << std::endl;
		// TODO implement me
		throw std::runtime_error("reading clipboard records not implemented yet!");
	}
	else if (selector == 8) // initial fill rule record
	{
		std::cout << "initial fill record" << std::endl;
		bool startWithAllPixels = ReadBinaryData<uint16_t>(document) == 1;
		record = std::make_shared<InitialFillRecord>(startWithAllPixels);
		skipBytes(document, 22);
	}
	else
	{
		// TODO: Throw error. Unexpected selector type
		throw std::runtime_error("Unexpected path record type!");
	}
	
	m_records.push_back(record);
}

void BezierKnotRecord::write(File& document) const
{
	if (m_closed && m_linked) {
	    WriteBinaryData<uint16_t>(document, 1);
	}
	else if (m_closed && !m_linked) {
	    WriteBinaryData<uint16_t>(document, 2);
	}
	else if (!m_closed && m_linked) {
	    WriteBinaryData<uint16_t>(document, 4);
	}
	else {
	    WriteBinaryData<uint16_t>(document, 5);
	}
	
	WriteBinaryData<uint32_t>(document, m_preceding.y);
	WriteBinaryData<uint32_t>(document, m_preceding.x);
	WriteBinaryData<uint32_t>(document, m_anchor.y);
	WriteBinaryData<uint32_t>(document, m_anchor.x);
	WriteBinaryData<uint32_t>(document, m_leaving.y);
	WriteBinaryData<uint32_t>(document, m_leaving.x);
};

void SubpathLengthRecord::write(File& document) const
{
	uint32_t selector = m_closed ? 0 : 3;
	WriteBinaryData<uint16_t>(document, selector);
	WriteBinaryData<uint16_t>(document, m_numPoints);
	WritePadddingBytes(document, 22);
};

void PathFillRecord::write(File& document) const
{
	WriteBinaryData<uint16_t>(document, 6);
	WritePadddingBytes(document, 24);
};
void ClipboardRecord::write(File& document) const
{
};
void InitialFillRecord::write(File& document) const
{
	WriteBinaryData<uint16_t>(document, 8);
	uint16_t startWithAllPixels = m_startWithAllPixels ? 1 : 0; // TODO simplify?
	WriteBinaryData<uint16_t>(document, startWithAllPixels);
	WritePadddingBytes(document, 22);
};

/// Write the stored UTF16 string to disk with a 4-byte length with the padding
/// defined in the constructor
void PathResourceData::write(File& document) const
{
	// verify 1st block is pathfill
	// second block is a subpath length record, clipboard record, or initial fill record
	// subpath length records that says there are x knot records after this
	// verify that there are x knot records after this
	// we must ensure that all knot records have a "parent" subpath length record
	// Go through every record, and tell them to write
	for (int i = 0; i < m_records.size(); i++)
	{
		std::cout << ".. writing record" << std::endl;
		std::shared_ptr resource = m_records[i];
		// TODO: IMPLEMENT ME
		resource->write(document);
	}
}
PSAPI_NAMESPACE_END


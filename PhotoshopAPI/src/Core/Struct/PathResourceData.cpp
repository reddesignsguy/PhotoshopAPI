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


void PathResourceData::read(File& document, const uint8_t padding)
{
	uint16_t selector = ReadBinaryData<uint16_t>(document);
	IPathRecord record;
	if (selector == 6) // path fill rule record
	{
		record = PathFillRecord();
	}
	else if (selector == 0 || selector == 3) //  subpath length record (open or closed)
	{
		uint16_t length = ReadBinaryData<uint16_t>(document);
		bool closed = selector == 0; 
		record = SubpathLengthRecord(closed, length);
	}
	else if (selector == 1 || selector == 2 || selector == 4 || selector == 5) // subpath bezier knot record (closed, open, linked, and unlinked)
	{
		Point2D preceding;
	       	preceding.x = ReadBinaryData<uint32_t>(document);
	       	preceding.y = ReadBinaryData<uint32_t>(document);

		Point2D anchor;
	       	anchor.x = ReadBinaryData<uint32_t>(document);
	       	anchor.y = ReadBinaryData<uint32_t>(document);

		Point2D leaving;
	       	leaving.x = ReadBinaryData<uint32_t>(document);
	       	leaving.y = ReadBinaryData<uint32_t>(document);

		switch (selector)
		{
			case 1:
				record = BezierKnotRecord(true, true,   preceding, anchor, leaving);
				break;
			case 2:
				record = BezierKnotRecord(true, false,  preceding, anchor, leaving);
				break;
			case 4:
				record = BezierKnotRecord(false, true,  preceding, anchor, leaving);
				break;
			case 5:
				record = BezierKnotRecord(false, false, preceding, anchor, leaving);
				break;
		}
	}
	else if (selector == 7) // clipboard record
	{
		// TODO implement me
		throw std::runtime_error("reading clipboard records not implemented yet!");
	}
	else if (selector == 8) // initial fill rule record
	{
		bool startWithAllPixels = ReadBinaryData<uint16_t>(document) == 1;
		record = InitialFillRecord(startWithAllPixels);
	}
	else
	{
		// TODO: Throw error. Unexpected selector type
		throw std::runtime_error("Unexpected path record type!");
	}
	
	m_records.push_back(record);
}

/// Write the stored UTF16 string to disk with a 4-byte length with the padding
/// defined in the constructor
void PathResourceData::write(File& document) const
{
	// Go through every record, and tell them to write
}
PSAPI_NAMESPACE_END


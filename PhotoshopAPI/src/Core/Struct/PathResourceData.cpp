#include "Core/Struct/PathResourceData.h"

PSAPI_NAMESPACE_BEGIN

PathRecord::PathRecord(Selector selector) // TODO rename to BasePathRecord
	{
		m_selector = static_cast<uint16_t>(selector);
	}	
		
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

BezierKnotRecord::BezierKnotRecord(bool closed,
			 bool linked,
			 Point2D precedingControlPoint,
			 Point2D anchorPoint,
			 Point2D leavingControlPoint)
				: PathRecord(closed
					? (linked ? ClosedSubpathBezierKnotLinked: ClosedSubpathBezierKnotUnlinked)
					: (linked ? OpenSubpathBezierKnotLinked: OpenSubpathBezierKnotUnlinked))
	{
		insertData(static_cast<uint32_t>(precedingControlPoint.y * (1 << m_binaryPoint)));
		insertData(static_cast<uint32_t>(precedingControlPoint.x * (1 << m_binaryPoint)));

		insertData(static_cast<uint32_t>(anchorPoint.y * (1 << m_binaryPoint)));
		insertData(static_cast<uint32_t>(anchorPoint.x * (1 << m_binaryPoint)));


		insertData(static_cast<uint32_t>(leavingControlPoint.y * (1 << m_binaryPoint)));
		insertData(static_cast<uint32_t>(leavingControlPoint.x * (1 << m_binaryPoint)));	
	}


SubpathLengthRecord::SubpathLengthRecord(bool closed, int numPoints)
				: PathRecord(closed ? ClosedSubpathLengthRecord : OpenSubpathLengthRecord)
{
	insertData(static_cast<uint16_t>(numPoints));
}


PSAPI_NAMESPACE_END


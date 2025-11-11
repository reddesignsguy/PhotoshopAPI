#pragma once

#include "Macros.h"
#include "Core/FileIO/Read.h"
#include "Core/Struct/Section.h"
#include "Core/Struct/File.h"
#include "Core/Geometry/Point.h"
#include "simdutf.h"

#include <string>

PSAPI_NAMESPACE_BEGIN



using Point2D = Geometry::Point2D<double>;

class PathRecord
{

	public:
       		void write(File& document) const{};
		enum Selector {
			ClosedSubpathLengthRecord        = 0,
			ClosedSubpathBezierKnotLinked    = 1,
			ClosedSubpathBezierKnotUnlinked  = 2,
			OpenSubpathLengthRecord          = 3,
			OpenSubpathBezierKnotLinked      = 4,
			OpenSubpathBezierKnotUnlinked    = 5,
			PathFillRuleRecord               = 6,
			ClipboardRecord                  = 7,
			InitialFillRuleRecord            = 8
		};
	protected:
		PathRecord(Selector selector);
		const int m_binaryPoint = 24;

		template <typename T>
		void insertData(T data)
		{
			std::memcpy(m_data.data() + m_dataOffset, &data, sizeof(T));
			m_dataOffset += sizeof(T);
		}
	private:
		
		// Byte-level info:
		// Each record is 26 bytes. Selector is 2 bytes so the rest of the data is 24
		uint16_t m_selector;
		std::array<uint8_t, 24> m_data = {};
		size_t m_dataOffset=0;
};

class BezierKnotRecord : PathRecord
{
//	ClipboardRecord                  
//	InitialFillRuleRecord            

	BezierKnotRecord(bool closed,
			 bool linked,
			 Point2D precedingControlPoint,
			 Point2D anchorPoint,
			 Point2D leavingControlPoint);
};


class SubpathLengthRecord : PathRecord
{
	SubpathLengthRecord(bool closed, int numPoints);
};

//class PathFillRecord : PathRecord
//{
//	PathFillRecord() : PathRecord(PathFillRecord)
//	{
//	}
//};

/// PathResource format is a block of 26-bytte path point record
/// length should always be a multiple of 26 
/// bytes we must multiply by 2).
struct PathResourceData : public FileSection
{
	/// Initialize an empty section.
	// PathResourceData() { FileSection::size(4u);}; // needed?
	
	std::vector<PathRecord> m_records;
	/// Construct a UnicodeString from the given UTF8 encoded string aligning 
	/// the section to the given padding.
//	UnicodeString(std::string str, const uint8_t padding);
	

	/// Read a Photoshop UnicodeString struct storing both the UTF8 and UTF16LE
	/// representation
	void read(File& document, const uint8_t padding);

	/// Write the stored UTF16 string to disk with a 4-byte length with the padding
	/// defined in the constructor
	void write(File& document) const;
private:

	uint8_t m_Padding = 1u;
};


PSAPI_NAMESPACE_END

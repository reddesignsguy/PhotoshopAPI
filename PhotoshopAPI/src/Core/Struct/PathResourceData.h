#pragma once

#include "Macros.h"
#include "Core/FileIO/Read.h"
#include "Core/FileIO/Write.h"
#include "Core/Struct/Section.h"
#include "Core/Struct/File.h"
#include "Core/Geometry/Point.h"
#include "simdutf.h"

#include <string>

PSAPI_NAMESPACE_BEGIN



using Point2D = Geometry::Point2D<uint32_t>;

//class PathRecord
//{
//
//	public:
//       		void write(File& document) const{};
//		enum Selector {
//			ClosedSubpathLengthRecord        = 0,
//			ClosedSubpathBezierKnotLinked    = 1,
//			ClosedSubpathBezierKnotUnlinked  = 2,
//			OpenSubpathLengthRecord          = 3,
//			OpenSubpathBezierKnotLinked      = 4,
//			OpenSubpathBezierKnotUnlinked    = 5,
//			PathFillRuleRecord               = 6,
//			ClipboardRecord                  = 7,
//			InitialFillRuleRecord            = 8
//		};
//		void write(File& document) const final
//		{
//			
//		}
//	protected:
//		const int m_binaryPoint = 24;
//
//		template <typename T>
//		void insertData(T data)
//		{
//			std::memcpy(m_data.data() + m_dataOffset, &data, sizeof(T));
//			m_dataOffset += sizeof(T);
//		}
//
//
//		template <typename T>
//		virtual void (T data)
//	private:
//		
//		// Byte-level info:
//		// Each record is 26 bytes. Selector is 2 bytes so the rest of the data is 24
//		uint16_t m_selector;
//		std::array<uint8_t, 24> m_data = {};
//		size_t m_dataOffset=0;
//};

// TODO
//	ClipboardRecord                  
//	InitialFillRuleRecord            

struct IPathRecord
{
	virtual ~IPathRecord(){};
	virtual void write(File& document) const = 0;
};

struct BezierKnotRecord : IPathRecord
{
	BezierKnotRecord(bool closed,
			 bool linked,
			 Point2D preceding, Point2D anchor, Point2D leaving)
			 : m_closed(closed), m_linked(linked), m_preceding(preceding), m_anchor(anchor), m_leaving(leaving)
	{};

	bool m_closed;
	bool m_linked;
	Point2D m_preceding;
	Point2D m_anchor;
	Point2D m_leaving;
	uint32_t // used for converting to PathPoint
	void write(File& document) const override;
};


struct SubpathLengthRecord : IPathRecord
{
	SubpathLengthRecord(bool closed, int numPoints): m_closed(closed), m_numPoints(numPoints){};
	bool m_closed;
	int m_numPoints;
	void write(File& document) const override;
};


struct PathFillRecord : IPathRecord
{
	PathFillRecord(){}; 
	void write(File& document) const override;
};

struct ClipboardRecord : IPathRecord
{
// TODO implement me	
	void write(File& document) const override;
};

struct InitialFillRecord : IPathRecord
{
	InitialFillRecord(bool startWithAllPixels): m_startWithAllPixels(startWithAllPixels){};
	bool m_startWithAllPixels;
	void write(File& document) const override;
};

/// PathResource format is a block of 26-bytte path point record
/// length should always be a multiple of 26 
/// bytes we must multiply by 2).
struct PathResourceData : public FileSection
{
	/// Initialize an empty section.
	PathResourceData() { // FileSection::size(4u); needed?
			     }; 
	
	// TODO: Privatize me and refactor
	std::vector<std::shared_ptr<IPathRecord>> m_records;

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

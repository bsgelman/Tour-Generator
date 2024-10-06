#ifndef GEODB_H
#define GEODB_H

#include "base_classes.h"
#include "hashmap.h"

class GeoDatabase : public GeoDatabaseBase
{
public:
	GeoDatabase();
	virtual ~GeoDatabase();
	virtual bool load(const std::string& map_data_file);
	virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;
	virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
	virtual std::string get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const;
private:
	struct StreetSegment {
		StreetSegment(std::string name, const GeoPoint& start, const GeoPoint& end) : m_streetName(name), m_start(start), m_end(end) {}

		std::string m_streetName;
		GeoPoint m_start;
		GeoPoint m_end;
	};

	HashMap<StreetSegment> streetMap; // string is the start/end Lon/Lat line (the two GeoPoints), mapped to a StreetSegment with it's streetname (get_street_name)
	HashMap<std::vector<GeoPoint>> connectedPoints; // string is a GeoPoint, mapped to all connected GeoPoints (get_connected_points)
	HashMap<GeoPoint> poiMap; // POI is the string, mapped to it's GeoPoint (get_poi_location)	
};


#endif
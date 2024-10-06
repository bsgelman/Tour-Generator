#ifndef ROUTER_H
#define ROUTER_H

#include "base_classes.h"
#include "hashmap.h"
#include "geotools.h"

class Router : public RouterBase
{
public:
	Router(const GeoDatabaseBase& geo_db);
	virtual ~Router() {}
	virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;
private:
	struct Cost {
		Cost(const GeoPoint& loc, const double& cost) : location(loc), fCost(cost) {}

		GeoPoint location;
		double fCost;
	};

	struct CompareCosts {
		bool operator()(const Cost& a, const Cost& b) const {
			return a.fCost > b.fCost; // Smaller fCost has higher priority
		}
	};

	const GeoDatabaseBase* geodbPtr;
};

#endif
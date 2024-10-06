#include <queue>
#include "router.h"

Router::Router(const GeoDatabaseBase& geo_db)
    : geodbPtr(&geo_db) {
}

std::vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const {
    std::priority_queue<Cost, std::vector<Cost>, CompareCosts> open;
    HashMap<GeoPoint> closed;

    HashMap<GeoPoint> locationOfPreviousWayPoint; // string is GeoPoint lat and lon, mapped to another GeoPoint	

    open.push(Cost(pt1, distance_earth_km(pt1, pt2)));

    while (!open.empty()) {
        Cost current = open.top();
        open.pop();

        if (current.location.sLatitude == pt2.sLatitude && current.location.sLongitude == pt2.sLongitude) {
            std::vector<GeoPoint> path;
            GeoPoint prev = pt2;
            while (prev.sLatitude != pt1.sLatitude || prev.sLongitude != pt1.sLongitude) {
                path.push_back(prev);
                std::string key = prev.sLatitude + " " + prev.sLongitude;
                const GeoPoint* ptr = locationOfPreviousWayPoint.find(key);
                prev = *ptr;
            }
            path.push_back(pt1);
            std::reverse(path.begin(), path.end());
            return path;
        }

        std::vector<GeoPoint> neighbors = geodbPtr->get_connected_points(current.location);
        for (size_t i = 0; i < neighbors.size(); i++) {
            std::string key = neighbors[i].sLatitude + " " + neighbors[i].sLongitude;
            GeoPoint* ptr = closed.find(key);
            if (ptr != nullptr) {
                continue; // already visited this neighbor
            }
            std::string streetName = geodbPtr->get_street_name(current.location, neighbors[i]);
            /*if (streetName == "Medical Plaza Driveway") {
                continue;
            }*/
            closed.insert(key, neighbors[i]);
            double g = distance_earth_km(pt1, neighbors[i]);
            double h = distance_earth_km(neighbors[i], pt2);
            open.push(Cost(neighbors[i], g + h));
            locationOfPreviousWayPoint.insert(key, current.location);
        }
    }

    return std::vector<GeoPoint>();
}
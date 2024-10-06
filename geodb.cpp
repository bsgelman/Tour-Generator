#include <iostream>
#include <fstream>
#include <sstream>  // needed in addition to <iostream> for string stream I/O
#include "geodb.h"
#include "geotools.h"
using namespace std;

GeoDatabase::GeoDatabase() {

}


GeoDatabase::~GeoDatabase() {

}


bool GeoDatabase::load(const string& map_data_file) {
	ifstream infile(map_data_file);    // infile is a name of our choosing
	if (!infile)		        // Did opening the file fail?
	{
		cerr << "Error: Cannot open map_data_file!" << endl;
		return false;
	}
	// Read each line.  The return value of getline is treated
	// as true if a line was read, false otherwise (e.g., because
	// the end of the file was reached).
	string line;
	while (getline(infile, line)) {
        string streetName;
        string startLat;
        string startLon;
        string endLat;
        string endLon;

        istringstream iss1(line);
        int p; // points of interest

        streetName = line; // read street name

        // coordinates processing
        getline(infile, line);
        istringstream iss2(line);
        if (!(iss2 >> startLat >> startLon >> endLat >> endLon)) {
            cout << "Ignoring badly-formatted input line: " << line << endl;
            continue;
        }
        string key = startLat + " " + startLon + " " + endLat + " " + endLon; // USE LATER
        string reverseKey = endLat + " " + endLon + " " + startLat + " " + startLon; // USE LATER
        GeoPoint startPoint(startLat, startLon);
        GeoPoint endPoint(endLat, endLon);
        // Add street segment to the HashMap
        StreetSegment segment1(streetName, startPoint, endPoint);
        StreetSegment segment2(streetName, endPoint, startPoint);
        streetMap.insert(key, segment1);
        streetMap.insert(reverseKey, segment2);
        connectedPoints[startLat + " " + startLon].push_back(endPoint);
        connectedPoints[endLat + " " + endLon].push_back(startPoint);

        getline(infile, line);
        istringstream iss3(line);
        p = stoi(line);

        if (p > 0) {
            // Calculate midpoint and add to midpoint map
            GeoPoint mid = midpoint(startPoint, endPoint);
            
            connectedPoints[startLat + " " + startLon].push_back(mid);
            connectedPoints[mid.sLatitude + " " + mid.sLongitude].push_back(startPoint);
            connectedPoints[mid.sLatitude + " " + mid.sLongitude].push_back(endPoint);
            connectedPoints[endLat + " " + endLon].push_back(mid);

            // add segments from start to mid, and from mid to end
            StreetSegment segment5(streetName, startPoint, mid);
            StreetSegment segment6(streetName, mid, endPoint);
            StreetSegment segment7(streetName, mid, startPoint);
            StreetSegment segment8(streetName, endPoint, mid);
            streetMap.insert((startLat + " " + startLon + " " + mid.sLatitude + " " + mid.sLongitude), segment5);
            streetMap.insert((mid.sLatitude + " " + mid.sLongitude + " " + endLat + " " + endLon), segment6);
            streetMap.insert((mid.sLatitude + " " + mid.sLongitude + " " + startLat + " " + startLon), segment7);
            streetMap.insert((endLat + " " + endLon + " " + mid.sLatitude + " " + mid.sLongitude), segment8);

            // Read points of interest
            for (int i = 0; i < p; i++) {
                getline(infile, line);
                istringstream iss4(line);
                string poiName;
                string poiLat;
                string poiLon;
                if (!(getline(iss4, poiName, '|'))) { // Read until '|'
                    cout << "Ignoring badly-formatted point of interest: " << line << endl;
                    continue;
                }
                if (!(iss4 >> poiLat >> poiLon)) { // Read the rest of the line seperating words based on ' '
                    cout << "Ignoring badly-formatted point of interest: " << line << endl;
                    continue;
                }
                GeoPoint poiPoint(poiLat, poiLon);
                poiMap.insert(poiName, poiPoint); // add the poi to the map of poi's
                StreetSegment poiSegment1("a path", mid, poiPoint);
                StreetSegment poiSegment2("a path", poiPoint, mid);
                streetMap.insert((mid.sLatitude + " " + mid.sLongitude + " " + poiLat + " " + poiLon), poiSegment1);
                streetMap.insert((poiLat + " " + poiLon + " " + mid.sLatitude + " " + mid.sLongitude), poiSegment2);
                connectedPoints[mid.sLatitude + " " + mid.sLongitude].push_back(poiPoint);
                connectedPoints[poiLat + " " + poiLon].push_back(mid);
            }
        }

    }

    return true;
}


bool GeoDatabase::get_poi_location(const string& poi, GeoPoint& point) const {
    const GeoPoint* ptr = poiMap.find(poi);
    if (ptr == nullptr) {
        return false;
    }
    point = *ptr;
    return true;
}


vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const {
    string key = pt.sLatitude + " " + pt.sLongitude;
    const vector<GeoPoint>* ptr = connectedPoints.find(key);
    if (ptr == nullptr) {
        vector<GeoPoint> connectedGeoPoints;
        return connectedGeoPoints;
    }
    return *ptr;
}


string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const {
    string key = pt1.sLatitude + " " + pt1.sLongitude + " " + pt2.sLatitude + " " + pt2.sLongitude;

    const StreetSegment* ptr = streetMap.find(key);

    if (ptr == nullptr) {
        return "";
    }
    return ptr->m_streetName;
}
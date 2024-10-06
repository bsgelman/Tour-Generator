#include "tour_generator.h"
#include "geotools.h"

TourGenerator::TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router)
	: geodbPtr(&geodb), routerPtr(&router) {
}

std::vector<TourCommand> TourGenerator::generate_tour(const Stops& stops) const {
    std::vector<TourCommand> tourCommands;
    GeoPoint start, end;

    // Iterate through stops
    for (int i = 0; i < stops.size(); ++i) {
        std::string poi;
        std::string talkingPoints;
        stops.get_poi_data(i, poi, talkingPoints);

        // Generate commentary tour command
        TourCommand commentaryCommand;
        commentaryCommand.init_commentary(poi, talkingPoints);
        tourCommands.push_back(commentaryCommand);

        // If not the last stop
        if (i < stops.size() - 1) {
            std::string currentPoi;
            std::string nextPoi;
            stops.get_poi_data(i, currentPoi, talkingPoints);
            stops.get_poi_data(i + 1, nextPoi, talkingPoints);

            // Get GeoPoints associated with current and next stops
            GeoPoint currentGeoPoint;
            geodbPtr->get_poi_location(currentPoi, currentGeoPoint);
            GeoPoint nextGeoPoint;
            geodbPtr->get_poi_location(nextPoi, nextGeoPoint);

            // Generate route between current and next stops
            std::vector<GeoPoint> route = routerPtr->route(currentGeoPoint, nextGeoPoint);
            // Generate tour commands for the route
            for (size_t j = 0; j < route.size() - 1; j++) {
                start = route[j];
                end = route[j + 1];
                double distance = distance_earth_miles(start, end);
                std::string streetName = geodbPtr->get_street_name(start, end); // Assume path name for now

                // Calculate direction based on angle of line
                double angle = angle_of_line(start, end);
                std::string direction;
                if (angle >= 0 && angle < 22.5)
                    direction = "east";
                else if (angle >= 22.5 && angle < 67.5)
                    direction = "northeast";
                else if (angle >= 67.5 && angle < 112.5)
                    direction = "north";
                else if (angle >= 112.5 && angle < 157.5)
                    direction = "northwest";
                else if (angle >= 157.5 && angle < 202.5)
                    direction = "west";
                else if (angle >= 202.5 && angle < 247.5)
                    direction = "southwest";
                else if (angle >= 247.5 && angle < 292.5)
                    direction = "south";
                else if (angle >= 292.5 && angle < 337.5)
                    direction = "southeast";
                else // angle >= 337.5
                    direction = "east";

                TourCommand proceedCommand;
                proceedCommand.init_proceed(direction, streetName, distance, start, end);
                tourCommands.push_back(proceedCommand);

                // Check for turn command
                if (j < route.size() - 2) {
                    GeoPoint nextPoint = route[j + 2];
                    std::string nextStreetName = geodbPtr->get_street_name(end, nextPoint);
                    double angleOfTurn = angle_of_turn(start, end, nextPoint);
                    if (streetName != nextStreetName && angleOfTurn >= 1 && angleOfTurn < 180) {
                        // Generate left turn command
                        TourCommand turnCommand;
                        turnCommand.init_turn("left", nextStreetName);
                        tourCommands.push_back(turnCommand);
                    }
                    else if (streetName != nextStreetName && angleOfTurn >= 180 && angleOfTurn <= 359) {
                        // Generate right turn command
                        TourCommand turnCommand;
                        turnCommand.init_turn("right", nextStreetName);
                        tourCommands.push_back(turnCommand);
                    }
                }
            }
        }
    }

    return tourCommands;
}
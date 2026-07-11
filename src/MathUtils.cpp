#include "MathUtils.h"
#include <cmath>

namespace {
    const double EARTH_RADIUS_KM = 6371.0;
    const double PI = 3.14159265358979323846; // define our own; M_PI isn't portable on MSVC

    double toRadians(double degrees) {
        return degrees * PI / 180.0;
    }
}

// haversine formula: accounting for curvature.
double MathUtils::haversineDistance(double lat1, double lon1,
                                    double lat2, double lon2) {
    double dLat = toRadians(lat2 - lat1);
    double dLon = toRadians(lon2 - lon1);

    double a = std::sin(dLat / 2.0) * std::sin(dLat / 2.0) +
               std::cos(toRadians(lat1)) * std::cos(toRadians(lat2)) *
               std::sin(dLon / 2.0) * std::sin(dLon / 2.0);

    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    return EARTH_RADIUS_KM * c;
}

// Free-Space Path Loss model: (signal weakens over distance)
double MathUtils::estimateSignalStrength(double distanceKm,
                                         double freqMHz,
                                         double txPowerDbm) {
    if (distanceKm < 0.001) {
        distanceKm = 0.001;
    }

    double fspl = 20.0 * std::log10(distanceKm) +
                  20.0 * std::log10(freqMHz) +
                  32.44;

    return txPowerDbm - fspl; // received power = transmit power - path loss
}

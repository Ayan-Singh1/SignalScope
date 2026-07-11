#pragma once

// haversine + fspl
namespace MathUtils {
    double haversineDistance(double lat1, double lon1,
                             double lat2, double lon2);

    // est received signal strength (dBm) at a given distance
    // defaults: 850 MHz carrier, 43 dBm TX power
    // higher (less negative) = stronger signal
    double estimateSignalStrength(double distanceKm,
                                  double freqMHz = 850.0,
                                  double txPowerDbm = 43.0);
}

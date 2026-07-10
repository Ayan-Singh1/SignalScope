#pragma once

// MathUtils: RF math for SignalScope.
// Great-circle distance (Haversine) + signal-strength estimate (FSPL model).
namespace MathUtils {

    // Great-circle distance between two lat/lon points, in kilometers.
    double haversineDistance(double lat1, double lon1,
                             double lat2, double lon2);

    // Estimate received signal strength (dBm) at a given distance.
    // Free-Space Path Loss. Defaults: 850 MHz carrier, 43 dBm TX power.
    // Higher (less negative) = stronger signal.
    double estimateSignalStrength(double distanceKm,
                                  double freqMHz = 850.0,
                                  double txPowerDbm = 43.0);
}

//
// Created by Juno Wang on 6/30/21.
//

#ifndef UNITTEST_ALEX_MULTIMAP_H_BIGTEST_H
#define UNITTEST_ALEX_MULTIMAP_H_BIGTEST_H
#include <memory>
#include <geos/geom/Envelope.h>

using geos::geom::Coordinate;
using geos::geom::Envelope;
using namespace std;
using namespace geos;

// Forward declaration
namespace geos {
    namespace geom {
        class Polygon;
        class CoordinateSequence;
        class GeometryFactory;
    }
}

class GeometryTestFactory {
public:
    static geom::Polygon* createBox(geom::GeometryFactory* fact, double minx, double miny, int nSide, double segLen);
    static geom::CoordinateSequence* createBox(double minx, double miny, int nSide, double segLen);
    static geom::CoordinateSequence* createCircle(double basex, double basey, double size, uint32_t nPts);
    static geom::Polygon* createCircle(geom::GeometryFactory* fact, double basex, double basey, double size, uint32_t nPts);
    static geom::CoordinateSequence* createSineStar(double basex, double basey, double size, double armLen, int nArms,
                                                    int nPts);
    static geom::Polygon* createSineStar(geom::GeometryFactory* fact, double basex, double basey, double size,
                                         double armLen, int nArms, int nPts);

};

#endif //UNITTEST_ALEX_MULTIMAP_H_BIGTEST_H

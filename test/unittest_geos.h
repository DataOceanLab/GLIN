//
// Created by Juno Wang on 6/29/21.
//

#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
#include <geos/io/WKTReader.h>
#include "bigtest.h"
#include <geos/geom/Point.h>
#include <geos/index/strtree/SimpleSTRtree.h>
#include <geos/index/strtree/GeometryItemDistance.h>
#include <geos/index/ItemVisitor.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Dimension.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/util/IllegalArgumentException.h>
#include <geos/geom/LineSegment.h>
#include <geos/index/quadtree/Quadtree.h>




using namespace geos::geom;
using geos::geom::Coordinate;
using geos::geom::Envelope;
using Polygon = geos::geom::Polygon;

// Nickname for classes
typedef std::unique_ptr<geos::geom::Geometry> GeomPtr;
typedef geos::geom::GeometryFactory GeometryFactory;
/*
* Generate a random point in the range of
* POINT(0..range, 0..range). Caller must
* free.
*/

TEST_SUITE("Basic GEOS ops") {
    TEST_CASE("Test covers") {
        GeometryFactory::Ptr factory = GeometryFactory::create();
        geos::io::WKTReader wkt_reader(*factory);
        std::string wkt_a("POLYGON ((0 0, 0 100, 100 100, 100 0, 0 0))");
        std::string wkt_b("POLYGON ((0 0, 0 100, 90 90, 90 0, 0 0))");
        GeomPtr g1(wkt_reader.read(wkt_a));
        GeomPtr g2(wkt_reader.read(wkt_b));
        CHECK(g1->covers(g2.get()));
    }

    TEST_CASE("Tree"){
        index::strtree::SimpleSTRtree t(10);
        std::vector<std::unique_ptr<geom::Geometry>> geoms;
        const int gridSize = 10;

        auto gf = geom::GeometryFactory::create();
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                geom::Coordinate c((double)i, (double)j);
                geom::Point* pt = gf->createPoint(c);
                geoms.emplace_back(pt);
                t.insert(pt);
            }
        }

        geom::Envelope qe(-0.5, 1.5, -0.5, 1.5);
        std::vector<void*> matches;
        t.query(&qe, matches);
        // std::cout << matches.size() << std::endl;
        CHECK(matches.size() == 4);
        t.query(&qe, matches);
        // std::cout << matches.size() << std::endl;
        CHECK(matches.size() == 8);

        // std::cout << t << std::endl;

    }
    TEST_CASE("Polygan covers point WKT"){
        GeometryFactory::Ptr factory = GeometryFactory::create();
        geos::io::WKTReader reader(*factory);
        GeomPtr g1(reader.read(
                "POLYGON ((-1.183864 52.951915, -1.183862 52.951903, -1.183890 52.951900, -1.183924 52.951897, -1.183958 52.951894, -1.183954 52.951880, -1.183954 52.951878, -1.183932 52.951841, -1.183904 52.951844, -1.183870 52.951847, -1.183832 52.951852, -1.183824 52.951838, -1.183820 52.951830, -1.183870 52.951819, -1.183886 52.951815, -1.183890 52.951819, -1.183929 52.951810, -1.183909 52.951776, -1.183861 52.951787, -1.183853 52.951788, -1.183842 52.951770, -1.183970 52.951742, -1.183983 52.951763, -1.183963 52.951768, -1.183975 52.951788, -1.183994 52.951785, -1.184009 52.951807, -1.184002 52.951808, -1.184009 52.951835, -1.183990 52.951836, -1.183990 52.951836, -1.183990 52.951838, -1.184001 52.951880, -1.184018 52.951954, -1.184020 52.951956, -1.183998 52.951957, -1.183998 52.951956, -1.183996 52.951948, -1.183970 52.951906, -1.183936 52.951909, -1.183864 52.951915))"
        ));
        GeomPtr g2(reader.read(
                "POINT (-1.183972 52.951871)"
        ));
        CHECK(g1->covers(g2.get()));
        CHECK(g2->coveredBy(g1.get()));
        CHECK(g1->contains(g2.get()));
    }
    TEST_CASE("LineSegment Class with line reverse"){
        auto gf = geos::geom::GeometryFactory::create();
        geos::geom::Coordinate proj_point(5, 2.0);
        geos::geom::Coordinate proj_result(0,0);
        geos::geom::Coordinate pv1(0,0);
        geos::geom::Coordinate pv2(5,5);
        geos::geom::LineSegment seg1(pv1,pv2);
        CHECK(seg1[0] == pv1);
        seg1.reverse();
        CHECK(seg1[1] == pv1);


        // can call reverse but dont know why cannot call project
    }
    TEST_CASE("LineSegment projection y=x"){
        geos::geom::Coordinate proj_point(5, 2.0);
        geos::geom::Coordinate proj_result(0,0);
        geos::geom::Coordinate pv1(0,0);
        geos::geom::Coordinate pv2(5,5);
        geos::geom::LineSegment seg1(pv1,pv2);
        seg1.project(proj_point,proj_result);
        CHECK(proj_result.x == 3.5);
        CHECK((proj_result.y) == 3.5);
    }
    TEST_CASE("LineSegment porject y = 3x+2"){
        geos::geom::Coordinate proj_point(5, 2.0);
        geos::geom::Coordinate proj_result(0,0);
        geos::geom::Coordinate pv1(0,2);
        geos::geom::Coordinate pv2(5,17);
        geos::geom::LineSegment seg2(pv1,pv2);
        seg2.project(proj_point,proj_result);
        CHECK(proj_result.x == 0.5);
        CHECK(proj_result.y == 3.5);
    }
}


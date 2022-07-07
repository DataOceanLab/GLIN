//
// Created by Congying Wang on 10/25/21.
//
#include "doctest.h"
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/LinearRing.h>

TEST_SUITE("Glin projection") {
    TEST_CASE("projection") {
        // We will use a coordinate list to build the linearring
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);;
        geos::geom::CoordinateArraySequence *cl = new CoordinateArraySequence();
        cl->add(geos::geom::Coordinate(0, 0));
        cl->add(geos::geom::Coordinate(0, 6));
        cl->add(geos::geom::Coordinate(6, 6));
        cl->add(geos::geom::Coordinate(6, 0));
        cl->add(geos::geom::Coordinate(0, 0));

        /*
         * Now that we have a CoordinateSequence we can create the linearring.
         * The newly created LinearRing will take ownershipof the CoordinateSequence.
         * linearring for outer shell
         */
        geos::geom::LinearRing *lr;
        lr = global_factory->createLinearRing(cl);
        // We need a LinearRing for the polygon shell
        geos::geom::LinearRing *outer = lr;
        //linearring for inner hole
        geos::geom::CoordinateArraySequence *cl1 = new CoordinateArraySequence();
        cl1->add(geos::geom::Coordinate(2, 2));
        cl1->add(geos::geom::Coordinate(2, 4));
        cl1->add(geos::geom::Coordinate(4, 4));
        cl1->add(geos::geom::Coordinate(4, 2));
        cl1->add(geos::geom::Coordinate(2, 2));
        geos::geom::LinearRing *lr1 = global_factory->createLinearRing(cl1);
        geos::geom::LinearRing *inner = lr1;
        /*
         * If we need to specify any hole, we do it using
         a vector of Geometry pointers
         */
        vector<geos::geom::LinearRing *> *holes = new vector<geos::geom::LinearRing *>;

        // We add the newly created geometry to the vector
        // of holes.
        holes->push_back(inner);
        geos::geom::Polygon *poly = global_factory->createPolygon(outer, holes);
        geos::geom::Coordinate pv1(0, 0);
        geos::geom::Coordinate pv2(0, 6);
        geos::geom::LineSegment seg1(pv1, pv2);
        long double min = 0;
        long double max = 0;
        shape_projection(poly, seg1, min, max);
        max = std::ceil(max * 100.0) / 100.0;
        min = std::ceil(min * 100.0) / 100.0;
//        std::cout<< min << std::endl;
//        std::cout << max << std::endl;

        // CHECK( max == 8.49);
        CHECK(min == 0);
    }

    TEST_CASE("project shape on y = 3x+2") {
        // create the polygon
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        geos::geom::CoordinateArraySequence *cl = new CoordinateArraySequence();
        cl->add(geos::geom::Coordinate(0, 0));
        cl->add(geos::geom::Coordinate(0, 6));
        cl->add(geos::geom::Coordinate(6, 6));
        cl->add(geos::geom::Coordinate(6, 0));
        cl->add(geos::geom::Coordinate(0, 0));
        geos::geom::LinearRing *lr1 = global_factory->createLinearRing(cl);
        geos::geom::LinearRing *outer = lr1;

        vector<geos::geom::LinearRing *> *holes = new vector<geos::geom::LinearRing *>;
        geos::geom::Polygon *poly = global_factory->createPolygon(outer, holes);

        geos::geom::Coordinate pv1(0, 0);
        geos::geom::Coordinate pv2(5, 17);
        geos::geom::LineSegment seg2(pv1, pv2);
        long double min;
        long double max;
        shape_projection(poly, seg2, min, max);

        max = std::ceil(max * 100.0) / 100.0;
        min = std::ceil(min * 100.0) / 100.0;
//        CHECK(max == 7.44999999999999999982);
        CHECK(min == 0);
    }

    TEST_CASE("Projection result to alex") {
        // insert project result to alex index
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        geos::geom::CoordinateArraySequence *cl = new CoordinateArraySequence();
        cl->add(geos::geom::Coordinate(0, 0));
        cl->add(geos::geom::Coordinate(0, 6));
        cl->add(geos::geom::Coordinate(6, 6));
        cl->add(geos::geom::Coordinate(6, 0));
        cl->add(geos::geom::Coordinate(0, 0));
        geos::geom::LinearRing *lr1 = global_factory->createLinearRing(cl);
        geos::geom::LinearRing *outer = lr1;

        vector<geos::geom::LinearRing *> *holes = new vector<geos::geom::LinearRing *>;
        geos::geom::Polygon *poly = global_factory->createPolygon(outer, holes);
        geos::geom::Coordinate pv1(0, 0);
        geos::geom::Coordinate pv2(5, 17);
        geos::geom::LineSegment seg2(pv1, pv2);
        long double dist_min;
        long double dist_max;
        shape_projection(poly, seg2, dist_min, dist_max);
        dist_max = std::ceil(dist_max * 100.0) / 100.0;
        dist_min = std::ceil(dist_min * 100.0) / 100.0;

        geos::geom::Coordinate pv3(0, 0);
        geos::geom::Coordinate pv4(6, 6);
        geos::geom::LineSegment seg1(pv3, pv4);
        long double min;
        long double max;
        shape_projection(poly, seg1, min, max);

        Alex<double, double> index;
        Alex<double, double>::V values[2];

        values[0].first = min;
        values[0].second = max;
        values[1].first = dist_min;
        values[1].second = dist_max;
        std::sort(values, values + 2);
        index.bulk_load(values, 2);
        for (int i = 0; i < 2; i++) {
            auto it = index.find(values[i].first);
            CHECK_EQ(values[i].first, it.key());
        }
    }
    TEST_CASE("find perpendicular") {
        alex::Glin<double, geos::geom::Geometry *> index1;
        geos::geom::LineSegment seg_45 = index1.create_line_seg(0, 5, 2);
        geos::geom::LineSegment seg_perpen = index1.get_perpendicular_line(seg_45);
//        std::cout<< "original "<< geos::algorithm::Angle::toDegrees(seg_45.angle()) << " perpendicular " <<geos::algorithm::Angle::toDegrees(seg_perpen.angle()) << endl;
//        std::cout << "angle" <<geos::algorithm::Angle::toDegrees(geos::algorithm::Angle::diff(seg_45.angle(),seg_perpen.angle()));
        CHECK(geos::algorithm::Angle::toDegrees(geos::algorithm::Angle::diff(seg_45.angle(), seg_perpen.angle())) == 90);
        double k1 = (seg_45.p1.y - seg_45.p0.y) / (seg_45.p1.x - seg_45.p0.x);
//        std::cout << k1<< endl;
        double k2 = (seg_perpen.p1.y - seg_perpen.p0.y) / (seg_perpen.p1.x - seg_perpen.p0.x);
//        std::cout << k2 << endl;
        CHECK(k1 * k2 == -1);
    }
};

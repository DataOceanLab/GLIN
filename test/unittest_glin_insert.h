//
// Created by cwang on 8/9/21.
//

#ifndef ALEX_UNITTEST_GLIN_INSERT_H
#define ALEX_UNITTEST_GLIN_INSERT_H

#include "doctest.h"
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/LinearRing.h>
#include "alex.h"
#include <map>
#include <geos/index/strtree/GeometryItemDistance.h>
#include <geos/index/ItemVisitor.h>
#include <geos/geom/Envelope.h>


//create non random size geometry, can choose which geometry to skip
std::vector<geos::geom::Geometry *>
create_geoms_non_random1(int num_of_poly, bool choose_skip, double geom_side_length, bool skip_multiple) {
    std::vector<geos::geom::Geometry *> poly_vec;
    std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
    geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);;
    for (int i = 0; i < num_of_poly; i++) {
        if (choose_skip == true) {
            if (i == 1) {
                continue;
            }
        }
        if (skip_multiple == true) {
            if (i >= 0 && i < 35 && i != 1) {
                continue;
            }

        }
        // std::cout<< i << " i " << endl;
        double minX = i;
        double minY = i;
        double maxX = i + geom_side_length;
        double maxY = i + geom_side_length;
        geos::geom::CoordinateArraySequence *cl6 = new CoordinateArraySequence();
        cl6->add(geos::geom::Coordinate(minX, minY));
        cl6->add(geos::geom::Coordinate(minX + 1, maxY + 1));
        cl6->add(geos::geom::Coordinate(minX, maxY));
        cl6->add(geos::geom::Coordinate(minX + 2, maxY));
        cl6->add(geos::geom::Coordinate(maxX, maxY));
        cl6->add(geos::geom::Coordinate(maxX, minY));
        cl6->add(geos::geom::Coordinate(maxX, minY + 1));
        cl6->add(geos::geom::Coordinate(minX, minY));
        geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl6);
        geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
        poly_vec.push_back(poly6);

    }
    return poly_vec;

}


TEST_SUITE("insert test suite with geometry") {
    TEST_CASE("insert senario one: normal insert, single piece, line projection") {
        int num_of_poly = 50;
        double poly_length = 20;
        std::vector<geos::geom::Geometry *> geometrys;
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        alex::Glin<double, geos::geom::Geometry *> index;
        std::vector<Geometry *> find_result;
        double piece_limit = 5;
        std::vector<tuple<double, double, double, double>> pieces;

        //create segment line
        geos::geom::Coordinate pv1(0, 5);
        geos::geom::Coordinate pv2(1, 6);
        geos::geom::LineSegment seg(pv1, pv2);

        for (auto i = 0; i < num_of_poly; i++) {
            if (i == 10 or i == 20) {
                continue;
            }
            double minX = i;
            double minY = i;
            double maxX = i + poly_length;
            double maxY = i + poly_length;
            geos::geom::CoordinateArraySequence *cl6 = new CoordinateArraySequence();
            cl6->add(geos::geom::Coordinate(minX, minY));
            cl6->add(geos::geom::Coordinate(minX + 1, maxY + 1));
            cl6->add(geos::geom::Coordinate(minX, maxY));
            cl6->add(geos::geom::Coordinate(minX + 2, maxY));
            cl6->add(geos::geom::Coordinate(maxX, maxY));
            cl6->add(geos::geom::Coordinate(maxX, minY));
            cl6->add(geos::geom::Coordinate(maxX, minY + 1));
            cl6->add(geos::geom::Coordinate(minX, minY));
            geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl6);
            geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
            geometrys.push_back(poly6);
        }

        index.bulk_load_with_lineseg(geometrys, seg, piece_limit, pieces);

        double minX = 10;
        double minY = 10;
        double maxX = 10 + poly_length;
        double maxY = 10 + poly_length;
        geos::geom::CoordinateArraySequence *cl7 = new CoordinateArraySequence();
        cl7->add(geos::geom::Coordinate(minX, minY));
        cl7->add(geos::geom::Coordinate(minX + 1, maxY + 1));
        cl7->add(geos::geom::Coordinate(minX, maxY));
        cl7->add(geos::geom::Coordinate(minX + 2, maxY));
        cl7->add(geos::geom::Coordinate(maxX, maxY));
        cl7->add(geos::geom::Coordinate(maxX, minY));
        cl7->add(geos::geom::Coordinate(maxX, minY + 1));
        cl7->add(geos::geom::Coordinate(minX, minY));
        geos::geom::LinearRing *lr7 = global_factory->createLinearRing(cl7);
        geos::geom::Polygon *poly_insert = global_factory->createPolygon(lr7, NULL);

        auto res = index.insert_with_lineseg(poly_insert, seg, piece_limit, pieces);

        CHECK( res.first.payload() == poly_insert);
        int count_filter = 0;
        index.find_with_lineseg(poly_insert, seg, pieces, find_result, count_filter);
        if (std::find(find_result.begin(), find_result.end(), poly_insert) != find_result.end()) {
            /* v contains x */
            CHECK(true);
        } else {
            CHECK(false);
        }
        std::vector<Geometry *> find_result1;

        double minX1 = 20;
        double minY1 = 20;
        double maxX1 = 20 + poly_length;
        double maxY1 = 20 + poly_length;
        geos::geom::CoordinateArraySequence *cl8 = new CoordinateArraySequence();
        cl8->add(geos::geom::Coordinate(minX1, minY1));
        cl8->add(geos::geom::Coordinate(minX1 + 1, maxY1 + 1));
        cl8->add(geos::geom::Coordinate(minX1, maxY1));
        cl8->add(geos::geom::Coordinate(minX1 + 2, maxY1));
        cl8->add(geos::geom::Coordinate(maxX1, maxY1));
        cl8->add(geos::geom::Coordinate(maxX1, minY1));
        cl8->add(geos::geom::Coordinate(maxX1, minY1 + 1));
        cl8->add(geos::geom::Coordinate(minX1, minY1));
        geos::geom::LinearRing *lr8 = global_factory->createLinearRing(cl8);
        geos::geom::Polygon *poly_insert1 = global_factory->createPolygon(lr8, NULL);
        auto res1 = index.insert_with_lineseg(poly_insert1, seg,piece_limit , pieces);
        CHECK(res1.first.payload() == poly_insert1);

        count_filter = 0;
        index.find_with_lineseg(poly_insert1, seg, pieces, find_result1, count_filter);
        if (std::find(find_result1.begin(), find_result1.end(), poly_insert) != find_result1.end()) {
            /* v contains x */
            CHECK(true);
        } else {
            CHECK(false);
        }

    }

//    TEST_CASE("insert to tile / empty") {
//        int num_of_poly = 50;
//        double poly_length = 20;
//        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
//        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
//        alex::Glin<double, geos::geom::Geometry *> index;
//        alex::Glin<double, geos::geom::Geometry *> index1;
//
//
//        double pieceLimitation = 10;
//        std::vector<tuple<double, double, double, double>> pieces;
//        std::vector<tuple<double, double, double, double>> pieces2;
//
//        //create segment line
//        geos::geom::Coordinate pv1(0, 5);
//        geos::geom::Coordinate pv2(1, 6);
//        geos::geom::LineSegment seg(pv1, pv2);
//
//        std::vector<geos::geom::Geometry *> polys = create_geoms_non_random1(num_of_poly, false, poly_length, false);
//        for (int i = 0; i < num_of_poly; i++) {
//            std::vector<Geometry *> find_result;
//            int count_filter = 0;
//            auto res = index.insert(polys[i], seg, pieceLimitation, pieces);
//            CHECK(res.first.payload() == polys[i]);
//            index.find(polys[i], seg, pieces, find_result, count_filter);
//            CHECK(std::find(find_result.begin(), find_result.end(), polys[i]) != find_result.end());
//        }
//        index1.load(polys, seg, pieceLimitation, pieces2);
//        CHECK_EQ(pieces.size(), pieces2.size());
//        for (int i = 0; i < pieces.size(); i++) {
//#ifdef DEBUG
//            std::cout << "tile insertion poly" << endl;
//            std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
//                      << get<3>(pieces[i]) << endl;
//            std::cout << " origianl poly " << endl;
//            std::cout << get<0>(pieces2[i]) << "  " << get<1>(pieces2[i]) << "  " << get<2>(pieces2[i]) << "  "
//                      << get<3>(pieces2[i]) << endl;
//#endif
//            CHECK_EQ(get<0>(pieces[i]), get<0>(pieces2[i]));
//            CHECK_EQ(get<1>(pieces[i]), get<1>(pieces2[i]));
//            CHECK_EQ(get<2>(pieces[i]), get<2>(pieces2[i]));
//            CHECK_EQ(get<3>(pieces[i]), get<3>(pieces2[i]));
//        }
//#ifdef DEBUG
//        std::cout <<"insert pieces size " << pieces.size()<<" \n "  <<endl;
//        std::cout<< "bulk load pieces size " << pieces2.size() <<" \n ""  "<<endl;
//        std::cout<< "endpoint " << "max_range "<< "count " << " sum " << endl;
//
//        std::cout<< "insert pieces" << endl;
//        for(int i = 0; i < pieces.size(); i++ ){
//            std::cout<<get<0>(pieces[i]) <<"  "<<  get<1>(pieces[i])<<"  "<< get<2>(pieces[i])<<"  " << get<3>(pieces[i])<<endl;
//        }
//        std::cout<< "bulkload pieces" << endl;
//        for(int i = 0; i < pieces2.size(); i++){
//            std::cout<< get<0>(pieces2[i])<<"  "<< get<1>(pieces2[i])<<"  "<< get<2>(pieces2[i])<<"  "<<get<3>(pieces2[i]) <<endl;
//        }
//#endif
//    }
};
TEST_SUITE("insert to head geometry") {
    TEST_CASE("insert into beginning") {
        int num_of_poly = 50;
        double poly_length = 20;
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        alex::Glin<double, geos::geom::Geometry *> index;
        std::vector<geos::geom::Geometry *> geometries;
        std::vector<Geometry *> find_result;
        double pieceLimitation = 10;
        std::vector<std::tuple<double, double, double, double>> pieces;
        std::vector<std::tuple<double, double, double, double>> pieces2;
        int count_filter = 0;
        //create segment line
        geos::geom::Coordinate pv1(0, 5);
        geos::geom::Coordinate pv2(1, 6);
        geos::geom::LineSegment seg(pv1, pv2);
        for (auto i = 35; i < num_of_poly; i++) {
            double minX = i;
            double minY = i;
            double maxX = i + poly_length;
            double maxY = i + poly_length;
            geos::geom::CoordinateArraySequence *cl2 = new CoordinateArraySequence();
            cl2->add(geos::geom::Coordinate(minX, minY));
            cl2->add(geos::geom::Coordinate(minX + 1, maxY + 1));
            cl2->add(geos::geom::Coordinate(minX, maxY));
            cl2->add(geos::geom::Coordinate(minX + 2, maxY));
            cl2->add(geos::geom::Coordinate(maxX, maxY));
            cl2->add(geos::geom::Coordinate(maxX, minY));
            cl2->add(geos::geom::Coordinate(maxX, minY + 1));
            cl2->add(geos::geom::Coordinate(minX, minY));
            geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl2);
            geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
            geometries.push_back(poly6);
        }
        index.bulk_load_with_lineseg(geometries, seg, pieceLimitation, pieces);

#ifdef DEBUG
        std::cout << "insert beginning before insertion" << endl;
        for (int i = 0; i < pieces.size(); i++) {
            std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
                      << get<3>(pieces[i]) << endl;
        }
#endif
//        std::cout << "insert beginning before insertion" << endl;
//        for (int i = 0; i < pieces.size(); i++) {
//            std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
//                      << get<3>(pieces[i]) << endl;
//        }
        std::vector<Geometry *> find_result1;
        double minX2 = 1;
        double minY2 = 1;
        double maxX2 = 1 + poly_length;
        double maxY2 = 1 + poly_length;
        geos::geom::CoordinateArraySequence *cl_in2 = new CoordinateArraySequence();
        cl_in2->add(geos::geom::Coordinate(minX2, minY2));
        cl_in2->add(geos::geom::Coordinate(minX2 + 1, maxY2 + 1));
        cl_in2->add(geos::geom::Coordinate(minX2, maxY2));
        cl_in2->add(geos::geom::Coordinate(minX2 + 2, maxY2));
        cl_in2->add(geos::geom::Coordinate(maxX2, maxY2));
        cl_in2->add(geos::geom::Coordinate(maxX2, minY2));
        cl_in2->add(geos::geom::Coordinate(maxX2, minY2 + 1));
        cl_in2->add(geos::geom::Coordinate(minX2, minY2));
        geos::geom::LinearRing *lr9 = global_factory->createLinearRing(cl_in2);
        geos::geom::Polygon *poly_in2 = global_factory->createPolygon(lr9, NULL);
        index.insert(poly_in2, seg, pieceLimitation, pieces);
        index.find(poly_in2, seg, pieces, find_result1, count_filter);
        if (std::find(find_result1.begin(), find_result1.end(), poly_in2) != find_result1.end()) {
            /* v contains x */
            CHECK(true);
        } else {
            CHECK(false);
        }

//        std::cout << "insert to beginning " << endl;
        for (int i = 0; i < pieces.size(); i++) {
#ifdef DEBUG
            std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
                      << get<3>(pieces[i]) << endl;
#endif
            if (i != 0) {
                CHECK(std::get<2>(pieces[i]) <= pieceLimitation );
            }
        }
        alex::Glin<double, geos::geom::Geometry *> index1;
        std::vector<geos::geom::Geometry *> head_inser_poly = create_geoms_non_random1(num_of_poly, false, poly_length,
                                                                                       true);
        index1.bulk_load_with_lineseg(head_inser_poly, seg, pieceLimitation, pieces2);
#ifdef DEBUG
        std::cout << "after insertion to head vector do bulkload" << endl;
#endif
        double orig_count = 0;
        for (int i = 0; i < pieces2.size(); i++) {
#ifdef DEBUG
            std::cout << get<0>(pieces2[i]) << "  " << get<1>(pieces2[i]) << "  " << get<2>(pieces2[i]) << "  "
                      << get<3>(pieces2[i]) << endl;
#endif

            if (i != 0) {
                CHECK(std::get<2>(pieces2[i]) <= pieceLimitation);
            }
            if(i == 1){
                CHECK(std::get<2>(pieces2[i]) == 10);
            }
            orig_count += std::get<2>(pieces2[i]);

        }
        double inserted_count = 0;
        for (int i = 0; i < pieces.size(); i++) {
            inserted_count += std::get<2>(pieces[i]);

        }
//        CHECK_EQ(inserted_count, orig_count);
    }
};
//
TEST_SUITE("piecewise only no geometry") {
TEST_CASE("manually test piecewise_insert function only ") {
    std::pair<double, double> records[6];
    records[0].first = 1;
    records[0].second = 1.5;
    records[1].first = 2;
    records[1].second = 4;
    records[2].first = 2;
    records[2].second = 8;
    records[3].first = 3;
    records[3].second = 9;
    records[4].first = 4;
    records[4].second = 9;
    records[5].first = 5;
    records[5].second = 7;
    std::vector<std::tuple<double, double, double, double>> pieces;
    double piecelimit = 3;
    piecewise(records, 6, piecelimit, pieces);
#ifdef DEBUG
    std::cout << "original pieces" << endl;
    for (int i = 0; i < pieces.size(); i++) {
        std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
                  << get<3>(pieces[i]) << endl;

    }
#endif
    CHECK(!pieces.empty());
    CHECK(pieces.size() == 3);
    CHECK(std::get<0>(pieces[1]) == 7);
    CHECK(std::get<1>(pieces[1]) == 1);
    CHECK(std::get<2>(pieces[1]) == 3);
    CHECK(std::get<3>(pieces[1]) == 8);
    CHECK(std::get<0>(pieces[2]) == 9);
    CHECK(std::get<1>(pieces[2]) == 2);
    CHECK(std::get<2>(pieces[2]) == 3);
    CHECK(std::get<3>(pieces[2]) == 9);


    // remove one record to make insert into front
    std::pair<double, double> records1[5];
    records1[0].first = 2;
    records1[0].second = 4;
    records1[1].first = 2;
    records1[1].second = 8;
    records1[2].first = 3;
    records1[2].second = 9;
    records1[3].first = 4;
    records1[3].second = 9;
    records1[4].first = 5;
    records1[4].second = 7;
    std::vector<std::tuple<double, double, double, double>> pieces1;
    piecewise(records1, 5, piecelimit, pieces1);

#ifdef DEBUG
    std::cout << " after remove one record" << endl;
    for (int i = 0; i < pieces1.size(); i++) {
        std::cout << get<0>(pieces1[i]) << "  " << get<1>(pieces1[i]) << "  " << get<2>(pieces1[i]) << "  "
                  << get<3>(pieces1[i]) << endl;

    }
#endif

    double start = 1;
    double end = 1.5;
    insert_pieces(start, end, piecelimit, pieces1);
#ifdef DEBUG
    std::cout << "after insert" << endl;
#endif
#ifdef DEBUG
    std::cout << " insert first one record" << endl;
    for (int i = 0; i < pieces1.size(); i++) {

        std::cout << get<0>(pieces1[i]) << "  " << get<1>(pieces1[i]) << "  " << get<2>(pieces1[i]) << "  "
                  << get<3>(pieces1[i]) << endl;
}
#endif


        CHECK_EQ(std::get<0>(pieces1[0]), 1);
        CHECK_EQ(std::get<0>(pieces1[1]), 1.5);
        CHECK_EQ(std::get<1>(pieces1[1]), 1);
        CHECK_EQ(std::get<2>(pieces1[1]), 1);
        CHECK_EQ(std::get<3>(pieces1[1]), 1);

        CHECK_EQ(std::get<0>(pieces1[2]), 8);
        CHECK_EQ(std::get<1>(pieces1[2]), 2);
        CHECK_EQ(std::get<2>(pieces1[2]), 3);
        CHECK_EQ(std::get<3>(pieces1[2]), 9);

        CHECK_EQ(std::get<0>(pieces1[3]), 9);
        CHECK_EQ(std::get<1>(pieces1[3]), 3);
        CHECK_EQ(std::get<2>(pieces1[3]), 2);
        CHECK_EQ(std::get<3>(pieces1[3]), 7);
}

TEST_CASE("test same range piecewise") {
    // all records share same range
    std::pair<double, double> records2[5];
    records2[0].first = 2;
    records2[0].second = 4;
    records2[1].first = 4;
    records2[1].second = 6;
    records2[2].first = 6;
    records2[2].second = 8;
    records2[3].first = 7;
    records2[3].second = 9;
    records2[4].first = 5;
    records2[4].second = 7;
    std::vector<std::tuple<double, double, double, double>> pieces2;
    piecewise(records2, 5, 2, pieces2);
        double total_count = 0;
        for (int i =0 ; i < pieces2.size(); i++){
            if(i != 0){
                total_count += std::get<2>(pieces2[i]) ;
            }

        }
#ifdef  DEBUG
    std::cout << " pieces with same range"  << endl;
    for (int i =0 ; i < pieces2.size(); i++){
        std::cout << get<0>(pieces2[i]) << "  " << get<1>(pieces2[i]) << "  " << get<2>(pieces2[i]) << "  "
        << get<3>(pieces2[i]) << endl;
    }
#endif
    CHECK(pieces2.size() == 4);
        CHECK(total_count == 5);
}

TEST_CASE("auto head insertion "){

    pair<double, double> records[200];
    std::vector<std::tuple<double, double, double, double>> orig_pieces;
    double start_number = 100.50;
    for(int i = 0; i < 200; i++){
        records[i].first = start_number-i;
        records[i].second = start_number;
        start_number --;
//        std::cout << records[i].first << " " << records[i].second << endl;
    }

        piecewise(records, 200,10, orig_pieces);


        std::vector<std::tuple<double, double, double, double>> inserted_pieces;
        for(int i = 0; i < 200; i++){
            insert_pieces(records[i].first,records[i].second,10,inserted_pieces);
        }
#ifdef DEBUG

        for (int i =0 ; i < orig_pieces.size(); i++) {
            std::cout << std::get<0>(orig_pieces[i]) << "  " << std::get<1>(orig_pieces[i]) << "  " << std::get<2>(orig_pieces[i])
                      << "  "
                      << std::get<3>(orig_pieces[i]) << endl;
        }

        for (int i =0 ; i < inserted_pieces.size(); i++) {
            std::cout << std::get<0>(inserted_pieces[i]) << "  " << std::get<1>(inserted_pieces[i]) << "  " << std::get<2>(inserted_pieces[i])
                      << "  "
                      << std::get<3>(inserted_pieces[i]) << endl;
        }
#endif
        for (int i =0 ; i < orig_pieces.size(); i++) {
            CHECK_EQ(std::get<0>(orig_pieces[i]),std::get<0>(inserted_pieces[i]));
            CHECK_EQ(std::get<1>(orig_pieces[i]),std::get<1>(inserted_pieces[i]));
            CHECK_EQ(std::get<2>(orig_pieces[i]), std::get<2>(inserted_pieces[i]));
            CHECK_EQ(std::get<3>(orig_pieces[i]), std::get<3>(inserted_pieces[i]) );
        }
}

};


TEST_SUITE("deletion with geometry") {
    TEST_CASE("delete") {
        int num_of_poly = 20;
        //side length
        double poly_length = 20;
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        // index initialization
        alex::Glin<double, geos::geom::Geometry *> index;
        std::vector<geos::geom::Geometry *> polys = create_geoms_non_random1(num_of_poly, false, poly_length, false);
        //create segment line
        geos::geom::Coordinate pv1(0, 5);
        geos::geom::Coordinate pv2(1, 6);
        geos::geom::LineSegment seg(pv1, pv2);
        std::vector<tuple<double, double, double, double>> pieces;
        double error_bound = 0.004;
        index.load(polys, seg, error_bound, pieces);
#ifdef DEBUG
        std::cout << "before remove" << endl;
        for (int i = 0; i < pieces.size(); i++) {
            std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
                      << get<3>(pieces[i]) << endl;
        }
#endif

        // polygan to be erased
        double minX = 2;
        double minY = 2;
        double maxX = 2 + poly_length;
        double maxY = 2 + poly_length;
        geos::geom::CoordinateArraySequence *cl_er = new CoordinateArraySequence();
        cl_er->add(geos::geom::Coordinate(minX, minY));
        cl_er->add(geos::geom::Coordinate(minX + 1, maxY + 1));
        cl_er->add(geos::geom::Coordinate(minX, maxY));
        cl_er->add(geos::geom::Coordinate(minX + 2, maxY));
        cl_er->add(geos::geom::Coordinate(maxX, maxY));
        cl_er->add(geos::geom::Coordinate(maxX, minY));
        cl_er->add(geos::geom::Coordinate(maxX, minY + 1));
        cl_er->add(geos::geom::Coordinate(minX, minY));
        geos::geom::LinearRing *lr_er = global_factory->createLinearRing(cl_er);
        geos::geom::Polygon *poly_er = global_factory->createPolygon(lr_er, NULL);

        int num_erase = index.erase_lineseg(poly_er, seg, error_bound, pieces);
        //std::cout << "number of erase " << num_erase << endl;
        assert(num_erase > 0);
#ifdef DEBUG
        std::cout << "after remove" << endl;
        for (int i = 0; i < pieces.size(); i++) {
            std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
                      << get<3>(pieces[i]) << endl;
        }
#endif
        // check this piece is deleted in pieces
        CHECK(std::get<2>(pieces[4]) == 0);
    }

};

#endif //ALEX_UNITTEST_GLIN_INSERT_H

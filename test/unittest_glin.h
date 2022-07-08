//
// Created by Juno Wang on 7/6/21.
//
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
#include "../glin/glin.h"
#include "../glin/piecewise.h"
//#include "../glin_benchmark/utils.h"
#include <geos/index/strtree/SimpleSTRtree.h>
#include <geos/index/strtree/GeometryItemDistance.h>
#include <geos/index/ItemVisitor.h>
#include <geos/geom/Envelope.h>
#include <geos/index/quadtree/Quadtree.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> point_type;
typedef boost::geometry::model::linestring<point_type> linestring_type;
typedef boost::geometry::model::polygon<point_type> polygon_type;
typedef boost::geometry::model::segment<point_type> segment_type;
typedef boost::geometry::model::multi_linestring<linestring_type> multi_lingstring_type;
typedef boost::geometry::model::multi_polygon<polygon_type> mpolygon_type;
typedef boost::variant<point_type, linestring_type, polygon_type, segment_type, multi_lingstring_type, mpolygon_type> geometryVariant;
//create non random size geometry, can choose which geometry to skip
std::vector<geos::geom::Geometry *>
create_geoms_non_random(int num_of_poly, bool choose_skip, double geom_side_length) {
    std::vector<geos::geom::Geometry *> poly_vec;
    std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
    geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
    for (int i = 0; i < num_of_poly; i++) {
        if (choose_skip == true) {
            if (i == 10) {
                continue;
            }
        }
        double minX = i;
        double minY = i;
        double maxX = i + geom_side_length;
        double maxY = i + geom_side_length;
        geos::geom::CoordinateArraySequence *cl6 = new CoordinateArraySequence();
        cl6->add(geos::geom::Coordinate(minX, minY));
        cl6->add(geos::geom::Coordinate(minX, maxY));
        cl6->add(geos::geom::Coordinate(maxX, maxY));
        cl6->add(geos::geom::Coordinate(maxX, minY));
        cl6->add(geos::geom::Coordinate(minX, minY));
        geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl6);
        geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
        poly_vec.push_back(poly6);

    }
    return poly_vec;

}

std::vector<geos::geom::Geometry *>
create_geoms_random(int num_of_poly) {
    std::vector<geos::geom::Geometry *> poly_vec;
    std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
    geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
    for (auto i = 0; i < num_of_poly; i++) {
        double length = (double) rand() / (RAND_MAX + 1.0);
        double minX = -i;
        double minY = i;
        double maxX = -i + length + (rand() % 5);
        double maxY = i + length + (rand() % 5);
        geos::geom::CoordinateArraySequence *cl6 = new CoordinateArraySequence();
        cl6->add(geos::geom::Coordinate(minX + 1.11123, minY + 1.23223));
        cl6->add(geos::geom::Coordinate(minX + 1.561587, maxY + 5.451355));
        cl6->add(geos::geom::Coordinate(minX + 4.126513, maxY + 3.526651));
        cl6->add(geos::geom::Coordinate(minX + 2.268415, maxY + 6.465681));
        cl6->add(geos::geom::Coordinate(maxX + 1.23456, maxY + 2.89462));
        cl6->add(geos::geom::Coordinate(maxX + 1.65156, minY + 5.54655));
        cl6->add(geos::geom::Coordinate(maxX + 1.21465, minY + 1.123544));
        cl6->add(geos::geom::Coordinate(minX + 1.11123, minY + 1.23223));
        geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl6);
        geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
        poly_vec.push_back(poly6);
    }
    return poly_vec;

}

TEST_SUITE("GLIN bulk load and find") {
    TEST_CASE("test bulk load and find function non-random") {
        //vector of polygans
        std::vector<Geometry *> poly_vec;
        std::vector<Geometry *> find_result;
        double error_bound = 10;
        std::vector<tuple<double, double, double, double>> pieces;
        alex::Glin<double, geos::geom::Geometry *> index;
        int count_filter = 0;
        //create segment line
        geos::geom::Coordinate pv1(-1, -2);
        geos::geom::Coordinate pv2(3, 6);
        geos::geom::LineSegment seg2(pv1, pv2);

        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);;
//
//
        geos::geom::CoordinateArraySequence *cl3 = new CoordinateArraySequence();
        cl3->add(geos::geom::Coordinate(20, 10));
        cl3->add(geos::geom::Coordinate(30, 60));
        cl3->add(geos::geom::Coordinate(80, 30));
        cl3->add(geos::geom::Coordinate(30, 10));
        cl3->add(geos::geom::Coordinate(20, 10));
        geos::geom::LinearRing *lr4 = global_factory->createLinearRing(cl3);
        geos::geom::LinearRing *outer4 = lr4;
        geos::geom::Polygon *query = global_factory->createPolygon(outer4, NULL);

        int num_of_poly = 100;
        for (int i = 0; i < num_of_poly; i++) {
            geos::geom::CoordinateArraySequence *cl6 = new CoordinateArraySequence();
            cl6->add(geos::geom::Coordinate(i + 10, i + 40));
            cl6->add(geos::geom::Coordinate(i, i + 50));
            cl6->add(geos::geom::Coordinate(i + 10, i + 70));
            cl6->add(geos::geom::Coordinate(i + 5, i + 30));
            cl6->add(geos::geom::Coordinate(i + 10, i + 40));
            geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl6);
            geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
            poly_vec.push_back(poly6);
        }
        // count intersection and compare with the find result.
        double count = 0;
        for (std::size_t i = 0, end = poly_vec.size(); i != end; ++i) {
            if (query->contains(poly_vec[i])) {
                count += 1;
            }
#ifdef PIECE
            if (query->intersects(poly_vec[i])) {
                count += 1;
            }
#endif
        }

        //load into index
        index.glin_bulk_load(poly_vec, error_bound, "z", -10.0, -10.0, 5.0, 5.0, pieces);
        // Iterator from beginning to end
        int num_keys = 0;
        for (auto it = index.begin(); it != index.end(); ++it) {
            num_keys++;
            // std::cout<<"key at this iterator" <<it.key() << endl;
        }
        CHECK_EQ(100, num_keys);

        index.glin_find(query, "z", -10.0, -10.0, 3.0, 3.0, pieces, find_result, count_filter);
        CHECK(find_result.size() == count);


//        CHECK(count > 0);
    }

    TEST_CASE("test curve projection load and find") {


        double num_of_poly = 10;
        std::vector<geos::geom::Geometry *> poly_vec = create_geoms_random(num_of_poly);
        double start = 0;
        double end = 0;
        double query_size = 1;
        std::vector<geos::geom::Geometry *> poly_vec_qw = create_geoms_random(query_size);
        alex::Glin<double, geos::geom::Geometry *> index;
        std::vector<tuple<double, double, double, double>> pieces_non_radm;
        std::vector<Geometry *> find_result_non_radm;
        int count_filter = 0;
        double piecelimit = 10;
        index.glin_bulk_load(poly_vec, piecelimit, "z", -300.0, -100.0, 0.05, 0.05, pieces_non_radm);
//        std::cout << pieces_non_radm.size() << std::endl;
        index.glin_find(poly_vec_qw[0], "z", -300.0, -100.0, 0.05, 0.05, pieces_non_radm, find_result_non_radm,
                        count_filter);
        double count = 0;
        for (auto &i: poly_vec) {
            if (poly_vec_qw[0]->contains(i)) {
                count += 1;
            }
#ifdef  PIECE
            if (poly_vec_qw[0]->intersects(i)) {
                count += 1;
            }
#endif
        }
        std::cout << "result from index search is " << find_result_non_radm.size() << " and " <<"Ground truth is "<< count << std::endl;
        CHECK(find_result_non_radm.size() == count);
    }

    TEST_CASE("non-randam find test") {
        std::vector<Geometry *> poly_vec;
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        alex::Glin<double, geos::geom::Geometry *> index_non_radm;
        std::vector<Geometry *> find_result_non_radm;
        double error_bound = 10;
        std::vector<tuple<double, double, double, double>> pieces_non_radm;
        int count_filter = 0;
        //create segment line
        geos::geom::Coordinate pv1(-1, -2);
        geos::geom::Coordinate pv2(3, 6);
        geos::geom::LineSegment seg_non_radm(pv1, pv2);

        int length = 20;
        int num_of_poly = 2000;
        for (int i = 0; i < num_of_poly; i++) {
            double minX = i;
            double minY = i;
            double maxX = i + length;
            double maxY = i + length;
            geos::geom::CoordinateArraySequence *cl6 = new CoordinateArraySequence();
            cl6->add(geos::geom::Coordinate(minX, minY));
            cl6->add(geos::geom::Coordinate(minX, maxY));
            cl6->add(geos::geom::Coordinate(maxX, maxY));
            cl6->add(geos::geom::Coordinate(maxX, minY));
            cl6->add(geos::geom::Coordinate(minX, minY));
            geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl6);
            geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
            poly_vec.push_back(poly6);
        }
        // construct query geometry
        geos::geom::CoordinateArraySequence *cl3 = new CoordinateArraySequence();
        cl3->add(geos::geom::Coordinate(2, 1));
        cl3->add(geos::geom::Coordinate(3, 6));
        cl3->add(geos::geom::Coordinate(8, 3));
        cl3->add(geos::geom::Coordinate(3, 1));
        cl3->add(geos::geom::Coordinate(2, 1));
        geos::geom::LinearRing *lr4 = global_factory->createLinearRing(cl3);
        geos::geom::LinearRing *outer4 = lr4;
        geos::geom::Polygon *query = global_factory->createPolygon(outer4, NULL);

        index_non_radm.loadCurve(poly_vec, error_bound, "z", -3000.0, -1000.0, 5.0, 5.0, pieces_non_radm);
        index_non_radm.glin_find(query, "z", -3000.0, -1000.0, 5.0, 5.0, pieces_non_radm, find_result_non_radm,
                                 count_filter);
        // count intersection and compare with the find result.
        double count = 0;
        for (auto &i: poly_vec) {
            if (query->contains(i)) {
                count += 1;
            }
#ifdef PIECE
            if (query->intersects(i)) {
                count += 1;
            }
#endif
        }
        CHECK(find_result_non_radm.size() == count);
//        CHECK(count > 0);
    }

    TEST_CASE("random polygon find test with line_project without node skipping") {
        std::vector<Geometry *> poly_vec;
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        alex::Glin<double, geos::geom::Geometry *> index_radm;
        std::vector<Geometry *> find_result_radm;
        double error_bound = 20;
        std::vector<tuple<double, double, double, double>> pieces_radm;
        int count_filter = 0;
        //create segment line
        geos::geom::Coordinate pv1(0, 5);
        geos::geom::Coordinate pv2(1, 6);
        geos::geom::LineSegment seg_radm(pv1, pv2);

        int length = rand() % 100000 + 1;
        int num_of_poly = 2000;
        for (int i = 0; i < num_of_poly; i++) {
            double minX = rand() % 100000 + 1;
            double minY = rand() % 100000 + 1;
            double maxX = minX + length;
            double maxY = minY + length;
            geos::geom::CoordinateArraySequence *cl7 = new CoordinateArraySequence();
            cl7->add(geos::geom::Coordinate(minX, minY));
            cl7->add(geos::geom::Coordinate(minX, maxY));
            cl7->add(geos::geom::Coordinate(maxX, maxY));
            cl7->add(geos::geom::Coordinate(maxX, minY));
            cl7->add(geos::geom::Coordinate(minX, minY));
            geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl7);
            geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
            poly_vec.push_back(poly6);
        }
        double qminX = rand() % 100000 + 1;
        double qminY = rand() % 100000 + 1;
        double qmaxX = qminX + length;
        double qmaxY = qminY + length;
        geos::geom::CoordinateArraySequence *cl3 = new CoordinateArraySequence();
        cl3->add(geos::geom::Coordinate(qminX, qminY));
        cl3->add(geos::geom::Coordinate(qminX, qmaxY));
        cl3->add(geos::geom::Coordinate(qmaxX, qmaxY));
        cl3->add(geos::geom::Coordinate(qmaxX, qminY));
        cl3->add(geos::geom::Coordinate(qminX, qminY));
        geos::geom::LinearRing *lr4 = global_factory->createLinearRing(cl3);
        geos::geom::LinearRing *outer4 = lr4;
        geos::geom::Polygon *query = global_factory->createPolygon(outer4, NULL);

        index_radm.load(poly_vec, seg_radm, error_bound, pieces_radm);
        index_radm.find(query, seg_radm, pieces_radm, find_result_radm, count_filter);
        double count = 0;
        for (auto &i: poly_vec) {
            if (query->intersects(i)) {
                count += 1;
            }
        }
        CHECK(count > 0);
        CHECK_EQ(find_result_radm.size(), count);
    }
};

TEST_SUITE("index compare tests") {
    TEST_CASE("compare STRtree / quadtree and glin_with_mbr") {
        std::vector<geos::geom::Geometry *> poly_vec;
        std::vector<geos::geom::Geometry *> poly_vec_qw;
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        alex::Glin<double, geos::geom::Geometry *> index_non_radm;
        std::vector<geos::geom::Geometry *> find_result_comp_non_r;
        double error_bound = 100;
        std::vector<tuple<double, double, double, double>> pieces_non_radm;
        //create segment line
        geos::geom::Coordinate pv1(0, 0);
        geos::geom::Coordinate pv2(0, 6);
        geos::geom::LineSegment seg_non_radm(pv1, pv2);
        std::vector<boost::geometry::model::box<point_type>> boost_geom_vector;
        double num_of_poly = 3000;
        for (auto i = 0; i < num_of_poly; i++) {
            double length = (double) rand() / (RAND_MAX + 1.0);
            double minX = -i;
            double minY = i;
            double maxX = -(i + length + (rand() % 5));
            double maxY = i + length + (rand() % 5);
            geos::geom::CoordinateArraySequence *cl6 = new CoordinateArraySequence();
            cl6->add(geos::geom::Coordinate(minX + 1.11123, minY + 1.23223));
            cl6->add(geos::geom::Coordinate(minX + 1.561587, maxY + 5.451355));
            cl6->add(geos::geom::Coordinate(minX + 4.126513, maxY + 3.526651));
            cl6->add(geos::geom::Coordinate(minX + 2.268415, maxY + 6.465681));
            cl6->add(geos::geom::Coordinate(maxX + 1.23456, maxY + 2.89462));
            cl6->add(geos::geom::Coordinate(maxX + 1.65156, minY + 5.54655));
            cl6->add(geos::geom::Coordinate(maxX + 1.21465, minY + 1.123544));
            cl6->add(geos::geom::Coordinate(minX + 1.11123, minY + 1.23223));
            geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl6);
            geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
            poly_vec.push_back(poly6);
        }
/*
//         query window format as (-x,y)
 */
        for (auto i = 0; i < 35; i++) {
            double length = (double) rand() / (RAND_MAX + 1.0);
            double minX = -i;
            double minY = i;
            double maxX = -(i + length + (rand() % 5));
            double maxY = i + length + (rand() % 5);
            geos::geom::CoordinateArraySequence *clqw = new CoordinateArraySequence();
            clqw->add(geos::geom::Coordinate(minX + 0.56454, minY + 0.65325));
            clqw->add(geos::geom::Coordinate(minX + 1.56784, maxY + 1.05612));
            clqw->add(geos::geom::Coordinate(minX + 1.111111, maxY + 2.23456));
            clqw->add(geos::geom::Coordinate(minX + 2.659423, maxY + 0.568921));
            clqw->add(geos::geom::Coordinate(maxX + 1.26484, maxY + 1.56123));
            clqw->add(geos::geom::Coordinate(maxX + 1.214532, minY + 1.546842));
            clqw->add(geos::geom::Coordinate(maxX + 0.522135, minY + 1.222315));
            clqw->add(geos::geom::Coordinate(minX + 0.56454, minY + 0.65325));
            geos::geom::LinearRing *lrqw = global_factory->createLinearRing(clqw);
            geos::geom::Polygon *poly_qw = global_factory->createPolygon(lrqw, NULL);
            poly_vec_qw.push_back(poly_qw);
        }

        /*
         * Rtree running
         */
        geos::index::strtree::SimpleSTRtree rtree(1000);
        for (int i = 0; i < num_of_poly; i++) {
            rtree.insert(poly_vec[i]);
        }
        long double rtree_result_sum = 0.0;
        auto tree_start = std::chrono::high_resolution_clock::now();
        for (int (i) = 0; (i) < poly_vec_qw.size(); ++(i)) {
            std::vector<void *> Rtree_results;
            rtree.query(poly_vec_qw[i]->getEnvelopeInternal(), Rtree_results);
            std::vector<geos::geom::Geometry *> Rtree_refine;
            //refine the rtree result
            for (auto &Rtree_result: Rtree_results) {
                if (poly_vec_qw[i]->intersects((geos::geom::Geometry *) Rtree_result)) {
                    Rtree_refine.push_back((geos::geom::Geometry *) Rtree_result);
                }
            }
            rtree_result_sum += Rtree_refine.size();
        }


        /*
         * quadtree running
         */
        geos::index::quadtree::Quadtree quadtree;
        for (int i = 0; i < num_of_poly; i++) {
            quadtree.insert(poly_vec[i]->getEnvelopeInternal(), poly_vec[i]);
        }
        long double quadtree_res_sum = 0.0;
        auto quad_start = std::chrono::high_resolution_clock::now();
        for (int (i) = 0; (i) <1; ++(i)) {
            std::vector<void *> quadtree_res;
            quadtree.query(poly_vec_qw[i]->getEnvelopeInternal(), quadtree_res);
            //refine the quadtree result
            std::vector<geos::geom::Geometry *> quad_refine;
            for (auto &quadtree_re: quadtree_res) {

                if (poly_vec_qw[i]->intersects((geos::geom::Geometry *) quadtree_re)) {
                    quad_refine.push_back((geos::geom::Geometry *) quadtree_re);
                }
            }
            quadtree_res_sum += quad_refine.size();
        }
        long double quad_avg_res = quadtree_res_sum / poly_vec_qw.size();
//        std::cout << "quad tree avg is "<< setprecision(15) <<quad_avg_res << std::endl;

        auto quad_stop = std::chrono::high_resolution_clock::now();
        quadtree.insert(poly_vec_qw[0]->getEnvelopeInternal(),poly_vec_qw[0]);
        long double quadtree_res_sum1 = 0.0;
        for (int (i) = 0; (i) <1; ++(i)) {
            std::vector<void *> quadtree_res1;
            quadtree.query(poly_vec_qw[i]->getEnvelopeInternal(), quadtree_res1);
            //refine the quadtree result
            std::vector<geos::geom::Geometry *> quad_refine1;
            for (auto &quadtree_re: quadtree_res1) {

                if (poly_vec_qw[i]->intersects((geos::geom::Geometry *) quadtree_re)) {
                    quad_refine1.push_back((geos::geom::Geometry *) quadtree_re);
                }
            }
            quadtree_res_sum1 += quad_refine1.size();
        }
        CHECK(quadtree_res_sum +1 == quadtree_res_sum1);
        auto quad_duration = std::chrono::duration_cast<std::chrono::microseconds>(quad_stop - quad_start);
//        std::cout << "quadTree duration " << quad_duration.count() << " microseconds" << endl;
        /*
         * grond chose running time
         */
//        auto ground_start = std::chrono::high_resolution_clock::now();
//        double count = 0;
////        double count_sum = 0.0;
//        double avg_count_sum = 0.0;
//        for (auto i = 0; i < num_of_poly; i++) {
//            for (auto j = 0; j < poly_vec_qw.size(); j++) {
//                if (poly_vec_qw[j]->intersects(poly_vec[i])) {
//                    count += 1;
//                }
//            }
//            avg_count_sum = avg_count_sum + (count / poly_vec_qw.size());
//        }
//        double avg_ground_truth = avg_count_sum / num_of_poly;
//        auto ground_stop = std::chrono::high_resolution_clock::now();
//        auto ground_duration = std::chrono::duration_cast<std::chrono::microseconds>(ground_stop - ground_start);
////        std::cout << "ground truth duration " << ground_duration.count() << " microseconds" << endl;
////        cout << "ground truth: " << avg_ground_truth << endl;
//        CHECK(avg_rtree_result == avg_result);
//        CHECK(quad_avg_res == avg_result);
    }

    TEST_CASE("random data Rtree/quadtree and glin compare") {
//        std::cout << " query time comparision for random data " << endl;
        std::vector<Geometry *> poly_vec;
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        alex::Glin<double, geos::geom::Geometry *> index_radm;
        std::vector<Geometry *> find_result_radm;
        double pieceLimitation = 50;
        std::vector<tuple<double, double, double, double>> pieces_radm;
        int count_filter = 0;
        //create segment line
        geos::geom::Coordinate pv1(0, 5);
        geos::geom::Coordinate pv2(1, 6);
        geos::geom::LineSegment seg_radm(pv1, pv2);

        double length = rand() % 100000 + 1;
        int num_of_poly = 2000;
        for (int i = 0; i < num_of_poly; i++) {
            double minX = rand() % 100000 + 1;
            double minY = rand() % 100000 + 1;
            double maxX = minX + length;
            double maxY = minY + length;
            geos::geom::CoordinateArraySequence *cl7 = new CoordinateArraySequence();
            cl7->add(geos::geom::Coordinate(minX, minY));
            cl7->add(geos::geom::Coordinate(minX + 1, maxY + 1));
            cl7->add(geos::geom::Coordinate(minX, maxY));
            cl7->add(geos::geom::Coordinate(minX + 2, maxY));
            cl7->add(geos::geom::Coordinate(maxX, maxY));
            cl7->add(geos::geom::Coordinate(maxX, minY));
            cl7->add(geos::geom::Coordinate(maxX, minY + 1));
            cl7->add(geos::geom::Coordinate(minX, minY));
            geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl7);
            geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
            poly_vec.push_back(poly6);
        }
//        std::cout << "number of geometry is " << num_of_poly << endl;
        double qminX = rand() % 100000 + 1;
        double qminY = rand() % 100000 + 1;
        double qmaxX = qminX + length;
        double qmaxY = qminY + length;
        geos::geom::CoordinateArraySequence *cl3 = new CoordinateArraySequence();
        cl3->add(geos::geom::Coordinate(qminX, qminY));
        cl3->add(geos::geom::Coordinate(qminX, qmaxY));
        cl3->add(geos::geom::Coordinate(qmaxX, qmaxY));
        cl3->add(geos::geom::Coordinate(qmaxX, qminY));
        cl3->add(geos::geom::Coordinate(qminX, qminY));
        geos::geom::LinearRing *lr4 = global_factory->createLinearRing(cl3);
        geos::geom::LinearRing *outer4 = lr4;
        geos::geom::Polygon *query = global_factory->createPolygon(outer4, NULL);

        index_radm.bulk_load_with_lineseg(poly_vec, seg_radm, pieceLimitation, pieces_radm);
        auto glin_start = std::chrono::high_resolution_clock::now();
        index_radm.find_with_lineseg(query, seg_radm, pieces_radm, find_result_radm, count_filter);
        auto glin_stop = std::chrono::high_resolution_clock::now();
        auto glin_duration = std::chrono::duration_cast<std::chrono::microseconds>(glin_stop - glin_start);
//        std::cout << "rand glin duration " << glin_duration.count() << " microseconds" << endl;


        // get r-rtree running time
        std::vector<void *> Rtree_results;
        std::vector<geos::geom::Geometry *> Rtree_refine;
        geos::index::strtree::SimpleSTRtree rtree(1000);
        for (int i = 0; i < num_of_poly; i++) {
            rtree.insert(poly_vec[i]);
        }
        auto tree_start = std::chrono::high_resolution_clock::now();
        rtree.query(query->getEnvelopeInternal(), Rtree_results);

        //refine the rtree result
        for (auto &Rtree_result: Rtree_results) {
            if (query->intersects((geos::geom::Geometry *) Rtree_result)) {
                Rtree_refine.push_back((geos::geom::Geometry *) Rtree_result);
            }
        }

        auto tree_stop = std::chrono::high_resolution_clock::now();
        auto tree_duration = std::chrono::duration_cast<std::chrono::microseconds>(tree_stop - tree_start);
//        std::cout << "rand rtree duration " << tree_duration.count() << " microseconds" << endl;

        // test quadtree time
        std::vector<void *> quadtree_res;
        std::vector<geos::geom::Geometry *> quad_refine;
        geos::index::quadtree::Quadtree quadtree;
        for (int i = 0; i < num_of_poly; i++) {
            quadtree.insert(poly_vec[i]->getEnvelopeInternal(), poly_vec[i]);
        }
        auto quad_start = std::chrono::high_resolution_clock::now();
        quadtree.query(query->getEnvelopeInternal(), quadtree_res);
        //refine the quadtree result
        for (auto &quadtree_re: quadtree_res) {
            if (query->intersects((geos::geom::Geometry *) quadtree_re)) {
                quad_refine.push_back((geos::geom::Geometry *) quadtree_re);
            }
        }
        auto quad_stop = std::chrono::high_resolution_clock::now();
        auto quad_duration = std::chrono::duration_cast<std::chrono::microseconds>(quad_stop - quad_start);
//        std::cout << "rand quadTree duration " << quad_duration.count() << " microseconds" << endl;

        //grond chose running time
        auto ground_start = std::chrono::high_resolution_clock::now();

        double count = 0;
        for (auto i = 0; i < num_of_poly; i++) {
            if (query->intersects(poly_vec[i])) {
                count += 1;
            }
        }
        auto ground_stop = std::chrono::high_resolution_clock::now();
        auto ground_duration = std::chrono::duration_cast<std::chrono::microseconds>(ground_stop - ground_start);
//        std::cout << "rand ground truth duration " << ground_duration.count() << " microseconds" << endl;

//        std::cout << "ground truth: " << count << std::endl;
//        std::cout << " glin result " << double(find_result_radm.size()) << std::endl;
//        std::cout << " rtree rusult "<< double(Rtree_refine.size()) << std::endl;

        CHECK(find_result_radm.size() == count);
//        CHECK(count > 0);
        CHECK(Rtree_refine.size() == count);
        CHECK(quad_refine.size() == count);
//        CHECK(tree_duration.count() > glin_duration.count());
    }

    TEST_CASE("modified alex iterator test") {
        std::vector<Geometry *> poly_vec;
        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);
        alex::Glin<double, geos::geom::Geometry *> index1;
        std::vector<Geometry *> find_result;
        double error_bound = 30;
        std::vector<tuple<double, double, double, double>> pieces;
        //create segment line
        geos::geom::Coordinate pv1(-1, -2);
        geos::geom::Coordinate pv2(3, 6);
        geos::geom::LineSegment seg_non_radm(pv1, pv2);
        int length = 20;
        int num_of_poly = 2000;
        for (int i = 0; i < num_of_poly; i++) {
            double minX = i;
            double minY = i;
            double maxX = i + length;
            double maxY = i + length;
            geos::geom::CoordinateArraySequence *cl6 = new CoordinateArraySequence();
            cl6->add(geos::geom::Coordinate(minX, minY));
            cl6->add(geos::geom::Coordinate(minX, maxY));
            cl6->add(geos::geom::Coordinate(maxX, maxY));
            cl6->add(geos::geom::Coordinate(maxX, minY));
            cl6->add(geos::geom::Coordinate(minX, minY));
            geos::geom::LinearRing *lr6 = global_factory->createLinearRing(cl6);
            geos::geom::Polygon *poly6 = global_factory->createPolygon(lr6, NULL);
            poly_vec.push_back(poly6);
        }
        index1.bulk_load_with_lineseg(poly_vec, seg_non_radm, error_bound, pieces);
        geos::geom::CoordinateArraySequence *cl7 = new CoordinateArraySequence();
        cl7->add(geos::geom::Coordinate(50, 80));
        cl7->add(geos::geom::Coordinate(30, 10));
        cl7->add(geos::geom::Coordinate(30, 90));
        cl7->add(geos::geom::Coordinate(20, 60));
        cl7->add(geos::geom::Coordinate(50, 80));
        geos::geom::LinearRing *outer4 = global_factory->createLinearRing(cl7);
        geos::geom::Polygon *query_window = global_factory->createPolygon(outer4, NULL);
        int count_filter = 0;
        index1.find_with_lineseg(query_window, seg_non_radm, pieces, find_result, count_filter);

        int count = 0;
        for (auto &i: poly_vec) {
            if (query_window->intersects(i)) {
                count += 1;
            }
        }
        CHECK(count == find_result.size());
        CHECK(find_result.size() <= count_filter);
    }

    TEST_CASE("quadtree and boost"){
        typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> point_type;
        typedef boost::geometry::model::box<point_type> box;
        typedef std::pair<box, int> Value;
        boost::geometry::index::rtree<Value, boost::geometry::index::linear<16>> boost_rtree;
        geos::index::quadtree::Quadtree quadtree;

        box box1(point_type(0,0),point_type(5.0,5.0));
        box box2(point_type(1,1),point_type(4,4));
        box box3(point_type(2,2), point_type(3,3));
        box box4(point_type(0,0),point_type(5.0,5.0));

        boost_rtree.insert(std::make_pair(box1,1));
        boost_rtree.insert(std::make_pair(box2,2));
        boost_rtree.insert(std::make_pair(box3,3));
        boost_rtree.insert(std::make_pair(box4,4));

        boost_rtree.remove(make_pair(box1,1));
        std::vector<Value> returned_values;
        boost_rtree.query(boost::geometry::index::intersects(box1),std::back_inserter(returned_values));
        for(auto candi: returned_values){
        }

        std::unique_ptr<geos::geom::PrecisionModel> pm(new geos::geom::PrecisionModel());
        geos::geom::GeometryFactory::Ptr global_factory = geos::geom::GeometryFactory::create(pm.get(), -1);

        geos::geom::Envelope env1(0,5,0,5);
        geos::geom::Envelope env2(1,4,1,4);
        geos::geom::Envelope env3(2,3,2,3);

        geos::geom::CoordinateArraySequence *cl01 = new CoordinateArraySequence();
        cl01->add(geos::geom::Coordinate(0,0));
        cl01->add(geos::geom::Coordinate(0,5));
        cl01->add(geos::geom::Coordinate(5,5));
        cl01->add(geos::geom::Coordinate(5,0));
        cl01->add(geos::geom::Coordinate(0,0));
        geos::geom::LinearRing *lr01 = global_factory->createLinearRing(cl01);
        geos::geom::Polygon *p1 = global_factory->createPolygon(lr01, NULL);
        geos::geom::Polygon *p1_clone = global_factory->createPolygon(lr01, NULL);

        geos::geom::CoordinateArraySequence *cl02 = new CoordinateArraySequence();
        cl02->add(geos::geom::Coordinate(1,1));
        cl02->add(geos::geom::Coordinate(1,4));
        cl02->add(geos::geom::Coordinate(4,4));
        cl02->add(geos::geom::Coordinate(4,1));
        cl02->add(geos::geom::Coordinate(1,1));
        geos::geom::LinearRing *lr02 = global_factory->createLinearRing(cl02);
        geos::geom::Polygon *p2 = global_factory->createPolygon(lr02, NULL);

        geos::geom::CoordinateArraySequence *cl03 = new CoordinateArraySequence();
        cl03->add(geos::geom::Coordinate(2,2));
        cl03->add(geos::geom::Coordinate(2,3));
        cl03->add(geos::geom::Coordinate(3,3));
        cl03->add(geos::geom::Coordinate(3,2));
        cl03->add(geos::geom::Coordinate(2,2));
        geos::geom::LinearRing *lr03 = global_factory->createLinearRing(cl03);
        geos::geom::Polygon *p3 = global_factory->createPolygon(lr03, NULL);


        geos::geom::CoordinateArraySequence *cl04 = new CoordinateArraySequence();
        cl04->add(geos::geom::Coordinate(0,0));
        cl04->add(geos::geom::Coordinate(0,5));
        cl04->add(geos::geom::Coordinate(3,3));
        cl04->add(geos::geom::Coordinate(5,5));
        cl04->add(geos::geom::Coordinate(5,0));
        cl04->add(geos::geom::Coordinate(0,0));
        geos::geom::LinearRing *lr04 = global_factory->createLinearRing(cl04);
        geos::geom::Polygon *p4 = global_factory->createPolygon(lr04, NULL);

        quadtree.insert(p1->getEnvelopeInternal(),p1);
        quadtree.insert(p2->getEnvelopeInternal(),p2);
        quadtree.insert(p3->getEnvelopeInternal(),p3);
        quadtree.insert(p4->getEnvelopeInternal(),p4);

        std::vector<void *> quadtree_res;
        quadtree.remove(p1->getEnvelopeInternal(),p1_clone);
        quadtree.query(p1->getEnvelopeInternal(),quadtree_res);
        assert(quadtree_res.size() == 4);

        quadtree_res.clear();
        quadtree.remove(p1->getEnvelopeInternal(),p1);
        quadtree.query(p1->getEnvelopeInternal(),quadtree_res);
        assert(quadtree_res.size() == 3);
    }


};

TEST_SUITE("avgdiff"){
    TEST_CASE("test_avg_diff"){
        alex::Glin<double, geos::geom::Geometry *> index_g;
        const int size = 6;
        std::pair<double, double> records[size];
        records[0].first = 2;
        records[0].second = 5;
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
        piecewise(records, size, 3, pieces);
//        for (int i = 0; i < pieces.size(); i++) {
//            std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
//                      << get<3>(pieces[i]) << endl;
//        }
        double avg_diff = 0.0 ;
        avg_diff = index_g.avg_diff(pieces);
double test_result = 2.0/9.0;
//std::cout << test_result << endl;
CHECK_EQ(avg_diff, test_result);

    }
};


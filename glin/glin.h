//
// Created by juno on 7/18/21.
//
#include "alex.h"
//#include "projection.h"
#include "piecewise.h"
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
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
#include <geos/io/WKTReader.h>
#include <geos/algorithm/Angle.h>

namespace alex {
    template<class T, class P, class Compare = AlexCompare,
            class Alloc = std::allocator<std::pair<T, P>>,
            bool allow_duplicates = true>
    class Glin : public Alex<T, P, Compare, Alloc, allow_duplicates> {

    public:
        std::chrono::nanoseconds index_probe_duration = std::chrono::nanoseconds::zero();
        std::chrono::nanoseconds index_refine_duration = std::chrono::nanoseconds::zero();
        double avg_num_visited_leaf = 0.0;
        double avg_num_loaded_leaf = 0.0;

/*
 * line segment creation
 */
        geos::geom::LineSegment create_line_seg(double x1, double x2, double k) {
            geos::geom::Coordinate pv1(x1, k * x1);
            geos::geom::Coordinate pv2(x2, k * x2);
            geos::geom::LineSegment seg(pv1, pv2);
            return seg;
        }

        /*
         * get perpendicular line of a line segment
         */
        geos::geom::LineSegment get_perpendicular_line(geos::geom::LineSegment segment) {
            if (geos::algorithm::Angle::toDegrees(segment.angle()) != 0 &&
                geos::algorithm::Angle::toDegrees(segment.angle()) != 90) {
                double current_slope = (segment.p1.y - segment.p0.y) / (segment.p1.x - segment.p0.x);
                double perpendicular_slope = -1.0 / current_slope;
                geos::geom::LineSegment perpen_line = create_line_seg(0, segment.p1.x, perpendicular_slope);
                return perpen_line;
            } else if (geos::algorithm::Angle::toDegrees(segment.angle()) == 0) {
                geos::geom::Coordinate pv1(0, 0);
                geos::geom::Coordinate pv2(0, 5);
                geos::geom::LineSegment perpen_line(pv1, pv2);
                return perpen_line;
            } else if (geos::algorithm::Angle::toDegrees(segment.angle()) == 90) {
                geos::geom::LineSegment perpen_line = create_line_seg(0, 5, 0);
                return perpen_line;
            }
        }

        /*
         * traditional load with line projection
         */
        void load(std::vector<geos::geom::Geometry *> geom, geos::geom::LineSegment segment, double pieceLimitation,
             std::vector<std::tuple<double, double, double, double>> &pieces) {
            auto num_of_keys = geom.size();
            //values for bulkload
            std::pair<double, double> *values = new std::pair<double, double>[num_of_keys];
            //values save for future search
            std::pair<double, geos::geom::Geometry *> *new_values = new std::pair<double, geos::geom::Geometry *>[num_of_keys];

            for (auto i = 0; i < num_of_keys; i++) {
                long double min = 0;
                long double max = 0;
                // add projected range start and end to the first pair
                shape_projection(geom[i], segment, min, max);
                //assert((max-min)!=0);
                values[i].first = min;
                values[i].second = max;
                // store a startpoint, geometry pair for future using to load into actual index
                new_values[i].first = min;
                new_values[i].second = geom[i];
            }
            piecewise(values, num_of_keys, pieceLimitation, pieces);

            delete[] values;
            // sort by start point
            std::sort(new_values, new_values + num_of_keys);

            alex::Alex<T, P>::bulk_load(new_values, num_of_keys);
            delete[] new_values;
            // into alex
        }

        /*
         * load with curve projection
         */
        void loadCurve(std::vector<geos::geom::Geometry *> geom, double pieceLimitation, std::string curve_type,
                       double cell_xmin, double cell_ymin,
                       double cell_x_intvl, double cell_y_intvl,
                       std::vector<std::tuple<double, double, double, double>> &pieces) {
            auto num_of_keys = geom.size();
            //values for bulkload
            std::pair<double, double> *values = new std::pair<double, double>[num_of_keys];
            //values save for future search
            std::pair<double, geos::geom::Geometry *> *new_values = new std::pair<double, geos::geom::Geometry *>[num_of_keys];

            for (auto i = 0; i < num_of_keys; i++) {
                double min = 0;
                double max = 0;
                // add projected range start and end to the first pair
                curve_shape_projection(geom[i], curve_type, cell_xmin, cell_ymin, cell_x_intvl, cell_y_intvl, min, max);
                //assert((max-min)!=0);
                values[i].first = min;
                values[i].second = max;
                // store a startpoint, geometry pair for future using to load into actual index
                new_values[i].first = min;
                new_values[i].second = geom[i];
            }
#ifdef PIECE
            piecewise(values, num_of_keys, pieceLimitation, pieces);
#endif

            delete[] values;
            // sort by start point
            std::sort(new_values, new_values + num_of_keys);
            // to print out cdf
//            for(int i =0; i < num_of_keys; i++){
//                std::cout<<"start_point," <<  i << ","<< std::to_string( new_values[i].first )<< std::endl;
//            }
            alex::Alex<T, P>::bulk_load(new_values, num_of_keys);
            delete[] new_values;
        }


        void bulk_load_with_lineseg(std::vector<geos::geom::Geometry *> geom, geos::geom::LineSegment segment,
                                    double pieceLimitation,
                                    std::vector<std::tuple<double, double, double, double>> &pieces) {
            load(geom, segment, pieceLimitation, pieces);
            geos::geom::LineSegment perpendicular_line = get_perpendicular_line(segment);
            auto it_start = this->begin();
            auto it_end = this->end();
            // Generate the MBR in each leaf node (data node)
            for (auto it = it_start; it != it_end; it.it_update_lineseg(perpendicular_line)) {
            }
        }

        void glin_bulk_load(std::vector<geos::geom::Geometry *> geom, double pieceLimitation,
                            std::string curve_type,
                            double cell_xmin, double cell_ymin,
                            double cell_x_intvl, double cell_y_intvl,
                            std::vector<std::tuple<double, double, double, double>> &pieces) {
            loadCurve(geom, pieceLimitation, curve_type, cell_xmin, cell_ymin, cell_x_intvl, cell_y_intvl,
                      pieces);
            auto it_start = this->begin();
            auto it_end = this->end();
            // Generate the MBR in each leaf node (data node)
            for (auto it = it_start; it != it_end; it.it_update_mbr()) {
            }
        }

        static bool sortbysec(const std::tuple<double, double, double, double> &a,
                              const std::tuple<double, double, double, double> &b) {
            return (std::get<0>(a) < std::get<0>(b));
        }

        /*
         * find with line projection without skipping nodes
         */
        void find(geos::geom::Geometry *query_window, geos::geom::LineSegment segment,
                  std::vector<std::tuple<double, double, double, double>> &pieces,
                  std::vector<geos::geom::Geometry *> &find_result,
                  int &count_filter) {
            // every time start a finding, the find_result should be empty for each find
            assert(find_result.empty());
            assert(count_filter == 0);

            //count index probe time
            auto start_find = std::chrono::high_resolution_clock::now();
            auto iterators = index_probe(query_window, segment, pieces);
            auto end_find = std::chrono::high_resolution_clock::now();
            index_probe_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_find - start_find);

            // refine tim=
            auto start_refine = std::chrono::high_resolution_clock::now();
            // refine the query result
            refine(query_window, iterators.first, iterators.second, find_result, count_filter);
            auto end_refine = std::chrono::high_resolution_clock::now();
//            std::cout << "Num visited leaf nodes in refine: " << it.num_visited_leaf << std::endl;
            index_refine_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_refine - start_refine);

        }

        /*
         * find with original line projection with node skipping
         */
        void find_with_lineseg(geos::geom::Geometry *query_window, geos::geom::LineSegment segment,
                               std::vector<std::tuple<double, double, double, double>> &pieces,
                               std::vector<geos::geom::Geometry *> &find_result,
                               int &count_filter) {
            // every time start a finding, the find_result should be empty for each find
            assert(find_result.empty());
            assert(count_filter == 0);
            //count index probe time
            auto start_find = std::chrono::high_resolution_clock::now();
            auto iterators = index_probe(query_window, segment, pieces);
            auto end_find = std::chrono::high_resolution_clock::now();
            index_probe_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_find - start_find);

            // refine time
            auto start_refine = std::chrono::high_resolution_clock::now();
            // refine the query result
            refine_with_lineseg(query_window, iterators.first, iterators.second, segment, find_result, count_filter);
            auto end_refine = std::chrono::high_resolution_clock::now();
//            std::cout << "Num visited leaf nodes in refine: " << it.num_visited_leaf << std::endl;
            index_refine_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_refine - start_refine);
        }

        /*
         * find with curve projection with node skipping
         */
        void glin_find(geos::geom::Geometry *query_window, std::string curve_type,
                       double cell_xmin, double cell_ymin,
                       double cell_x_intvl, double cell_y_intvl,
                       std::vector<std::tuple<double, double, double, double>> &pieces,
                       std::vector<geos::geom::Geometry *> &find_result,
                       int &count_filter) {
            // every time start a finding, the find_result should be empty for each find
            assert(find_result.empty());
            assert(count_filter == 0);
            //count index probe time
            auto start_find = std::chrono::high_resolution_clock::now();
            auto iterator_end = index_probe_curve(query_window, curve_type,
                                               cell_xmin, cell_ymin,
                                               cell_x_intvl, cell_y_intvl, pieces);
            auto end_find = std::chrono::high_resolution_clock::now();
            index_probe_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_find - start_find);
            // refine time
            auto start_refine = std::chrono::high_resolution_clock::now();
            // refine the query result
//            refine_with_curveseg(query_window, iterators.first, iterators.second, find_result, count_filter);
            refine_with_curveseg(query_window,iterator_end.first, iterator_end.second,find_result, count_filter );
            auto end_refine = std::chrono::high_resolution_clock::now();
//            std::cout << "Num visited leaf nodes in refine: " << it.num_visited_leaf << std::endl;
            index_refine_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_refine - start_refine);
        }

        /*
         * original index probe with line projection
         */
        std::pair<typename alex::Alex<T, P>::Iterator, typename alex::Alex<T, P>::Iterator> index_probe
                (geos::geom::Geometry *query_window, geos::geom::LineSegment segment,
                 std::vector<std::tuple<double, double, double, double>> &pieces) {
            // project + augment
            long double min_start;
            long double max_end;
            shape_projection(query_window, segment, min_start, max_end);
            // use current end point to search which bucket the records belong to
            std::vector<std::tuple<double, double, double, double>>::iterator up;
            up = std::lower_bound(pieces.begin(), pieces.end(), std::make_tuple(max_end, -1, -1, -1), sortbysec);
            //augment the start point
            if (max_end > std::get<0>(pieces[pieces.size() - 1])) {
                min_start = min_start - std::get<1>(pieces[(up - pieces.begin() - 1)]);
            } else {
                min_start = min_start - std::get<1>(pieces[(up - pieces.begin())]);
            }
#ifdef DEBUG

            std::cout<< "the current end is " <<current_end << "current pieces is " <<  std::get<0>(pieces[up - pieces.begin() ]) <<"current piece -1 is" <<  std::get<0>(pieces[(up - pieces.begin() - 1)]) << endl;
            assert(current_end <= std::get<0>(pieces[up - pieces.begin() ]));
            assert(current_end > std::get<0>(pieces[(up - pieces.begin() - 1)]));
#endif
            auto it_start = alex::Alex<T, P>::lower_bound(min_start);
            auto it_end = alex::Alex<T, P>::upper_bound(max_end);
            return std::make_pair(it_start, it_end);
        }

        /*
         *  index probe with curve projection
         */
        std::pair<typename alex::Alex<T, P>::Iterator, double> index_probe_curve
                (geos::geom::Geometry *query_window, std::string curve_type,
                 double cell_xmin, double cell_ymin,
                 double cell_x_intvl, double cell_y_intvl,
                 std::vector<std::tuple<double, double, double, double>> &pieces) {
            // project + augment
            double min_start;
            double max_end;
            curve_shape_projection(query_window, curve_type, cell_xmin, cell_ymin, cell_x_intvl, cell_y_intvl,
                                   min_start, max_end);
            //   std::cout << "find poly start" << min_start << " find poly end " << max_end << endl;
#ifdef PIECE
            // use current end point to search which bucket the records belong to
            std::vector<std::tuple<double, double, double, double>>::iterator it;
            it = std::lower_bound(pieces.begin(), pieces.end(), std::make_tuple(min_start, -1, -1, -1), sortbysec);
            // The min function here is to make sure that the iterator never reaches the end of the pieces vector even if
            // the query window itself may exceed the max end point of pieces
            double start_augment_zmin = std::numeric_limits<double>::max();
//            // Should always check the max range of the upper bound
            while (it != pieces.end()) {
                start_augment_zmin = std::min(start_augment_zmin, std::get<1>(pieces[it - pieces.begin()]));
                it++;
            }
            min_start = start_augment_zmin;
#endif
#ifdef DEBUG
            std::cout<< "the current end is " <<current_end << "current pieces is " <<  std::get<0>(pieces[up - pieces.begin() ]) <<"current piece -1 is" <<  std::get<0>(pieces[(up - pieces.begin() - 1)]) << endl;
            assert(current_end <= std::get<0>(pieces[up - pieces.begin() ]));
            assert(current_end > std::get<0>(pieces[(up - pieces.begin() - 1)]));
#endif
            auto it_start = alex::Alex<T, P>::lower_bound(min_start);
//            auto it_end = alex::Alex<T, P>::upper_bound(max_end);
//            return std::make_pair(it_start, it_end);
            return std::make_pair(it_start, max_end);
        }

        /*
         * original refine without any node skipping
         */
        void refine(geos::geom::Geometry *query_window, typename alex::Alex<T, P>::Iterator it_start,
                    typename alex::Alex<T, P>::Iterator it_end, std::vector<geos::geom::Geometry *> &find_result,
                    int &count_filter) {
            // refine the query result
            for (auto it = it_start; it != it_end; it++) {
                geos::geom::Geometry *payload = it.payload();
                if (query_window->intersects(payload)) {
                    find_result.push_back(payload);
                }
                //count all geometries after the probe
                count_filter += 1;
            }
        }
/*
 * refine with line projection and skipping node with line segment checking
 */
        void refine_with_lineseg(geos::geom::Geometry *query_window, typename alex::Alex<T, P>::Iterator it_start,
                                 typename alex::Alex<T, P>::Iterator it_end, geos::geom::LineSegment seg,
                                 std::vector<geos::geom::Geometry *> &find_result, int &count_filter) {
            // refine the query result
            typename alex::Alex<T, P>::Iterator it;
            geos::geom::LineSegment project_seg = get_perpendicular_line(seg);
            long double query_start;
            long double query_end;

            shape_projection(query_window, project_seg, query_start, query_end);

            for (it = it_start; it != it_end; it.it_check_lineseg(query_start, query_end, it_end)) {
                geos::geom::Geometry *payload = it.payload();
                if (query_window->intersects(payload)) {
                    find_result.push_back(payload);
                }
                //count all geometries after the probe
                count_filter += 1;
//                std::cout << "num visited leaf " << it.num_visited_leaf << " num loaded leaf " << it.num_loaded_leaf << std::endl;

            }
            assert(find_result.size() != 0);
            assert(count_filter != 0);
            avg_num_visited_leaf = it.num_visited_leaf;
            avg_num_loaded_leaf = it.num_loaded_leaf;
//            std::cout << "num visited leaf " << it.num_visited_leaf << " num loaded leaf " << it.num_loaded_leaf << std::endl;
        }

        /*
         * refine with curve and skip node with mbr checking
         */
        void refine_with_curveseg(geos::geom::Geometry *query_window, typename alex::Alex<T, P>::Iterator it_start, double max_end,
                                  std::vector<geos::geom::Geometry *> &find_result, int &count_filter) {
            // refine the query result
            typename alex::Alex<T, P>::Iterator it;
            geos::geom::Envelope env_query_window = *query_window->getEnvelopeInternal();
            for (it = it_start; it.cur_leaf_ != nullptr && it.key() <= max_end; it.it_check_mbr(&env_query_window, max_end)) {
                geos::geom::Geometry *payload = it.payload();
#ifdef PIECE
                if (query_window->intersects(payload)) {
                    find_result.push_back(payload);
                }
#else
                if(query_window->contains(payload)){
                    find_result.push_back(payload);
                }
#endif
                //count all geometries after the probe
                count_filter += 1;
            }
//            assert(find_result.size() != 0);
//            assert(count_filter!=0);
            avg_num_visited_leaf = it.num_visited_leaf;
            avg_num_loaded_leaf = it.num_loaded_leaf;
//            std::cout << "num visited leaf " << it.num_visited_leaf << " num loaded leaf " << it.num_loaded_leaf << std::endl;
        }

        /*
         * insert function
         * input:  geometry tobe inserted
         *          piecewise function to determine which piece the inserted data should go
         *          insert by alex
         *
         */
        std::pair<typename alex::Alex<T, P>::Iterator, bool>
        insert(geos::geom::Geometry *geometry, geos::geom::LineSegment segment, double error_bound,
               std::vector<std::tuple<double, double, double, double>> &pieces) {
            // first project the inpute geomeotry
            long double range_start;
            long double range_end;
            shape_projection(geometry, segment, range_start, range_end);
            // std::cout << "insert start " << range_start << " insert end " << range_end << endl;
            auto res_start = alex::Alex<T, P>::insert(range_start, geometry);
            //upper postion
            // search which bucket the records belong to
#ifdef PIECE
            insert_pieces(range_start, range_end, error_bound, pieces);
#endif
            return res_start;
        }

        std::pair<typename alex::Alex<T, P>::Iterator, bool>
        glin_insert(std::tuple<geos::geom::Geometry*, geos::geom::Envelope*> geo_tuple, std::string curve_type,
                    double cell_xmin, double cell_ymin,
                    double cell_x_intvl, double cell_y_intvl, double pieceLimit,
                    std::vector<std::tuple<double, double, double, double>> &pieces) {
            // first project the inpute geomeotry
            double range_start;
            double range_end;
            geos::geom::Geometry*  geometry = std::get<0>(geo_tuple);
            geos::geom::Envelope*  envelope = std::get<1>(geo_tuple);
            curve_shape_projection(envelope, curve_type, cell_xmin, cell_ymin, cell_x_intvl, cell_y_intvl, range_start,
                                   range_end);
            // std::cout << "insert start " << range_start << " insert end " << range_end << endl;
            std::pair<typename alex::Alex<T, P>::Iterator, bool> res_start = alex::Alex<T, P>::insert(range_start,
                                                                                                      geometry);

            res_start.first.cur_leaf_->mbr.expandToInclude(envelope);
            // search which bucket the records belong to
#ifdef PIECE
            insert_pieces(range_start, range_end, pieceLimit, pieces);
#endif
            return res_start;
        }

        /*
         * insertion with line segment checking
         */
        std::pair<typename alex::Alex<T, P>::Iterator, bool>
        insert_with_lineseg(geos::geom::Geometry *geometry, geos::geom::LineSegment segment, double pieceLimit,
                            std::vector<std::tuple<double, double, double, double>> &pieces) {
            // first project the inpute geomeotry
            long double range_start;
            long double range_end;
            geos::geom::LineSegment perpen_segment;
            shape_projection(geometry, segment, range_start, range_end);
            // std::cout << "insert start " << range_start << " insert end " << range_end << endl;
            std::pair<typename alex::Alex<T, P>::Iterator, bool> res_start = alex::Alex<T, P>::insert(range_start,
                                                                                                      geometry);
            long double perpen_start;
            long double perpen_end;
            perpen_segment = get_perpendicular_line(segment);
            shape_projection(geometry, perpen_segment, perpen_start, perpen_end);
            if (perpen_start < res_start.first.cur_leaf_->line_seg_start) {
                res_start.first.cur_leaf_->line_seg_start = perpen_start;
            }
            if (perpen_end > res_start.first.cumulated_line_end) {
                res_start.first.cur_leaf_->line_seg_end = perpen_end;
            }
            //upper postion
            // search which bucket the records belong to
            insert_pieces(range_start, range_end, pieceLimit, pieces);
            return res_start;
        }

        double avg_error(std::vector<std::tuple<double, double, double, double>> &pieces) {
            long double error_sum = 0.0;
            long double error_avg = 0.0;

            for (int i = 0; i < pieces.size(); i++) {
                int count = std::get<2>(pieces[i]);
                double max = std::get<1>(pieces[i]);
                double sum = std::get<3>(pieces[i]);
                if (max != 0) {
                    double error = cal_error(count, max, sum);
                    error_sum += error;
                } else {
                    continue;
                }

            }
            error_avg = error_sum / pieces.size();
            return error_avg;

        }
        double cal_diff(int current_count, double current_zmin, double current_sum) {
            double current_average = current_sum / current_count;
            double error = std::abs(current_zmin - current_average) / current_average;
            return error;
        }

        double avg_diff(std::vector<std::tuple<double, double, double, double>> &pieces){
            long double error_sum = 0.0;
            long double error_avg = 0.0;
            for(int i = 1; i < pieces.size(); i++ ){
                double current_zmin = std::get<1>(pieces[i]);
                int count = std::get<2>(pieces[i]);
                double current_sum = std::get<3>(pieces[i]);
                if(current_sum!= 0 ){
                    double diff = cal_diff(count, current_zmin,current_sum);
                    error_sum += diff;
                }else{
                    continue;
                }

            }
            error_avg = error_sum/pieces.size();
            return error_avg;
        }

        // erase all key with certain key value using line segment
        int erase_lineseg(geos::geom::Geometry *geometry, geos::geom::LineSegment segment, double error_bound,
                  std::vector<std::tuple<double, double, double, double>> &pieces) {
            long double del_start;
            long double del_end;

            shape_projection(geometry, segment, del_start, del_end);
            // remove all key with certain key value in alex
            int num_erase = alex::Alex<T, P>::erase(del_start);
            // find the position of erase key
            std::vector<std::tuple<double, double, double, double>>::iterator erase_position;
            erase_position = std::upper_bound(pieces.begin(), pieces.end(), std::make_tuple(del_end, -1, -1, -1),
                                              sortbysec);

            double update_count = std::get<2>(pieces[erase_position - pieces.begin()]) - 1;
            double update_sum = (std::get<3>(pieces[erase_position - pieces.begin()])) - (del_end - del_start);
            std::get<2>(pieces[erase_position - pieces.begin()]) = update_count;
            std::get<3>(pieces[erase_position - pieces.begin()]) = update_sum;
            if (avg_error(pieces) > error_bound) {
//                std::cout << "please rebuild the index" << std::endl;
            }
            return num_erase;
        }

        /*
         * erase exact item that user would like to erase with node mbr checking
         */
        int erase(geos::geom::Geometry *envelope,std::string curve_type,
                  double cell_xmin, double cell_ymin,
                  double cell_x_intvl, double cell_y_intvl, double pieceLimitation,
                  std::vector<std::tuple<double, double, double, double>> &pieces) {
            double del_start;
            double del_end;
            curve_shape_projection(envelope,curve_type,cell_xmin,cell_ymin,cell_x_intvl,cell_y_intvl, del_start, del_end);
            // remove all key with certain key value in alex
            int num_erase = alex::Alex<T, P>::erase_geo(del_start, envelope);
            // find the position of erase key
            std::vector<std::tuple<double, double, double, double>>::iterator erase_position;
#ifdef PIECE
            erase_position = std::upper_bound(pieces.begin(), pieces.end(), std::make_tuple(del_end, -1, -1, -1),
                                              sortbysec);
            double update_count = std::get<2>(pieces[erase_position - pieces.begin()]) - 1;
            double update_sum = (std::get<3>(pieces[erase_position - pieces.begin()])) -  del_start;
            std::get<2>(pieces[erase_position - pieces.begin()]) = update_count;
            std::get<3>(pieces[erase_position - pieces.begin()]) = update_sum;
#endif
            return num_erase;
        }
    };


}


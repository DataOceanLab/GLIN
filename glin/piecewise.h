//
// Created by juno on 7/10/21.
//
#include "alex.h"
#include "array"

#ifndef ALEX_PIECEWISE_H
#define ALEX_PIECEWISE_H

//const int size = 0;
double cal_error(int current_count, double current_max, double current_sum) {
    double current_average = current_sum / current_count;
    double error = (current_max - current_average) / current_max;
    return error;
}

bool endpointLess(std::pair<double, double> a, std::pair<double, double> b) { return a.second < b.second; }

static bool sortbysec_insert(const std::tuple<double, double, double, double> &a,
                             const std::tuple<double, double, double, double> &b) {
    return (std::get<0>(a) < std::get<0>(b));
}

// the returned pieces aka piecewise function vectors with max range
//void piecewise(std::pair<double, double> records[], int size, double piece_limitation,
//               std::vector<std::tuple<double, double, double, double>> &pieces) {
//    //sort by range end point
//    std::sort(records, (records + size), endpointLess);
//    for(int i =0; i < size; i++){
//        std::cout<<"endpoint,"<< i << ","<< std::to_string( records[i].second )<< std::endl;
//    }
//    // maintain 3 parameter for bucket
//    int current_count = 1;
//    double current_max = records[0].second - records[0].first;
//    //push the very first start point as piecewise function's first piece
//    pieces.emplace_back(records[0].first, std::numeric_limits<double>::min(), std::numeric_limits<double>::min(),
//                       std::numeric_limits<double>::min());
//
//    double current_sum = current_max;
//
//    for (int i = 1; i < size; i++) {
//#ifdef DEBUG
//        std::cout<<"sorted record is " << records[i].first <<"  " << records[i].second << endl;
//        assert(records[i].second >= records[i-1].second);
//#endif
//
//        double current_range = records[i].second - records[i].first;
//
//        if(current_range != 0 ) {
//            assert(current_range != 0);
//            current_count += 1;
//            if(current_count > piece_limitation) {
//                double old_max = current_max;
//                double old_sum = current_sum;
//                current_max = std::max(current_max, current_range);
//                current_sum = current_sum + current_range;
//#ifdef DEBUG
//                cout<<"the current error to check if start a new pieace "<< current_error << endl;
//#endif
//#ifdef DEBUG
//                    std::cout<< "endpoint is " << records[i-1].second <<" max range " << old_max<<endl;
//#endif
//                    int target_offset = i - 1;
//                    //push where this pieces end to form the piece
//                    // check if the records share the same end point, if the next record is same, keep reading until next different
//                    // endpoint record.
////                    check if last record has 0 range. if has 0 range, then continue to next record
//                    if (records[target_offset].second - records[target_offset].first == 0) {
//                        current_count += 1;
//                        continue;
//                    }
//                    while (records[target_offset].second == records[target_offset + 1].second && target_offset < size) {
//#ifdef DEBUG
//                        std::cout << target_offset << "  offset " << endl;
//#endif
//                        old_max = std::max(old_max, current_range);
//                        current_count += 1;
//                        old_sum = current_sum;
//                        current_sum += records[target_offset + 1].second - records[target_offset + 1].first;
//                        target_offset += 1;
//                    }
//                    pieces.emplace_back(
//                            (std::make_tuple(records[target_offset].second, old_max, current_count - 1, old_sum)));
//#ifdef DEBUG
//                    cout<<"now having "<<pieces.size()<<" buckets"<<endl;
//#endif
//                    current_count = 1;
//                    current_max = current_range;
//                    current_sum = current_range;
//                    i = target_offset + 1;
//
//            }else{
//                current_max = std::max(current_max, current_range);
//                current_sum = current_sum + current_range;
//                if(i == size -1){
//                    pieces.emplace_back(records[size - 1].second, current_max, current_count, current_sum);
//                }
//                continue;
//            }
//        }else{
//            current_count+=1;
//            continue;
//        }
//
//    }
//    if(records[size -1].second == std::get<0>(pieces.back())){
//        return;
//    }else {
//        pieces.emplace_back(records[size - 1].second, current_max, current_count, current_sum);
//    }
//}

/*
 * pieceiwise function with Zmin
 */
void piecewise(std::pair<double, double> records[], int size, double piece_limitation,
               std::vector<std::tuple<double, double, double, double>> &pieces) {
    //sort by range end point
    std::sort(records, (records + size), endpointLess);
    //print out for cdf
//    for(int i =0; i < size; i++){
//        std::cout<<"endpoint,"<< i << ","<< std::to_string( records[i].second )<< std::endl;
//    }
    // maintain 3 parameter for bucket
    int current_count = 1;
    double current_min_zmin = records[0].first;
    //push the very first start point as piecewise function's first piece
    pieces.emplace_back(records[0].first, std::numeric_limits<double>::min(), std::numeric_limits<double>::min(),
                        std::numeric_limits<double>::min());

    double current_sum = current_min_zmin;

    for (int i = 1; i < size; i++) {
#ifdef DEBUG
        std::cout<<"sorted record is " << records[i].first <<"  " << records[i].second << endl;
        assert(records[i].second >= records[i-1].second);
#endif

        double current_zmin =  records[i].first;
            current_count += 1;
            if(current_count > piece_limitation) {
                double old_min_zmin = current_min_zmin;
                double old_sum = current_sum;
                current_min_zmin = std::min(current_min_zmin, current_zmin);
                current_sum = current_sum + current_zmin;
#ifdef DEBUG

                cout<<"the current error to check if start a new pieace "<< current_error << endl;
#endif
#ifdef DEBUG
                std::cout<< "endpoint is " << records[i-1].second <<" max range " << old_max<<endl;
#endif
                int target_offset = i - 1;
                //push where this pieces end to form the piece
                // check if the records share the same end point, if the next record is same, keep reading until next different
                // endpoint record.
//                    check if last record has 0 range. if has 0 range, then continue to next record

                while (records[target_offset].second == records[target_offset + 1].second && target_offset < size) {
#ifdef DEBUG
                    std::cout << target_offset << "  offset " << endl;
#endif
                    old_min_zmin = std::min(old_min_zmin, current_zmin);
                    current_count += 1;
                    old_sum = current_sum;
                    current_sum += records[target_offset + 1].first;
                    target_offset += 1;
                }
                pieces.emplace_back(
                        (std::make_tuple(records[target_offset].second, old_min_zmin, current_count - 1, old_sum)));
#ifdef DEBUG
                cout<<"now having "<<pieces.size()<<" buckets"<<endl;
#endif
                current_count = 1;
                current_min_zmin = current_zmin;
                current_sum = current_zmin;
                i = target_offset + 1;

            }else{
                current_min_zmin = std::min(current_min_zmin, current_zmin);
                current_sum = current_sum + current_zmin;
                if(i == size -1){
                    pieces.emplace_back(records[size - 1].second, current_min_zmin, current_count, current_sum);
                }
                continue;
            }
    }
    if(records[size -1].second == std::get<0>(pieces.back())){
        return;
    }else {
        pieces.emplace_back(records[size - 1].second, current_min_zmin, current_count, current_sum);
    }
}



// update pieces
void insert_pieces(double range_start, double range_end, double piece_limit,
                   std::vector<std::tuple<double, double, double, double>> &pieces) {

    std::vector<std::tuple<double, double, double, double>>::iterator up;
    //need to consider the index is empty
    //when the index is empty
    // check if empty first
    if (pieces.empty()) {
        pieces.emplace_back(range_start, std::numeric_limits<double>::min(), std::numeric_limits<double>::min(),
                                    std::numeric_limits<double>::min()); // numeric limit
        pieces.emplace_back(range_end,  range_start, 1.0, range_start);
        return;
    }
    // lowerbound position that this data should be insert
    // update found piece's count, max, sum
    // lowerbound:  found the most left value that not less than the look up value
    up = std::lower_bound(pieces.begin(), pieces.end(), std::make_tuple(range_end, -1.0, -1.0, -1.0), sortbysec_insert);
    auto piece_position = up - pieces.begin();
    //current distance to the pieces iterator end, end - currentposition should > 0
//    auto dis_to_end = pieces.end() - up;
    // the start is large than the piecewise's start, and end < piecewise's end
    // count/max/sum in the piece, will be update by inserted data
    if (piece_position == pieces.size()) { // the inserted data start is larger than the very last piece
        double piece_min_zmin = std::get<1>(pieces[piece_position - 1]);
        double piece_count = std::get<2>(pieces[piece_position -1]);
        double piece_sum = std::get<3>(pieces[piece_position -1]);
        double current_min_zmin = std::min(piece_min_zmin, range_start);
        double current_count = piece_count + 1;
        double current_sum = piece_sum + (range_start);
//        double current_error = cal_error(current_count, current_min_zmin, current_sum);

#ifdef DEBUG
        std::cout << "cal error in insert" << current_error << endl;
#endif
        // add new record will make very last piece's count > piece limit
        if (current_count > piece_limit) {
            // directly push new record as new bucket
            pieces.emplace_back((std::make_tuple(range_end, range_start, 1,
                                        range_start)));
        } else { //new record make last piece count < piece limit
            //update all parameters accordingly
            std::get<0>(pieces[piece_position -1]) = range_end;
            std::get<1>(pieces[piece_position-1]) = current_min_zmin;
            std::get<2>(pieces[piece_position -1]) = current_count;
            //  std::cout << " updated count2 " << get<2>(pieces[piece_position]) << endl;
            std::get<3>(pieces[piece_position-1]) = current_sum;
        }
        return;
    }
        //or insert into the begnning
    else if (piece_position == 0) { // head insert
        //get the piece info in pieces[1]
        if (range_end != std::get<0>(pieces[0])) {
            double piece_min_zmin = std::get<1>(pieces[piece_position + 1]);
            double piece_count = std::get<2>(pieces[piece_position + 1]);
            double piece_sum = std::get<3>(pieces[piece_position + 1]);
            double current_min_zmin = std::min(piece_min_zmin, range_start);
            double current_count = piece_count + 1;
            double current_sum = piece_sum +  range_start;
//            double current_error = cal_error(current_count, current_min_zmin, current_sum);

            if (current_count > piece_limit) {
                std::get<0>(pieces[0]) = range_end;
                std::get<1>(pieces[0]) = range_start;
                std::get<2>(pieces[0]) = 1;
                std::get<3>(pieces[0]) = range_start;
                pieces.insert(pieces.begin(),std:: make_tuple(range_start, std::numeric_limits<double>::min(),
                                                         std::numeric_limits<double>::min(),
                                                         std::numeric_limits<double>::min()));
            } else {
                //piece 0 is dummy piece
                std::get<0>(pieces[0]) = range_start;
                std::get<1>(pieces[0]) = current_min_zmin;
                std::get<2>(pieces[0]) = current_count;
                std::get<3>(pieces[0]) = current_sum;
                std::get<1>(pieces[piece_position]) = current_min_zmin;
                std::get<2>(pieces[piece_position]) = current_count;
                std::get<3>(pieces[piece_position]) = current_sum;

            }
            return;
        } else {
            piece_position++;
        }

    }

    double piece_min_zmin = std::get<1>(pieces[piece_position]);
    double piece_count = std::get<2>(pieces[piece_position]);
    double piece_sum = std::get<3>(pieces[piece_position]);
    double current_min_zmin = std::min(piece_min_zmin,  range_start);
    double current_count = piece_count + 1;
    //    std::cout<< "current count "<< piece_count << endl;
    double current_sum = piece_sum +  range_start;
    // double current_error = cal_error(current_count,current_min_zmin,current_sum);
    // finish the update of piece that tobe inserted
    std::get<1>(pieces[piece_position]) = current_min_zmin;
    std::get<2>(pieces[piece_position]) = current_count;
#ifdef DEBUG
    assert(current_count = piece_count+1);
#endif
//     std::cout << " updated count " << get<2>(pieces[piece_position]) << endl;
   std::get<3>(pieces[piece_position]) = current_sum;


}

#endif //ALEX_PIECEWISE_H

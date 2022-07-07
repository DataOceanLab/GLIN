//
// Created by Congying Wang on 10/25/21.
//
#include "doctest.h"
#include <map>

TEST_SUITE("piecewise") {
    TEST_CASE("piecewise with zero range") {
        const int size = 7;
        std::pair<double, double> records[size];
        records[0].first = 2;
        records[0].second = 2;
        records[1].first = 2;
        records[1].second = 2;
        records[2].first = 2;
        records[2].second = 2;
        records[3].first = 5;
        records[3].second = 5;
        records[4].first = 3;
        records[4].second = 3;
        records[5].first = 4;
        records[5].second = 4;
        records[6].first = 5;
        records[6].second = 5;
        std::vector<std::tuple<double, double, double, double>> pieces;
        piecewise(records, size, 3, pieces);
        CHECK(!pieces.empty());
        CHECK(pieces.size() == 3);
        CHECK(std::get<0>(pieces[1]) == 2.0);
        CHECK(std::get<1>(pieces[1]) == 2.0);
        CHECK(std::get<2>(pieces[1]) == 3.0);
        CHECK(std::get<3>(pieces[1]) == 6.0);

#ifdef DEBUG
        for (int i = 0; i < pieces.size(); i++) {
            std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
                      << get<3>(pieces[i]) << endl;
        }
#endif

    }


TEST_CASE(" piecewise") {
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
    CHECK(!pieces.empty());
    CHECK(pieces.size() == 3);
    CHECK(std::get<0>(pieces[1]) == 7);
    CHECK(std::get<1>(pieces[1]) == 2);
    CHECK(std::get<2>(pieces[1]) == 3);
    CHECK(std::get<0>(pieces[2]) == 9);
    CHECK(std::get<1>(pieces[2]) == 2);
    CHECK(std::get<2>(pieces[2]) == 3);

#ifdef  DEBUG
    for (int i = 0; i < pieces.size(); i++) {
        std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
                  << get<3>(pieces[i]) << endl;
        }
#endif
}

TEST_CASE("Using loop form data and perform piecewise") {
    pair<double, double> records[200];
    std::vector<std::tuple<double, double, double, double>> orig_pieces;
    double start_number = 100.50;
    for (int i = 0; i < 200; i++) {
        records[i].first = start_number - i;
        records[i].second = start_number;
        start_number--;
//        std::cout << records[i].first << " " << records[i].second << endl;
    }

    piecewise(records, 200, 10, orig_pieces);
    double count = 0;
    for (int i = 0; i < orig_pieces.size(); i++) {
        if (i != 0) {
            count += std::get<2>(orig_pieces[i]);
            CHECK(std::get<2>(orig_pieces[i]) <= 10);
        }

    }
    CHECK(count == 200);
}

TEST_CASE("piecewise with repeating data") {
        /*
         * this records contains large amount of data with repeating end.
         * For repeating data, we ignore the error bound and put all data with same end together until repeating ends
         */
        // test piecewise ffunction by push reeesult back to
        // build up the record
        const int size = 100;
        std::pair<double, double> records1[size];
        for (int i = 0; i < size; i++) {
            records1[i].first = i;
            records1[i].second = i + 15;
            if (i % 2 == 0) {
                records1[i].second = i + 10;
            }
            if (i % 5 == 0) {
                records1[i].second = 100;
            }
        }
        std::vector<std::tuple<double, double, double, double>> pieces;
        double piece_limit = 50;
        piecewise(records1, 100, piece_limit, pieces);
        CHECK(!pieces.empty());
        CHECK(pieces.size() == 3);
        for (int i = 0; i < pieces.size(); i++) {
#ifdef DEBUG

            std::cout << get<0>(pieces[i]) << "  " << get<1>(pieces[i]) << "  " << get<2>(pieces[i]) << "  "
                      << get<3>(pieces[i]) << endl;
#endif
            if (i != 0) {
                CHECK(get<2>(pieces[i]) >= piece_limit);
            }
        }
    }
};
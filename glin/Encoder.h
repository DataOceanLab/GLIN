#include "hilbert/hilbert.h"
#include "libmorton/morton.h"

template <class INPUT>
class Encoder {
private:
    INPUT xmin; // The minimum X value in the entire space
    INPUT xintvl; // X interval which is the cell size on X
    INPUT ymin; // The minimum X value in the entire space
    INPUT yintvl; // Y interval which is the cell size on Y
public:
    // Build an encoder using min, cell interval (aka cell size)
    Encoder(INPUT xmin, INPUT xintvl, INPUT ymin, INPUT yintvl): xmin(xmin), xintvl(xintvl), ymin(ymin), yintvl(yintvl){
    }

    // Build an encoder using min, max and number of cells
    Encoder(INPUT xmin, INPUT xmax, int xnum, INPUT ymin, INPUT ymax, int ynum):
    xmin(xmin), xintvl((xmax - xmin) * 1.0 / xnum), ymin(ymin), yintvl((ymax - ymin) * 1.0 / ynum){
    }

    template<class OUTPUT>
    inline OUTPUT d2i (INPUT d, INPUT min, INPUT interval) {
        auto i = (INPUT)((d - min) / interval);
        return i;
    }
    
    inline uint_fast64_t encode_z(INPUT x, INPUT y) {
        auto x_int = d2i<uint_fast32_t>(x, xmin, xintvl);
        auto y_int = d2i<uint_fast32_t>(y, ymin, yintvl);
        return libmorton::morton2D_64_encode(x_int, y_int);
    }

    inline bitmask_t encode_h(INPUT x, INPUT y) {
        auto x_int = d2i<bitmask_t>(x, xmin, xintvl);
        auto y_int = d2i<bitmask_t>(y, ymin, yintvl);
        bitmask_t coord[2] = {x_int, y_int};
        return hilbert_c2i(2, 32, coord);
    }

    /**
     * The min max Z order curve range of a rectangle are the IDs of minXY and maxXY
     * See https://stackoverflow.com/questions/30170783/how-to-use-morton-orderz-order-curve-in-range-search
     * See https://aws.amazon.com/blogs/database/z-order-indexing-for-multifaceted-queries-in-amazon-dynamodb-part-1/?sc_channel=sm&sc_campaign=zackblog&sc_country=global&sc_geo=global&sc_category=rds&sc_outcome=aware&adbsc=awsdbblog_social_20170517_72417147&adbid=864895517733470208&adbpl=tw&adbpr=66780587
     * @param x1_dbl minX
     * @param y1_dbl minY
     * @param x2_dbl maxX
     * @param y2_dbl maxY
     * @return
     */
    inline std::pair<uint_fast64_t, uint_fast64_t> encode_z(INPUT x1_dbl, INPUT y1_dbl, INPUT x2_dbl, INPUT y2_dbl){
        auto x1 = d2i<uint_fast32_t>(x1_dbl, xmin, xintvl);
        auto y1 = d2i<uint_fast32_t>(y1_dbl, ymin, yintvl);
        auto x2 = d2i<uint_fast32_t>(x2_dbl, xmin, xintvl);
        auto y2 = d2i<uint_fast32_t>(y2_dbl, ymin, yintvl);
        auto min = libmorton::morton2D_64_encode(x1, y1);
        auto max = libmorton::morton2D_64_encode(x2, y2);
        return std::make_pair(min, max);
    }

    /**
     * The min and max Hilbert curve ID ranges of a rectangle lie on the boundary of a rectangle
     * See https://stackoverflow.com/questions/12772893/how-to-use-morton-order-in-range-search
     * See https://github.com/davidmoten/hilbert-curve
     * @param x1_dbl minX
     * @param y1_dbl minY
     * @param x2_dbl maxX
     * @param y2_dbl maxY
     * @return
     */
    inline std::pair<bitmask_t, bitmask_t> encode_h(INPUT x1_dbl, INPUT y1_dbl, INPUT x2_dbl, INPUT y2_dbl){
        auto x1 = d2i<bitmask_t>(x1_dbl, xmin, xintvl);
        auto y1 = d2i<bitmask_t>(y1_dbl, ymin, yintvl);
        auto x2 = d2i<bitmask_t>(x2_dbl, xmin, xintvl);
        auto y2 = d2i<bitmask_t>(y2_dbl, ymin, yintvl);
        /**
         * ********
         * *      *
         * *      *
         * ********
         */
        bitmask_t working_co[2] = {0, 0};
        auto min = std::numeric_limits<bitmask_t>::max();
        auto max = std::numeric_limits<bitmask_t>::min();
        for (bitmask_t i = x1; i <= x2; ++i) {
            working_co[0] = i;
            working_co[1] = y1;
            auto working_id = hilbert_c2i(2, 32, working_co);
            min = std::min(min, working_id);
            max = std::max(max, working_id);
        }
        for (bitmask_t i = x1; i <= x2; ++i) {
            working_co[0] = i;
            working_co[1] = y2;
            auto working_id = hilbert_c2i(2, 32, working_co);
            min = std::min(min, working_id);
            max = std::max(max, working_id);
        }
        for (bitmask_t i = y1 + 1; i <= y2 -1 ; ++i) {
            working_co[0] = x1;
            working_co[1] = i;
            auto working_id = hilbert_c2i(2, 32, working_co);
            min = std::min(min, working_id);
            max = std::max(max, working_id);
        }
        for (bitmask_t i = y1 + 1; i <= y2 -1 ; ++i) {
            working_co[0] = x2;
            working_co[1] = i;
            auto working_id = hilbert_c2i(2, 32, working_co);
            min = std::min(min, working_id);
            max = std::max(max, working_id);
        }
        return std::make_pair(min, max);
    }
};
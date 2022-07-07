//
// Created by Juno Wang on 7/2/21.
//
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Dimension.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/util/IllegalArgumentException.h>
#include <geos/geom/LineSegment.h>
#include "../../glin/Encoder.h"
/*
 * project geometry on certain line(y = kx+b)
 */
void shape_projection(geos::geom::Geometry *geom, geos::geom::LineSegment segment, long double &dist_min, long double &dist_max){
    dist_min = std::numeric_limits<long double>::max();
    dist_max = std::numeric_limits<long double>::min();
    geos::geom::Coordinate original(0,0);
    std::unique_ptr<geos::geom::CoordinateSequence> coordSeq;
    geos::geom::Coordinate proj_result(0,0);
    coordSeq = geom->getCoordinates();
#ifdef DEBUG
    cout <<"coord Sequence" << coordSeq->toString() << endl;
#endif
    //convert the geometry to a range
    for(size_t i = 0; i < coordSeq->size();i++){
        segment.project(coordSeq->getAt(i),proj_result);
        long double dist = proj_result.distance(original);
        // if the results of projection have
            if (dist < dist_min){
                dist_min = dist;

            }
            if(dist> dist_max){
                dist_max = dist;
            }
    }
#ifdef DEBUG
    cout<<"max dist result" << dist_max << endl;
    cout<<"min dist result" << dist_min << endl;
#endif
}
/*
 * project geometry on moore hilbert curve or z-order curve
 */
void curve_shape_projection(geos::geom::Geometry *geometry, std::string curve_type,
                            double cell_xmin, double cell_ymin, double cell_x_intvl,double cell_y_intvl,double &dist_start, double &dist_end){
    dist_start = std::numeric_limits<double>::max();
    dist_end = std::numeric_limits<double>::min();
    const geos::geom::Envelope *envelope = geometry->getEnvelopeInternal();
    double minX = envelope->getMinX();
    double minY = envelope->getMinY();
    double maxX = envelope->getMaxX();
    double maxY = envelope->getMaxY();


    auto encoder = new Encoder<double>(cell_xmin, cell_x_intvl, cell_ymin, cell_y_intvl );

    if(curve_type == "h"){
            auto index_double_h = encoder->encode_h(minX,minY,maxX,maxY);
            dist_start = index_double_h.first;
            dist_end = index_double_h.second;
    }
    if(curve_type == "z"){
        auto index_z = encoder->encode_z(minX,minY,maxX,maxY);
        dist_start = index_z.first;
        dist_end = index_z.second;
    }

}
void curve_shape_projection(geos::geom::Envelope *envelope, std::string curve_type, double cell_xmin, double cell_ymin, double cell_x_intvl,double cell_y_intvl,double &dist_start, double &dist_end){
    dist_start = std::numeric_limits<double>::max();
    dist_end = std::numeric_limits<double>::min();
    double minX = envelope->getMinX();
    double minY = envelope->getMinY();
    double maxX = envelope->getMaxX();
    double maxY = envelope->getMaxY();

    auto encoder = new Encoder<double>(cell_xmin, cell_x_intvl, cell_ymin, cell_y_intvl );

    if(curve_type == "h"){
        auto index_double_h = encoder->encode_h(minX,minY,maxX,maxY);
        dist_start = index_double_h.first;
        dist_end = index_double_h.second;
    }
    if(curve_type == "z"){
        auto index_z = encoder->encode_z(minX,minY,maxX,maxY);
        dist_start = index_z.first;
        dist_end = index_z.second;
    }

}

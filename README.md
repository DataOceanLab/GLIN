# GLIN

## Introduction
This paper introduces GLIN, a generic learned index for spatial range queries on complex geometries. To achieve that, GLIN transforms geometries to Z-address intervals, and builds a hierarchical model to learn the cumulative distribution function between these intervals and the record positions. The lightweight hierarchical model greatly shortens the index probing time. Furthermore, GLIN augments spatial range query using an add-on piecewise function to guarantee the query accuracy for both 𝐶𝑜𝑛𝑡𝑎𝑖𝑛𝑠 and 𝐼𝑛𝑡𝑒𝑟𝑠𝑒𝑐𝑡𝑠 spatial relationships.

##Highlights
- GLIN transforms geometries to 1-dimensional sortable values,namely Z-address intervals, using Z-order curve.
- GLIN is the first learned index that handles non-point geospatial values.
- GLIN updates its index structure efficiently with query accuracy guaranteed.

## Code Structure
- GLIN: contains core glin fucntion other than Recursive model index(RMI) as following
  - Z-Address Interval transformation function
  - Index search function and update function (insertion and deletion)
  - Piecewise function initialization, query window augmentation, piecewise function updates
- src: GLIN-customized ALEX recursive model, including nodes, node updates function, and model training.
- test: tests related to GLIN-customized ALEX recursive model and GLIN's function

## Dependencies 
- GEOS: brew install geos
- BOOST C++ library : brew install boost

## Build and Run test
- Compile the project using CMake. It will build a new direcotory named as ``` build```
  ```./build.sh ```
- Run unit tests without query augmentation, for Contains relationship only
 ``` ./build/test_glin ```
  - run unit tests with query augmentation. It supports both Contains and Intersects relationship
  
  ``` ./build/test_glin_piece```
- GitHub Action automatically tests every commit using all unit tests.

## API Doc
- To construct a index for geometry colomn 

 ```alex::Glin<double, geos::geom::Geometry *> index```
 - Bulkload to Index

```index.glin_bulk_load(poly_vec, piecelimitation, "z", cell_xmin,cell_ymin, cell_x_intvl, cell_y_intvl, pieces)```

- Parameters:
  - poly_vec : a vector of Geometries in std::vector<geos::geom::Geometry *>
  - piecelimitation: a double, limits how many records is summarized in a single piece
  - space-filling-curve : z-order-filling-curve
  - cell_xmin: the minimal value of x in a geometry, for real-world data, set cell_xmin = -180
  - cell_ymin: the minimal value of y in a geometry, for real-world data, set cell_ymin = -180
  - cell_x_intvl, cell_y_intvl:: a small enough value is valid for all data, by default we set to 0.0000005, but it is tunable parameter
  - pieces: vector of piecewise function. If the user does not choose PIECEWISE setting, this value is empty.

* Search in index

`   index.glin_find(query, "z",  cell_xmin,cell_ymin, cell_x_intvl, cell_y_intvl, pieces, find_result, count_filter)`
* Parameters:
    * querywindow: the query window to perform search with
    * space-filling-curve :  z-order-filling-curve
    * cell_xmin:  the minimal value of x in a geometry, for real-world data, set cell_xmin = -180
    * cell_ymin:  the minimal value of y in a geometry, for real-world data, set cell_ymin = -180
    * cell_x_intvl, cell_y_intvl:: a small enough value is valid for all data, by default we set to 0.0000005, but it is tunable parameter
    * pieces: vector of piecewise function. If the user does not choose PIECEWISE setting, this value is empty.
    * find result:  The result after refine
    * count_filter :  the number of result before refine


- Insert geometry into index

```index.glin_insert(insert_tuple, "z", cell_xmin,cell_ymin, cell_x_intvl, cell_y_intvl, piece_limit, pieces);```

 - Parameters
   - insert_tuple:  A tuple of to-be-inserted-Geometry and its MBR  std::tuple<geos::geom::Geometry*, geos::geom::Envelope*>
   - space-filling-curve :  z-order-filling-curve
   - cell_xmin:  the minimal value of x in a geometry, for real-world data, set cell_xmin = -180
   - cell_ymin:  the minimal value of y in a geometry, for real-world data, set cell_ymin = -180
   - cell_x_intvl, cell_y_intvl:: a small enough value is valid for all data, by default we set to 0.0000005, but it is tunable parameter
   - piece_limit: piece_limitation
   - pieces: vector of piecewise function. If the user does not choose PIECEWISE setting, this value is empty.


-Delete geometry from index

  ` int num_erase = index.erase(poly, "z", cell_xmin,cell_ymin, cell_x_intvl, cell_y_intvl, piecelimitation, pieces);`
    
* Parameters:
  * poly:  A to-be-remove geometry
  * space-filling-curve :  z-order-filling-curve
  * cell_xmin:  the minimal value of x in a geometry, for real-world data, set cell_xmin = -180
  * cell_ymin:  the minimal value of y in a geometry, for real-world data, set cell_ymin = -180
  * cell_x_intvl, cell_y_intvl:: a small enough value is valid for all data, by default we set to 0.0000005, but it is tunable parameter
  * piece_limit: piece_limitation
  * pieces: vector of piecewise function. If the user does not choose PIECEWISE setting, this value is empty.
  * This function return a number of record are erased

Example can be found at unittest_glin.h and unittest_glin_maintenance.h
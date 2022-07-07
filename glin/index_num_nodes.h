// Boost.Geometry Index
//
// R-tree visitor calculating index size
//
// Copyright (c) 2022 Jia Yu
//
// This file was modified by Oracle on 2019-2020.
// Modifications copyright (c) 2019-2020 Oracle and/or its affiliates.
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle
//
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <tuple>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/index/detail/rtree/utilities/view.hpp>


namespace boost { namespace geometry { namespace index { namespace detail { namespace rtree { namespace utilities {

namespace visitors {

template <typename MembersHolder>
struct index_num_nodes
    : public MembersHolder::visitor_const
{
    typedef typename MembersHolder::internal_node internal_node;
    typedef typename MembersHolder::leaf leaf;

    inline index_num_nodes()
        : internal_size(0)
        , leaf_size(0)
    {}

    inline void operator()(internal_node const& n)
    {
        typedef typename rtree::elements_type<internal_node>::type elements_type;
        typedef typename boost::geometry::model::box<boost::geometry::model::point
                <double, 2, boost::geometry::cs::cartesian>> box_type;
        elements_type const& elements = rtree::elements(n);

        // Count this internal node
        internal_size++;
        for (typename elements_type::const_iterator it = elements.begin();
            it != elements.end(); ++it)
        {
            rtree::apply_visitor(*this, *it->second);
        }
    }

    inline void operator()(leaf const& n)
    {   
        leaf_size++;
    }
    
    std::size_t internal_size;
    std::size_t leaf_size;
};

} // namespace visitors

template <typename Rtree> inline
std::tuple<std::size_t, std::size_t>
index_num_nodes(Rtree const& tree)
{
    typedef utilities::view<Rtree> RTV;
    RTV rtv(tree);

    visitors::index_num_nodes< typename RTV::members_holder> index_num_nodes_v;

    rtv.apply_visitor(index_num_nodes_v);
    
    return std::make_tuple(index_num_nodes_v.internal_size, index_num_nodes_v.leaf_size);
}

}}}}}} // namespace boost::geometry::index::detail::rtree::utilities
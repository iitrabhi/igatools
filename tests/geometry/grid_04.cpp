//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------

/**
 *  @file
 *  @brief  CartesianGrid::get_sub_grid<k>
 *  @author pauletti
 *  @date  2015-08-19
 */

#include "../tests.h"
#include <igatools/geometry/cartesian_grid.h>
#include <igatools/geometry/grid_element.h>

template<int dim, int k = dim-1>
void get_subgrid(const TensorSize<dim> &n_knots)
{
    OUTSTART
    using Grid =  CartesianGrid<dim>;

    auto grid = Grid::create(n_knots);
    out.begin_item("Grid:");
    grid->print_info(out);
    out.end_item();

    for (auto &i : UnitElement<dim>::template elems_ids<k>())
    {
        out.begin_item("Sub element: " + to_string(i));
        std::map<typename CartesianGrid<k>::IndexType,typename Grid::IndexType> elem_map;
        auto sub_grid = grid->template get_sub_grid<k>(i, elem_map);
        sub_grid->print_info(out);
        for (auto x : elem_map)
            out << "Elem. in Grid<" << k << ">: " << x.first
                << "   ---   "
                << "Elem. in Grid<" << dim << ">: " << x.second << endl;
        out.end_item();
    }
    out << endl;

    OUTEND
}



int main()
{
    out.depth_console(10);

    get_subgrid<1>(TensorSize<1>(sequence<1>(2)));
    get_subgrid<2>(TensorSize<2>(sequence<2>(2)));
    get_subgrid<3>(TensorSize<3>(sequence<3>(2)));

    return  0;
}
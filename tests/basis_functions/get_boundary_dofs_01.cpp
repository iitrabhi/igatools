//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2016  by the igatools authors (see authors.txt).
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
 *  Test for basis_tools::get_boundary_dofs
 *
 *  author: pauletti
 *  date: 2015-03-27
 *
 */

#include "../tests.h"

#include <igatools/basis_functions/bspline.h>
#include <igatools/basis_functions/basis_tools.h>

using basis_tools::get_boundary_dofs;

template<int dim, int range = 1, int rank = 1>
void get_bdry_dof(const int deg = 1, const int n_knots = 3)
{
  OUTSTART
  using RefBasis = ReferenceBasis<dim, range, rank>;
  using Basis = BSpline<dim, range, rank>;
  auto grid = Grid<dim>::create(n_knots);
//  grid->set_boundary_id(0,1);

  auto space = SplineSpace<dim,range,rank>::create(deg, grid);
  auto basis = Basis::create(space);



  std::set<int>  piece_one  = {0};
  std::set<int>  piece_zero;
  for (int face_id = 1; face_id < UnitElement<dim>::n_faces ; ++face_id)
    piece_zero.insert(face_id);

  auto one_dofs = get_boundary_dofs<RefBasis>(basis, piece_one);
  auto zero_dofs = get_boundary_dofs<RefBasis>(basis, piece_zero);

  // TODO (pauletti, Mar 27, 2015): we should create iga::set with print_info
  for (auto &x : one_dofs)
    out << x << " ";
  out << endl;

  for (auto &x : zero_dofs)
    out << x << " ";
  out << endl;

  OUTEND
}



int main()
{
  get_bdry_dof<1>();
  get_bdry_dof<2>();
  get_bdry_dof<3>();

  get_bdry_dof<1>(2);
  get_bdry_dof<2>(2);

  return 0;
}

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

/*
 *  Test for periodic BSpline solving poisson
 *
 *  author: pauletti
 *  date: 2015-03-12
 *
 */

// TODO (pauletti, Mar 12, 2015): this is not a unit test, this tests
// has to be splitted into simpler tests (this one goes onto consistency test)

#include "../tests.h"
#include <igatools/functions/function_lib.h>
#include <igatools/basis_functions/bspline.h>
#include <igatools/basis_functions/bspline_element.h>
#include <igatools/base/quadrature_lib.h>
#include <igatools/linear_algebra/epetra_solver.h>
#include <igatools/io/writer.h>
#include <igatools/functions/ig_grid_function.h>
#include <igatools/functions/grid_function_lib.h>

using namespace EpetraTools;

using functions::ConstantFunction;
template <int dim, int range=1>
void assemble_matrix(const int n_knots, const int deg)
{
  out.begin_item("assemble_matrix<dim=" + to_string(dim) +
                 ",range=" + to_string(range) + ">");

  using Space  = SplineSpace<dim, range>;
  using Basis  = BSpline<dim, range>;

  using LinFunction = grid_functions::LinearGridFunction<dim,range>;
  using Value = typename LinFunction::Value;
  using Gradient = typename LinFunction::Gradient;


  TensorIndex<dim> deg1(deg);
  typename Space::DegreeTable degt(deg1);

  auto grid  = Grid<dim>::const_create(n_knots);
  auto space = Space::const_create(deg, grid, InteriorReg::maximum, true);
  auto basis = Basis::const_create(space,BasisEndBehaviour::periodic);

  /*
  out.begin_item("Grid:");
  grid->print_info(out);
  out.end_item();
  //*/
  /*
    out.begin_item("BSpline basis:");
    basis->print_info(out);
    out.end_item();
  //*/
  Gradient A;
  Value b;
  for (int j = 0; j < range; ++j)
  {
    for (int i = 0; i < dim; ++i)
      if (i==j)
        A[i][j]=10.*(i+1);
    b[j] = -5.;
  }



  auto f = LinFunction::const_create(grid, A, b);


  auto matrix = create_matrix(*basis,DofProperties::active,Epetra_SerialComm());
  auto rhs = create_vector(matrix->RangeMap());
  auto solution = create_vector(matrix->DomainMap());

  auto elem_handler = basis->create_cache_handler();
  auto f_handler = f->create_cache_handler();

  using Flags = basis_element::Flags;
  auto flag = Flags::value | Flags::gradient |Flags::w_measure;
  elem_handler->set_element_flags(flag);

  f_handler->set_element_flags(grid_function_element::Flags::D0);

  auto elem   = basis->begin();
  const auto elem_end = basis->end();
  auto f_elem = f->begin();

  auto elem_quad = QGauss<dim>::create(deg);
  elem_handler->init_element_cache(elem,elem_quad);
  f_handler->init_element_cache(f_elem,elem_quad);


  const int n_qp = elem_quad->get_num_points();

  int elem_id = 0;
  for (; elem != elem_end; ++elem, ++f_elem, ++elem_id)
  {
    out.begin_item("Element : " + to_string(elem_id));
    out << "Element index: " << elem->get_index() << std::endl;

    const int n_basis = elem->get_num_basis(DofProperties::active);
    DenseMatrix loc_mat(n_basis, n_basis);
    loc_mat = 0.0;

    DenseVector loc_rhs(n_basis);
    loc_rhs = 0.0;

    elem_handler->fill_element_cache(elem);
    f_handler->fill_element_cache(f_elem);

    auto phi = elem->get_element_values();
    auto grad_phi  = elem->get_element_gradients();
    auto w_meas = elem->get_element_w_measures();

    out.begin_item("Basis function values:");
    phi.print_info(out);
    out.end_item();

    out.begin_item("Basis function gradient:");
    grad_phi.print_info(out);
    out.end_item();

    out.begin_item("W * Measure:");
    w_meas.print_info(out);
    out.end_item();

    auto f_values = f_elem->get_element_values_D0();
    for (int i = 0; i < n_basis; ++i)
    {
      auto grad_phi_i = grad_phi.get_function_view(i);
      auto phi_i = phi.get_function_view(i);
      for (int j = 0; j < n_basis; ++j)
      {
        auto grad_phi_j = grad_phi.get_function_view(j);
        auto phi_j = phi.get_function_view(j);
        for (int qp = 0; qp < n_qp; ++qp)
          loc_mat(i,j) +=
            (scalar_product(grad_phi_i[qp], grad_phi_j[qp])
             +
             scalar_product(phi_i[qp], phi_j[qp])
            )
            * w_meas[qp];
      }

      for (int qp=0; qp<n_qp; ++qp)
        loc_rhs(i) += scalar_product(phi_i[qp], f_values[qp])
                      * w_meas[qp];
    }

    const auto loc_dofs = elem->get_local_to_global(DofProperties::active);
    matrix->add_block(loc_dofs, loc_dofs,loc_mat);
    rhs->add_block(loc_dofs, loc_rhs);

    out.end_item();
  }
  matrix->FillComplete();
  out.begin_item("Stiffness matrix:");
  matrix->print_info(out);
  out.end_item();

  auto solver = create_solver(*matrix, *solution, *rhs);
  solver->solve();


  const int n_plot_points = deg+1;
  Writer<dim> writer(basis->get_grid(), n_plot_points);

  using IgFunc = IgGridFunction<dim,range>;
  auto solution_function = IgFunc::const_create(basis, *solution);
  writer.template add_field(*solution_function, "solution");
  string filename = "poisson_problem-" + to_string(deg) + "-" + to_string(dim) + "d" ;
  writer.save(filename);

  out.end_item();
}


int main()
{
  const int max_deg=3;
  for (int deg = 1; deg<max_deg; ++deg)
  {
    const int n_knots = 5 + deg;
    assemble_matrix<1>(n_knots, deg);
    assemble_matrix<2>(n_knots, deg);
    assemble_matrix<2,2>(n_knots, deg);
  }
  return 0;
}

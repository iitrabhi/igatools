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

//#include <igatools/base/quadrature_lib.h>
#include <igatools/geometry/domain.h>
//#include <igatools/geometry/domain_element.h>
//#include <igatools/geometry/domain_handler.h>
#include <igatools/geometry/grid_function_lib.h>
#include <igatools/io/writer.h>


#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

#include "vtkUnstructuredGridOverlay_3.h"

#include "../tests.h"


template<int dim, int codim>
void domain()
{
  OUTSTART
  using std::string;

  using IdentityFunc = grid_functions::IdentityGridFunction<dim>;
  using LinearFunc = grid_functions::LinearGridFunction<dim,dim>;

  const int n_pts_dir = 2;

  SafeSTLArray<string,2> names;

  names[0] = "domain_A";
  names[1] = "domain_B";

//#if 0
  SafeSTLArray<int,dim> n_knots;
  n_knots[0] = 3;
  n_knots[1] = 2;

  {
    auto grid_a = Grid<dim>::create(n_knots[0]);

    typename LinearFunc::template Derivative<1> A;
    typename LinearFunc::Value b;
    for (int i = 0 ; i < dim ; ++i)
      A[i][i] = 1.0 - 1.0e-1;

    auto func_a = LinearFunc::create(grid_a,A,b);
    auto domain_a = Domain<dim,codim>::create(func_a);

    Writer<dim,codim> writer(domain_a,n_pts_dir);
    writer.save(names[0]);
  }


  {
    auto grid_b = Grid<dim>::create(n_knots[1]);
    auto func_b = IdentityFunc::create(grid_b);
    auto domain_b = Domain<dim,codim>::create(func_b);

    Writer<dim,codim> writer(domain_b,n_pts_dir);
    writer.save(names[1]);
  }
//#endif


  using std::cout;
  using std::endl;

  //read all the data from the file
  using ReaderVTU = vtkXMLUnstructuredGridReader;

  using VTKUGridPtr = vtkSmartPointer<vtkUnstructuredGrid>;
  SafeSTLArray<VTKUGridPtr,2> unstruct_grids;

  for (int i = 0 ; i <= 1 ; ++i)
  {
    const string filename = names[i] + ".vtu";
    cout << "Reading the file \"" << filename << "\"...";
    auto reader = vtkSmartPointer<ReaderVTU>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    unstruct_grids[i] = reader->GetOutput();
    int n_elems = unstruct_grids[i]->GetNumberOfCells();
    cout << "done (n_elements = " << n_elems << ")" << endl;
  }


  auto grid_slave = unstruct_grids[0];
  auto grid_master = unstruct_grids[1];

  const double mesh_distance = 0.0;
  const bool partial_overlay = true;
  cout << "Computing overlay...";
  VTKUGridPtr mesh_intersection =
    vtkUnstructuredGridOverlay_3(grid_slave, grid_master, mesh_distance, partial_overlay, false);
  int n_elems = mesh_intersection->GetNumberOfCells();
  cout << "done (n_elements = " << n_elems << ")" << endl;

  /*
  //This compute the "parent" vertices. Useful in Numea to avoid multiple interpolation
  cout << "BuildParentVertices...";
  BuildParentVertices(res, ugA.GetPointer(), ugB.GetPointer(), 0.0000001);
  cout << "done" << endl;
  //*/

  // Write the unstructured grid (different treatment for .vtk and .vtk file)
  std::string outputname = "intersection_" + names[0] + "_" + names[1] + ".vtu";
  auto writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
  writer->SetDataModeToAscii();
  writer->SetFileName(outputname.c_str());
  writer->SetInputData(mesh_intersection);
  writer->Write();

  OUTEND
}


int main()
{
//  domain<1,0>();
  domain<2,0>();
//  domain<3,0>();

  return 0;
}

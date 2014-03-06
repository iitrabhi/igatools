//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
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
//TODO put copyright here

#include <igatools/io/ig_reader.h>
#include <igatools/basis_functions/nurbs_element_accessor.h>

using std::vector;
using std::shared_ptr;

IGA_NAMESPACE_OPEN


template<int dim_ref_domain, int dim_phys_domain>
IgReader< dim_ref_domain, dim_phys_domain>::IgReader()
{}

template<int dim_ref_domain, int dim_phys_domain>
void
IgReader< dim_ref_domain, dim_phys_domain>::
load_xml(const std::string &filename)
{

    using boost::property_tree::ptree;
    ptree pt;

    read_xml(filename, pt);

    BOOST_FOREACH(ptree::value_type const& patch, pt.get_child("XMLFile.Patch"))
    {
        if (boost::iequals(patch.first, "<xmlattr>"))
        {
            const int dim_phys = patch.second.get<int>("DimPhysicalDomain");
            const int dim_ref  = patch.second.get<int>("DimReferenceDomain");

            AssertThrow(dim_phys == dim_phys_domain,
                        ExcDimensionMismatch(dim_phys,dim_phys_domain));
            AssertThrow(dim_ref == dim_ref_domain,
                        ExcDimensionMismatch(dim_ref,dim_ref_domain));

        }

        if (boost::iequals(patch.first, "KnotVector"))
        {

            BOOST_FOREACH(boost::property_tree::ptree::value_type const& knot, patch.second)
            {
                if (boost::iequals(knot.first, "<xmlattr>"))
                {
                    int d = knot.second.get<int>("Degree");
                    deg.push_back(d);
                }
                if (boost::iequals(knot.first, "BreakPoints"))
                {
                    std::string iss = knot.second.get<std::string>("");
                    std::vector<Real> row;
                    Real num;
                    std::stringstream line_stream(iss);
                    while (line_stream >> num) row.push_back(num);
                    breack_point.push_back(row);
                }
                if (boost::iequals(knot.first, "Multiplicities"))
                {
                    std::string iss = knot.second.get<std::string>("");
                    std::vector<Index> row;
                    Index num;
                    std::stringstream line_stream(iss);
                    while (line_stream >> num) row.push_back(num);
                    mlt.push_back(row);
                }
            }
        }

        if (boost::iequals(patch.first, "ControlPoints"))
        {
            vector<Real> weights_vector;
            BOOST_FOREACH(boost::property_tree::ptree::value_type const& cp, patch.second)
            {
                if (boost::iequals(cp.first, "NumDir"))
                {
                    std::string iss = cp.second.get<std::string>("");
                    std::stringstream line_stream(iss);
                    int num;
                    int direction_id = 0;
                    while (line_stream >> num)
                    {
                        AssertThrow(direction_id >= 0 && direction_id < dim_ref_domain,
                                    ExcIndexRange(direction_id,0,dim_ref_domain));
                        cp_per_ref_dir[direction_id] = num;
                        direction_id++;
                    }

                }
                if (boost::iequals(cp.first, "Coordinates"))
                {
                    std::string iss = cp.second.get<std::string>("");
                    std::vector<Real> row;
                    Real num;
                    std::stringstream line_stream(iss);
                    while (line_stream >> num) row.push_back(num);
                    control_point.push_back(row);
                }
                if (boost::iequals(cp.first, "Weights"))
                {
                    std::string iss = cp.second.get<std::string>("");
                    std::stringstream line_stream(iss);
                    Real num;
                    while (line_stream >> num) weights_vector.push_back(num);
                }
            }

            weights_.resize(cp_per_ref_dir);
            const int n_entries = weights_.flat_size();
            AssertThrow(weights_.flat_size() == weights_vector.size(),
                        ExcDimensionMismatch(weights_.flat_size(),weights_vector.size()));
            for (Size i=0 ; i < n_entries ; ++i)
                weights_(i) = weights_vector[i];

        }
    }//PATCH LOOP
    read = true;

    for (int i = 0; i<breack_point.size(); ++i)
    {
        n_knots[i] = breack_point[i].size();
        degree[i] = deg[i];
    }

    for (int i = 0 ; i < dim_ref_domain ; ++i)
        coord[i].resize(n_knots[i]);

    for (int i = 0 ; i < dim_ref_domain ; ++i)
        for (int j = 0 ; j < n_knots[i] ; ++j)
            coord[i][j] = breack_point[i][j];

    grid_ =  CartesianGrid<dim_ref_domain>::create(
                 CartesianProductArray< Real, dim_ref_domain>(coord));
}

template<int dim_ref_domain, int dim_phys_domain>
std::shared_ptr<NURBSSpace<dim_ref_domain,dim_phys_domain,1> >
IgReader< dim_ref_domain, dim_phys_domain>::get_nurbs_space()
{
    Assert(read,ExcMessage("If you want to get something from the IgReader, you should read first, use load functions"));

    for (int i = 0; i<breack_point.size(); ++i)
    {
        degree[i] = deg[i];
    }

    //mult.resize(mlt.size()) ;
    for (int i = 0 ; i < mlt.size() ; ++i)
    {
        mult.copy_data_direction(i, mlt[i]) ;
    }

    return NURBSSpace<dim_ref_domain,dim_phys_domain,1>::create(grid_,mult,degree,weights_);
}

template<int dim_ref_domain, int dim_phys_domain>
std::shared_ptr<IgMapping<NURBSSpace< dim_ref_domain,dim_phys_domain,1> > >
IgReader< dim_ref_domain, dim_phys_domain>::get_mapping_iga()
{
    Assert(read,ExcMessage("If you want to get something from the IgReader, you should read first, use load functions"));

    auto space_ptr = get_nurbs_space();

    Index l = 0;
    for (int i = 0; i < control_point.size(); ++i)
    {
        Index s = control_point[i].size();
        l = l+s;
    }

    std::vector<Index> global_dofs(l) ;
    for (Index dof = 0 ; dof < l ; ++dof)
        global_dofs[dof] = dof ;

    std::vector<Real> cp_ptr(global_dofs.size());

    std::array<Index, dim_phys_domain> off_set;

    off_set[0] = 0;
    for (int i = 1; i<control_point.size(); ++i)
        off_set[i] = off_set[i-1] + control_point[i-1].size();

    for (int i = 0; i<control_point.size(); ++i)
        for (int j = 0; j<control_point[i].size(); ++j)
        {
            Index el = off_set[i] + j;
            cp_ptr[el] = control_point[i][j];
        }

    return std::shared_ptr< IgMapping<NURBSSpace< dim_ref_domain,dim_phys_domain,1>> > (new
            IgMapping<NURBSSpace< dim_ref_domain,dim_phys_domain,1>>(space_ptr,cp_ptr));
}


template <int dim, int codim = 0>
std::shared_ptr< Mapping<dim,codim> >
ig_mapping_reader(const std::string &filename)
{

    using boost::property_tree::ptree;
    ptree xml_tree;

    read_xml(filename, xml_tree);


    using tree_value_t = typename ptree::value_type;


    LogStream out ;
    TensorIndex<dim> degree;
	CartesianProductArray<Real,dim> knots_unique_values;
	Multiplicity<dim> multiplicities;

	shared_ptr<CartesianGrid<dim>> grid;

    for (const tree_value_t & patch : xml_tree.get_child("XMLFile.Patch"))
    {
        if (boost::iequals(patch.first,"<xmlattr>"))
        {
            const int dim_ref  = patch.second.get<int>("DimReferenceDomain");
            const int dim_phys = patch.second.get<int>("DimPhysicalDomain");

            AssertThrow(dim_ref == dim,
                        ExcDimensionMismatch(dim_ref,dim));
            AssertThrow(dim_phys == dim+codim,
                        ExcDimensionMismatch(dim_phys,dim+codim));
        }



        if (boost::iequals(patch.first,"KnotVector"))
        {
            Index deg = -1 ;
            Index direction_id = -1 ;
            Size n_break_pts = 0 ;

            vector<Real> knots_unique_values_dir;
            vector<Index> multiplicities_dir;
            for (const auto & knot : patch.second)
            {
                if (boost::iequals(knot.first,"<xmlattr>"))
                {
                    deg = knot.second.get<Index>("Degree");
                    AssertThrow(deg >= 1, ExcLowerRange(deg,1));

                    direction_id = knot.second.get<Index>("Direction");
                    AssertThrow(direction_id >= 0, ExcLowerRange(direction_id,0));

                    n_break_pts = knot.second.get<Index>("NumBreakPoints");
                    AssertThrow(n_break_pts >= 2, ExcLowerRange(n_break_pts,2));
                }

                if (boost::iequals(knot.first, "BreakPoints"))
                {
                    Real knt;
                    std::stringstream line_stream(knot.second.get<std::string>(""));
                    while (line_stream >> knt)
                        knots_unique_values_dir.push_back(knt);
                }

                if (boost::iequals(knot.first, "Multiplicities"))
                {
                    Index m;
                    std::stringstream line_stream(knot.second.get<std::string>(""));
                    while (line_stream >> m)
                        multiplicities_dir.push_back(m);
                }
                //*/
            }
            AssertThrow(n_break_pts == knots_unique_values_dir.size(),
            		ExcDimensionMismatch(n_break_pts,knots_unique_values_dir.size()));
            AssertThrow(n_break_pts == multiplicities_dir.size(),
            		ExcDimensionMismatch(n_break_pts,multiplicities_dir.size()));

            degree[direction_id] = deg;
            knots_unique_values.copy_data_direction(direction_id,knots_unique_values_dir);
            multiplicities.copy_data_direction(direction_id,multiplicities_dir);
        }


        if (boost::iequals(patch.first, "ControlPoints"))
        {
            vector<Real> weights_vector;
            TensorSize<dim> n_control_points;
            vector<Real> weights;
            for (const auto &cp : patch.second)
            {
                if (boost::iequals(cp.first, "NumDir"))
                {
                    std::stringstream line_stream(cp.second.get<std::string>(""));
                    int direction_id = 0;
                    while (line_stream >> n_control_points[direction_id]);
                    {
                        AssertThrow(direction_id >= 0 && direction_id < dim,
                                    ExcIndexRange(direction_id,0,dim));
                        direction_id++;
                    }

                }
                /*
                if (boost::iequals(cp.first, "Coordinates"))
                {
                    std::string iss = cp.second.get<std::string>("");
                    std::vector<Real> row;
                    Real num;
                    std::stringstream line_stream(iss);
                    while (line_stream >> num) row.push_back(num);
                    control_point.push_back(row);
                }//*/
                if (boost::iequals(cp.first, "Weights"))
                {
                    std::stringstream line_stream(cp.second.get<std::string>(""));
                    Real w;
                    while (line_stream >> w)
                    	weights.push_back(w);
                }
                //*/
            }
            /*
                        weights_.resize(cp_per_ref_dir);
                        const int n_entries = weights_.flat_size();
                        AssertThrow(weights_.flat_size() == weights_vector.size(),
                                    ExcDimensionMismatch(weights_.flat_size(),weights_vector.size()));
                        for (Size i=0 ; i < n_entries ; ++i)
                            weights_(i) = weights_vector[i];
            //*/
        }
    }//PATCH LOOP
    grid = CartesianGrid<dim>::create(knots_unique_values);
    grid->print_info(out);
//    bool read = true;

#if 0
    for (int i = 0; i<breack_point.size(); ++i)
    {
        n_knots[i] = breack_point[i].size();
        degree[i] = deg[i];
    }

    for (int i = 0 ; i < dim_ref_domain ; ++i)
        coord[i].resize(n_knots[i]);

    for (int i = 0 ; i < dim_ref_domain ; ++i)
        for (int j = 0 ; j < n_knots[i] ; ++j)
            coord[i][j] = breack_point[i][j];

    grid_ =  CartesianGrid<dim_ref_domain>::create(
                 CartesianProductArray< Real, dim_ref_domain>(coord));
#endif

    std::shared_ptr< Mapping<dim,codim> > map;

    AssertThrow(false,ExcNotImplemented());

    return map;
}



IGA_NAMESPACE_CLOSE


#include <igatools/io/ig_reader.inst>


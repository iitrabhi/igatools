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

#include <igatools/basis_functions/reference_element_handler.h>

#include <igatools/basis_functions/bspline_element.h>
#include <igatools/basis_functions/bspline_element_handler.h>

#include <igatools/basis_functions/nurbs_space.h>
#include <igatools/basis_functions/nurbs_element_handler.h>

using std::shared_ptr;

IGA_NAMESPACE_OPEN


template<int dim_, int range_ , int rank_>
ReferenceElementHandler<dim_, range_, rank_>::
ReferenceElementHandler(shared_ptr<const Space> space)
    :
    grid_handler_(space->get_grid()),
    space_(space)
{
    Assert(space != nullptr, ExcNullPtr());
};


template<int dim_, int range_ , int rank_>
shared_ptr<ReferenceElementHandler<dim_,range_,rank_> >
ReferenceElementHandler<dim_, range_, rank_>::
create(shared_ptr<const Space> space)
{
    using BSplineSp = const BSplineSpace<dim_,range_,rank_>;
    auto bsp_space = std::dynamic_pointer_cast< BSplineSp >(space);

    using NURBSSp = const NURBSSpace<dim_,range_,rank_>;
    auto nrb_space = std::dynamic_pointer_cast< NURBSSp >(space);

    std::shared_ptr<ReferenceElementHandler<dim_,range_,rank_> > elem_handler = nullptr;
    if (bsp_space)
    {
        elem_handler = BSplineElementHandler<dim_,range_,rank_>::create(bsp_space);
    }
    else if (nrb_space)
    {
#ifdef NURBS
        elem_handler = NURBSElementHandler<dim_,range_,rank_>::create(nrb_space);
#else
        Assert(false,ExcMessage("NURBS support disabled from configuration cmake parameters."));
        AssertThrow(false,ExcMessage("NURBS support disabled from configuration cmake parameters."));
#endif
    }
    else
    {
        Assert(false,ExcInvalidState());
        AssertThrow(false,ExcInvalidState());
    }
    return elem_handler;
}


template<int dim_, int range_ , int rank_>
auto
ReferenceElementHandler<dim_, range_, rank_>::
get_space() const -> shared_ptr<const Space>
{
    Assert(space_ != nullptr,ExcNullPtr());
    return space_;
}

template<int dim_, int range_ , int rank_>
const GridElementHandler<dim_> &
ReferenceElementHandler<dim_, range_, rank_>::
get_grid_handler() const
{
    return this->grid_handler_;
}


template<int dim_, int range_ , int rank_>
void
ReferenceElementHandler<dim_, range_, rank_>::
reset(const ValueFlags &flag, const eval_pts_variant &eval_pts)
{
    using ElemProperty = typename CartesianGrid<dim_>::ElementProperty;
    const std::set<int> active_elems_id =
        this->get_space()->get_grid()->get_elements_id_same_property(ElemProperty::active);

    this->reset_selected_elements(
        flag,
        eval_pts,
        vector<int>(active_elems_id.begin(),active_elems_id.end()));
}



template<int dim_, int range_ , int rank_>
void
ReferenceElementHandler<dim_, range_, rank_>::
reset_one_element(
    const ValueFlags &flag,
    const eval_pts_variant &eval_points,
    const int elem_flat_id)
{
    this->reset_selected_elements(flag,eval_points,vector<int>(1,elem_flat_id));
}



IGA_NAMESPACE_CLOSE

#include <igatools/basis_functions/reference_element_handler.inst>
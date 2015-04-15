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



#include <igatools/basis_functions/space_element.h>

IGA_NAMESPACE_OPEN





template<int dim,int codim,int range,int rank>
SpaceElement<dim,codim,range,rank>::
SpaceElement(const self_t &elem,
             const CopyPolicy &copy_policy)
    :
    base_t(elem,copy_policy)
{
    if (elem.local_cache_ != nullptr)
    {
        if (copy_policy == CopyPolicy::shallow)
        {
            local_cache_ = elem.local_cache_;
        }
        else
        {
            local_cache_ = std::shared_ptr<LocalCache>(new LocalCache(*elem.local_cache_));
        }
    }
}



template<int dim,int codim,int range,int rank>
void
SpaceElement<dim,codim,range,rank>::
copy_from(const self_t &elem,
          const CopyPolicy &copy_policy)
{
    if (this != &elem)
    {
        base_t::copy_from(elem,copy_policy);


        if (copy_policy == CopyPolicy::deep)
        {
            Assert(elem.local_cache_ != nullptr, ExcNullPtr());
            local_cache_ = std::shared_ptr<LocalCache>(new LocalCache(*elem.local_cache_));
        }
        else if (copy_policy == CopyPolicy::shallow)
        {
            local_cache_ = elem.local_cache_;
        }
        else
        {
            Assert(false,ExcNotImplemented());
            AssertThrow(false,ExcNotImplemented());
        }
    }
}



template<int dim,int codim,int range,int rank>
void
SpaceElement<dim,codim,range,rank>::
deep_copy_from(const self_t &elem)
{
    this->copy_from(elem,CopyPolicy::deep);
}



template<int dim,int codim,int range,int rank>
void
SpaceElement<dim,codim,range,rank>::
shallow_copy_from(const self_t &elem)
{
    this->copy_from(elem,CopyPolicy::shallow);
}



template<int dim,int codim,int range,int rank>
auto
SpaceElement<dim,codim,range,rank>::
operator=(const self_t &element) -> self_t &
{
    this->shallow_copy_from(element);
    return (*this);
}



template<int dim,int codim,int range,int rank>
void
SpaceElement<dim,codim,range,rank>::
print_info(LogStream &out) const
{
    base_t::print_info(out);
}

template<int dim,int codim,int range,int rank>
void
SpaceElement<dim,codim,range,rank>::
print_cache_info(LogStream &out) const
{
    base_t::print_cache_info(out);

    Assert(local_cache_ != nullptr,ExcNullPtr());
    local_cache_->print_info(out);
}


template<int dim,int codim,int range,int rank>
void
SpaceElement<dim,codim,range,rank>::LocalCache::
print_info(LogStream &out) const
{
    cacheutils::print_caches(values_, out);

//    out.begin_item("Element Cache:");
//    get_value_cache<0>(0).print_info(out);
//    out.end_item();
//
//    for (int i = 0 ; i < n_faces ; ++i)
//    {
//        out.begin_item("Face "+ std::to_string(i) + " Cache:");
//        get_value_cache<1>(i).print_info(out);
//        out.end_item();
//    }

}


IGA_NAMESPACE_CLOSE


#include <igatools/basis_functions/space_element.inst>



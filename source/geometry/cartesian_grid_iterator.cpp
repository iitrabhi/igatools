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

#include <igatools/geometry/cartesian_grid_iterator.h>
#include <igatools/geometry/cartesian_grid.h>

using std::shared_ptr;

IGA_NAMESPACE_OPEN

template <class Accessor>
CartesianGridIteratorBase<Accessor>::
CartesianGridIteratorBase(std::shared_ptr<ContainerType> grid,
                          const ListIt index,
                          const PropId &prop)
    :
    accessor_(grid->create_element(index,prop))
{}


#if 0
template <class Accessor>
CartesianGridIteratorBase<Accessor>::
CartesianGridIteratorBase(
    std::shared_ptr<Accessor> accessor_ptr,
    const std::string &elem_property)
    :
    accessor_(accessor_ptr),
    elem_property_(elem_property)
{
#ifndef NDEBUG
    Assert(accessor_ != nullptr,ExcNullPtr());

    const auto flat_index = accessor_->get_flat_index();
    if (flat_index != IteratorState::pass_the_end)
        Assert(accessor_->get_grid()->test_if_element_has_property(flat_index,elem_property_),
               ExcMessage("Iterator defined on an element not satisfying the requested property \""
                          + elem_property_ + "\"."));
#endif
}
#endif


template <class Accessor>
CartesianGridIteratorBase<Accessor>::
CartesianGridIteratorBase(const CartesianGridIteratorBase<Accessor> &it,
                          const CopyPolicy &copy_policy)
{
    if (copy_policy == CopyPolicy::deep)
    {
        accessor_->deep_copy_from(*(it.accessor_));
    }
    else if (copy_policy == CopyPolicy::shallow)
    {
        accessor_->shallow_copy_from(*(it.accessor_));
    }
    else
    {
        Assert(false,ExcNotImplemented());
        AssertThrow(false,ExcNotImplemented());
    }
}


#if 0
template <class Accessor>
bool
CartesianGridIteratorBase<Accessor>::
jump(const TensIndex &increment)
{
    auto tensor_index = accessor_->get_tensor_index();
    tensor_index += increment;

    const auto grid = accessor_->get_grid();
    const auto n_elems = grid->get_num_intervals();
    bool valid_tensor_index = true;
    Index flat_index;
    for (const auto i : UnitElement<ContainerType::dim>::active_directions)
        if (tensor_index[i] < 0 || tensor_index[i] >= n_elems[i])
        {
            valid_tensor_index = false;
            flat_index = IteratorState::invalid;
            break;
        }

    if (valid_tensor_index)
        flat_index = grid->tensor_to_flat(tensor_index);

    accessor_->move_to(flat_index);

    return valid_tensor_index;
}

template <class Accessor>
void
CartesianGridIteratorBase<Accessor>::
move_to(const Index flat_index)
{
    accessor_->move_to(flat_index);
}


template <class Accessor>
void
CartesianGridIteratorBase<Accessor>::
move_to(const TensIndex &tensor_index)
{
    this->move_to(accessor_->get_grid()->tensor_to_flat(tensor_index));
}
#endif

template <class Accessor>
CartesianGridIteratorBase<Accessor> &
CartesianGridIteratorBase<Accessor>::
operator++()
{
    accessor_->operator++();
    return *this;
}



template <class Accessor>
bool
CartesianGridIteratorBase<Accessor>::
operator==(const CartesianGridIteratorBase<Accessor> &i) const
{
    return *accessor_ == *i.accessor_;
}



template <class Accessor>
bool
CartesianGridIteratorBase<Accessor>::
operator>(const CartesianGridIteratorBase<Accessor> &i) const
{
    return (*accessor_ > *i.accessor_);
}

template <class Accessor>
bool
CartesianGridIteratorBase<Accessor>::
operator<(const CartesianGridIteratorBase<Accessor> &i) const
{
    return (*accessor_ < *i.accessor_);
}



template <class Accessor>
bool
CartesianGridIteratorBase<Accessor>::
operator!=(const CartesianGridIteratorBase<Accessor> &i) const
{
    return accessor_->operator != (*(i.accessor_));
}

#if 0
template <class Accessor>
Index
CartesianGridIteratorBase<Accessor>::
get_flat_index() const
{
    return accessor_->get_flat_index();
}

template <class Accessor>
auto
CartesianGridIteratorBase<Accessor>::
get_tensor_index() const -> TensIndex
{
    return accessor_->get_tensor_index();
}
#endif



template <class Accessor>
Accessor &
CartesianGridIterator<Accessor>::
operator * ()
{
    return *this->accessor_;
}



template <class Accessor>
Accessor *
CartesianGridIterator<Accessor>::
operator -> ()
{
    return this->accessor_.get();
}


template <class Accessor>
const Accessor &
CartesianGridIterator<Accessor>::
operator * () const
{
    return *this->accessor_;
}


template <class Accessor>
const Accessor *
CartesianGridIterator<Accessor>::
operator -> () const
{
    return this->accessor_.get();
}


template <class Accessor>
const Accessor &
CartesianGridConstIterator<Accessor>::
operator * () const
{
    return *this->accessor_;
}



template <class Accessor>
const Accessor *
CartesianGridConstIterator<Accessor>::
operator -> () const
{
    return this->accessor_.get();
}

IGA_NAMESPACE_CLOSE

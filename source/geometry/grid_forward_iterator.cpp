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

#include <igatools/geometry/grid_forward_iterator.h>

IGA_NAMESPACE_OPEN

template <typename Accessor>
inline
GridForwardIterator<Accessor>::
GridForwardIterator(std::shared_ptr<ContainerType> grid,
                    const Index index)
    :
    accessor_(grid, index)
{}



template <typename Accessor>
inline
GridForwardIterator<Accessor>::
GridForwardIterator(std::shared_ptr<ContainerType> grid,
                    const TensorIndex<dim> &index)
    :
    accessor_(grid, index)
{}


template <typename Accessor>
inline
bool
GridForwardIterator<Accessor>::
jump(const TensorIndex<dim> &increment)
{
    return accessor_.jump(increment);
}

template <typename Accessor>
inline
void GridForwardIterator<Accessor>::
move_to(const Index flat_index)
{
    accessor_.move_to(flat_index);
}


template <typename Accessor>
inline
void
GridForwardIterator<Accessor>::
move_to(const TensorIndex<dim> &tensor_index)
{
    accessor_.move_to(tensor_index);
}


template <typename Accessor>
inline
GridForwardIterator<Accessor> &
GridForwardIterator<Accessor>::
operator++()
{
    ++accessor_;
    return *this;
}



template <typename Accessor>
inline
const Accessor &
GridForwardIterator<Accessor>::
operator * () const
{
    return accessor_;
}



template <typename Accessor>
inline
Accessor &
GridForwardIterator<Accessor>::
operator * ()
{
    return accessor_;
}



template <typename Accessor>
inline
const Accessor *
GridForwardIterator<Accessor>::
operator -> () const
{
    return &(this->operator* ());
}



template <typename Accessor>
inline
Accessor *
GridForwardIterator<Accessor>::
operator -> ()
{
    return &(this->operator* ());
}



template <typename Accessor>
inline
bool
GridForwardIterator<Accessor>::
operator==(const GridForwardIterator<Accessor> &i) const
{
    return accessor_ == i.accessor_;
}



template <typename Accessor>
inline
bool
GridForwardIterator<Accessor>::
operator!=(const GridForwardIterator<Accessor> &i) const
{
    return accessor_.operator != (i.accessor_);
}

template <typename Accessor>
inline
Index
GridForwardIterator<Accessor>::
get_flat_index() const
{
    return accessor_.get_flat_index();
}

template <typename Accessor>
inline
auto
GridForwardIterator<Accessor>::
get_tensor_index() const -> TensorIndex<dim>
{
    return accessor_.get_tensor_index();
}

IGA_NAMESPACE_CLOSE

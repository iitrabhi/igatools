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

#ifndef GRID_FORWARD_ITERATOR_H_
#define GRID_FORWARD_ITERATOR_H_

#include <igatools/base/config.h>
#include <igatools/utils/tensor_index.h>

#include <iterator>
#include <memory>

IGA_NAMESPACE_OPEN



/**
 * @brief Forward iterator on objects that have a "grid-like" structure.
 *
 *
 * Its main features are:
 * - it takes an accessor's type as template parameter;
 * - can be incremented (using the prefix operator <tt>++</tt>) in order to point to the next element
 * in the container;
 * - when dereferenced (using the dereferencing operator <tt>*</tt> or <tt>-></tt>),
 * it returns an accessor object.
 *
 * It not fulfills all the requirements to be an
 * <a href="http://www.cplusplus.com/reference/iterator/ForwardIterator/">STL forward iterator</a>
 * because:
 * - it is not default constructible;
 * - the postfix operator <tt>++</tt> is not defined.
 *
 * The object pointed to the GridForwardIterator is called <em>accessor</em>
 * and its type is passed as template argument <tt>Accessor</tt>
 * of the GridForwardIterator.
 *
 * The <em>accessor</em> is an object that can fetch and use data stored in objects that have
 * a "grid-like" structure. The type of the object with this "grid-like" structure,
 * associated to the type <tt>Accessor</tt> can be retrieved with the type
 * <tt>Accessor::ContainerType</tt>.
 *
 * Using the accessor, the structure of the "grid-like" classes is hidden
 * from the application program.
 *
 * <h3>Purpose</h3>
 *
 * Iterators are used whenever a loop over all (some) elements
 * is to be performed. These loops can then be coded like this:
 * @code
   auto elem = grid.begin();
   const auto elem_end = grid.end();
   for (; elem!=end; ++elem)
     if (elem->at_boundary())
        elem->vertex(k);
  @endcode
 * Note the usage of <tt>++i</tt> instead of <tt>i++</tt> since this
 * does not involve temporaries and copying. It is recommended to use
 * a fixed value <tt>end</tt> inside the loop instead of
 * <tt>patch.end()</tt>, since the creation and copying of these
 * iterators is rather expensive compared to normal pointers.
 *
 * The same previous loop can be performed or using the C++11 syntax called
 * <em>range-based for loop</em>
 * @code
   for (const auto & elem : grid) // the elem type is: const Accessor&
     if (elem.at_boundary())
        elem.vertex(k);
  @endcode
 *
 *
 * Iterators are not much slower than operating directly on the data
 * structures, since they perform the loops that you had to handcode
 * yourself anyway. Most iterator and accessor functions are inlined.
 *
 * The main functionality of iterators, resides in the <tt>++</tt> operator.
 * This move the iterator forward
 * just as if it were a pointer into an array.
 * Here, this operation is
 * not so easy, since it may include dealing with a tensor product
 * structure or something else. But this is completely
 * hidden from the user, though you can still create an iterator
 * pointing to an arbitrary element.  Actually, the operation of
 * moving iterator forward is not done in the iterator
 * classes, but rather in the <tt>Accessor</tt> classes.
 * Since these are passed
 * as template arguments, you can write your own versions here to add
 * more functionality.
 *
 * Furthermore, the iterators described here satisfy the requirement of
 * input and forward iterators as stated by the C++ standard and
 * the STL documentation. It is therefore possible to use the
 * functions from the algorithm section of the C++ standard,
 * e.g. <em>count_if</em>.
 *
 * <h3>Implementation</h3>
 *
 * The iterator class itself does not have much functionality. It only
 * becomes useful when assigned an <tt>Accessor</tt> (the template
 * parameter), which really does the access to data. An <tt>Accessor</tt>
 * has to fulfill some requirements:
 *
 * <ul>
 * <li> It must have a type called <tt>ContainerType</tt> representing
 * the type of the "grid-like" container from which the accessor is getting the data.
 *
 * <li> It must have a member named
 * <tt>present_index</tt> storing the address of the element in the
 * <tt>ContainerType</tt> object presently pointed to. These data have to be
 * accessible by all grid iterator.
 *
 * <li> It must have a constructor which takes a <tt>const AccessorOfType*</tt> argument
 * representing the accessed container and
 * an <tt>Index</tt> argument, denoting the index within the "grid-like container".
 *
 * <li> It must have void operator <tt>++</tt> that implements the advance of the accessor
 * whitin the container.
 * </ul>
 * Then the iterator is able to do what it is supposed to. All of the
 * necessary functions are implemented in the <tt>Accessor</tt> base
 * class, but you may write your own version (non-virtual, since we
 * use templates) to add functionality.
 *
 *
 *
 * <h3>Past-the-end iterators</h3>
 *
 * There is a representation of past-the-end-pointers, denoted by special
 * values of the member variable @p present_index:
 * <ul>
 * <li> <tt>present_index != IteratorState::past_the_end</tt>, then the object is valid
 * <li> <tt>present_index == IteratorState::past_the_end</tt>, then the iterator points
 * past the end; in all other cases, the iterator is considered invalid.
 * </ul>
 *
 * @see IteratorState
 *
 * @tparam Accessor Type of the accessor.
 *
 * @ingroup iterators
 * @author M.Martinelli, S.Pauletti
 * @date 2012,2013,2014
 */
template <typename Accessor>
class GridForwardIterator
    : public std::iterator<std::forward_iterator_tag, Accessor>
{
public:
    /** Type of the accessor. */
    using AccessorType = Accessor;

    /** Type of the grid-like container . */
    using ContainerType = typename Accessor::ContainerType;

    static const int dim = ContainerType::dim;

    /** @name Constructors & destructor */
    ///@{
    /**
     * Default constructor. Not allowed to be used.
     */
    GridForwardIterator() = default;

    /**
     * Construct an iterator on a grid-type container
     * grid pointing to the element of given index.
     */
    GridForwardIterator(std::shared_ptr<ContainerType> grid,
                        const Index index);

    /**
     * Construct an iterator on a grid-type container
     * grid pointing to the element of given index.
     */
    GridForwardIterator(std::shared_ptr<ContainerType> grid,
                        const TensorIndex<dim> &index);

    /** Copy constructor. */
    GridForwardIterator(const GridForwardIterator<Accessor> &it) = default;

    /** Move constructor. */
    GridForwardIterator(GridForwardIterator<Accessor> &&it) = default;

    /** Destructor */
    ~GridForwardIterator() = default ;
    ///@}

    /** @name Assignment operators */
    ///@{
    /** Copy assignment operator. */
    GridForwardIterator<Accessor> &
    operator=(const GridForwardIterator<Accessor> &it)
    {
        accessor_ = it.accessor_;
        return *this;
    }

    /** Move assignment operator. */
    GridForwardIterator<Accessor> &
    operator=(GridForwardIterator<Accessor> &&) = default;
    ///@}

    /** @name Dereferencing operators */
    ///@{
    /**
     *  Dereferencing operator, returns a
     *  const reference to the accessor.
     */
    const Accessor &operator*() const;

    /**
     *  Dereferencing operator, returns a
     *  reference to the accessor.
     */
    Accessor &operator*();

    /**
     *  Dereferencing operator, returns a
     *  const pointer to the accessor.
     */
    const Accessor *operator->() const;

    /**
     *  Dereferencing operator, returns a
     *  pointer to the accessor.
     */
    Accessor *operator->();
    ///@}

    /** @name Comparison operators */
    ///@{
    /** Compare for equality.*/
    bool operator== (const GridForwardIterator &) const;

    /** Compare for inequality.*/
    bool operator!= (const GridForwardIterator &) const;
    ///@}


    /** @name Functions/operators for moving the element in the CartesianGrid.*/
    ///@{
    /**
     * Moves the iterator to the position that differs from the current one
     * for the quantity given by @p increment.
     *
     * If the resulting position after the movement is valid (i.e. within the grid), then the function
     * returns true, otherwise it returns false.
     */
    bool jump(const TensorIndex<dim> &increment);

    /**
     * Sets the index of the iterator using the flatten representation.
     * @note This function also updates the index for the tensor representation.
     * @warning This may be a dangerous function, be careful when using it
     * as it is easy to use incorrectly. Only use it if you know what you
     * are doing.
     */
    void move_to(const Index flat_index);


    /**
     * Sets the index of the iterator using the tensor representation.
     * @note This function also updates the index for the flatten representation.
     * @warning this may be a dangerous function, be careful when using it
     * as it is easy to use incorrectly. Only use it if you know what you
     * are doing.
     */
    void move_to(const TensorIndex<dim> &tensor_index);

    /**
     *  Prefix <tt>++</tt> operator: <tt>++i</tt>. This
     *  operator advances the iterator to
     *  the next element and returns
     *  a reference to <tt>*this</tt>.
     */
    GridForwardIterator<Accessor> &operator++();
    ///@}


    /**
     * @name Functions related to the indices of the element in the CartesianGrid pointed
     * by the iterator.
     */
    ///@{
    /** Returns the index of the element in its flatten representation. */
    Index get_flat_index() const;

    /** Returns the index of the element in its tensor representation. */
    TensorIndex<dim> get_tensor_index() const;
    ///@}

protected:
    /** Object holding the Real data. */
    Accessor accessor_ ;
};


template <typename Accessor>
bool operator> (const GridForwardIterator<Accessor> &it1, const GridForwardIterator<Accessor> &it2)
{
    return it1.get_flat_index() > it2.get_flat_index();
}

template <typename Accessor>
bool operator< (const GridForwardIterator<Accessor> &it1, const GridForwardIterator<Accessor> &it2)
{
    return it1.get_flat_index() < it2.get_flat_index();
}
IGA_NAMESPACE_CLOSE

#endif /* PATCH_ITERATORS_H_ */

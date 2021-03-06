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

#ifndef __OBJECTS_CONTAINER_READER_H_
#define __OBJECTS_CONTAINER_READER_H_

#include <igatools/base/config.h>

#include <set>

#ifdef IGATOOLS_WITH_XML_IO


IGA_NAMESPACE_OPEN

class XMLElement;
class ObjectsContainer;
template <int dim> class Grid;
template <class T> class SafeSTLVector;
class IgCoefficients;
class LogStream;
template <class T1, class T2> class SafeSTLMap;
template <int dim, int range, int rank> class ReferenceBasis;
template <int dim, int range> class GridFunction;
template <int dim, int codim, int range, int rank> class Function;

/**
 * @brief Helper class for creating an @ref ObjectsContainer parsed from
 * a XML file format.
 *
 * It receives an input file that is parsed by creating a
 * @ref XMLDocument. This XML documents contains a main @ref XMLElement,
 * with the tag name <tt>Igatools</tt>, that contains all the information.
 *
 * Before starting to parse the @ref XMLDocument, it is validated against
 * an XML schema definition. This schema definition controls the main
 * structure of the file: which elements should be present, attributes,
 * types, enumerations, etc.
 * But the schema is unable to control other questions, e.g. if a certain
 * @ref Grid, used by a @ref SplineSpace, is defined in the file.
 * This schema is stored as a @ref std::string in the private member
 * @ref XML_SCHEMA_.
 *
 * The current igatools file format version is specified by static
 * variable @ref IGATOOLS_FILE_FORMAT_VERSION.
 *
 * Currently, this class is able to parse the following classes:
 * - @ref Grid
 * - @ref SplineSpace
 * - @ref BSpline
 * - @ref NURBS
 * - @ref grid_functions::IdentityGridFunction
 * - @ref grid_functions::ConstantGridFunction
 * - @ref grid_functions::LinearGridFunction
 * - @ref IgGridFunction
 * - @ref Domain
 * - @ref PhysicalBasis
 * - @ref functions::ConstantFunction
 * - @ref functions::LinearFunction
 * - @ref IgFunction
 *
 * If any type different from the ones above is found, an exception
 * is thrown.
 *
 * The @ref ObjectsContainerXMLWriter class writes containers into files
 * with the format that this class is expecting for being parsed.
 *
 * The XML format is detailed here @subpage in_out.
 *
 * @warning It could be unsafe to take a @ref XMLElement, contained
 * inside in the @ref XMLDocument outside of the class in order to be
 * reused somewhere else. It could cause problems with the @p Xerces-c
 * memory deallocation. Not getter should be provided for retrieving it.
 *
 * @see ObjectsContainer
 * @see ObjectsContainerXMLWriter
 * @see XMLDocument
 * @see XMLElement
 *
 * @author P. Antolin
 * @date 2015
 */
class ObjectsContainerXMLReader
{
private:

  /** @name Types and static values */
  ///@{

  /** Type for the current class. */
  typedef ObjectsContainerXMLReader Self_;

  /** Type for a shared pointer of the current class. */
  typedef std::shared_ptr<Self_> SelfPtr_;

  /** Type for mapping between @p Id numbers local to the input file and
   *  object unique @p Id. */
  typedef SafeSTLMap<Index, Index> IdMap_;

  /// Alias for a shared pointer of const @ref ReferenceBasis
  template <int dim, int range, int rank>
  using RefBasisPtr_ = std::shared_ptr<const ReferenceBasis<dim, range, rank>>;

  /// Alias for a shared pointer of const @ref GridFunction
  template <int dim, int range>
  using GridFuncPtr_ = std::shared_ptr<const GridFunction<dim, range>>;

  /// Alias for a shared pointer of const @ref Function
  template <int dim, int codim, int range, int rank>
  using FuncPtr_ = std::shared_ptr<const Function<dim, codim, range, rank>>;

public:
  /// Current igatools file format version.
  static const std::string IGATOOLS_FILE_FORMAT_VERSION;

private:
  /// Statically defined string defining the XML schema for validating the input files.
  static const std::string XML_SCHEMA_;

  ///@}

  /** @name Constructors, destructor, assignment operators and creators */
  ///@{

  /**
   * @brief Default constructor.
   * @note Deleted, not allowed to be used.
   */
  ObjectsContainerXMLReader() = delete;

  /**
   * @brief Copy constructor.
   * @note Deleted, not allowed to be used.
   */
  ObjectsContainerXMLReader(const ObjectsContainerXMLReader &) = delete;

  /**
   * @brief Move constructor.
   * @note Deleted, not allowed to be used.
   */
  ObjectsContainerXMLReader(ObjectsContainerXMLReader &&) = delete;

  /**
   * @brief Copy assignment operator.
   * @note Deleted, not allowed to be used.
   */
  ObjectsContainerXMLReader &operator= (const ObjectsContainerXMLReader &) = delete;

  /**
   * @brief Move assignment operator.
   * @note Deleted, not allowed to be used.
   */
  ObjectsContainerXMLReader &operator= (ObjectsContainerXMLReader &&) = delete;

  ///@}

public:

  /** @name Container parsers */
  ///@{

  /**
   * @brief Creates an @ref ObjectsContainer by reading the objects
   * from a given @p file_path and inserting them as non constant.
   *
   * @param[in] file_path Path of the file to be parsed.
   * @return Objects container with all the objects stored inside.
   */
  static std::shared_ptr<ObjectsContainer> parse(const std::string &file_path);

  /**
   * @brief Creates an @ref ObjectsContainer by reading the objects
   * from a given @p file_path and creating and inserting them as
   * constant.
   *
   * @param[in] file_path Path of the file to be parsed.
   * @return Objects container with all the objects stored inside.
   */
  static std::shared_ptr<ObjectsContainer> parse_const(const std::string &file_path);

  ///@}

private:


  /** @name Methods for parsing all the objects. */
  ///@{

  /**
   * @brief Reads all the @ref Grid contained into the XML document
   * @p xml_elem and stores them into the @p container.
   *
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the objects
   *                and also retrieving other ones needed.
   */
  static void parse_grids(const std::shared_ptr<XMLElement> xml_elem,
                          const bool parse_as_constant,
                          IdMap_ &id_map,
                          const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads all the @ref SplineSpace
   * contained into the XML document @p xml_elem and stores them into
   * the @p container.
   *
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the objects
   *                and also retrieving other ones needed.
   */
  static void parse_spline_spaces(const std::shared_ptr<XMLElement> xml_elem,
                                  const bool parse_as_constant,
                                  IdMap_ &id_map,
                                  const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads all the @ref ReferenceBasis and @ref GridFunction
   * contained into the XML document @p xml_elem and stores them into
   * the @p container.
   *
   * They are parsed in the proper order to avoid conflicts in the
   * declaration of @ref NURBS that take @ref IgGridFunction as arguments.
   *
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the objects
   *                and also retrieving other ones needed.
   */
  static void parse_ref_bases_and_grid_functions(const std::shared_ptr<XMLElement> xml_elem,
                                                 const bool parse_as_constant,
                                                 IdMap_ &id_map,
                                                 const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads all the @ref RerenceBasis
   * contained into the XML document @p xml_elem and stores them into
   * the @p container.
   *
   * This function must be called twice. The first time, all the @ref
   * ReferenceBasis based on @ref BSpline are parsed.
   * During the second call, the @ref NURBS basis are parsed.
   *
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] indicates if this the first time that the function is called,
   * or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the objects
   *                and also retrieving other ones needed.
   */
  static void parse_ref_bases(const std::shared_ptr<XMLElement> xml_elem,
                              const bool parse_as_constant,
                              const bool &first_parsing,
                              IdMap_ &id_map,
                              const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads all the @ref GridFunction
   * contained into the XML document @p xml_elem and stores them into
   * the @p container.
   *
   * This function must be called twice. The first time, all the @ref
   * GridFunction, except for the @ref IgGridFunction build upon @ref NURBS
   * basis, are parsed.
   *
   * In the second call, the @ref IgGridFunction depending on
   * @ref NURBS basis are parsed.
   *
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] indicates if this the first time that the function is called,
   * or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the objects
   *                and also retrieving other ones needed.
   */
  static void parse_grid_functions(const std::shared_ptr<XMLElement> xml_elem,
                                   const bool parse_as_constant,
                                   const bool &first_parsing,
                                   IdMap_ &id_map,
                                   const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads all the @ref Domain
   * contained into the XML document @p xml_elem and stores them into
   * the @p container.
   *
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the objects
   *                and also retrieving other ones needed.
   */
  static void parse_domains(const std::shared_ptr<XMLElement> xml_elem,
                            const bool parse_as_constant,
                            IdMap_ &id_map,
                            const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads all the @ref PhysicalBasis
   * contained into the XML document @p xml_elem and stores them into
   * the @p container.
   *
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the objects
   *                and also retrieving other ones needed.
   */
  static void parse_phys_bases(const std::shared_ptr<XMLElement> xml_elem,
                               const bool parse_as_constant,
                               IdMap_ &id_map,
                               const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads all the @ref Function
   * contained into the XML document @p xml_elem and stores them into
   * the @p container.
   *
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the objects
   *                and also retrieving other ones needed.
   */
  static void parse_functions(const std::shared_ptr<XMLElement> xml_elem,
                              const bool parse_as_constant,
                              IdMap_ &id_map,
                              const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads a single @ref Grid XML element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the Grid.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the object
   *                and also retrieving other ones needed.
   */
  template <int dim>
  static void parse_grid(const std::shared_ptr<XMLElement> xml_elem,
                         const bool parse_as_constant,
                         IdMap_ &id_map,
                         const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads a single @ref SplineSpace XML element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the SplineSpace.
   * @tparam range Range of the SplineSpace.
   * @tparam rank Rank of the SplineSpace.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the object
   *                and also retrieving other ones needed.
   */
  template <int dim, int range, int rank>
  static void parse_spline_space(const std::shared_ptr<XMLElement> xml_elem,
                                 const bool parse_as_constant,
                                 IdMap_ &id_map,
                                 const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads a single @ref BSpline XML element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the BSpline basis.
   * @tparam range Range of the BSpline basis.
   * @tparam rank Rank of the BSpline basis.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   */
  template <int dim, int range, int rank>
  static RefBasisPtr_<dim, range, rank>
  parse_bspline(const std::shared_ptr<XMLElement> xml_elem,
                const Index &local_object_id,
                const bool parse_as_constant,
                const std::shared_ptr<const ObjectsContainer> container,
                IdMap_ &id_map);

  /**
   * @brief Reads a single @ref NURBS XML element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the NURBS basis.
   * @tparam range Range of the NURBS basis.
   * @tparam rank Rank of the NURBS basis.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   */
  template <int dim, int range, int rank>
  static RefBasisPtr_<dim, range, rank>
  parse_nurbs(const std::shared_ptr<XMLElement> xml_elem,
              const Index &local_object_id,
              const bool parse_as_constant,
              const std::shared_ptr<const ObjectsContainer> container,
              IdMap_ &id_map);

  /**
   * @brief Reads a single @ref grid_functions::IdentityGridFunction XML
   * element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the grid function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   */
  template <int dim, int range>
  static GridFuncPtr_<dim, range>
  parse_identity_grid_function(const std::shared_ptr<XMLElement> xml_elem,
                               const Index &local_object_id,
                               const bool parse_as_constant,
                               const std::shared_ptr<const ObjectsContainer> container,
                               IdMap_ &id_map,
                               typename std::enable_if_t<dim == range> * = 0);

  /**
   * @brief Reads a single @ref grid_functions::IdentityGridFunction XML
   * element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the grid function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   *
   * @warning This version of the method corresponds to the case
   * in which <tt> dim != range </tt>, that is not valid for
   * @ref IdentityGridFunction. An exception will be thrown.
   */
  template <int dim, int range>
  static GridFuncPtr_<dim, range>
  parse_identity_grid_function(const std::shared_ptr<XMLElement> xml_elem,
                               const Index &local_object_id,
                               const bool parse_as_constant,
                               const std::shared_ptr<const ObjectsContainer> container,
                               IdMap_ &id_map,
                               typename std::enable_if_t<dim != range> * = 0);

  /**
   * @brief Reads a single @ref grid_functions::ConstantGridFunction XML
   * element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the grid function.
   * @tparam range Range of the grid function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   */
  template <int dim, int range>
  static GridFuncPtr_<dim, range>
  parse_constant_grid_function(const std::shared_ptr<XMLElement> xml_elem,
                               const Index &local_object_id,
                               const bool parse_as_constant,
                               const std::shared_ptr<const ObjectsContainer> container,
                               IdMap_ &id_map);

  /**
   * @brief Reads a single @ref grid_functions::LinearGridFunction XML
   * element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the grid function.
   * @tparam range Range of the grid function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   */
  template <int dim, int range>
  static GridFuncPtr_<dim, range>
  parse_linear_grid_function(const std::shared_ptr<XMLElement> xml_elem,
                             const Index &local_object_id,
                             const bool parse_as_constant,
                             const std::shared_ptr<const ObjectsContainer> container,
                             IdMap_ &id_map);

  /**
   * @brief Reads a single @ref IgGridFunction XML element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the grid function.
   * @tparam range Range of the grid function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   */
  template <int dim, int range>
  static GridFuncPtr_<dim, range>
  parse_ig_grid_function(const std::shared_ptr<XMLElement> xml_elem,
                         const Index &local_object_id,
                         const bool parse_as_constant,
                         const std::shared_ptr<const ObjectsContainer> container,
                         IdMap_ &id_map);

  /**
   * @brief Checks if the @ref ReferenceBasis reference
   * from an @ref IgGridFunction has already being parsed.
   *
   * @tparam dim Dimension of the grid function.
   * @tparam range Range of the grid function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return True if the reference space has already being parsed, false
   * otherwise
   */
  template <int dim, int range>
  static bool is_ref_basis_for_ig_grid_function_parsed
  (const std::shared_ptr<XMLElement> xml_elem,
   const bool parse_as_constant,
   const std::shared_ptr<const ObjectsContainer> container,
   IdMap_ &id_map);

  /**
   * @brief Reads a single @ref Domain XML element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the domain.
   * @tparam codim Codimension of the domain.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the object
   *                and also retrieving other ones needed.
   */
  template <int dim, int codim>
  static void parse_domain(const std::shared_ptr<XMLElement> xml_elem,
                           const bool parse_as_constant,
                           IdMap_ &id_map,
                           const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads a single @ref PhysicalBasis XML element contained
   * in @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the physical basis.
   * @tparam range Range of the physical basis.
   * @tparam rank Rank of the physical basis.
   * @tparam codim Codimension of the physical basis.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @param[in,out] container Container for inserting the object
   *                and also retrieving other ones needed.
   */
  template <int dim, int codim, int range, int rank>
  static void parse_phys_basis(const std::shared_ptr<XMLElement> xml_elem,
                               const bool parse_as_constant,
                               IdMap_ &id_map,
                               const std::shared_ptr<ObjectsContainer> container);

  /**
   * @brief Reads an single @ref IgFunction XML element contained in
   * @p xml_elem and inserts it into the objects @p container.
   *
   * @tparam dim Dimension of the function.
   * @tparam codim Codimension of the function.
   * @tparam range Range of the function.
   * @tparam rank Rank of the function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   */
  template <int dim, int codim, int range, int rank>
  static FuncPtr_<dim, codim, range, rank>
  parse_ig_function(const std::shared_ptr<XMLElement> xml_elem,
                    const Index &local_object_id,
                    const bool parse_as_constant,
                    const std::shared_ptr<ObjectsContainer> container,
                    IdMap_ &id_map);

  /**
   * @brief Reads an single @ref functions::ConstantFunction XML element
   * and inserts it into the objects container.
   *
   * @tparam dim Dimension of the function.
   * @tparam codim Codimension of the function.
   * @tparam range Range of the function.
   * @tparam rank Rank of the function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   */
  template <int dim, int codim, int range, int rank>
  static FuncPtr_<dim, codim, range, rank>
  parse_constant_function(const std::shared_ptr<XMLElement> xml_elem,
                          const Index &local_object_id,
                          const bool parse_as_constant,
                          const std::shared_ptr<ObjectsContainer> container,
                          IdMap_ &id_map);

  /**
   * @brief Reads an single @ref functions::LinearFunction XML element
   * and inserts it into the objects container.
   *
   * @tparam dim Dimension of the function.
   * @tparam codim Codimension of the function.
   * @tparam range Range of the function.
   * @tparam rank Rank of the function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   */
  template <int dim, int codim, int range, int rank>
  static FuncPtr_<dim, codim, range, rank>
  parse_linear_function(const std::shared_ptr<XMLElement> xml_elem,
                        const Index &local_object_id,
                        const bool parse_as_constant,
                        const std::shared_ptr<ObjectsContainer> container,
                        IdMap_ &id_map,
                        typename std::enable_if_t<rank == 1> * = 0);

  /**
   * @brief Reads an single @ref functions::LinearFunction XML element
   * and inserts it into the objects container.
   *
   * @tparam dim Dimension of the function.
   * @tparam codim Codimension of the function.
   * @tparam range Range of the function.
   * @tparam rank Rank of the function.
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] local_object_id Local object id number.
   * @param[in] parse_as_constant Flag indicating if the objects must be
   *            parsed as constant, or not.
   * @param[in] container Container for retrieving previously defined
   *            objects.
   * @param[in,out] id_map Map between the local Ids of the input file and
   *                the unique object Id.
   * @return Shared pointer wrapping the parsed object.
   *
   * @warning This version of the method corresponds to the case
   * in which <tt> rank != 1 </tt>, that is not valid for
   * @ref LinearFunction. An exception will be thrown.
   */
  template <int dim, int codim, int range, int rank>
  static FuncPtr_<dim, codim, range, rank>
  parse_linear_function(const std::shared_ptr<XMLElement> xml_elem,
                        const Index &local_object_id,
                        const bool parse_as_constant,
                        const std::shared_ptr<ObjectsContainer> container,
                        IdMap_ &id_map,
                        typename std::enable_if_t<rank != 1> * = 0);

  ///@}

  /**
   * @brief Reads a <tt>Name</tt> XML element contained in @p xml_elem.
   *
   * @param[in] xml_elem XML element to be parsed.
   * return Name string parsed.
   */
  static std::string parse_name(const std::shared_ptr<XMLElement> xml_elem);

  /**
   * @brief Reads a <tt>DofsProperty</tt> XML element contained in
   * @p xml_elem.
   *
   * @param[in] xml_elem XML element to be parsed.
   * return Dofs property string parsed.
   */
  static std::string parse_dofs_property(const std::shared_ptr<XMLElement> xml_elem);

  /**
   * @brief Produces an string of the given type and dimensions and the
   * object id associated to it.
   *
   * It will produce a string like
   * <tt>Type<X, Y, Z> (IgaObjectId (W))</tt>.
   * This is used for error messaging purposes.
   *
   * @param[in] object_type String with the object type.
   * @param[in] object_id Id associated to the object.
   * @param[in] dims Vector with the dimensions of the object.
   * @return String containing the object type and its id
   */
  static std::string get_type_id_string(const std::string &object_type,
                                        const Index &object_id,
                                        const SafeSTLVector<int> &dims);

  /**
   * @brief Reads a single @ref IgCoefficients vector from the given
   * @p xml_elem.
   * The indices of the parsed coefficients are checked with
   * @p space_global_dofs, that are the global indices of the dofs
   * of the space associated to the coefficients.
   *
   * @param[in] xml_elem XML element to be parsed.
   * @param[in] parsing_msg String containing information about the
   *             XML element document to produce richer error messages.
   * @param[in] space_global_dofs Global indices of the dofs of
   *            space to which the coefficients are associated to.
   * @return IgCoefficients shared pointer vector.
   */
  static std::shared_ptr<IgCoefficients>
  parse_ig_coefficients(const std::shared_ptr<XMLElement> xml_elem,
                        const std::string &parsing_msg,
                        const std::set<Index> &space_global_dofs);

};

IGA_NAMESPACE_CLOSE

#endif // IGATOOLS_WITH_XML_IO

#endif // __OBJECTS_CONTAINER_READER_H_

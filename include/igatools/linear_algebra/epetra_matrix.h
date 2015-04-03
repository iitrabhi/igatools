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

#ifndef __EPETRA_MATRIX_H_
#define __EPETRA_MATRIX_H_

#include <igatools/base/config.h>
#include <igatools/linear_algebra/epetra_graph.h>
#include <igatools/linear_algebra/dense_matrix.h>
#include <Epetra_CrsMatrix.h>

IGA_NAMESPACE_OPEN

namespace EpetraTools
{
/**
 * Distributed matrix
 */
class  Matrix : public Epetra_CrsMatrix
{
public:
	using Epetra_CrsMatrix::Epetra_CrsMatrix;

	void add_block(const vector<Index> &rows_id,
			const vector<Index> &cols_id,
			const DenseMatrix &loc_matrix);

	void print_info(LogStream &out) const;
};

using MatrixPtr = std::shared_ptr<Matrix>;

/**
 * Creates a pointer to the matrix
 */
MatrixPtr
create_matrix(GraphPtr graph);

};

IGA_NAMESPACE_CLOSE

#endif
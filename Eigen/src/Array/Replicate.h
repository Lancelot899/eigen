// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2009-2010 Gael Guennebaud <g.gael@free.fr>
//
// Eigen is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Eigen is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Eigen. If not, see <http://www.gnu.org/licenses/>.

#ifndef EIGEN_REPLICATE_H
#define EIGEN_REPLICATE_H

/** \nonstableyet
  * \class Replicate
  *
  * \brief Expression of the multiple replication of a matrix or vector
  *
  * \param MatrixType the type of the object we are replicating
  *
  * This class represents an expression of the multiple replication of a matrix or vector.
  * It is the return type of DenseBase::replicate() and most of the time
  * this is the only way it is used.
  *
  * \sa DenseBase::replicate()
  */
template<typename MatrixType,int RowFactor,int ColFactor>
struct ei_traits<Replicate<MatrixType,RowFactor,ColFactor> >
 : ei_traits<MatrixType>
{
  typedef typename MatrixType::Scalar Scalar;
  typedef typename ei_traits<MatrixType>::StorageType StorageType;
  typedef typename ei_nested<MatrixType>::type MatrixTypeNested;
  typedef typename ei_unref<MatrixTypeNested>::type _MatrixTypeNested;
  enum {
    RowsAtCompileTime = RowFactor==Dynamic || int(MatrixType::RowsAtCompileTime)==Dynamic
                      ? Dynamic
                      : RowFactor * MatrixType::RowsAtCompileTime,
    ColsAtCompileTime = ColFactor==Dynamic || int(MatrixType::ColsAtCompileTime)==Dynamic
                      ? Dynamic
                      : ColFactor * MatrixType::ColsAtCompileTime,
    MaxRowsAtCompileTime = RowsAtCompileTime,
    MaxColsAtCompileTime = ColsAtCompileTime,
    Flags = (_MatrixTypeNested::Flags & HereditaryBits),
    CoeffReadCost = _MatrixTypeNested::CoeffReadCost
  };
};

template<typename MatrixType,int RowFactor,int ColFactor> class Replicate
  : public MatrixType::template MakeBase< Replicate<MatrixType,RowFactor,ColFactor> >::Type
{
  public:

    typedef typename MatrixType::template MakeBase< Replicate<MatrixType,RowFactor,ColFactor> >::Type Base;
    EIGEN_DENSE_PUBLIC_INTERFACE(Replicate)

    template<typename OriginalMatrixType>
    inline explicit Replicate(const OriginalMatrixType& matrix)
      : m_matrix(matrix), m_rowFactor(RowFactor), m_colFactor(ColFactor)
    {
      EIGEN_STATIC_ASSERT((ei_is_same_type<MatrixType,OriginalMatrixType>::ret),
                          THE_MATRIX_OR_EXPRESSION_THAT_YOU_PASSED_DOES_NOT_HAVE_THE_EXPECTED_TYPE)
      ei_assert(RowFactor!=Dynamic && ColFactor!=Dynamic);
    }
 
    template<typename OriginalMatrixType>
    inline Replicate(const OriginalMatrixType& matrix, int rowFactor, int colFactor)
      : m_matrix(matrix), m_rowFactor(rowFactor), m_colFactor(colFactor)
    {
      EIGEN_STATIC_ASSERT((ei_is_same_type<MatrixType,OriginalMatrixType>::ret),
                          THE_MATRIX_OR_EXPRESSION_THAT_YOU_PASSED_DOES_NOT_HAVE_THE_EXPECTED_TYPE)
    }

    inline int rows() const { return m_matrix.rows() * m_rowFactor.value(); }
    inline int cols() const { return m_matrix.cols() * m_colFactor.value(); }

    inline Scalar coeff(int row, int col) const
    {
      // try to avoid using modulo; this is a pure optimization strategy
      // - it is assumed unlikely that RowFactor==1 && ColFactor==1
      if (RowFactor==1)
        return m_matrix.coeff(m_matrix.rows(), col%m_matrix.cols());
      else if (ColFactor==1)
        return m_matrix.coeff(row%m_matrix.rows(), m_matrix.cols());
      else
        return m_matrix.coeff(row%m_matrix.rows(), col%m_matrix.cols());
    }

  protected:
    const typename MatrixType::Nested m_matrix;
    const ei_int_if_dynamic<RowFactor> m_rowFactor;
    const ei_int_if_dynamic<ColFactor> m_colFactor;
};

/** \nonstableyet
  * \return an expression of the replication of \c *this
  *
  * Example: \include MatrixBase_replicate.cpp
  * Output: \verbinclude MatrixBase_replicate.out
  *
  * \sa VectorwiseOp::replicate(), DenseBase::replicate(int,int), class Replicate
  */
template<typename Derived>
template<int RowFactor, int ColFactor>
inline const Replicate<Derived,RowFactor,ColFactor>
DenseBase<Derived>::replicate() const
{
  return Replicate<Derived,RowFactor,ColFactor>(derived());
}

/** \nonstableyet
  * \return an expression of the replication of \c *this
  *
  * Example: \include MatrixBase_replicate_int_int.cpp
  * Output: \verbinclude MatrixBase_replicate_int_int.out
  *
  * \sa VectorwiseOp::replicate(), DenseBase::replicate<int,int>(), class Replicate
  */
template<typename Derived>
inline const Replicate<Derived,Dynamic,Dynamic>
DenseBase<Derived>::replicate(int rowFactor,int colFactor) const
{
  return Replicate<Derived,Dynamic,Dynamic>(derived(),rowFactor,colFactor);
}

/** \nonstableyet
  * \return an expression of the replication of each column (or row) of \c *this
  *
  * Example: \include DirectionWise_replicate_int.cpp
  * Output: \verbinclude DirectionWise_replicate_int.out
  *
  * \sa VectorwiseOp::replicate(), DenseBase::replicate(), class Replicate
  */
template<typename ExpressionType, int Direction>
const Replicate<ExpressionType,(Direction==Vertical?Dynamic:1),(Direction==Horizontal?Dynamic:1)>
VectorwiseOp<ExpressionType,Direction>::replicate(int factor) const
{
  return Replicate<ExpressionType,Direction==Vertical?Dynamic:1,Direction==Horizontal?Dynamic:1>
          (_expression(),Direction==Vertical?factor:1,Direction==Horizontal?factor:1);
}

#endif // EIGEN_REPLICATE_H

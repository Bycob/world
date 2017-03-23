// Copyright (C) 2010-2015 National ICT Australia (NICTA)
// 
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
// -------------------------------------------------------------------
// 
// Written by Conrad Sanderson - http://conradsanderson.id.au


//! \addtogroup fn_join
//! @{



template<typename T1, typename T2>
inline
typename
enable_if2
  <
  (is_arma_type<T1>::value && is_arma_type<T2>::value && is_same_type<typename T1::elem_type, typename T2::elem_type>::value),
  const Glue<T1, T2, glue_join_cols>
  >::result
join_cols(const T1& A, const T2& B)
  {
  arma_extra_debug_sigprint();
  
  return Glue<T1, T2, glue_join_cols>(A, B);
  }



template<typename T1, typename T2>
inline
typename
enable_if2
  <
  (is_arma_type<T1>::value && is_arma_type<T2>::value && is_same_type<typename T1::elem_type, typename T2::elem_type>::value),
  const Glue<T1, T2, glue_join_cols>
  >::result
join_vert(const T1& A, const T2& B)
  {
  arma_extra_debug_sigprint();
  
  return Glue<T1, T2, glue_join_cols>(A, B);
  }



template<typename T1, typename T2>
inline
typename
enable_if2
  <
  (is_arma_type<T1>::value && is_arma_type<T2>::value && is_same_type<typename T1::elem_type, typename T2::elem_type>::value),
  const Glue<T1, T2, glue_join_rows>
  >::result
join_rows(const T1& A, const T2& B)
  {
  arma_extra_debug_sigprint();
  
  return Glue<T1, T2, glue_join_rows>(A, B);
  }



template<typename T1, typename T2>
inline
typename
enable_if2
  <
  (is_arma_type<T1>::value && is_arma_type<T2>::value && is_same_type<typename T1::elem_type, typename T2::elem_type>::value),
  const Glue<T1, T2, glue_join_rows>
  >::result
join_horiz(const T1& A, const T2& B)
  {
  arma_extra_debug_sigprint();
  
  return Glue<T1, T2, glue_join_rows>(A, B);
  }



//
// for cubes

template<typename T1, typename T2>
inline
const GlueCube<T1, T2, glue_join_slices>
join_slices(const BaseCube<typename T1::elem_type,T1>& A, const BaseCube<typename T1::elem_type,T2>& B)
  {
  arma_extra_debug_sigprint();
  
  return GlueCube<T1, T2, glue_join_slices>(A.get_ref(), B.get_ref());
  }



template<typename T1, typename T2>
inline
Cube<typename T1::elem_type>
join_slices(const Base<typename T1::elem_type,T1>& A, const Base<typename T1::elem_type,T2>& B)
  {
  arma_extra_debug_sigprint();
  
  typedef typename T1::elem_type eT;
  
  const unwrap<T1> UA(A.get_ref());
  const unwrap<T2> UB(B.get_ref());
  
  arma_debug_assert_same_size(UA.M.n_rows, UA.M.n_cols, UB.M.n_rows, UB.M.n_cols, "join_slices(): incompatible dimensions");
  
  Cube<eT> out(UA.M.n_rows, UA.M.n_cols, 2);
  
  arrayops::copy(out.slice_memptr(0), UA.M.memptr(), UA.M.n_elem);
  arrayops::copy(out.slice_memptr(1), UB.M.memptr(), UB.M.n_elem);
  
  return out;
  }



template<typename T1, typename T2>
inline
Cube<typename T1::elem_type>
join_slices(const Base<typename T1::elem_type,T1>& A, const BaseCube<typename T1::elem_type,T2>& B)
  {
  arma_extra_debug_sigprint();
  
  typedef typename T1::elem_type eT;
  
  const unwrap<T1> U(A.get_ref());
  
  const Cube<eT> M(const_cast<eT*>(U.M.memptr()), U.M.n_rows, U.M.n_cols, 1, false);
  
  return join_slices(M,B);
  }



template<typename T1, typename T2>
inline
Cube<typename T1::elem_type>
join_slices(const BaseCube<typename T1::elem_type,T1>& A, const Base<typename T1::elem_type,T2>& B)
  {
  arma_extra_debug_sigprint();
  
  typedef typename T1::elem_type eT;
  
  const unwrap<T2> U(B.get_ref());
  
  const Cube<eT> M(const_cast<eT*>(U.M.memptr()), U.M.n_rows, U.M.n_cols, 1, false);
  
  return join_slices(A,M);
  }



//
// for sparse matrices

template<typename T1, typename T2>
inline
const SpGlue<T1, T2, spglue_join_cols>
join_cols(const SpBase<typename T1::elem_type,T1>& A, const SpBase<typename T1::elem_type,T2>& B)
  {
  arma_extra_debug_sigprint();
  
  return SpGlue<T1, T2, spglue_join_cols>(A.get_ref(), B.get_ref());
  }



template<typename T1, typename T2>
inline
const SpGlue<T1, T2, spglue_join_cols>
join_vert(const SpBase<typename T1::elem_type,T1>& A, const SpBase<typename T1::elem_type,T2>& B)
  {
  arma_extra_debug_sigprint();
  
  return SpGlue<T1, T2, spglue_join_cols>(A.get_ref(), B.get_ref());
  }



template<typename T1, typename T2>
inline
const SpGlue<T1, T2, spglue_join_rows>
join_rows(const SpBase<typename T1::elem_type,T1>& A, const SpBase<typename T1::elem_type,T2>& B)
  {
  arma_extra_debug_sigprint();
  
  return SpGlue<T1, T2, spglue_join_rows>(A.get_ref(), B.get_ref());
  }



template<typename T1, typename T2>
inline
const SpGlue<T1, T2, spglue_join_rows>
join_horiz(const SpBase<typename T1::elem_type,T1>& A, const SpBase<typename T1::elem_type,T2>& B)
  {
  arma_extra_debug_sigprint();
  
  return SpGlue<T1, T2, spglue_join_rows>(A.get_ref(), B.get_ref());
  }



//! @}

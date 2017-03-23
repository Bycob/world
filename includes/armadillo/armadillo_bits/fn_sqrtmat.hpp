// Copyright (C) 2016 National ICT Australia (NICTA)
// 
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
// -------------------------------------------------------------------
// 
// Written by Conrad Sanderson - http://conradsanderson.id.au


//! \addtogroup fn_sqrtmat
//! @{



template<typename T1>
arma_inline
typename enable_if2< (is_supported_blas_type<typename T1::elem_type>::value && is_cx<typename T1::elem_type>::no), const mtOp<std::complex<typename T1::elem_type>, T1, op_sqrtmat> >::result
sqrtmat(const Base<typename T1::elem_type,T1>& X)
  {
  arma_extra_debug_sigprint();
  
  return mtOp<std::complex<typename T1::elem_type>, T1, op_sqrtmat>(X.get_ref());
  }



template<typename T1>
arma_inline
typename enable_if2< (is_supported_blas_type<typename T1::elem_type>::value && is_cx<typename T1::elem_type>::yes), const Op<T1, op_sqrtmat_cx> >::result
sqrtmat(const Base<typename T1::elem_type,T1>& X)
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_sqrtmat_cx>(X.get_ref());
  }



template<typename T1>
inline
typename enable_if2< (is_supported_blas_type<typename T1::elem_type>::value && is_cx<typename T1::elem_type>::no), bool >::result
sqrtmat(Mat< std::complex<typename T1::elem_type> >& Y, const Base<typename T1::elem_type,T1>& X)
  {
  arma_extra_debug_sigprint();
  
  return op_sqrtmat::apply_direct(Y, X.get_ref());
  }



template<typename T1>
inline
typename enable_if2< (is_supported_blas_type<typename T1::elem_type>::value && is_cx<typename T1::elem_type>::yes), bool >::result
sqrtmat(Mat<typename T1::elem_type>& Y, const Base<typename T1::elem_type,T1>& X)
  {
  arma_extra_debug_sigprint();
  
  return op_sqrtmat_cx::apply_direct(Y, X.get_ref());
  }



//! @}

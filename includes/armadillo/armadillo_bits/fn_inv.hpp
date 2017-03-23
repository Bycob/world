// Copyright (C) 2008-2016 National ICT Australia (NICTA)
// 
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
// -------------------------------------------------------------------
// 
// Written by Conrad Sanderson - http://conradsanderson.id.au


//! \addtogroup fn_inv
//! @{



template<typename T1>
arma_inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, const Op<T1, op_inv> >::result
inv
  (
  const Base<typename T1::elem_type,T1>& X
  )
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_inv>(X.get_ref());
  }



template<typename T1>
arma_inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, const Op<T1, op_inv> >::result
inv
  (
  const Base<typename T1::elem_type,T1>& X,
  const bool   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_inv>(X.get_ref());
  }



template<typename T1>
arma_inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, const Op<T1, op_inv> >::result
inv
  (
  const Base<typename T1::elem_type,T1>& X,
  const char*   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_inv>(X.get_ref());
  }



template<typename T1>
arma_inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, const Op<T1, op_inv_tr> >::result
inv
  (
  const Op<T1, op_trimat>& X
  )
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_inv_tr>(X.m, X.aux_uword_a, 0);
  }



template<typename T1>
arma_inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, const Op<T1, op_inv_tr> >::result
inv
  (
  const Op<T1, op_trimat>& X,
  const bool   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_inv_tr>(X.m, X.aux_uword_a, 0);
  }



template<typename T1>
arma_inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, const Op<T1, op_inv_tr> >::result
inv
  (
  const Op<T1, op_trimat>& X,
  const char*   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_inv_tr>(X.m, X.aux_uword_a, 0);
  }



template<typename T1>
inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, bool >::result
inv
  (
         Mat<typename T1::elem_type>&    out,
  const Base<typename T1::elem_type,T1>& X
  )
  {
  arma_extra_debug_sigprint();
  
  try
    {
    out = inv(X);
    }
  catch(std::runtime_error&)
    {
    return false;
    }
  
  return true;
  }



template<typename T1>
inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, bool >::result
inv
  (
         Mat<typename T1::elem_type>&    out,
  const Base<typename T1::elem_type,T1>& X,
  const bool   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return inv(out,X);
  }



template<typename T1>
inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, bool >::result
inv
  (
         Mat<typename T1::elem_type>&    out,
  const Base<typename T1::elem_type,T1>& X,
  const char*   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return inv(out,X);
  }



template<typename T1>
arma_inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, const Op<T1, op_inv_sympd> >::result
inv_sympd
  (
  const Base<typename T1::elem_type, T1>& X
  )
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_inv_sympd>(X.get_ref());
  }



template<typename T1>
arma_inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, const Op<T1, op_inv_sympd> >::result
inv_sympd
  (
  const Base<typename T1::elem_type, T1>& X,
  const bool   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_inv_sympd>(X.get_ref());
  }



template<typename T1>
arma_inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, const Op<T1, op_inv_sympd> >::result
inv_sympd
  (
  const Base<typename T1::elem_type, T1>& X,
  const char*   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return Op<T1, op_inv_sympd>(X.get_ref());
  }



template<typename T1>
inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, bool >::result
inv_sympd
  (
         Mat<typename T1::elem_type>&    out,
  const Base<typename T1::elem_type,T1>& X
  )
  {
  arma_extra_debug_sigprint();
  
  try
    {
    out = inv_sympd(X);
    }
  catch(std::runtime_error&)
    {
    return false;
    }
  
  return true;
  }



template<typename T1>
inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, bool >::result
inv_sympd
  (
         Mat<typename T1::elem_type>&    out,
  const Base<typename T1::elem_type,T1>& X,
  const bool   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return inv_sympd(out,X);
  }



template<typename T1>
inline
typename enable_if2< is_supported_blas_type<typename T1::elem_type>::value, bool >::result
inv_sympd
  (
         Mat<typename T1::elem_type>&    out,
  const Base<typename T1::elem_type,T1>& X,
  const char*   // argument kept only for compatibility with old user code
  )
  {
  arma_extra_debug_sigprint();
  
  return inv_sympd(out,X);
  }



//! @}

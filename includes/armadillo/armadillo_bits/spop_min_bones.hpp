// Copyright (C) 2012-2015 National ICT Australia (NICTA)
// 
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
// -------------------------------------------------------------------
// 
// Written by Conrad Sanderson - http://conradsanderson.id.au
// Written by Ryan Curtin


//! \addtogroup spop_min
//! @{


class spop_min
  {
  public:
  
  template<typename T1>
  inline static void apply(SpMat<typename T1::elem_type>& out, const SpOp<T1, spop_min>& in);
  
  //
  
  template<typename T1>
  inline static void apply_proxy(SpMat<typename T1::elem_type>& out, const SpProxy<T1>& p, const uword dim, const typename arma_not_cx<typename T1::elem_type>::result* junk = 0);
  
  template<typename T1>
  inline static typename T1::elem_type vector_min(const T1& X, const typename arma_not_cx<typename T1::elem_type>::result* junk = 0);
  
  template<typename T1>
  inline static typename arma_not_cx<typename T1::elem_type>::result min(const SpBase<typename T1::elem_type, T1>& X);
  
  template<typename T1>
  inline static typename arma_not_cx<typename T1::elem_type>::result min_with_index(const SpProxy<T1>& P, uword& index_of_min_val);
  
  //
  
  template<typename T1>
  inline static void apply_proxy(SpMat<typename T1::elem_type>& out, const SpProxy<T1>& p, const uword dim, const typename arma_cx_only<typename T1::elem_type>::result* junk = 0);
  
  template<typename T1>
  inline static typename T1::elem_type vector_min(const T1& X, const typename arma_cx_only<typename T1::elem_type>::result* junk = 0);
  
  template<typename T1>
  inline static typename arma_cx_only<typename T1::elem_type>::result min(const SpBase<typename T1::elem_type, T1>& X);
  
  template<typename T1>
  inline static typename arma_cx_only<typename T1::elem_type>::result min_with_index(const SpProxy<T1>& P, uword& index_of_min_val);
  };


//! @}

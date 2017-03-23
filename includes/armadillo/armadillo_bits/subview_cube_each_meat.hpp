// Copyright (C) 2015 National ICT Australia (NICTA)
// 
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
// -------------------------------------------------------------------
// 
// Written by Conrad Sanderson - http://conradsanderson.id.au


//! \addtogroup subview_cube_each
//! @{


//
//
// subview_cube_each_common

template<typename eT>
inline
subview_cube_each_common<eT>::subview_cube_each_common(const Cube<eT>& in_p)
  : P(in_p)
  {
  arma_extra_debug_sigprint();
  }



template<typename eT>
inline
void
subview_cube_each_common<eT>::check_size(const Mat<eT>& A) const
  {
  if(arma_config::debug == true)
    {
    if( (A.n_rows != P.n_rows) || (A.n_cols != P.n_cols) )
      {
      arma_stop( incompat_size_string(A) );
      }
    }
  }



template<typename eT>
arma_cold
inline
const std::string
subview_cube_each_common<eT>::incompat_size_string(const Mat<eT>& A) const
  {
  std::stringstream tmp;
  
  tmp << "each_slice(): incompatible size; expected " << P.n_rows << 'x' << P.n_cols << ", got " << A.n_rows << 'x' << A.n_cols;
  
  return tmp.str();
  }
  


//
//
// subview_cube_each1



template<typename eT>
inline
subview_cube_each1<eT>::~subview_cube_each1()
  {
  arma_extra_debug_sigprint();
  }



template<typename eT>
inline
subview_cube_each1<eT>::subview_cube_each1(const Cube<eT>& in_p)
  : subview_cube_each_common<eT>::subview_cube_each_common(in_p)
  {
  arma_extra_debug_sigprint();
  }



template<typename eT>
template<typename T1>
inline
void
subview_cube_each1<eT>::operator= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < p_n_slices; ++i)  { arrayops::copy( p.slice_memptr(i), A_mem, p_n_elem_slice ); }
  }



template<typename eT>
template<typename T1>
inline
void
subview_cube_each1<eT>::operator+= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
  
  const eT* A_mem = A.memptr();
    
  for(uword i=0; i < p_n_slices; ++i)  { arrayops::inplace_plus( p.slice_memptr(i), A_mem, p_n_elem_slice ); }
  }



template<typename eT>
template<typename T1>
inline
void
subview_cube_each1<eT>::operator-= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < p_n_slices; ++i)  { arrayops::inplace_minus( p.slice_memptr(i), A_mem, p_n_elem_slice ); }
  }



template<typename eT>
template<typename T1>
inline
void
subview_cube_each1<eT>::operator%= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < p_n_slices; ++i)  { arrayops::inplace_mul( p.slice_memptr(i), A_mem, p_n_elem_slice ); }
  }



template<typename eT>
template<typename T1>
inline
void
subview_cube_each1<eT>::operator/= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < p_n_slices; ++i)  { arrayops::inplace_div( p.slice_memptr(i), A_mem, p_n_elem_slice ); }
  }



//
//
// subview_cube_each2



template<typename eT, typename TB>
inline
subview_cube_each2<eT,TB>::~subview_cube_each2()
  {
  arma_extra_debug_sigprint();
  }



template<typename eT, typename TB>
inline
subview_cube_each2<eT,TB>::subview_cube_each2(const Cube<eT>& in_p, const Base<uword, TB>& in_indices)
  : subview_cube_each_common<eT>::subview_cube_each_common(in_p)
  , base_indices(in_indices)
  {
  arma_extra_debug_sigprint();
  }



template<typename eT, typename TB>
inline
void
subview_cube_each2<eT,TB>::check_indices(const Mat<uword>& indices) const
  {
  arma_debug_check( ((indices.is_vec() == false) && (indices.is_empty() == false)), "each_slice(): list of indices must be a vector" );
  }



template<typename eT, typename TB>
template<typename T1>
inline
void
subview_cube_each2<eT,TB>::operator= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const unwrap<TB> U( base_indices.get_ref() );
  
  check_indices(U.M);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
    
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
    arrayops::copy(p.slice_memptr(slice), A_mem, p_n_elem_slice);
    }
  }



template<typename eT, typename TB>
template<typename T1>
inline
void
subview_cube_each2<eT,TB>::operator+= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const unwrap<TB> U( base_indices.get_ref() );
  
  check_indices(U.M);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
    
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
    arrayops::inplace_plus(p.slice_memptr(slice), A_mem, p_n_elem_slice);
    }
  }



template<typename eT, typename TB>
template<typename T1>
inline
void
subview_cube_each2<eT,TB>::operator-= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const unwrap<TB> U( base_indices.get_ref() );
  
  check_indices(U.M);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
    
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
    arrayops::inplace_minus(p.slice_memptr(slice), A_mem, p_n_elem_slice);
    }
  }



template<typename eT, typename TB>
template<typename T1>
inline
void
subview_cube_each2<eT,TB>::operator%= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const unwrap<TB> U( base_indices.get_ref() );
  
  check_indices(U.M);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
    
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
    arrayops::inplace_mul(p.slice_memptr(slice), A_mem, p_n_elem_slice);
    }
  }



template<typename eT, typename TB>
template<typename T1>
inline
void
subview_cube_each2<eT,TB>::operator/= (const Base<eT,T1>& in)
  {
  arma_extra_debug_sigprint();
  
  Cube<eT>& p = access::rw(subview_cube_each_common<eT>::P);
  
  const unwrap<T1>   tmp( in.get_ref() );
  const Mat<eT>& A = tmp.M;
  
  subview_cube_each_common<eT>::check_size(A);
  
  const unwrap<TB> U( base_indices.get_ref() );
  
  check_indices(U.M);
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
    
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
    arrayops::inplace_div(p.slice_memptr(slice), A_mem, p_n_elem_slice);
    }
  }



//
//
// subview_cube_each1_aux



template<typename eT, typename T2>
inline
Cube<eT>
subview_cube_each1_aux::operator_plus
  (
  const subview_cube_each1<eT>& X,
  const Base<eT,T2>&            Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = X.P;
  
  const uword p_n_rows   = p.n_rows;
  const uword p_n_cols   = p.n_cols;
  const uword p_n_slices = p.n_slices;
  
  Cube<eT> out(p_n_rows, p_n_cols, p_n_slices);
  
  const unwrap<T2>   tmp(Y.get_ref());
  const Mat<eT>& A = tmp.M;
  
  X.check_size(A);
  
  for(uword i=0; i < p_n_slices; ++i)
    {
          Mat<eT> out_slice(              out.slice_memptr(i),  p_n_rows, p_n_cols, false, true);
    const Mat<eT>   p_slice(const_cast<eT*>(p.slice_memptr(i)), p_n_rows, p_n_cols, false, true);
    
    out_slice = p_slice + A;
    }
  
  return out;
  }



template<typename eT, typename T2>
inline
Cube<eT>
subview_cube_each1_aux::operator_minus
  (
  const subview_cube_each1<eT>& X,
  const Base<eT,T2>&            Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = X.P;
  
  const uword p_n_rows   = p.n_rows;
  const uword p_n_cols   = p.n_cols;
  const uword p_n_slices = p.n_slices;
  
  Cube<eT> out(p_n_rows, p_n_cols, p_n_slices);
  
  const unwrap<T2>   tmp(Y.get_ref());
  const Mat<eT>& A = tmp.M;
  
  X.check_size(A);
  
  for(uword i=0; i < p_n_slices; ++i)
    {
          Mat<eT> out_slice(              out.slice_memptr(i),  p_n_rows, p_n_cols, false, true);
    const Mat<eT>   p_slice(const_cast<eT*>(p.slice_memptr(i)), p_n_rows, p_n_cols, false, true);
    
    out_slice = p_slice - A;
    }
  
  return out;
  }



template<typename T1, typename eT>
inline
Cube<eT>
subview_cube_each1_aux::operator_minus
  (
  const Base<eT,T1>&            X,
  const subview_cube_each1<eT>& Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = Y.P;
  
  const uword p_n_rows   = p.n_rows;
  const uword p_n_cols   = p.n_cols;
  const uword p_n_slices = p.n_slices;
  
  Cube<eT> out(p_n_rows, p_n_cols, p_n_slices);
  
  const unwrap<T1>   tmp(X.get_ref());
  const Mat<eT>& A = tmp.M;
  
  Y.check_size(A);
  
  for(uword i=0; i < p_n_slices; ++i)
    {
          Mat<eT> out_slice(              out.slice_memptr(i),  p_n_rows, p_n_cols, false, true);
    const Mat<eT>   p_slice(const_cast<eT*>(p.slice_memptr(i)), p_n_rows, p_n_cols, false, true);
    
    out_slice = A - p_slice;
    }
  
  return out;
  }



template<typename eT, typename T2>
inline
Cube<eT>
subview_cube_each1_aux::operator_schur
  (
  const subview_cube_each1<eT>& X,
  const Base<eT,T2>&            Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = X.P;
  
  const uword p_n_rows   = p.n_rows;
  const uword p_n_cols   = p.n_cols;
  const uword p_n_slices = p.n_slices;
  
  Cube<eT> out(p_n_rows, p_n_cols, p_n_slices);
  
  const unwrap<T2>   tmp(Y.get_ref());
  const Mat<eT>& A = tmp.M;
  
  X.check_size(A);
  
  for(uword i=0; i < p_n_slices; ++i)
    {
          Mat<eT> out_slice(              out.slice_memptr(i),  p_n_rows, p_n_cols, false, true);
    const Mat<eT>   p_slice(const_cast<eT*>(p.slice_memptr(i)), p_n_rows, p_n_cols, false, true);
    
    out_slice = p_slice % A;
    }
  
  return out;
  }



template<typename eT, typename T2>
inline
Cube<eT>
subview_cube_each1_aux::operator_div
  (
  const subview_cube_each1<eT>& X,
  const Base<eT,T2>&            Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = X.P;
  
  const uword p_n_rows   = p.n_rows;
  const uword p_n_cols   = p.n_cols;
  const uword p_n_slices = p.n_slices;
  
  Cube<eT> out(p_n_rows, p_n_cols, p_n_slices);
  
  const unwrap<T2>   tmp(Y.get_ref());
  const Mat<eT>& A = tmp.M;
  
  X.check_size(A);
  
  for(uword i=0; i < p_n_slices; ++i)
    {
          Mat<eT> out_slice(              out.slice_memptr(i),  p_n_rows, p_n_cols, false, true);
    const Mat<eT>   p_slice(const_cast<eT*>(p.slice_memptr(i)), p_n_rows, p_n_cols, false, true);
    
    out_slice = p_slice / A;
    }
  
  return out;
  }



template<typename T1, typename eT>
inline
Cube<eT>
subview_cube_each1_aux::operator_div
  (
  const Base<eT,T1>&            X,
  const subview_cube_each1<eT>& Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = Y.P;
  
  const uword p_n_rows   = p.n_rows;
  const uword p_n_cols   = p.n_cols;
  const uword p_n_slices = p.n_slices;
  
  Cube<eT> out(p_n_rows, p_n_cols, p_n_slices);
  
  const unwrap<T1>   tmp(X.get_ref());
  const Mat<eT>& A = tmp.M;
  
  Y.check_size(A);
  
  for(uword i=0; i < p_n_slices; ++i)
    {
          Mat<eT> out_slice(              out.slice_memptr(i),  p_n_rows, p_n_cols, false, true);
    const Mat<eT>   p_slice(const_cast<eT*>(p.slice_memptr(i)), p_n_rows, p_n_cols, false, true);
    
    out_slice = A / p_slice;
    }
  
  return out;
  }



//
//
// subview_cube_each2_aux



template<typename eT, typename TB, typename T2>
inline
Cube<eT>
subview_cube_each2_aux::operator_plus
  (
  const subview_cube_each2<eT,TB>& X,
  const Base<eT,T2>&               Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = X.P;
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
  
  Cube<eT> out = p;
  
  const unwrap<T2>   tmp(Y.get_ref());
  const Mat<eT>& A = tmp.M;
  
  const unwrap<TB> U(X.base_indices.get_ref());
  
  X.check_size(A);
  X.check_indices(U.M);
  
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
    arrayops::inplace_plus(out.slice_memptr(slice), A_mem, p_n_elem_slice);
    }
  
  return out;
  }



template<typename eT, typename TB, typename T2>
inline
Cube<eT>
subview_cube_each2_aux::operator_minus
  (
  const subview_cube_each2<eT,TB>& X,
  const Base<eT,T2>&               Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = X.P;
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
  
  Cube<eT> out = p;
  
  const unwrap<T2>   tmp(Y.get_ref());
  const Mat<eT>& A = tmp.M;
  
  const unwrap<TB> U(X.base_indices.get_ref());
  
  X.check_size(A);
  X.check_indices(U.M);
  
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
    arrayops::inplace_minus(out.slice_memptr(slice), A_mem, p_n_elem_slice);
    }
  
  return out;
  }



template<typename T1, typename eT, typename TB>
inline
Cube<eT>
subview_cube_each2_aux::operator_minus
  (
  const Base<eT,T1>&               X,
  const subview_cube_each2<eT,TB>& Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = Y.P;
  
  const uword p_n_rows   = p.n_rows;
  const uword p_n_cols   = p.n_cols;
  const uword p_n_slices = p.n_slices;
  
  Cube<eT> out = p;
  
  const unwrap<T1>   tmp(X.get_ref());
  const Mat<eT>& A = tmp.M;
  
  const unwrap<TB> U(Y.base_indices.get_ref());
  
  Y.check_size(A);
  Y.check_indices(U.M);
  
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
          Mat<eT> out_slice(              out.slice_memptr(slice),  p_n_rows, p_n_cols, false, true);
    const Mat<eT>   p_slice(const_cast<eT*>(p.slice_memptr(slice)), p_n_rows, p_n_cols, false, true);
    
    out_slice = A - p_slice;
    }
  
  return out;
  }



template<typename eT, typename TB, typename T2>
inline
Cube<eT>
subview_cube_each2_aux::operator_schur
  (
  const subview_cube_each2<eT,TB>& X,
  const Base<eT,T2>&               Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = X.P;
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
  
  Cube<eT> out = p;
  
  const unwrap<T2>   tmp(Y.get_ref());
  const Mat<eT>& A = tmp.M;
  
  const unwrap<TB> U(X.base_indices.get_ref());
  
  X.check_size(A);
  X.check_indices(U.M);
  
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
    arrayops::inplace_mul(out.slice_memptr(slice), A_mem, p_n_elem_slice);
    }
  
  return out;
  }



template<typename eT, typename TB, typename T2>
inline
Cube<eT>
subview_cube_each2_aux::operator_div
  (
  const subview_cube_each2<eT,TB>& X,
  const Base<eT,T2>&               Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = X.P;
  
  const uword p_n_slices     = p.n_slices;
  const uword p_n_elem_slice = p.n_elem_slice;
  
  Cube<eT> out = p;
  
  const unwrap<T2>   tmp(Y.get_ref());
  const Mat<eT>& A = tmp.M;
  
  const unwrap<TB> U(X.base_indices.get_ref());
  
  X.check_size(A);
  X.check_indices(U.M);
  
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  const eT* A_mem = A.memptr();
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
    arrayops::inplace_div(out.slice_memptr(slice), A_mem, p_n_elem_slice);
    }
  
  return out;
  }



template<typename T1, typename eT, typename TB>
inline
Cube<eT>
subview_cube_each2_aux::operator_div
  (
  const Base<eT,T1>&               X,
  const subview_cube_each2<eT,TB>& Y
  )
  {
  arma_extra_debug_sigprint();
  
  const Cube<eT>& p = Y.P;
  
  const uword p_n_rows   = p.n_rows;
  const uword p_n_cols   = p.n_cols;
  const uword p_n_slices = p.n_slices;
  
  Cube<eT> out = p;
  
  const unwrap<T1>   tmp(X.get_ref());
  const Mat<eT>& A = tmp.M;
  
  const unwrap<TB> U(Y.base_indices.get_ref());
  
  Y.check_size(A);
  Y.check_indices(U.M);
  
  const uword* indices_mem = U.M.memptr();
  const uword  N           = U.M.n_elem;
  
  for(uword i=0; i < N; ++i)
    {
    const uword slice = indices_mem[i];
    
    arma_debug_check( (slice >= p_n_slices), "each_slice(): index out of bounds" );
    
          Mat<eT> out_slice(              out.slice_memptr(slice),  p_n_rows, p_n_cols, false, true);
    const Mat<eT>   p_slice(const_cast<eT*>(p.slice_memptr(slice)), p_n_rows, p_n_cols, false, true);
    
    out_slice = A / p_slice;
    }
  
  return out;
  }



//! @}

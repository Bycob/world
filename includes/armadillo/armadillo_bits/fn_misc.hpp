// Copyright (C) 2008-2016 National ICT Australia (NICTA)
// 
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
// -------------------------------------------------------------------
// 
// Written by Conrad Sanderson - http://conradsanderson.id.au


//! \addtogroup fn_misc
//! @{



//! \brief
//! Generate a vector with 'num' elements.
//! The values of the elements linearly increase from 'start' upto (and including) 'end'.

template<typename out_type>
inline
typename
enable_if2
  <
  is_Mat<out_type>::value,
  out_type
  >::result
linspace
  (
  const typename out_type::pod_type start,
  const typename out_type::pod_type end,
  const uword                       num = 100u
  )
  {
  arma_extra_debug_sigprint();
  
  typedef typename out_type::elem_type eT;
  typedef typename out_type::pod_type   T;
  
  out_type x;
  
  if(num >= 2)
    {
    x.set_size(num);
    
    eT* x_mem = x.memptr();
    
    const uword num_m1 = num - 1;
    
    if(is_non_integral<T>::value == true)
      {
      const T delta = (end-start)/T(num_m1);
      
      for(uword i=0; i<num_m1; ++i)
        {
        x_mem[i] = eT(start + i*delta);
        }
      
      x_mem[num_m1] = eT(end);
      }
    else
      {
      const double delta = (end >= start) ? double(end-start)/double(num_m1) : -double(start-end)/double(num_m1);
      
      for(uword i=0; i<num_m1; ++i)
        {
        x_mem[i] = eT(double(start) + i*delta);
        }
      
      x_mem[num_m1] = eT(end);
      }
    }
  else
    {
    x.set_size(1);
    
    x[0] = eT(end);
    }
  
  return x;
  }



inline
vec
linspace(const double start, const double end, const uword num = 100u)
  {
  arma_extra_debug_sigprint();
  return linspace<vec>(start, end, num);
  }



template<typename out_type>
inline
typename
enable_if2
  <
  (is_Mat<out_type>::value && is_real<typename out_type::pod_type>::value),
  out_type
  >::result
logspace
  (
  const typename out_type::pod_type A,
  const typename out_type::pod_type B,
  const uword                       N = 50u
  )
  {
  arma_extra_debug_sigprint();
  
  typedef typename out_type::elem_type eT;
  typedef typename out_type::pod_type   T;
  
  out_type x = linspace<out_type>(A,B,N);
  
  const uword n_elem = x.n_elem;
  
  eT* x_mem = x.memptr();
  
  for(uword i=0; i < n_elem; ++i)
    {
    x_mem[i] = std::pow(T(10), x_mem[i]);
    }
  
  return x;
  }



inline
vec
logspace(const double A, const double B, const uword N = 50u)
  {
  arma_extra_debug_sigprint();
  return logspace<vec>(A, B, N);
  }



//
// log_exp_add

template<typename eT>
inline
typename arma_real_only<eT>::result
log_add_exp(eT log_a, eT log_b)
  {
  if(log_a < log_b)
    {
    std::swap(log_a, log_b);
    }
  
  const eT negdelta = log_b - log_a;
  
  if( (negdelta < Datum<eT>::log_min) || (arma_isfinite(negdelta) == false) )
    {
    return log_a;
    }
  else
    {
    return (log_a + arma_log1p(std::exp(negdelta)));
    }
  }



// for compatibility with earlier versions
template<typename eT>
inline
typename arma_real_only<eT>::result
log_add(eT log_a, eT log_b)
  {
  return log_add_exp(log_a, log_b);
  }
  


template<typename eT>
arma_inline
arma_warn_unused
bool
is_finite(const eT x, const typename arma_scalar_only<eT>::result* junk = 0)
  {
  arma_ignore(junk);
  
  return arma_isfinite(x);
  }



template<typename T1>
inline
arma_warn_unused
typename
enable_if2
  <
  is_arma_type<T1>::value,
  bool
  >::result
is_finite(const T1& X)
  {
  arma_extra_debug_sigprint();
  
  typedef typename T1::elem_type eT;
  
  const Proxy<T1> P(X);
  
  const bool have_direct_mem = (is_Mat<typename Proxy<T1>::stored_type>::value) || (is_subview_col<typename Proxy<T1>::stored_type>::value);
  
  if(have_direct_mem)
    {
    const quasi_unwrap<typename Proxy<T1>::stored_type> tmp(P.Q);
    
    return tmp.M.is_finite();
    }
  
  
  if(Proxy<T1>::prefer_at_accessor == false)
    {
    const typename Proxy<T1>::ea_type Pea = P.get_ea();
    
    const uword n_elem = P.get_n_elem();
    
    uword i,j;
    
    for(i=0, j=1; j<n_elem; i+=2, j+=2)
      {
      const eT val_i = Pea[i];
      const eT val_j = Pea[j];
      
      if( (arma_isfinite(val_i) == false) || (arma_isfinite(val_j) == false) )  { return false; }
      }
    
    if(i < n_elem)
      {
      if(arma_isfinite(Pea[i]) == false)  { return false; }
      }
    }
  else
    {
    const uword n_rows = P.get_n_rows();
    const uword n_cols = P.get_n_cols();
    
    for(uword col=0; col<n_cols; ++col)
    for(uword row=0; row<n_rows; ++row)
      {
      if(arma_isfinite(P.at(row,col)) == false)  { return false; }
      }
    }
  
  return true;
  }



template<typename T1>
inline
arma_warn_unused
bool
is_finite(const SpBase<typename T1::elem_type,T1>& X)
  {
  arma_extra_debug_sigprint();
  
  const SpProxy<T1> P(X.get_ref());
  
  if(is_SpMat<typename SpProxy<T1>::stored_type>::value)
    {
    const unwrap_spmat<typename SpProxy<T1>::stored_type> tmp(P.Q);
    
    return tmp.M.is_finite();
    }
  else
    {
    typename SpProxy<T1>::const_iterator_type it     = P.begin();
    typename SpProxy<T1>::const_iterator_type it_end = P.end();
    
    while(it != it_end)
      {
      if(arma_isfinite(*it) == false)  { return false; }
      ++it;
      }
    }
  
  return true;
  }



template<typename T1>
inline
arma_warn_unused
bool
is_finite(const BaseCube<typename T1::elem_type,T1>& X)
  {
  arma_extra_debug_sigprint();
  
  typedef typename T1::elem_type eT;
  
  const unwrap_cube<T1> tmp(X.get_ref());
  const Cube<eT>& A =   tmp.M;
  
  return A.is_finite();
  }



//! DO NOT USE IN NEW CODE; change instances of inv(sympd(X)) to inv_sympd(X)
template<typename T1>
arma_deprecated
inline
const T1&
sympd(const Base<typename T1::elem_type,T1>& X)
  {
  arma_extra_debug_sigprint();
  
  arma_debug_warn("sympd() is deprecated and will be removed; change inv(sympd(X)) to inv_sympd(X)");
  
  return X.get_ref();
  }



template<typename eT>
inline
void
swap(Mat<eT>& A, Mat<eT>& B)
  {
  arma_extra_debug_sigprint();
  
  A.swap(B);
  }



template<typename eT>
inline
void
swap(Cube<eT>& A, Cube<eT>& B)
  {
  arma_extra_debug_sigprint();
  
  A.swap(B);
  }



template<typename T1>
arma_inline
const Op<T1, op_orth>
orth(const Base<typename T1::elem_type, T1>& X, const typename T1::pod_type tol = 0.0, const typename arma_blas_type_only<typename T1::elem_type>::result* junk = 0)
  {
  arma_extra_debug_sigprint();
  arma_ignore(junk);
  
  typedef typename T1::elem_type eT;
  
  return Op<T1, op_orth>(X.get_ref(), eT(tol));
  }



template<typename T1>
inline
bool
orth(Mat<typename T1::elem_type>& out, const Base<typename T1::elem_type, T1>& X, const typename T1::pod_type tol = 0.0, const typename arma_blas_type_only<typename T1::elem_type>::result* junk = 0)
  {
  arma_extra_debug_sigprint();
  arma_ignore(junk);
  
  try { out = orth(X,tol); } catch (std::runtime_error&) { return false; }
  
  return true;
  }



template<typename T1>
arma_inline
const Op<T1, op_null>
null(const Base<typename T1::elem_type, T1>& X, const typename T1::pod_type tol = 0.0, const typename arma_blas_type_only<typename T1::elem_type>::result* junk = 0)
  {
  arma_extra_debug_sigprint();
  arma_ignore(junk);
  
  typedef typename T1::elem_type eT;
  
  return Op<T1, op_null>(X.get_ref(), eT(tol));
  }



template<typename T1>
inline
bool
null(Mat<typename T1::elem_type>& out, const Base<typename T1::elem_type, T1>& X, const typename T1::pod_type tol = 0.0, const typename arma_blas_type_only<typename T1::elem_type>::result* junk = 0)
  {
  arma_extra_debug_sigprint();
  arma_ignore(junk);
  
  try { out = null(X,tol); } catch (std::runtime_error&) { return false; }
  
  return true;
  }



inline
uvec
ind2sub(const SizeMat& s, const uword i)
  {
  arma_extra_debug_sigprint();
  
  arma_debug_check( (i >= (s.n_rows * s.n_cols) ), "ind2sub(): index out of range" );
  
  uvec out(2);
  
  out[0] = i % s.n_rows;
  out[1] = i / s.n_rows;
  
  return out;
  }



inline
uvec
ind2sub(const SizeCube& s, const uword i)
  {
  arma_extra_debug_sigprint();
  
  arma_debug_check( (i >= (s.n_rows * s.n_cols * s.n_slices) ), "ind2sub(): index out of range" );
  
  const uword n_elem_slice = s.n_rows * s.n_cols;
  
  const uword slice  = i / n_elem_slice;
  const uword j      = i - (slice * n_elem_slice);
  const uword row    = j % s.n_rows;
  const uword col    = j / s.n_rows;
  
  uvec out(3);
  
  out[0] = row;
  out[1] = col;
  out[2] = slice;
  
  return out;
  }



arma_inline
uword
sub2ind(const SizeMat& s, const uword row, const uword col)
  {
  arma_extra_debug_sigprint();
  
  arma_debug_check( ((row >= s.n_rows) || (col >= s.n_cols)), "sub2ind(): subscript out of range" );
  
  return uword(row + col*s.n_rows);
  }



arma_inline
uword
sub2ind(const SizeCube& s, const uword row, const uword col, const uword slice)
  {
  arma_extra_debug_sigprint();
  
  arma_debug_check( ((row >= s.n_rows) || (col >= s.n_cols) || (slice >= s.n_slices)), "sub2ind(): subscript out of range" );
  
  return uword( (slice * s.n_rows * s.n_cols) + (col * s.n_rows) + row );
  }



//! @}

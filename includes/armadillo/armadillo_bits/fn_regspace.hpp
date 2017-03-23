// Copyright (C) 2016 National ICT Australia (NICTA)
// 
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
// -------------------------------------------------------------------
// 
// Written by Conrad Sanderson - http://conradsanderson.id.au


//! \addtogroup fn_regspace
//! @{



template<typename eT>
inline
void
internal_regspace_default_delta
  (
  Mat<eT>& x,
  const typename Mat<eT>::pod_type start,
  const typename Mat<eT>::pod_type end
  )
  {
  arma_extra_debug_sigprint();
  
  typedef typename Mat<eT>::pod_type T;
  
  const bool ascend = (start <= end);
  
  const uword N = uword(1) + uword((ascend) ? (end-start) : (start-end));
  
  x.set_size(N);
  
  eT* x_mem = x.memptr();
  
  if(ascend)
    {
    for(uword i=0; i < N; ++i)  { x_mem[i] = eT(start + T(i)); }
    }
  else
    {
    for(uword i=0; i < N; ++i)  { x_mem[i] = eT(start - T(i)); }
    }
  }



template<typename eT, typename sT>
inline
typename enable_if2< (is_signed<sT>::value == true), void >::result
internal_regspace_var_delta
  (
  Mat<eT>& x,
  const typename Mat<eT>::pod_type start,
  const sT                         delta,
  const typename Mat<eT>::pod_type end
  )
  {
  arma_extra_debug_sigprint();
  arma_extra_debug_print("internal_regspace_var_delta(): signed version");
  
  typedef typename Mat<eT>::pod_type T;
  
  if( ((start < end) && (delta < sT(0))) || ((start > end) && (delta > sT(0))) || (delta == sT(0)) )  { return; }
  
  const bool ascend = (start <= end);
  
  const T inc = (delta < sT(0)) ? T(-delta) : T(delta);
  
  const T M = ((ascend) ? T(end-start) : T(start-end)) / T(inc);
  
  const uword N = uword(1) + ( (is_non_integral<T>::value) ? uword(std::floor(double(M))) : uword(M) );
  
  x.set_size(N);
  
  eT* x_mem = x.memptr();
  
  if(ascend)
    {
    for(uword i=0; i < N; ++i) { x_mem[i] = eT( start + T(i*inc) ); }
    }
  else
    {
    for(uword i=0; i < N; ++i) { x_mem[i] = eT( start - T(i*inc) ); }
    }
  }



template<typename eT, typename uT>
inline
typename enable_if2< (is_signed<uT>::value == false), void >::result
internal_regspace_var_delta
  (
  Mat<eT>& x,
  const typename Mat<eT>::pod_type start,
  const          uT                delta,
  const typename Mat<eT>::pod_type end
  )
  {
  arma_extra_debug_sigprint();
  arma_extra_debug_print("internal_regspace_var_delta(): unsigned version");
  
  typedef typename Mat<eT>::pod_type T;
  
  if( ((start > end) && (delta > uT(0))) || (delta == uT(0)) )  { return; }
  
  const bool ascend = (start <= end);
  
  const T inc = T(delta);
  
  const T M = ((ascend) ? T(end-start) : T(start-end)) / T(inc);
  
  const uword N = uword(1) + ( (is_non_integral<T>::value) ? uword(std::floor(double(M))) : uword(M) );
  
  x.set_size(N);
  
  eT* x_mem = x.memptr();
  
  if(ascend)
    {
    for(uword i=0; i < N; ++i) { x_mem[i] = eT( start + T(i*inc) ); }
    }
  else
    {
    for(uword i=0; i < N; ++i) { x_mem[i] = eT( start - T(i*inc) ); }
    }
  }



template<typename vec_type, typename sT>
inline
typename enable_if2< is_Mat<vec_type>::value && (is_signed<sT>::value == true), vec_type >::result
regspace
  (
  const typename vec_type::pod_type start,
  const          sT                 delta,
  const typename vec_type::pod_type end
  )
  {
  arma_extra_debug_sigprint();
  arma_extra_debug_print("regspace(): signed version");
  
  vec_type x;
  
  if( ((delta == sT(+1)) && (start <= end)) || ((delta == sT(-1)) && (start > end)) )
    {
    internal_regspace_default_delta(x, start, end);
    }
  else
    {
    internal_regspace_var_delta(x, start, delta, end);
    }
  
  if(x.n_elem == 0)
    {
    if(is_Mat_only<vec_type>::value)  { x.set_size(1,0); }
    }
  
  return x;
  }



template<typename vec_type, typename uT>
inline
typename enable_if2< is_Mat<vec_type>::value && (is_signed<uT>::value == false), vec_type >::result
regspace
  (
  const typename vec_type::pod_type start,
  const          uT                 delta,
  const typename vec_type::pod_type end
  )
  {
  arma_extra_debug_sigprint();
  arma_extra_debug_print("regspace(): unsigned version");
  
  vec_type x;
  
  if( (delta == uT(+1)) && (start <= end) )
    {
    internal_regspace_default_delta(x, start, end);
    }
  else
    {
    internal_regspace_var_delta(x, start, delta, end);
    }
  
  if(x.n_elem == 0)
    {
    if(is_Mat_only<vec_type>::value)  { x.set_size(1,0); }
    }
  
  return x;
  }



template<typename vec_type>
inline
typename
enable_if2
  <
  is_Mat<vec_type>::value,
  vec_type
  >::result
regspace
  (
  const typename vec_type::pod_type start,
  const typename vec_type::pod_type end
  )
  {
  arma_extra_debug_sigprint();
  
  vec_type x;
  
  internal_regspace_default_delta(x, start, end);
  
  if(x.n_elem == 0)
    {
    if(is_Mat_only<vec_type>::value)  { x.set_size(1,0); }
    }
  
  return x;
  }



inline
vec
regspace(const double start, const double delta, const double end)
  {
  arma_extra_debug_sigprint();
  
  return regspace<vec>(start, delta, end);
  }



inline
vec
regspace(const double start, const double end)
  {
  arma_extra_debug_sigprint();
  
  return regspace<vec>(start, end);
  }



//! @}

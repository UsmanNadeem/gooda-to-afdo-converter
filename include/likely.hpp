//=======================================================================
// Copyright Baptiste Wicht 2012-2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

/*!
 * \file likely.hpp
 * \brief Contains macro helping GCC to produce efficient code.
 */

#ifndef LIKELY_H
#define LIKELY_H

#ifdef __GNUC__

/*!
 * \def likely
 * \brief Indicate that the x condition is expected to be likely true
 * \param x The condition that is expected to be likely true
 */
#define likely(x)   __builtin_expect(!!(x), 1)

/*!
 * \def unlikely
 * \brief Indicate that the x condition is expected to be likely false
 * \param x The condition that is expected to be likely false
 */
#define unlikely(x) __builtin_expect(!!(x), 0)

#else

#define likely(x)   x
#define unlikely(x) x

#endif

#endif

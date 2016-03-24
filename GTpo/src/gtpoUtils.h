/*
    The MIT License (MIT)

    Copyright (c) 2016 Benoit AUTHEMAN

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

//-----------------------------------------------------------------------------
// This file is a part of the GTpo software library.
//
// \file	gtpoUtils.h
// \author	benoit@qanava.org
// \date	2016 01 22
//-----------------------------------------------------------------------------

#ifndef gtpoUtils_h
#define gtpoUtils_h

// STD headers
#include <algorithm>
#include <exception>    // std::runtime_error
#include <memory>       // std::weak_ptr
#include <iostream>     // std::cout

namespace gtpo { // ::gtpo

//! Empty class
class Empty { };

/*! \brief Exception thrown by GTpo to notify user that a topology related error occurs.
 *
 * Use what() to have a detailled error description.
 */
class bad_topology_error : public std::runtime_error
{
public:
    explicit bad_topology_error (const std::string& what_arg) : std::runtime_error( what_arg ) { }
    explicit bad_topology_error (const char* what_arg) : std::runtime_error( what_arg ) { }
    bad_topology_error () : bad_topology_error( "GTpo topology unrecoverable error." ) { }
};

/*! Standard GTpo utility to assert an expression  \c expr and throw an exception if \c expr is *false*.
 *
 * Used with no template parameter, assert_throw will throw a gtpo::bad_topology_error with a given \c message.
 *
 * \note This function does not rely on N_DEBUG, the test will be run even in release mode.
 */
template < class E = gtpo::bad_topology_error >
auto    assert_throw( bool expr, std::string message = std::string{ "" } ) noexcept( false ) -> void {
    if ( !expr )
        throw E( message );
}

//! Compare two std::weak_ptr that must have been checked for expired() and nullptr content (ie use_count()==0).
template < class T >
auto    compare_weak_ptr( const std::weak_ptr<T>& left, const std::weak_ptr<T>& right ) -> bool const {
    return !left.owner_before( right ) && !right.owner_before( left );
}

/*! Configuration interface for accessing graph containers.
 *
 * GTpo GenGraph Config configuration struct could inherit from ContainerConfig to
 * help how GenGraph access to its nodes and edges container.
 *
 * For example, if you define class GenGraph< QtConfig >, your Qt config class could inherit
 * from gtpo::QtContainerConfig (include gtpoQtUtils.h), for the standard library, use
 * gtpo::StdContainerConfig (DefaultConfig inherits STDContainerConfig to provide
 * default implementations for accessors working with most STL containers.
 */
struct ContainerConfig {
    /*! Generic "item inserter" for list/set/vector, specialize for your custom container types.
     */
    template <typename T>
    struct insert { };

    /*! Generic "item remover" for list/set/vector, specialize for your custom container types.
     *
     * \code
     *   using IntList = std::list< int >;
     *   IntList l;
     *   l.push_back( 42 );
     *   // Same interface for std::list/set/vector
     *   gtpo::DefaultConfig::remove<IntList>::from( l, 42 );
     * \endcode
     */
    template <typename T>
    struct remove { };
};

} // ::gtpo

namespace std
{
    //! Specialization of std::hash for weak_ptr type.
    template< typename T >
    struct hash< std::weak_ptr< T > >
    {
        typedef std::weak_ptr< T > argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& key ) const {
            return std::hash< typename std::shared_ptr<T> >()( key.lock() );
        }
    };

    //! Specialization of std::equal_to for weak_ptr type.
    template< typename T >
    struct equal_to< std::weak_ptr< T > >
    {
        typedef bool result_type;
        typedef std::weak_ptr< T > first_argument_type;
        typedef std::weak_ptr< T > second_argument_type;
        result_type operator()( const first_argument_type& left, const second_argument_type& right ) const
        {
           return !left.owner_before( right ) && !right.owner_before( left );
        }
    };
}

#endif // gtpoUtils_h


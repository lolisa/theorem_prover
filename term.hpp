#ifndef TERM_HPP
#define TERM_HPP
#include "function.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <set>
//#define self (*this)
namespace first_order_logic
{
	struct term
	{
		enum class type { function, constant, variable };
		struct internal
		{
			type term_type;
			std::string name;
			std::vector< term > arguments;
			internal( type term_type, const std::string & name, const std::vector< term > & arguments ) :
				term_type( term_type ), name( name ), arguments( arguments ) { }
		};
		std::shared_ptr< internal > data;
		term( type term_type, const std::string & name, const std::vector< term > & arguments ) : data( new internal( term_type, name, arguments ) ) { }
		term( const std::shared_ptr< internal > & data ) : data( data ) { }
		std::set< term > constants( ) const
		{
			switch ( (*this)->term_type )
			{
			case type::variable:
				return { };
			case type::constant:
				return { term( data ) };
			case type::function:
				{
					std::set< term > ret;
					std::transform( (*this)->arguments.begin( ),
									(*this)->arguments.end( ),
									boost::make_function_output_iterator( [&]( const std::set< term > & s ){ ret.insert( s.begin( ), s.end( ) ); } ),
									[&]( const term & t ){ return t.constants( ); } );
					return ret;
				}
			}
		}
		size_t length( ) const { return std::accumulate( (*this)->arguments.begin( ), (*this)->arguments.end( ), 1, []( size_t s, const term & t ){ return s + t.length( ); } ); }
		std::set< term > free_variables( )
		{
			switch ( (*this)->term_type )
			{
			case type::variable:
				return { term( data ) };
			case type::constant:
				return { };
			case type::function:
				{
					std::set< term > ret;
					std::transform( (*this)->arguments.begin( ),
									(*this)->arguments.end( ),
									boost::make_function_output_iterator( [&]( const std::set< term > & s ){ ret.insert( s.begin( ), s.end( ) ); } ),
									[&]( const term & t ){ return t.constants( ); } );
					return ret;
				}
			}
		}
		std::set< function > functions( ) const
		{
			std::set< function > ret;
			if ( (*this)->term_type == type::function ) { ret.insert( function( (*this)->name, (*this)->arguments.size( ) ) ); }
			std::for_each(
						(*this)->arguments.begin( ),
						(*this)->arguments.end( ),
						[&]( const term & t )
						{
							auto tem = t.functions( );
							ret.insert( tem.begin( ), tem.end( ) );
						} );
			return ret;
		}
		const internal * operator -> ( ) const { return data.get( ); }
		explicit operator std::string( ) const
		{
			throw;
		}
		bool operator < ( const term & comp ) const { throw comp; }
		term( ) { }
	};
}
#endif // TERM_HPP
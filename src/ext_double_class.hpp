/*
 * ext_double_class.hpp
 *
 *  Created on: Oct 11, 2012
 *      Author: chris
 *      A class for extended doubles (i.e. includes infinity and NaN)
 */

#ifndef EXT_DOUBLE_CLASS_HPP_
#define EXT_DOUBLE_CLASS_HPP_

#include <iostream> // for ostream::operator<<
#include <cmath> // for pow and log overloading

#define INFTY ext_double(true)
#define NANUM ext_double(false)


class ext_double {
public:
	ext_double(); // zero
	ext_double(double ); // init with a number
	ext_double(long double ); // idem
	ext_double(int ); //idem
	ext_double(bool ); // ext_double(true) = \infty, ext_infty(false) = NaN
	ext_double(const ext_double & ); // copy constructor
	ext_double & operator=(const ext_double & ); // copy assignment constructor
	ext_double & operator+=(const ext_double & );
	ext_double & operator*=(const ext_double & );
	ext_double & operator-=(const ext_double & );
	ext_double & invert(); // this --> 1/this
	const ext_double reciprocal() const; // take the inverse
	ext_double & operator/=(const ext_double & );
	const ext_double operator+(const ext_double & ) const;
	const ext_double operator*(const ext_double & ) const;
	const ext_double operator/(const ext_double & ) const;
	const ext_double operator-() const;
	const ext_double operator-(const ext_double & ) const;
	bool operator==(const ext_double & ) const;
	bool operator!=(const ext_double & ) const;
	bool operator<(const ext_double & ) const; // BRAND NEW... (CHECK!!)
	bool operator>(const ext_double & ) const; // NOT IMPLEMENTED YET
	bool operator>=(const ext_double & ) const; // NOT IMPLEMENTED YET
	bool operator<=(const ext_double & ) const;
	friend std::ostream & operator<<(std::ostream & , const ext_double & );
	friend ext_double pow(const ext_double & , const ext_double & );
	friend ext_double log10(const ext_double & );
	friend bool isNUM(const ext_double & );
	friend bool isFinite(const ext_double & );
private:
	long double value;
	int sign;
	bool isFinite;
	bool isNumber;
};



std::ostream & operator<<(std::ostream & , const ext_double & );

ext_double pow(const ext_double & , const ext_double & ); // overloaded power function
ext_double log10(const ext_double & ); // overloaded log10 function
ext_double max(const ext_double & , const ext_double & );
ext_double min(const ext_double & , const ext_double & );
bool isNUM(const ext_double & );
bool isFinite(const ext_double & );

#endif /* EXT_DOUBLE_CLASS_HPP_ */

/*
 * ext_double_class.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: chris
 */

#include"ext_double_class.hpp"


// TODO ext_double: catch inf and nan from overflow

ext_double::ext_double() {
	isNumber = true;
	isFinite = true;
	value = 0;
	sign = 0;
}

ext_double::ext_double(long double value) {
	if ( !isnan(value) ) {
		if ( !isinf(value) ) {
			this->value = value;
			isFinite = true;
			isNumber = true;
			sign = ((int) (value != 0)) * (value > 0 ? 1 : -1);
		}
		else {
			this->value = 0;
			sign = ( value < 0 ? -1 : 1 );
			isFinite = false;
			isNumber = true;
		}
	}
	else {
		this->value = 0;
		isFinite = true;
		isNumber = false;
		sign = 0;
	}
}

ext_double::ext_double(double value) {
	*this = ext_double((long double) value);
}

ext_double::ext_double(int value) {
	*this = ext_double((long double) value);
}

ext_double::ext_double(bool infty) { // positive infinity
	value = 0;
	if ( infty ) {
		isFinite = false;
		isNumber = true;
		sign = 1;
	}
	else {
		isFinite = true;
		isNumber = false;
		sign = 0;
	}
}

ext_double::ext_double(const ext_double & x) {
	isFinite = x.isFinite;
	isNumber = x.isNumber;
	value = x.value;
	sign = x.sign;
}

ext_double & ext_double::operator=(const ext_double & x) {
	isFinite = x.isFinite;
	isNumber = x.isNumber;
	value = x.value;
	sign = x.sign;
	return *this;
}

ext_double & ext_double::operator+=(const ext_double & x) {
	if ( ! isNumber || ! x.isNumber ) { // x + NaN = NaN
		isNumber = false;
		isFinite = true;
		sign = 0;
		value = 0;
	}
	else {
		if ( ! isFinite || ! x.isFinite ) {
			if ( sign != x.sign && !isFinite && !x.isFinite ) { // \infty - \infty = NaN
				isNumber = false;
				isFinite = true;
				value = 0;
				sign = 0;
			}
			else { // \infty + x = \infty, -\infty + x = -\infty
				isNumber = true;
				sign = ( isFinite ? x.sign : sign );
				isFinite = false;
				value = 0;
			}
		}
		else {
			value += x.value;
			sign = ((int) (value != 0)) * (value > 0 ? 1 : -1);
			isNumber = true;
			isFinite = true;
		}
	}
	return *this;
}

ext_double & ext_double::operator*=(const ext_double & x) {
	if ( ! isNumber || ! x.isNumber ) { // x * NaN = NaN
		isNumber = false;
		isFinite = true;
		sign = 0;
		value = 0;
	}
	else {
		if (isFinite && x.isFinite) {
			value *= x.value;
			sign = ((int) (value != 0)) * (value > 0 ? 1 : -1);
			isNumber = true;
			isFinite = true;
		}
		else {
			if ( (x.isFinite && x.value == 0) || (isFinite && value == 0)) { // \infty * 0 = NaN
				isNumber = false;
				isFinite = true;
				value = 0;
				sign = 0;
			}
			else {
				value = 0;
				isFinite = false;
				isNumber = true;
				sign *= x.sign;
			}
		}
	}
	return *this;
}

const ext_double ext_double::operator+(const ext_double & x) const {
	return ext_double(*this) += x;
}

const ext_double ext_double::operator*(const ext_double & x) const {
	return ext_double(*this) *= x;
}

ext_double & ext_double::invert() {
	if ( isNumber ) {
		if ( isFinite ) {
			if ( value == 0 ) { // 1/0 = \infty
				isFinite = false;
				sign = 1;
				isNumber = true;
			}
			else {
				value = 1/value;
			}
		}
		else { // infinite 1/\infty = 0
			isFinite = true;
			sign = 0;
		}
	}
	return *this;
}

const ext_double ext_double::reciprocal() const {
	return ext_double(*this).invert();
}

ext_double & ext_double::operator/=(const ext_double & x) {
	operator*=(ext_double(x).invert());
	return *this;
}

const ext_double ext_double::operator/(const ext_double & x) const {
	return ext_double(*this) /= x;
}

bool ext_double::operator==(const ext_double & x) const {
	if ( isNumber && x.isNumber ) {
		if ( value == x.value && sign == x.sign )
			return true;
		else return false;
	}
	else return false;
}

bool ext_double::operator!=(const ext_double & x) const {
	return ! operator==(x);
}

bool ext_double::operator<=(const ext_double & x) const {
	if ( isNumber && x.isNumber ) {
		if ( !isFinite || !x.isFinite ) {
			if ( (!isFinite && sign == -1) || (!x.isFinite && x.sign == 1) ) {
				return true;
			}
			else return false;
		}
		else return value <= x.value;
	}
	else return false; // if NaN always return false
}

bool ext_double::operator>=(const ext_double & x) const {
	return (x <= *this);
}

bool ext_double::operator<(const ext_double & x) const {
	return ( operator<=(x) && ! operator==(x) );
}

bool ext_double::operator>(const ext_double & x) const {
	return (x < *this);
}



ext_double & ext_double::operator-=(const ext_double & x) {
	operator+=(-x);
	return *this;
}

const ext_double ext_double::operator-() const {
	if ( isNumber ) {
		if ( isFinite ) {
			return ext_double(-value);
		}
		else {
			ext_double x(true); // infty
			x.sign = -sign;
			return x;
		}
	}
	else return ext_double(false);
}

const ext_double ext_double::operator-(const ext_double & x) const {
	return ext_double(*this) -= x;
}


/**************************** ostream overload ****************************/

std::ostream & operator<<(std::ostream & os, const ext_double & x) {
	if ( !x.isNumber ) os << "NaN";
	else {
		if ( !x.isFinite ) {
			if ( x.sign == -1 ) os << "-Inf";
			else os << "Inf";
		}
		else os << x.value;
	}
    return os;
}

/************************** pow and log overloaded ***********************/

ext_double pow(const ext_double & base, const ext_double & power) {
	if ( base.isNumber && power.isNumber ) {
		switch ( base.sign ) {
			case 1: {
				if ( base.isFinite ) {
					if ( power.isFinite ) return ext_double(pow(base.value,power.value));
					else {
						if ( power.sign == 1 ) return INFTY;
						else return ext_double(0.0);
					}
				}
				else { // base is +\infty
					switch ( power.sign ) {
						case 1: return INFTY; break;
						case 0: return NANUM; break;
						case -1: return ext_double(0.0); break;
						default: return NANUM;
					} // switch ( power.sign )
				}
				break;
			}
			case 0: { // base == 0
				if ( power.isFinite && power.sign != 0 ) return ext_double(0.0);
				else return NANUM;
				break;
			}
			case -1: {
				if ( power.sign == 0 ) return ext_double(1.0);
				else return NANUM; // TODO pow defined on integer powers
				break;
			}
			default: return NANUM;
		}
	}
	else { // ! (base.isNumber && power.isNumber)
		return NANUM;
	}
}


ext_double log10(const ext_double & x) {
	if ( x.isNumber ) {
		switch ( x.sign ) {
			case 1: {
				if ( x.isFinite ) return ext_double(log10(x.value));
				else return INFTY;
				break;
			}
			case 0: {
				return -(INFTY);
				break;
			}
			case -1: {
				return NANUM;
				break;
			}
			default:  return NANUM;
		}
	} // if ( x.isNumber )
	else {
		return NANUM;
	}
}

ext_double max(const ext_double & x1, const ext_double & x2) {
	if ( isNUM(x1) && isNUM(x2) ) {
		if ( x1 <= x2 ) return x2;
		else return x1;
	}
	else return NANUM;
}

ext_double min(const ext_double & x1, const ext_double & x2) {
	if ( isNUM(x1) && isNUM(x2) ) {
		if ( x1 <= x2 ) return x1;
		else return x2;
	}
	else return NANUM;
}


/****************************** test if ext_double is NANUM ******************/

bool isNUM(const ext_double & x) {
	return x.isNumber;
}

bool isFinite(const ext_double & x) {
	return x.isFinite;
}

#include "rational.h"

int gcd ( int a, int b )
{
  int c;
  while ( a != 0 ) {
     c = a; a = b%a;  b = c;
  }
  return b;
}

rational::rational(int num, int denom){
	int g = gcd(num, denom);
	this->num   = num / g;
	this->denom = denom / g;
}

rational::rational(int num) {
	this->num   = num;
	this->denom = 1;
}

int rational::getNum() const {
	return num;
}
int rational::getDenom() const {
	return denom;
}

rational rational::operator +(rational const &b) const {
	return rational(num*b.getDenom() + denom*b.getNum(), denom*b.getDenom());
}
rational rational::operator -(rational const &b) const {
	return rational(num*b.getDenom() - denom*b.getNum(), denom*b.getDenom());
}
rational rational::operator *(rational const &b) const {
	return rational(num * b.getNum(), denom * b.getDenom());
}
rational rational::operator /(rational const &b) const {
	return rational(num * b.getDenom(), denom + b.getNum());
}
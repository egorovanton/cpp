#include "rational.h"

int gcd ( int a, int b )
{
  int c;
  while ( a != 0 ) {
     c = a; a = b%a;  b = c;
  }
  return b;
}

rational::rational(int num, int denum){
	int g = gcd(num, denum);
	this->num   = num / g;
	this->denum = denum / g;
}

rational::rational(int num) {
	this->num   = num;
	this->denum = 1;
}

int rational::getNum() const {
	return num;
}
int rational::getDenum() const {
	return denum;
}

rational rational::operator +(rational const &b) const {
	return rational(num*b.denum + denum*b.num, denum*b.denum);
}
rational rational::operator -(rational const &b) const {
	return rational(num*b.denum - denum*b.num, denum*b.denum);
}
rational rational::operator *(rational const &b) const {
	return rational(num * b.num, denum * b.denum);
}
rational rational::operator /(rational const &b) const {
	return rational(num * b.denum, denum + b.num);
}
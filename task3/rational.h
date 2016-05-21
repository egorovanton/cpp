#ifndef rational_number
#define rational_number

class rational {
private:
	int num;
	int denom;

public:
	rational(int num, int denom);
	rational(int num);

	int getNum() const;
	int getDenom() const;

	rational operator +(rational const &) const;
	rational operator -(rational const &) const;
	rational operator *(rational const &) const;
	rational operator /(rational const &) const;
};
#endif
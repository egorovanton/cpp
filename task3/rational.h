class rational {
private:
	int num;
	int denum;
	
public:
	rational(int num, int denum);
	rational(int num);

	int getNum() const;
	int getDenum() const;

	rational operator +(rational const &) const;
	rational operator -(rational const &) const;
	rational operator *(rational const &) const;
	rational operator /(rational const &) const;
};
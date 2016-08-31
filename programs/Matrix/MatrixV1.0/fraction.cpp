#include <math.h>
#include "fraction.h"

typedef std::list<int>::iterator ListIter;

DivideByZero divide_by_zero;

//Default constructor
Fraction::Fraction(){}

//Initialization constructor for two integers
Fraction::Fraction(const int numerator, const int denominator)
	:num(numerator), den(denominator)
{
	if (!den) throw divide_by_zero;
	lowest_terms();
}

//Initialization construction for std::pair<int,int> (num,den)
Fraction::Fraction(const std::pair<int, int> n)
	:num(n.first), den(n.second)
{
	if (!den) throw divide_by_zero;
	lowest_terms();
}

//Initialization constructor for doubles
Fraction::Fraction(const double n, const int sig){
	double intpart, fractpart;

	fractpart = modf(n, &intpart);
	if (fractpart == 0.0){//whole number
		num = (int) intpart;
		den = 1;
	}else{//well crap
		num = (int) (n * pow(10.0, (double) sig));
		den = (int) pow(10.0, (double) sig);
	}
	//Pray to god that the numerator isn't a big prime
	lowest_terms();
}

Fraction::~Fraction(){
}

//Returns (true, n) if n is prime else (false, factor)
const std::pair<bool,int> Fraction::is_prime(const int &n) const{
	if (n <= 1) return std::make_pair(false, -1);
	const double sqrt_n = sqrt((double) n);
	
	if (!(n % 2)) return std::make_pair(false, 2);
	for (int i = 3; i <= sqrt_n; i += 2)
		if (!(n % i)) return std::make_pair(false, i);
	return std::make_pair(true, n);
}

//Returns the next prime after n
const int &Fraction::get_next_prime(int &n) const{
	while (!is_prime(++n).first) ;
	return n;
}

//Makes the fraction into lowest terms
void Fraction::lowest_terms(){
	bool sign;
	int max;
	
	if (num == 0){
		den = 1;
		return;
	}else if (den < 0){
		num *= -1;
		den *= -1;
	}
	if (num < 0){
		sign = true;
		num *= -1;
	}else{
		sign = false;
	}

	max = (num > den) ? num : den;
	for (int i = 2; i <= max; get_next_prime(i)){
		while ((num % i == 0) && (den % i == 0)){
			num /= i;
			den /= i;
		}
	}
	if (sign) num *= -1;
}

//Returns a list of prime factors given an integer > 1
const std::list<int> Fraction::factor(int n) const{
	std::list<int> factors;
	std::pair<bool, int> p;
	int i;

	if (n <= 1) return factors;
	if ((p = is_prime(n)).first){//prime off the bat
		factors.push_back(n);
	}else{						//composite
		i = p.second;
		do{
			if (n % i){
				get_next_prime(i);
			}else{
				factors.push_back(i);
				n /= i;
			}
		}while (n != 1);
	}
	return factors;
}

//Returns std::pair<int,int> i.e. (numerator,denominator)
const std::pair<int, int> Fraction::GetFraction() const{
	return std::make_pair(num, den);
}

//Returns numerator
const int Fraction::Numerator(){
	return num;
}

const int &Fraction::Numerator() const{
	return num;
}

//Returns denominator
const int Fraction::Denominator(){
	return den;
}

const int &Fraction::Denominator() const{
	return den;
}

//Returns numerator's factors
const std::list<int> Fraction::NumeratorFactors() const{
	std::list<int> temp;
	(num > 0) ? temp = factor(num) : temp = factor(-num);
	return temp;
}

//Returns denominator's factors
const std::list<int> Fraction::DenominatorFactors() const{
	return factor(den);
}

//Mathematical operators
//With Fractions:
const Fraction Fraction::operator + (const Fraction &param) const{
	int numerator = num * param.Denominator() + param.Numerator() * den;
	int denominator = den * param.Denominator();
	Fraction temp(numerator, denominator);
	return temp;
}

const Fraction Fraction::operator - (const Fraction &param) const{
	int numerator = num * param.Denominator() - param.Numerator() * den;
	int denominator = den * param.Denominator();
	Fraction temp(numerator, denominator);
	return temp;
}

const Fraction Fraction::operator - () const{ //negation
	Fraction temp(-num, den);
	return temp;
}

const Fraction Fraction::operator * (const Fraction &param) const{
	int numerator = num * param.Numerator();
	int denominator = den * param.Denominator();
	Fraction temp(numerator,denominator);
	return temp;
}

const Fraction Fraction::operator / (const Fraction &param) const{
	int numerator = num * param.Denominator();
	int denominator = den * param.Numerator();
	Fraction temp(numerator,denominator);
	return temp;
}

const Fraction Fraction::operator = (const Fraction &param){
	num = param.Numerator();
	den = param.Denominator();
	return *this;
}

const bool Fraction::operator < (const Fraction &param) const{
	return ((num * param.Denominator()) < (param.Numerator() * den));
}

const bool Fraction::operator > (const Fraction &param) const{
	return ((num * param.Denominator()) > (param.Numerator() * den));
}

const Fraction Fraction::operator += (const Fraction &param){
	return *this = *this + param;
}

const Fraction Fraction::operator -= (const Fraction &param){
	return *this = *this - param;
}

const Fraction Fraction::operator *= (const Fraction &param){
	return *this = *this * param;
}

const Fraction Fraction::operator /= (const Fraction &param){
	return *this = *this / param;
}

const bool Fraction::operator == (const Fraction &param) const{
	return ((num * param.Denominator()) == (param.Numerator() * den));
}

const bool Fraction::operator != (const Fraction &param) const{
	return ((num * param.Denominator()) != (param.Numerator() * den));
}

const bool Fraction::operator <= (const Fraction &param) const{
	return ((num * param.Denominator()) <= (param.Numerator() * den));
}

const bool Fraction::operator >= (const Fraction &param) const{
	return ((num * param.Denominator()) >= (param.Numerator() * den));
}

//With doubles
const Fraction Fraction::operator + (const double param) const{
	return *this + Fraction(param);
}

const Fraction Fraction::operator - (const double param) const{
	return *this - Fraction(param);
}

const Fraction Fraction::operator * (const double param) const{
	return *this * Fraction(param);
}

const Fraction Fraction::operator / (const double param) const{
	return *this / Fraction(param);
}

const Fraction Fraction::operator = (const double param){
	return *this = Fraction(param);
}

const bool Fraction::operator < (const double param) const{
	return (((double) num)/((double) den) < param);
}

const bool Fraction::operator > (const double param) const{
	return (((double) num)/((double) den) > param);
}

const Fraction Fraction::operator += (const double param){
	return *this = *this + Fraction(param);
}

const Fraction Fraction::operator -= (const double param){
	return *this = *this - Fraction(param);
}

const Fraction Fraction::operator *= (const double param){
	return *this = *this * Fraction(param);
}

const Fraction Fraction::operator /= (const double param){
	return *this = *this / Fraction(param);
}

const bool Fraction::operator == (const double param) const{
	return (((double) num)/((double) den) == param);
}

const bool Fraction::operator != (const double param) const{
	return (((double) num)/((double) den) != param);
}

const bool Fraction::operator <= (const double param) const{
	return (((double) num)/((double) den) <= param);
}

const bool Fraction::operator >= (const double param) const{
	return (((double) num)/((double) den) >= param);
}

//Output operators
std::ostream &operator << (std::ostream &os, const Fraction &param){
	os << param.Numerator() << '/' << param.Denominator();
	return os;
}
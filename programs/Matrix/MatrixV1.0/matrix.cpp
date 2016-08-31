#include <iomanip>
#include <sstream>
#include <string>
#include <stdlib.h>
#include "matrix.h"

//FourSubsets static member initialization
const char *FourSubsets::titles[4] = {
	"Null Space:", "Row Space:",
	"Column Space:", "Left Null Space:"
};

//Default constructor
Matrix::Matrix() {}

//Constructor accepting vector of vectors to represent matrix
Matrix::Matrix(MatrixType m){
	VectorType v;

	for (MatrixType::iterator row = m.begin(); row != m.end(); row++){
		v.clear();
		for (VectorType::iterator col = row->begin(); col != row->end(); col++){
			v.push_back(*col);
		}
		matrix.push_back(v);
	}
}

//Initialization constructor accepting an array of an array of Fractions
Matrix::Matrix(Fraction **f, size_t rows, size_t cols){
	VectorType v;

	matrix.resize(rows);
	for (unsigned int i = 0; i < rows; i++){
		v.assign(f[i], f[i] + cols);
		matrix[i] = v;
	}
}

//Initialization constructor that reads matrix from file
Matrix::Matrix(std::istream &is, const bool header){
	std::istringstream iss;
	std::string s;
	VectorType v;

	if (header == true) std::getline(is, s);
	for (unsigned int count = 1; std::getline(is, s); count++){
		iss.clear();	//empty
		iss.str(s);		//reinitialize
		v.clear();
		while (iss >> s){//parse with delimiter = ' '
			v.push_back(atof(s.c_str()));
		}
		matrix.push_back(v);
	}
}

//Destructor that does nothing
Matrix::~Matrix() {}

//Protected methods------------------------------------------------------------
//Swaps rows a and b
void Matrix::row_swap(unsigned int a, unsigned int b){
	matrix[a].swap(matrix[b]);
}

//Creates a leading one at col and scales the rest of the entries of that row
void Matrix::create_leading_one(unsigned int row, unsigned int col){
	Fraction divisor = matrix[row][col];

	//leading one exists
	if (divisor == 1) return;
	for (VectorType::iterator it = matrix[row].begin() + col;
		 it != matrix[row].end(); it++)
	{
		*it /= divisor;
	}
}

//Creates a pivot column at col, while keeping the leading one at row
void Matrix::create_pivot_col(unsigned int row, unsigned int col){
	Fraction scale;

	for (unsigned int i = 0; i < matrix.size(); i++){
		if (i != row){
			scale = -matrix[i][col];
			for (unsigned int j = col; j < matrix.front().size(); j++){
				matrix[i][j] += scale * matrix[row][j];
			}
		}
	}
}

//Finds pivot columns from a row reduced matrix
const std::list<unsigned int> Matrix::find_pivot_cols(const MatrixType &m) const{
	std::list<unsigned int> pc;
	unsigned int col = 0;
	
	if (m.empty()) return pc;
	for (unsigned int row = 0; row < m.size() && col < m.front().size(); row++){
		while (col < m.front().size()){
			if (m[row][col] != 1){
				col++;
			}else{
				pc.push_back(col + 1);
				col++;
				break;
			}
		}
	}
	return pc;
}

//Instantiates null space
const BasisType Matrix::find_null_space(const MatrixType &m,
										const unsigned int v_size) const
{
	BasisType null_space_basis;
	VectorType v;
	std::list<unsigned int> pc = find_pivot_cols(m), free_cols;
	std::list<unsigned int>::const_iterator it, d;

	//If matrix doesn't exist, push back identity matrix and return (can be anything)
	if (m.empty()){
		MatrixType temp = CreateIdentityMatrix(v_size);
		return null_space_basis = find_row_space(temp, v_size);
	}
	//If no free columns (identity matrix), push back zero vector and return
	if (pc.size() == m.front().size()){
		null_space_basis.push_back(CreateZeroVector(v_size));
		return null_space_basis;
	}
	//Generate list of free cols
	it = pc.begin();
	for (unsigned int j = 1; j <= m.front().size(); j++){
		if (it == pc.end()){
			free_cols.push_back(j);
		} else {
			if (*it == j){//is pivot col
				it++;
			} else {//is free col
				free_cols.push_back(j);
			}
		}
	}
	//Generate basis for null space
	for (it = free_cols.begin(); it != free_cols.end(); it++){
		d = pc.begin();
		for (unsigned int row = 0, col = 1; col <= m.front().size(); col++){
			if (d != pc.end()){
				if (*d == col){//element of pivot_cols
					v.push_back(-m[row][*it-1]);
					d++;
					row++;
					continue;
				}
			}
			//element of free_cols
			if (*it == col){
				v.push_back(1);
			} else {
				v.push_back(0);
			}
		}
		null_space_basis.push_back(v);
		v.clear();
	}
	return null_space_basis;
}

const BasisType Matrix::find_row_space(const MatrixType &m, const unsigned int v_size) const{
	BasisType row_space_basis;

	if (m.empty()){
		row_space_basis.push_back(CreateZeroVector(v_size));
	}
	//Generate basis for row space (no zero rows guaranteed by the algorithm)
	for (MatrixType::const_iterator row = m.begin(); row != m.end(); row++){
		row_space_basis.push_back(*row);
	}
	return row_space_basis;
}

//Instantiates the four subsets
/*
Uses:      RREF             [C|K]
M = [A|Im] ---> N = [B|J] = [0|L]
Where:
J = A^-1
N(A) = N(C), R(A) = R(C)
C(A) = N(L), L(A) = R(L)
*/
const FourSubsets Matrix::find_four_subsets() const{
	FourSubsets fs;
	Matrix temp = *this;
	MatrixType N, C, L;
	VectorType v, zero_vector;
	unsigned int rows, cols;
	
	if (matrix.empty()) return fs;

	rows = matrix.size();
	cols = matrix.front().size();
	zero_vector = CreateZeroVector(cols);

	//Initialize matrix N
	temp.PushRightMatrix(CreateIdentityMatrix(rows));
	N = temp.RREF();

	//Initialize matrices C and L
	for (MatrixType::iterator row = N.begin(); row != N.end(); row++){
		v.clear();
		v.insert(v.begin(), row->begin(), row->begin() + cols);
		if (v != zero_vector){
			C.push_back(v);
		}else{
			v.clear();
			v.insert(v.begin(), row->begin() + cols, row->end());
			L.push_back(v);
		}
	}

	//Find the four subsets using the previously mentioned algorithm
	fs.Space[FourSubsets::Null] = find_null_space(C, cols);
	fs.Space[FourSubsets::Row] = find_row_space(C, cols);
	fs.Space[FourSubsets::Col] = find_null_space(L, rows);
	fs.Space[FourSubsets::LeftNull] = find_row_space(L, rows);
	return fs;
}

//Prints a basis to file (default is stdout)
void Matrix::print_basis(const BasisType b, std::ostream &os,
						 const char *title) const
{
	BasisType::const_iterator lit;

	os << title << std::endl;
	if (b.empty()) return;
	//Set precision for 3 significant digits
	os << std::setprecision(3);
	for (unsigned int i = 0; i < b.front().size(); i++){
		for (lit = b.begin(); lit != b.end(); lit++){
			//Set width for 6 char's long 
			//(3 sig. digits for a possible negative fraction ex. -0.421)
			os << std::setw(6) << (*lit)[i] << " ";
		}
		os << std::endl;
	}
}

//Mutator Methods--------------------------------------------------------------
//Pops a vector off the right side (returns empty vector if empty)
const VectorType Matrix::PopRightVector(){
	VectorType v;
	for (MatrixType::iterator it = matrix.begin(); it != matrix.end(); it++){
		v.push_back(it->back());
		it->pop_back();
	}
	return v;
}

//Pushes a vector onto the right side (returns false if improper row size)
const bool Matrix::PushRightVector(const VectorType &v){
	unsigned int rows = matrix.size();
	if (rows != v.size() && rows != 0) return false;
	for (unsigned int row = 0; row < v.size(); row++){
		matrix[row].push_back(v[row]);
	}
	return true;
}

//Pops a vector off the bottom side (returns empty vector if empty)
const VectorType Matrix::PopBottomVector(){
	VectorType v;
	if (matrix.size() != 0){
		v = matrix.back();
		matrix.pop_back();
	}
	return v;
}

//Pushes a vector onto the right side (returns false if improper row size)
const bool Matrix::PushBottomVector(const VectorType &v){
	if (matrix.size() == 0){
		matrix.push_back(v);
		return true;
	}else if (matrix.front().size() == v.size()){
		matrix.push_back(v);
		return true;
	}else{
		return false;
	}
}

//Pushes a matrix onto the right side (returns false if improper row size)
const bool Matrix::PushRightMatrix(const Matrix &m){
	return PushRightMatrix(m.GetMatrix());
}

const bool Matrix::PushRightMatrix(const MatrixType &m){
	if (matrix.size() != m.size()) return false;

	MatrixType temp = m; //remove const-ness
	for (unsigned int row = 0; row < matrix.size(); row++){
		matrix[row].insert(matrix[row].end(), temp[row].begin(), 
						   temp[row].end());
	}
	
	return true;
}

//Gauss-Jordan elimination aka row reduced echelon form, returns reduced matrix
const MatrixType Matrix::RREF(){
	unsigned int r = 0, i = 0, rows, cols; //empty rows, row iterator

	if (matrix.empty()) return matrix;

	rows = matrix.size();
	cols = matrix.front().size();
	for (unsigned int j = 0; j < cols; j++){
		for (i = r; i < rows; i++){
			if (matrix[i][j] != 0){
				row_swap(i, r);				//move non-zero row to row 'r'
				create_leading_one(r, j);	//create leading 1 at pos. 'j' and scale rest
				create_pivot_col(r, j);		//use row operations to create pivot col.
				r++;
				break;
			}
		}
	}
	return matrix;
}

//Transposes matrix;
const MatrixType Matrix::Transpose(){
	Fraction temp;
	unsigned int rows, cols;

	if ((rows = matrix.size()) == 0) return matrix;
	cols = matrix.front().size();
	if (rows > cols){
		for (unsigned int row = 0; row < cols; row++){
			matrix[row].resize(rows);
			for (unsigned int col = row; col < rows; col++){
				temp = matrix[row][col];
				matrix[row][col] = matrix[col][row];
				matrix[col][row] = temp;
			}
		}
		for (unsigned int row = cols; row < rows; row++){
			matrix.pop_back();
		}
	}else if (rows < cols){
		VectorType v = CreateZeroVector(rows);
		matrix.resize(cols, v);
		for (unsigned int row = 0; row < rows; row++){
			for (unsigned int col = row; col < cols; col++){
				temp = matrix[row][col];
				matrix[row][col] = matrix[col][row];
				matrix[col][row] = temp;
			}
			matrix[row].resize(rows);
		}
	}else{
		for (unsigned int row = 0; row < rows; row++){
			for (unsigned int col = 0; col < cols; col++){
				temp = matrix[row][col];
				matrix[row][col] = matrix[col][row];
				matrix[col][row] = temp;
			}
		}
	}
	return matrix;
}

//Accessor Methods-------------------------------------------------------------
//Returns the number of rows in the matrix
const unsigned int Matrix::GetRowNumber() const{
	return matrix.size();
}

//Returns the number of cols in the matrix
const unsigned int Matrix::GetColNumber() const{
	return (matrix.empty()) ? 0 : matrix.front().size();
}

//Returns a list of pivot columns
const std::list<unsigned int> Matrix::GetPivotCols() const{
	Matrix temp(matrix);
	return find_pivot_cols(temp.RREF());
}

//Returns matrix
const MatrixType Matrix::GetMatrix() const{
	return matrix;
}

//Returns matrix from (r1, c1) to (r2, c2) if params are in range
//i.e. 1 <= r1 < r2 <= rows , 1 <= c1 < c2 <= cols
const MatrixType Matrix::GetMatrix(unsigned int r1, unsigned int c1,
								   unsigned int r2, unsigned int c2) const
{
	VectorType v;
	MatrixType m;

	if (matrix.empty()) return m;

	unsigned int rows = matrix.size(), cols = matrix.front().size();

	if ((1 <= r1) && (r1 < r2) && (r2 <= rows) &&
		(1 <= c1) && (c1 < c2) && (c2 <= cols))
	{
		//-1 since begin() starts at 1st element
		//i.e. begin() + 1 will get 2nd element (but you wanted 1st)
		for (MatrixType::const_iterator rit = matrix.begin() + r1 - 1; 
			 rit != matrix.begin() + r2; rit++)
		{
			v.clear();
			v.insert(v.begin(), rit->begin() + c1 - 1, rit->begin() + c2);
			m.push_back(v);
		}
	}
	return m;
}

//Returns null space in a list
const BasisType Matrix::GetNullSpace() const{
	Matrix temp = *this;
	MatrixType rref = temp.RREF();
	if (rref.size()){
		return find_null_space(rref, rref.front().size());
	}else{
		BasisType b;
		return b;
	}
}

//Returns row space in a list
const BasisType Matrix::GetRowSpace() const{
	Matrix temp = *this;
	MatrixType rref = temp.RREF();

	if (rref.size()){
		VectorType v = CreateZeroVector(rref.front().size());
		for (unsigned int row = rref.size() - 1; row > 0; row--)
		{
			if (rref[row] == v) rref.pop_back();
			else break;
		}
		return find_row_space(rref, rref.front().size());
	}else{
		BasisType b;
		return b;
	}
}

//Returns column space in a list
const BasisType Matrix::GetColumnSpace() const{
	Matrix temp = *this;
	temp.Transpose();
	MatrixType rref = temp.RREF();

	if (rref.size()){
		VectorType v = CreateZeroVector(rref.front().size());
		for (unsigned int row = rref.size() - 1; row > 0; row--)
		{
			if (rref[row] == v) rref.pop_back();
			else break;
		}
		return find_row_space(rref, rref.front().size());
	}else{
		BasisType b;
		return b;
	}
}

//Returns left null space in a list
const BasisType Matrix::GetLeftNullSpace() const{
	Matrix temp = *this;
	temp.Transpose();
	MatrixType rref = temp.RREF();
	if (rref.size()){
		return find_null_space(rref, rref.front().size());
	}else{
		BasisType b;
		return b;
	}
}

//Returns four subsets in an array of lists
const FourSubsets Matrix::GetFourSubsets() const{
	return find_four_subsets();
}

//Output Methods---------------------------------------------------------------
//Prints matrix to file (default is stdout)
void Matrix::PrintMatrix(std::ostream &os, const char *title) const{
	os << title << std::endl;
	//Set precision for 3 significant digits
	os << std::setprecision(3);
	for (MatrixType::const_iterator row = matrix.begin(); row != matrix.end();
		 row++)
		{
		for (VectorType::const_iterator col = row->begin(); col != row->end();
			 col++)
		{
			//Set width for 6 char's long 
			//(3 sig. digits for a possible negative fraction ex. -0.421)
			os << std::setw(6) << *col << " ";
		}
		os << std::endl;
	}
}

//Prints null space to file (default is stdout)
void Matrix::PrintNullSpace(std::ostream &os, const char *title) const{
	print_basis(GetNullSpace(), os, title);
}

//Prints row space to file (default is stdout)
void Matrix::PrintRowSpace(std::ostream &os, const char *title) const{
	print_basis(GetRowSpace(), os, title);
}

//Prints column space to file (default is stdout)
void Matrix::PrintColumnSpace(std::ostream &os, const char *title) const{
	print_basis(GetColumnSpace(), os, title);
}

//Prints left null space to file (default is stdout)
void Matrix::PrintLeftNullSpace(std::ostream &os, const char *title) const{
	print_basis(GetLeftNullSpace(), os, title);
}

//Prints four subsets to file (default is stdout)
void Matrix::PrintFourSubsets(std::ostream &os, const char **titles) const{
	FourSubsets fs = find_four_subsets();
	for (unsigned int i = 0; i < 4; i++){
		print_basis(fs.Space[i], os, titles[i]);
	}
}

//Mathematical operators-------------------------------------------------------
const VectorType Matrix::operator [] (unsigned int row) const{
	return matrix[row];
}

//Useful Functions-------------------------------------------------------------

//Creates a zero vector of size n
const VectorType CreateZeroVector(const unsigned int n){
	VectorType v(n, Fraction(0,1));
	return v;
}

//Creates an identity matrix of size n
const MatrixType CreateIdentityMatrix(const unsigned int n){
	MatrixType m;
	VectorType v;

	for (unsigned int i = 0; i < n; i++){
		v.clear();
		for (unsigned int j = 0; j < n; j++){
			if (i == j)
				v.push_back(Fraction(1,1));
			else
				v.push_back(Fraction(0,1));
		}
		m.push_back(v);
	}
	return m;
}
/**
 * @file Header_pgm.cpp
 * @brief
 * @author Nicolas Ciferri
 * @date 05.19.17
 */

#include "Header_pgm.hpp"

using namespace std;
using My_io_pgm::Header;


Header::Header(){
	magic_number = new char[100];
	strcpy((*this).magic_number,"");
	comment = new char[100];
	strcpy((*this).comment,"");

	(*this).dimx = 0;
	(*this).dimy = 0;
	(*this).dimz = 0;
	(*this).number_grey_level = 0;
}

Header::Header ( const char* filepath_pgm ){
	cout <<"Read file " << filepath_pgm << endl ;
	ifstream pgm_image ( filepath_pgm  ) ;
	if (!pgm_image){
		cerr << "Error open file : "<< filepath_pgm << endl ;
		exit(-1);
	}

	char buffer[512] ;


	magic_number = new char[100];
	pgm_image.getline (buffer,512) ;
	strcpy((*this ).magic_number ,buffer);

	comment = new char[100];
	pgm_image.getline (buffer,512) ;
	strcpy((*this ).comment ,buffer);

	int dimx ;
	pgm_image >> dimx ;
	(*this ).dimx = dimx ;
	cout << dimx << endl;

	int dimy ;
	pgm_image >> dimy ;
	(*this ).dimy = dimy ;
	cout << dimy << endl;

	int dimz ;
	pgm_image >> dimz ;
	(*this ).dimz = dimz ;

	int number_grey_level;
	pgm_image >> number_grey_level;
	(*this).number_grey_level = number_grey_level;

	pgm_image.close();
}

Header::~Header(){
}

const char* Header::get_MagicNumber() const{
	const char* magic_number = (*this).magic_number;
	return magic_number;
}

const char* Header::get_Comment() const{
	const char* comment = (*this).comment;
	return (*this).comment;
}

int Header::get_dimx() const{
	return (*this).dimx;
}

int Header::get_dimy() const{
	return (*this).dimy;
}

int Header::get_dimz() const{
	return (*this).dimz;
}

int Header::get_NumberGreyLevel() const{
	return (*this).number_grey_level;
}

void Header::print() const{
	cout << "magic_number : " << (*this).magic_number << endl;
	cout << "Comment : " << (*this).comment << endl;
	cout << "dimx :  " << (*this).dimx << endl;
	cout << "dimy :  " << (*this).dimy << endl;
	cout << "dimz :  " << (*this).dimz << endl;
	cout << "number grey level : " << (*this).number_grey_level << endl;
}

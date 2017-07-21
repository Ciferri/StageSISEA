/**
 * @file Header_pgm.hpp
 * @brief
 * @author Nicolas Ciferri
 * @date 05.19.17
 */

#ifndef _HEADER_PGM_H
#define _HEADER_PGM_H

#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <cassert>
#include <cstdlib>



namespace My_io_pgm{
	class Header{

	public:
		Header();
		Header(const char* filepath_pgm);
		~Header();
		const char* get_MagicNumber() const;
		const char* get_Comment() const;
		int get_dimx() const;
		int get_dimy() const;
		int get_dimz() const;
		int get_NumberGreyLevel() const;
		void print() const;


	private:
		char* magic_number;
		char* comment;
		int dimx,dimy,dimz,number_grey_level;


	};

}
#endif

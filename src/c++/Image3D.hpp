/**
 * @file Image3D.hpp
 * @brief
 * @author Nicolas Ciferri
 * @date 05.19.17
 */

#ifndef _IMAGE2D_H
#define _IMAGE2D_H

#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <vector>
#include <assert.h>
#include "prostTissue.hpp"
#include "diffusion3D.hpp"
#include "model.hpp"
#include "bmp.hpp"
#include "lodepng.h"
#include <iterator>

namespace MyImage{

	class Image3D{
		int nbx;   //!< nombre de colonnes ou voxels sur l'axe x.
		int nby;   //!< nombre de lignes ou voxels sur l'axe y.
		int nbz;	 //!< nombre de couches ou voxels sur l'axe z.
		int number_grey_level;   //!<Nombre de niveaux de gris.
		long int size;   //!< taille du volume =nbx*nby.
		double dx;   //!< taille d'un pixel en millimetres.
		double dy;   //!< taille d'un pixel en millimetres.
		double dz;   //!< taille d'un pixel en millimetres.
	vector<double> vect;

	protected:

		int*** ptr;

		/** @brief Allocation mémoire dynamique.
		 *  @param int dimx : nombre pixels axe x.
		 *  @param int dimy : nombre pixels axe y.
		 */
		void alloue_partie_dynamique(int dimx, int dimy, int dimz);
		//**********
		/** @brief Libère mémoire dynamique.
		 */
		void libere_partie_dynamique();
		//**********
		/** @brief Initialise l'image avec une valeur.
		 *  @param int valeur_init : valeur d'initialisation image.
		 */
		void init(int valeur_init);
		//**********
	public :
		/** @brief constructeur par defaut.
		 */
		Image3D();
		
		//Image3D(Coupler *coupler);
		//**********
		/** @brief constructeur.
		 *  @param int _nbx : nombre pixels axe x.
		 *  @param int _nby : nombre pixels axe y.
		 *  @param int _number_grey_level : Nombre de niveaux de gris.
		 *  @param double _dx : dimension pixel x.
		 *  @param double _dy : dimension pixel y.
		 *  @param int valeur_init : valeur de niveau de gris permettant d'initialiser l'image.
		 */
		Image3D(int _nbx, int _nby, int _nbz, double _dx, double _dy, double _dz);
		//**********
		/** @brief constructeur à partir d'une image au format pgm ascii.
		 *  @param const char* filepath_pgm : Chemin vers l'image pgm ascii.
		 *  @param double _dx : dimension pixel x.
		 *  @param double _dy : dimension pixel y.
		 */
		Image3D(const string colorMap,int _nbx, int _nby, int _nbz, double _dx, double _dy, double _dz);
		//**********
		/** @brief constructeur à partir d'une image au format pgm ascii.
		 *  @param const char* filepath_pgm : Chemin vers l'image pgm ascii.
		 *  @param double _dx : dimension pixel x.
		 *  @param double _dy : dimension pixel y.
		 */
		Image3D(const char* filepath_pgm, double _dx, double _dy, double _dz); 
		//**********
		/** @brief Constructeur.
		 *  @param const Image3D& ima : image à copier.
		 */
		Image3D( const Image3D& ima );
		//**********
		/** @brief destructeur.
		 */
		~Image3D();
		//**********
		/** @brief Retourne la valeur de nbx.
		 */
		int get_Nbx() const;
		//**********
		/** @brief Modifie nbx.
		 */
		void set_Nbx(int n);
		//**********
		/** @brief Retourne la valeur de nby.
		 */
		int get_Nby() const;
		//**********
		/** @brief Modifie nby.
		 */
		void set_Nby(int n);
		//**********
		/** @brief Retourne la valeur de nbz.
		 */
		int get_Nbz() const;
		//**********
		/** @brief Modifie nbz.
		 */
		void set_Nbz(int n);
		//**********
		/** @brief Retourne la valeur de size.
		 */
		long int get_Size() const ;
		//**********
		/** @brief Modifie la valeur de size.
		 */
		void set_Size(long int s) ;
		//**********
		/** @brief Retourne la valeur de number_grey_level.
		 */
		int get_NumberGreyLevel() const;
		//**********
		/** @brief Modifie la valeur de number_grey_level.
		 */
		void set_NumberGreyLevel(int n) ;
		//**********
		/** @brief Retourne la valeur de dx.
		 */
		double get_Dx() const;
		//**********
		/** @brief Modifie la valeur de dx.
		 */
		void set_Dx(double d) ;
		//**********
		/** @brief Retourne la valeur de dy.
		 */
		double get_Dy() const;
		//**********
		/** @brief Modifie la valeur de dy.
		 */
		void set_Dy(double d) ;
		//**********
		/** @brief Retourne la valeur de dz.
		 */
		double get_Dz() const;
		//**********
		/** @brief Modifie la valeur de dz.
		 */
		void set_Dz(double d) ;
		//**********
		/** @brief Retourne la valeur du pixel k,i,j.
		 */
		int get_PixelValue(int k,int i, int j) const ;
		//**********
		/** @brief Initialise la valeur du pixel[k][i][j] a la valeur val.
		 */
		void set_PixelValue(int k,int i, int j, int val);
		//**********
		/** @brief Retourne la valeur de l'intensité min de l'image.
		 */
		void print() const;




		int get_MinPixelValue() const;

		/** @brief Retourne la valeur de l'intensité max de l'image.
		 */
		int get_MaxPixelValue() const;

		/** @brief Surcharge de l'opérateur d'affectation.
		 *  @param const Image3D& ima : image rvalue.
		 */
		Image3D& operator= ( const Image3D& ima );

		/** @brief Surcharge de l'opérateur +=.
		 *  @param const Image3D& ima : image rvalue.
		 */
		Image3D& operator+= ( const Image3D& ima );

		/** @brief Surcharge de l'opérateur -=.
		 *  @param const Image3D& ima : image rvalue.
		 */
		Image3D& operator-= ( const Image3D& ima );

		/** @brief Surcharge de l'opérateur ==.
		 *  @param const Image3D& ima : image rvalue.
		 */
		bool operator== ( const Image3D& ima );

		/** @brief Ecrit l'image dans une image au format pgm ascii.
		 *  @param const char* filename : nom de fichier.
		 */
		void write_pgm(const char* filename) const;
		
		void write_pgm_with_index(const char* filename, const char* type, const int nb) const;



		/** @brief Est-ce que le pixel est dans l'image ?
		 *  @param int x : indice x;
		 *  @param int y : indice y;
		 */
		bool pixel_is_in_image(int x, int y) const ;

		/** @brief Interpolation au plus proche voisin. Renvoie l'indice du pixel le plus proche.
		 *  @param float x,y : position;
		 */
		int interpolation_nn(float xy) const {return static_cast<int>(floor(xy));}


		/** @brief Applique de manière uniforme une tranlation à l'image et enregistre l'image resultante dans une image de type pgm.
		 *  @param float t_x : translation en x de t_x mm.
		 *  @param float t_y : translation en y de t_y mm.
		 *  @param const char* filename : nom de fichier de l'image resultante.
		 */
		void translation(float t_x, float t_y, const char* filename) const;

		/** @brief Applique de manière uniforme une tranlation à l'image et retourne l'image translatee.
		 *  @param float t_x : translation en x de t_x mm.
		 *  @param float t_y : translation en y de t_y mm.
		 */
		Image3D translation(float t_x, float t_y) ;

		/** @brief Applique une rotation à l'image et écrit l'image dans une image pgm.
		 *  @param float theta : angle de rotation en degres;
		 */
		void rotation(float theta, const char* filename) const ;

		/** @brief Applique une rotation à l'image et retourne l'image après rotation.
		 *  @param float theta : angle de rotation en degres;
		 */
		Image3D rotation(float theta) ;

		/** @brief Seuillage d'une image. Si val >= threshold alors val = 255, sinon val = 0. Ecrit l'image dans un fichier pgm.
		 *  @param int thresh : seuil;
		 *  @param
		 */
		void threshold(int thresh, const char* filename) ;

		/** @brief Masque une image en utilisant un masque binaire (0, 255). Ecrit l'image résultante dans un fichier pgm.
		 *  @param const char* mask_filename : Nom du fichier masque binaire.
		 *  @param const char* output_filename : Nom du fichier masque binaire.
		 */
		void applyBinaryMask(const char* mask_filename, const char* output_filename) ;
				
		void printImageWithVector(vector<double>& PO2,
															vector<double>& state,
															vector<double>& Timer, 
															int nb, Image3D image);		
		
		Pixel colorBar(double val,double vmin,double vmax);
		Pixel colorMap(double val,double vmin,double vmax);
		Pixel colorBar(double val,double dv);
		
		void printImageBMP(vector<double>& PO2,
															vector<double>& state,
															vector<double>& Timer,
															vector<double>& cycle, 
															int nb, Image3D image);		
															
		void printImagePNG(vector<double>& PO2,
																	vector<double>& state,
																	vector<double>& Timer, 
																	vector<double>& cycle,
																	int nb, Image3D image);
															

	};
}

#endif

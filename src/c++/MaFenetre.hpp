/**
 * @file MaFenetre.cpp
 * @brief
 * @author Nicolas Ciferri
 * @date 05.19.17
 */

#ifndef DEF_MAFENETRE
#define DEF_MAFENETRE
 
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLCDNumber>
#include <QSlider>
#include <QProgressBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFormLayout>
#include <QHBoxLayout>
#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;
 
class MaFenetre : public QWidget
{
	public:
    MaFenetre();
    MaFenetre(int m_ncol,int m_nrow,int m_simTime);

	private:
    QSlider *m_slider;
    QLabel *image;
		int m_index;
		
	public slots :
		void indexChanged(int value);
};
 
#endif

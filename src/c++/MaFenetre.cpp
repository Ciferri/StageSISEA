/**
 * @file MaFenetre.hpp
 * @brief
 * @author Nicolas Ciferri
 * @date 05.19.17
 */

#include "MaFenetre.hpp"
using namespace std;

MaFenetre::MaFenetre() : QWidget(){
}

MaFenetre::MaFenetre(int ncol,int nrow,int simTime) : QWidget()
{

  m_ncol = ncol;
  m_nrow = nrow;
  m_simTime = simTime;

  QGridLayout *gridLayout = new QGridLayout; //Layout pour ajouter éléments

  m_slider =new QSlider(Qt::Horizontal);
  image = new QLabel(this);
  QPixmap *pixmap_img = new QPixmap("./data_out/imgState/imgState_0.png");
  
  image->setPixmap(pixmap_img->scaled(10*m_ncol,10*m_nrow,Qt::KeepAspectRatio));
  m_slider->setRange(0,m_simTime-1);
  QObject::connect(m_slider, &QSlider::valueChanged, this, &MaFenetre::indexChanged) ;
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(image,1.0);
  layout->addWidget(m_slider,1.0);

  // Création du layout principal de la fenêtre (vertical)

  QGridLayout *layoutPrincipal = new QGridLayout;
  layoutPrincipal->addLayout(gridLayout,0,3,1,1); // Ajout du layout de formulaire
  layoutPrincipal->addLayout(layout,0,0,3,3); 

  QPushButton *boutonQuitter = new QPushButton("Quitter");
  QWidget::connect(boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));
  layoutPrincipal->addWidget(boutonQuitter,3,3,1,1); // Ajout du bouton

  this->setLayout(layoutPrincipal);
}

void MaFenetre::indexChanged(int value)
{
  QString chemin = "./data_out/imgState/imgState_";
  QPixmap *pixmap_img = new QPixmap(chemin.append(QString::number(value)).append(".png"));
  image->setPixmap(pixmap_img->scaled(10*m_ncol,10*m_ncol,Qt::KeepAspectRatio));
}


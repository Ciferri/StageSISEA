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
  layoutPrincipal->addLayout(layout,0,1,12,5);

  QPushButton *boutonDiffusion = new QPushButton("Diffusion");
  layoutPrincipal->addWidget(boutonDiffusion,0,0,1,1); 
  QPushButton *boutonCycle = new QPushButton("Cycle");
  layoutPrincipal->addWidget(boutonCycle,1,0,1,1); 
  QPushButton *boutonState = new QPushButton("State");
  layoutPrincipal->addWidget(boutonState,2,0,1,1);
  
	//Cycle duration :
  QLabel *textCycleDuration = new QLabel("Cycle duration : ");
  layoutPrincipal->addWidget(textCycleDuration,0,6,1,1);
  QLineEdit *editCycleDuration = new QLineEdit("");
  layoutPrincipal->addWidget(editCycleDuration,0,7,1,3);
  QLabel *textHour = new QLabel("H");
  layoutPrincipal->addWidget(textHour,0,10,1,1);

  //Phase
  QLabel *textphase = new QLabel("Phase");
  layoutPrincipal->addWidget(textphase,1,6,1,1);
  QLabel *textAlpha = new QLabel("Alpha");
  layoutPrincipal->addWidget(textAlpha,1,7,1,1);
  QLabel *textBeta = new QLabel("Beta");
  layoutPrincipal->addWidget(textBeta,1,9,1,1);

  //G1
  QLabel *textG1 = new QLabel("G1 : ");
  layoutPrincipal->addWidget(textG1,2,6,1,1);
  QLineEdit *editG1Alpha = new QLineEdit("");
  layoutPrincipal->addWidget(editG1Alpha,2,7,1,1);
  QLabel *textGrey = new QLabel("Gy");
  layoutPrincipal->addWidget(textGrey,2,8,1,1);
  QLineEdit *editG1Beta = new QLineEdit("");
  layoutPrincipal->addWidget(editG1Beta,2,9,1,1);
  layoutPrincipal->addWidget(textGrey,2,10,1,1);

  //S
  QLabel *textS = new QLabel("S : ");
  layoutPrincipal->addWidget(textS,3,6,1,1);
  QLineEdit *editSAlpha = new QLineEdit("");
  layoutPrincipal->addWidget(editSAlpha,3,7,1,1);
  layoutPrincipal->addWidget(textGrey,3,8,1,1);
  QLineEdit *editSBeta = new QLineEdit("");
  layoutPrincipal->addWidget(editSBeta,3,9,1,1);
  layoutPrincipal->addWidget(textGrey,3,10,1,1);

  //G2
  QLabel *textG2 = new QLabel("G2 : ");
  layoutPrincipal->addWidget(textG2,4,6,1,1);
  QLineEdit *editG2Alpha = new QLineEdit("");
  layoutPrincipal->addWidget(editG2Alpha,4,7,1,1);
  layoutPrincipal->addWidget(textGrey,4,8,1,1);
  QLineEdit *editG2Beta = new QLineEdit("");
  layoutPrincipal->addWidget(editG2Beta,4,9,1,1);
  layoutPrincipal->addWidget(textGrey,4,10,1,1);

  //M
  QLabel *textM = new QLabel("M : ");
  layoutPrincipal->addWidget(textM,5,6,1,1);
  QLineEdit *editMAlpha = new QLineEdit("");
  layoutPrincipal->addWidget(editMAlpha,5,7,1,1);
  layoutPrincipal->addWidget(textGrey,5,8,1,1);
  QLineEdit *editMBeta = new QLineEdit("");
  layoutPrincipal->addWidget(editMBeta,5,9,1,1);
  layoutPrincipal->addWidget(textGrey,5,10,1,1);

  //Resorbtion time
  QLabel *textResorbtionTime = new QLabel("Resorbtion time : ");
  layoutPrincipal->addWidget(textResorbtionTime,6,6,1,1);
  QLabel *textNecrose = new QLabel("Necrose : ");
  layoutPrincipal->addWidget(textNecrose,6,7,1,1);
  QLineEdit *editNecrose = new QLineEdit("");
  layoutPrincipal->addWidget(editNecrose,6,8,1,2);
  QLabel *textApoptose = new QLabel("Apoptose : ");
  layoutPrincipal->addWidget(textApoptose,7,7,1,1);
  QLineEdit *editApoptose = new QLineEdit("");
  layoutPrincipal->addWidget(editApoptose,7,8,1,2);

  //Fraction
  QLabel *textFraction = new QLabel("Fraction : ");
  layoutPrincipal->addWidget(textFraction,9,6,1,1);
  QLineEdit *editFraction = new QLineEdit("");
  layoutPrincipal->addWidget(editFraction,9,7,1,3);
  layoutPrincipal->addWidget(textGrey,9,8,1,1);

  //Total dose
  QLabel *textTotalD = new QLabel("Total dose : ");
  layoutPrincipal->addWidget(textTotalD,10,6,1,1);
  QLineEdit *editTotalD = new QLineEdit("");
  layoutPrincipal->addWidget(editTotalD,10,7,1,3);
  layoutPrincipal->addWidget(textGrey,10,8,1,1);

  QPushButton *boutonQuitter = new QPushButton("Quit");
  QWidget::connect(boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));
  layoutPrincipal->addWidget(boutonQuitter,12,10,1,1); // Ajout du bouton
  
  QPushButton *boutonRun = new QPushButton("Run");
  //QWidget::connect(boutonRun, SIGNAL(clicked()), this, &MaFenetre::monSlot);
  layoutPrincipal->addWidget(boutonRun,12,9,1,1); // Ajout du bouton

  this->setLayout(layoutPrincipal);
}


void MaFenetre::indexChanged(int value)
{
  QString chemin = "./data_out/imgState/imgState_";
  QPixmap *pixmap_img = new QPixmap(chemin.append(QString::number(value)).append(".png"));
  image->setPixmap(pixmap_img->scaled(10*m_ncol,10*m_ncol,Qt::KeepAspectRatio));
}


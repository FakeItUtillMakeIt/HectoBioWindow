#include "HectoBioWindow.h"
#include <QtWidgets/QApplication>
#include <QBrush>
#include <QPalette>
#include <QBitmap>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HectoBioWindow w;

	w.setWindowTitle("Basecalling operater");
	w.setWindowIcon(QIcon(QStringLiteral("res/hectobio.ico")));

	w.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    

    //QPixmap pmap(":/HectoBioWindow/res/wind.png");
    //
    //QPalette plt;
    //
    //plt.setBrush(w.backgroundRole(), QBrush(pmap));
   
    //w.setPalette(plt);
    //w.setMask(pmap.mask());
    ////pmap.createMaskFromColor(Qt::color0);
    //
    //w.setAutoFillBackground(true);
	w.setStyleSheet("QPushButton{background-color: #fefefe;color:black;border:1px  groove gray;border-style:solid;border-radius:6px;padding:2px 4px}\
					QPushButton:pressed{background-color:#bbffff} QPushButton:default{backgroud-color:#000000}\
					QPushButton:flat{border:none}\
					QTextBrowser{ background-color: #fefefe;color:black; border:1px solid groove gray; border - radius:6px; padding:2px 4px }\
					QLineEdit{background-color: #fefefe; color:black; border:1px groove gray; border - radius:6px; padding:2px 4px }\
					QCheckBox{background-color: #fefefe; color:black; border:1px groove gray; border - radius:6px; padding:2px 4px }\
					QwtPlot{background-color: #eeeeee;border:gray; border - radius:50px; padding:10px 10px ;border:none;}\
					QComboBox{ color:black; border:1px solid groove gray; border - radius:6px; padding:2px 4px }\
					QMenuBar{background-color:#eeeeee; border - style: double; border - radius: 5px; color: brown; spacing:10px; }\
					QMenu{background-color:#fefefe; border - style: double; border - radius: 5px; color: brown;border:2px solid; }\
					QGroupBox{padding-top:5px;}\
					QMainWindow{border-bottom;5}\
					");



	QPalette plt;
	plt.setColor(w.backgroundRole(), QColor("#eeeeee"));
	plt.setColor(w.foregroundRole(), QColor("#fff5f7"));
	w.setPalette(plt);
    
 
    w.show();
    return a.exec();
}

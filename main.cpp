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
    
 
    w.show();
    return a.exec();
}

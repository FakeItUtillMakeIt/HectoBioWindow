
//HectoBioWindow主要函数及功能定义

void HectoBioWindow::init_circuit()//初始化AD卡设置
void HectoBioWindow::init_window()//初始化界面设置
void HectoBioWindow::on_selectAll_btn()//使能全部通道
void HectoBioWindow::on_cancelAll_btn()//取消全部通道
void HectoBioWindow::on_selectChnOK_btn()//选中界面相应通道
void HectoBioWindow::on_acqparaOK_btn()//采集参数设置
void HectoBioWindow::on_startCtnAcq_btn()//连续采集使能按钮
void HectoBioWindow::timerUpdate()//定时器
void HectoBioWindow::displayPlotZoomeIncrease()//改变上边界
void HectoBioWindow::displayPlotZoomeDecrease()//改变下边界
void HectoBioWindow::on_stopCtnAcq_btn()//停止连续采集
void HectoBioWindow::on_DA0_btn(）//DA0
void HectoBioWindow::on_DA0_btn(）//DA1
void HectoBioWindow::on_DA0_btn(）//DA2
void HectoBioWindow::on_DA0_btn(）//DA3
void HectoBioWindow::on_baselineRd_btn()//读取零偏
void HectoBioWindow::on_saveoutinfo_btn()//保存textBroswer输出信息
void HectoBioWindow::on_clearinfo_btn()//清空textBroswer信息
void HectoBioWindow::on_disp_btn()//显示按钮
void HectoBioWindow::on_earse_btn()//清除按钮
void HectoBioWindow::on_stop_btn()//暂定按钮
void HectoBioWindow::saveQVectordataAsStream(QString& save_name, QVector<double> savedata,char* mode)//保存信号格式：saveDataStream 保存为二进制模式，以C++文件方式写
void HectoBioWindow::saveDataAsStream(QString& save_name, double* savedata)//保存为二进制模式，以C++文件方式写
void HectoBioWindow::saveDataAsText(QString& save_name, double* savedata)// 保存信号，以QTTextStream文件流写
void HectoBioWindow::saveDataAsHdf5(const char* save_name, double* savedata)//  保存信号，以HDF5文件写

//on_acqparaOK_btn()
//采集函数
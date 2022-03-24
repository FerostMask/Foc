/*--------------------------------------------------------------*/
/*							头文件加载							*/
/*==============================================================*/
#include "foc.h"
#include "math.h"
#include "funcinter.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "zf_uart.h"
#include "stdio.h"
/*--------------------------------------------------------------*/
/* 							 函数声明 							*/
/*==============================================================*/
// Foc
static void focInit(struct Foc *foc, struct Driver *driver, struct Magenc *encoder);
static void transform(struct Foc *foc);
static void tempScope(float data1, float data2, float data3, const float max);
// PID
void augmentedPID(struct pidpara *para, const short tar);
void positionPID(struct pidpara *para, const short tar);

float pi;
float angle2Radian;
float valueOfSin;
/*--------------------------------------------------------------*/
/* 							 变量定义 							*/
/*==============================================================*/
Foc foc = {
    .init = focInit,
};

pidpara currentLoopQ = {
    .alpha = 0.3,
    .Kp = 0.8,
    .Ki = 0.8,
    .Kd = 0,
    .thrsod = 1,
};

pidpara currentLoopD;
/*--------------------------------------------------------------*/
/* 							 函数定义 							*/
/*==============================================================*/
/*------------------------------*/
/*		    Foc初始化    	     */
/*==============================*/
static void focInit(struct Foc *foc, struct Driver *driver, struct Magenc *encoder)
{
    foc->transform = transform;

    foc->driver = driver;
    foc->sensor = driver->sensor;
    foc->encoder = encoder;

    foc->currentA = 0;
    foc->currentB = &(driver->sensor->current2);
    foc->currentC = &(driver->sensor->current1);

    foc->voltage = &(driver->sensor->voltage);

    foc->angle = &(encoder->absAngle);

    foc->cycleGain = 7;

    foc->targetCurrent = 0;

    pi = acos(-1.f);
    angle2Radian = pi / 180;
    valueOfSin = sin(pi / 3); // sin(2*pi/3) = sin(pi/3)

    // PID初始化
    currentLoopD.alpha = currentLoopQ.alpha;
    currentLoopD.Kp = currentLoopQ.Kp;
    currentLoopD.Ki = currentLoopQ.Ki;
    currentLoopD.Kd = currentLoopQ.Kd;
    currentLoopD.thrsod = currentLoopQ.thrsod;

    currentLoopQ.act = &foc->Iq;
    currentLoopD.act = &foc->Id;
    foc->afterIq = &currentLoopQ.rs;
    foc->afterId = &currentLoopD.rs;
}
/*------------------------------*/
/*		    电流变换     	     */
/*==============================*/
static inline void clarkTransform(struct Foc *foc)
{
    foc->alpha = foc->currentA - (*foc->currentB / 2) - (*foc->currentC / 2); // cos(2*pi/3) = -0.5 | no need to consider direction
    foc->beta = valueOfSin * (*foc->currentB - *foc->currentC);

    // tempScope(foc->currentA, *foc->currentB, *foc->currentC, 2);
    // tempScope(foc->alpha, foc->beta, 0, 2);

    //	ips114_showfloat(0, 0, foc->alpha, 3, 3);
    //	ips114_showfloat(0, 1, foc->beta, 3, 3);
}

static inline void parkTransform(struct Foc *foc, const float radian)
{
    foc->Id = foc->alpha * cos(foc->cycleGain * radian) + foc->beta * sin(foc->cycleGain * radian);
    foc->Iq = -foc->alpha * sin(foc->cycleGain * radian) + foc->beta * cos(foc->cycleGain * radian);

    // tempScope(foc->Id, foc->Iq, 0, 2);

    //    ips114_showfloat(0, 0, foc->Id, 3, 3);
    //    ips114_showfloat(0, 1, foc->Iq, 3, 3);
}

static inline void reverseParkTransform(struct Foc *foc, const float radian)
{
    foc->revAlpha = *foc->afterId * cos(foc->cycleGain * radian) - *foc->afterIq * sin(foc->cycleGain * radian);
    foc->revBeta = *foc->afterId * sin(foc->cycleGain * radian) + *foc->afterIq * cos(foc->cycleGain * radian);

    // tempScope(foc->revAlpha, foc->revBeta, 0, 2);
    // ips114_showfloat(0, 0, foc->revAlpha, 3, 3);
    // ips114_showfloat(0, 1, foc->revBeta, 3, 3);
}

static void transform(struct Foc *foc)
{
    foc->sensor->sampling(foc->sensor);               // 电流电压采样
    foc->encoder->read();                             // 读取编码器位置信息
    foc->currentA = -*foc->currentB - *foc->currentC; // 计算A相电流

    float radian = foc->encoder->absAngle * angle2Radian; // 转子角度转弧度

    clarkTransform(foc);        // Clark变换
    parkTransform(foc, radian); // Park变换

    positionPID(&currentLoopQ, foc->targetCurrent); // 电流环PID计算
    positionPID(&currentLoopD, 0);

    reverseParkTransform(foc, radian); // 反Park变换
}
/*------------------------------*/
/*		    临时示波器   	     */
/*==============================*/
static void tempScope(float data1, float data2, float data3, const float max)
{
    const static int16_t length = 240;
    const static int16_t height = 135 / 2;
    static int16_t pos = 0;

    pos++;
    if (pos >= length)
    {
        ips114_clear(WHITE);
        pos = 0;
        //		foc.cycleGain = ((int)foc.cycleGain +1) % 12;
        //		ips114_showuint16(0, 7, foc.cycleGain);
    }

    data1 = (data1 / max) * height;
    data2 = (data2 / max) * height;
    data3 = (data3 / max) * height;

    ips114_drawpoint(pos, (int)data1 + height, RED);
    ips114_drawpoint(pos, (int)data2 + height, BLUE);
    ips114_drawpoint(pos, (int)data3 + height, GREEN);
}

/*----------------------*/
/*		增量PID模块		*/
/*======================*/
void augmentedPID(struct pidpara *para, const short tar)
{
    //	参数列表-> para:调定参数 | tar:目标值 | act:实际值 | value:控制量 | thrsod:阈值
    //	变量定义
    float yn;
    //	保存和计算误差
    para->e3 = para->e2;
    para->e2 = para->e1;
    para->e1 = tar - *para->act;
    yn = para->I;
    //	PID公式
    para->I = para->Ki * para->e1;
    //	一阶低通滤波（积分项
    para->I = para->alpha * para->I + (1 - para->alpha) * yn;
    para->rs += para->Kp * (para->e1 - para->e2) + para->I + para->Kd * (para->e1 - 2 * para->e2 + para->e3);
    //	阈值限定
    if (fabs(para->rs) > para->thrsod)
    {
        if (para->rs >= 0)
            para->rs = para->thrsod;
        else
            para->rs = -para->thrsod;
    }
}
/*----------------------*/
/*		位置PID模块		*/
/*======================*/
void positionPID(struct pidpara *para, const short tar)
{
    //	参数列表-> para:调定参数 | tar:目标值 | act:实际值 | max:最大控制值 | min:最小控制值
    //	保存和计算误差
    para->e2 = para->e1;
    para->e1 = tar - *para->act;
    //	PID公式
    para->rs = (para->Kp) * para->e1 + para->Kd * (para->e1 - para->e2);
    //	阈值限定
    if (fabs(para->rs) > para->thrsod)
    {
        if (para->rs >= 0)
            para->rs = para->thrsod;
        else
            para->rs = -para->thrsod;
    }
}
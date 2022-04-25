//?
//?          _    _    _   __     __   _
//?         / _  / )  / )  ) )    ) ) )_)\_) (
//?        (__( (_/  (_/  /_/    /_/ / /  /  o
//?
//?
/*------------------------------------------------------*/
/*                      头文件加载                       */
/*======================================================*/
#include "pid.h"
#include "math.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/

/*------------------------------------------------------*/
/*                       函数声明                       */
/*======================================================*/

/*------------------------------------------------------*/
/*                       变量定义                       */
/*======================================================*/
parameterPID_t *instance;
/*------------------------------------------------------*/
/*                       函数定义                       */
/*======================================================*/
static void _errorHandler(void)
{
    while (1)
        ;
}

void updateSetPoint(struct parameterPID_t *param, object_t value)
{
    param->setPoint = value; // 更新目标值
}

object_t getControllerOutput(struct parameterPID_t *param)
{
    return param->controllerOutput; // 返回PID输出
}
/*------------------------------*/
/*           构造函数           */
/*==============================*/
parameterPID_t *_parameterPID(float Kp, float Ki, float Kd, object_t threshold, object_t *PV)
{
    // 申请内存空间
    parameterPID_t *p;
    p = (parameterPID_t *)malloc(sizeof(parameterPID_t));
    if (p == 0)
    {
        _errorHandler(); // *没有申请到空间，直接用的话会炸 | 写个while(1)在这里防止程序继续执行
    }
    memset(p, 0, sizeof(parameterPID_t));
    // 参数初始化
    p->Kp = Kp;
    p->Ki = Ki;
    p->Kd = Kd;
    p->threshold = threshold;
    p->processValue = PV;
    return p;
}
/*------------------------------*/
/*           增量PID            */
/*==============================*/
void augmentedPIDTypeC(struct parameterPID_t *param)
{
    // 保存实际值、计算误差
    param->PV2 = param->PV1;
    param->PV1 = *param->processValue;
    param->e1 = param->setPoint - *param->processValue;
    // PID公式计算 | Type C
    param->controllerOutput = -param->Kp * (*param->processValue - param->PV1) + param->Ki * param->e1 - param->Kd * (*param->processValue - 2 * param->PV1 + param->PV2);
    // 阈值限制
    if (fabs(param->controllerOutput) > param->threshold)
    {
        param->controllerOutput = (param->controllerOutput > 0) ? param->threshold : -param->threshold;
    }
}

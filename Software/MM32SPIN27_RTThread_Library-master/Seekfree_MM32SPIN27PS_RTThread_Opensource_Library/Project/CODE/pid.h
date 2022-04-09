#ifndef _pid_h
#define _pid_h
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/
typedef struct parameterPID_t
{
    // 指针
    float (*getCO)();   // 获取计算结果
    void (*updateSP)(); // 更新目标值
    void (*augmPID)(struct parameterPID_t *);
    // 地址存储
    float *processValue; // 实际值
    // PID参数
    float Kp;
    float Ki;
    float Kd;
    // 计算相关
    float e1, e2, e3;
    float controllerOutput;
    float threshold; // 阈值
    float PV1;
    float PV2;
    float setPoint; // 目标值
} parameterPID_t;
/*------------------------------------------------------*/
/*                       外部声明                       */
/*======================================================*/
parameterPID_t *_parameterPID(float Kp, float Ki, float Kd, float threshold, float *PV);
parameterPID_t *_parameterPID_PIonly(float Kp, float Ki, float threshold, float *PV);

float getControllerOutput(struct parameterPID_t *param);
void updateSetPoint(struct parameterPID_t *param, float value);
// 多态
#define ParameterPID(Kp, Ki, Kd, threshold, pv) parameterPID(Kp, Ki, Kd, threshold, pv)
#define ParameterPID(Kp, Ki, threshold, pv) parameterPID_PIonly(Kp, Ki, threshold, pv)
#endif

#ifndef _pid_h
#define _pid_h
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/
typedef float object_t;

typedef struct parameterPID_t
{
    // 指针
    object_t (*getCO)(); // 获取计算结果
    // 地址存储
    object_t *processValue; // 实际值
    // PID参数
    float Kp;
    float Ki;
    float Kd;
    // 计算相关
    object_t e1, e2, e3;
    object_t controllerOutput;
    object_t threshold; // 阈值
    object_t PV1;
    object_t PV2;
    object_t setPoint; // 目标值
} parameterPID_t;
/*------------------------------------------------------*/
/*                       外部声明                       */
/*======================================================*/
parameterPID_t *_parameterPID(float Kp, float Ki, float Kd, object_t threshold, object_t *PV);
void augmentedPIDTypeC(struct parameterPID_t *param);

object_t getControllerOutput(struct parameterPID_t *param);
void updateSetPoint(struct parameterPID_t *param, object_t value);
#endif

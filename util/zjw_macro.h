#pragma once
//setteing the macro for zjw use

//debug ʱ���ר��
#define ZJW_DEUG

//��ӡ��Ϣר��
//#define ZJW_PRINT_INFO

//�Ƿ�򿪼�ʱ��
#define ZJW_TIMER

//��ʱ��
/*
�÷���
	TIME_START
	TIME_END("mesh  load time")
*/
//static innoreal::InnoRealTimer timer_zjw;
//#define TIME_START timer_zjw.TimeRestart();timer_zjw.TimeStart();
//#define TIME_END(str) 		 timer_zjw.TimeEnd();timer_zjw.TimeGap_in_ms();\
//		std::printf("%s: %f ms\n", str,timer_zjw.Time_in_ms());

//------------------������Ӧ��--------------------------

//����������˹���������ֵ������ֻ������һ
//#define USE_EIGEN
//#define USE_ARPACK

//ԭ���ó��ܴ洢������˹����������ϡ��ġ�
#define USE_SPARSE
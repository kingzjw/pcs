#pragma once
#include <stdlib.h>
//setteing the macro for zjw use

//debug ʱ���ר��
#define ZJW_DEBUG

//��ӡ��Ϣר��
//#define ZJW_PRINT_INFO
#define ZJW_INFO(a) printf("%s\n",a);
//��ӡ��Ϣ
//#define PRINT_CHEBY_COEFF

//�Ƿ�򿪼�ʱ��
#define ZJW_TIMER


//------------------������Ӧ��--------------------------

//�źŵ����͡� ��������Ǿ��Ծ��롣�����˾�����Ծ���
//#define RELATIVE_DIS_SIGNAL


//��������ֻ��ѡһ������ΪCallTGetGraph �Ĺ��캯������

//����������˹���������ֵ��
//1
//#define USE_EIGEN

//2
//#define USE_ARPACK

//3
//ԭ���ó��ܴ洢������˹����������ϡ��ġ�
#define USE_SPARSE
#define SGWT_DEBUG





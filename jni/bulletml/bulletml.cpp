// bulletml.cpp : DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g���`���܂��B
//

//#include "stdafx.h"
#include "bulletml.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


// ����̓G�N�X�|�[�g���ꂽ�ϐ��̗�ł��B
BULLETML_API int nBulletml=0;

// ����̓G�N�X�|�[�g���ꂽ�֐��̗�ł��B
BULLETML_API int fnBulletml(void)
{
	return 42;
}

// ����̓G�N�X�|�[�g���ꂽ�N���X�̃R���X�g���N�^�ł��B
// �N���X�̒�`�ɂ��Ă� bulletml.h ���Q�Ƃ��Ă��������B
CBulletml::CBulletml()
{ 
	return; 
}


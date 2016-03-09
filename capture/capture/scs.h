#ifndef __SCS_H__
#define __SCS_H__ 1

typedef struct _scs CScs;
typedef struct echat_camer_upload_info echat_camer_upload_info;

CScs* CScs_CreateInstance(IShell* pIShell, void *pParentApp);
/*
return value :	0 �����ɹ� ��
				-1 ͼƬ��Ϊ��
				-4 ��ȡͼƬ����ʧ��
				-2 �û�id��ȡʧ��
				-3 INIû������
*/
int CScs_UploadPic(CScs* pApp, const char* pic_full_path);


void CScs_CancelUpload(CScs* pApp);
void Cscs_SetUploadInfo( CScs* pApp , echat_camer_upload_info* pInfo);
void CScs_Release(CScs* pApp);
#endif


  ��������һ����ģ�����϶�λBREWӦ�ó����ڴ�й¶λ�õĹ��ߣ����Ը����ڴ�й¶λ�õĵ���ջ��Ϣ������MALLOC/REALLOC�����õ�λ�ã�Ҳ�ܶ�λ��ISHELL_CreateInstance�����õ�λ�ã���
  This program is a tool that help you positioning memory leak locations of an QUALCOMM BREW application on simulator. It can give the call stack of memory leaks (including the locations that MALLOC/REALLOC been called, and also the locations that ISHELL_CreateInstance been called).

ʹ�÷������£�

  1. ����Ӧ��copy��Simulator��Ӧ��applet dir��ʹ���������
  2. �༭�����ļ�config.ini���޸�[ClassID]����д����Ӧ��(��������Ϊtestapp)��class id
  3. ��VC����F5-Debugģʽ����Simulator����Attach��Simulator��Ӧ�Ľ��̽���Debug״̬
  4. ���б�Ӧ�ã�Ȼ������testapp(���������л�ͨ����Ӧ��ֱ������)����testapp�˳�ʱ����VC��������ڲ鿴�ڴ�й¶���棬���и�����testapp�ڴ�й¶λ�õĵ���ջ��Ϣ������˫����ת����Ӧ��Դ�ļ�

Notice:

  1. ������Ŀǰ��BREW SDK v3.1.5�Ͼ����Կ��ã�4.x�汾�����Ͽ��ã���δ����֤
  2. Brew MP SDKƽ̨�Ͼ�����Ҳ���ã�����������.dll��applet���ݲ�֧��.dll1��Ӧ�û�ʹ���½ӿ�(IEnv_ErrMalloc��IEnv_ErrRealloc��)������ڴ棬�Լ�ISHELL_CreateInstance�����Ķ�����ʹ���½ӿڷ����ڴ�
  3. ��Ӧ�������class id���������Ӧ���г�ͻ���������޸�mif�ļ�����Ϊ����idʹ��
  4. ���ʹ��VC6��������Ҫ��.dll��Ӧ��.pdb��������Ϣ���ļ�����ͬһĿ¼����applet dir������������ȷ��ʾ�����Ϣ; Visual Studio 2003�����޴�����

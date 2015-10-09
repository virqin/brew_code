  本程序是一个在模拟器上定位BREW应用程序内存泄露位置的工具，可以给出内存泄露位置的调用栈信息（包括MALLOC/REALLOC被调用的位置，也能定位到ISHELL_CreateInstance被调用的位置）。
  This program is a tool that help you positioning memory leak locations of an QUALCOMM BREW application on simulator. It can give the call stack of memory leaks (including the locations that MALLOC/REALLOC been called, and also the locations that ISHELL_CreateInstance been called).

使用方法如下：

  1. 将本应用copy至Simulator对应的applet dir，使其可以运行
  2. 编辑配置文件config.ini，修改[ClassID]项填写待测应用(假设名称为testapp)的class id
  3. 在VC中用F5-Debug模式启动Simulator，或Attach到Simulator对应的进程进入Debug状态
  4. 运行本应用，然后运行testapp(可自行运行或通过本应用直接启动)，当testapp退出时，从VC的输出窗口查看内存泄露报告，其中给出了testapp内存泄露位置的调用栈信息，可以双击跳转到对应的源文件

Notice:

  1. 本程序目前在BREW SDK v3.1.5上经测试可用，4.x版本理论上可用，但未经验证
  2. Brew MP SDK平台上经测试也可用，不过仅限于.dll的applet，暂不支持.dll1的应用或使用新接口(IEnv_ErrMalloc、IEnv_ErrRealloc等)分配的内存，以及ISHELL_CreateInstance创建的对象本身使用新接口分配内存
  3. 本应用自身的class id如果与其他应用有冲突，可自行修改mif文件，改为其他id使用
  4. 如果使用VC6开发，需要将.dll对应的.pdb（调试信息）文件放在同一目录（即applet dir），否则不能正确显示结果信息; Visual Studio 2003以上无此限制

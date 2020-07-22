编译前先运行
mailcore2\build-windows\build_headers.bat


相关依赖包下载：
链接：https://pan.baidu.com/s/1pMwcPZH
 密码：0jro


 在vs2019中遇到的问题及解决
 严重性	代码	说明	项目	文件	行	禁止显示状态
错误	C1189	#error:  <hash_map> is deprecated and will be REMOVED. Please use <unordered_map>. You can define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS to acknowledge that you have received this warning.	mailcore2	C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.24.28314\include\hash_map	22	
解决在 mailcore2项目属性->配置属性->C/C++ -> 预处理器->预处理器定义 中加入 _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

严重性	代码	说明	项目	文件	行	禁止显示状态
错误	C2011	“timespec”:“struct”类型重定义	mailcore2	F:\github\he_chengjin@outlook.com\mailcore2\Externals\include\pthread.h	320	
解决方法：
在
#if !defined( PTHREAD_H )
#define PTHREAD_H
下面加上
#define HAVE_STRUCT_TIMESPEC
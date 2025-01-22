算是凑合能用了
实现原理就是etwhook通过共享内存传递给R3  R3全部展示出来，下断点也不是真正的下断点，是ETW进行WaitForSingleObj 让R3卡死。 获取堆栈的原理是插入APC进行获取。
1.etw功能仅支持win10 18XX+  具体抄谁的懒得找了,闲了再补充
2.运行的话手动加载驱动，然后点击加载符号，选择api的配置然后运行，首次运行闪退正常，是在下载符号，后面再改
![00155d62a8d89faac379f9d532096e35](https://github.com/user-attachments/assets/a6d6dc24-8457-4b59-bf7c-f00c0dfed07d)

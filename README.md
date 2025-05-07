# 模拟原神多人抽卡程序
**纯Python实现武器池抽卡。[看源码](原神多人抽卡模拟器.py)**  
**Python与C++共同链接实现角色池抽卡。[看Py源码](PyGachaSystem.py)|[看C++源码](GenshinImpact.cpp)**
***
## 说明
* 1.本项目中使用的UI界面，如PyQt5需要遵循GPLv3协议
* 2.纯Python实现的武器池抽卡模拟，Py与C++链接实现的角色池抽卡模拟
  * 纯Python实现的抽取生成情况是我在模拟3亿次后得出的，基本上99%与原游戏相同！
  * Py与C++链接实现的角色池抽卡只是我为了学习验证制作，所以概率可能并不完美还原！
***
## 移植注意事项
武器池实现的纯Py代码可以直接配置好解释器环境后直接使用  
角色池实现的Py与C++链接需要注意GachaSystem.dll，需放置在源码相同目录下！  
***
## 其他描述
### 关于打包命令
打包项目中所有Py文件均使用`Nuitka`打包，以小幅度提升性能和速度  
1、打包 `原神多人抽卡模拟器.py` 使用命令  
```
python -m nuitka --standalone --onefile --remove-output --windows-console-mode="disable" --enable-plugins="pyqt5" --output-filename="原神模拟多人抽卡器.exe" --main="原神多人抽卡模拟器.py" --windows-icon-from-ico="icon.ico" 
```
**命令讲解**  
1.基础调用Nuitka命令 `python -m nuitka`  
2.独立模拟`--standalone`，此命令可以使打包后的程序能在无Python环境中运行  
3.单文件模式`--onefile`，此命令可以使创建出的并不是依赖文件夹+可执行文件，而是被压缩的可执行文件，可以美观目录结构和隐藏dll与其他依赖暴露  
4.删除构建文件夹`--remove-output`，可以使Nuitka在打包完成后删除构建可执行文件时使用的构建文件夹  
5.控制台模式`--windows-console-mode`，这条命令会使打包后的可执行文件在Windows操作系统中运行后禁用终端控制台  
6.启用插件支持`--enable-plugins="pyqt5"`，使用此命令才能支持打包使用了PyQt5的Python文件为可执行文件  
7.生成可执行文件命名`--output-filename=".exe"`，使用此命令打包为可执行文件时就不会因无文件名而报错  
8.程序入口`--main=""`，表示要打包的主文件入口  
9.打包程序图标`--windows-icon-from-ico=""`，指定路径后打包相关的ico图标为最后生成的可执行文件图标  

2、打包 `PyGachaSystem.py` 使用命令  
```
python -m nuitka --standalone --onefile --remove-output --windows-console-mode="disable" --enable-plugins="pyqt5" --output-filename="UP多人模拟抽卡器.exe" --main="PyGachaSystem.py" --windows-icon-from-ico="icon.ico"
```
**与上述打包Py文件命令讲解类似**  
但在类似命令结尾添加新的命令，此命令以支持打包指定路径下的dll  
`--include-data-file="GachaSystem.dll=GachaSystem.dll"`

3、编译 `GenshinImpact.cpp` 为动态链接dll
```
x86_64-w64-mingw32-g++ -shared -o GachaSystem.dll GenshinImpact.cpp -static-libgcc -static-libstdc++ -std=c++11 -lwinmm
```

4、因为技术原因导致的角色抽卡不还原  
技术力尚有提升空间，所以角色池抽并未还原捕获明光机制，但是为了保证拥有45%不歪的概率所以参考了崩铁中的抽卡机制  
在歪了可以后通过抽取添加了UP角色的常驻列表，这样就能实现在歪了之后还有一定的概率不歪  
但是也有缺点，如果常驻角色增加则会导致UP概率稀释，最终无限接近回50%  

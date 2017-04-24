# CrystalEngine

## 简介
  一个简单的3d游戏引擎。目前只有物理引擎部分和简单的图形显示功能。
### 物理引擎
    物理引擎的实现参考了《Game Physics Engine Development》（Ian Millington著）一书。
    实现的功能包括粒子系统，刚体的受力运动和旋转，基本形状的碰撞检测和碰撞处理。
### 图形显示
     图形的显示部分使用了OpenGL Core（3.3版）实现。
     目前能绘制基本的形状，导入纹理及模型，以及简单的平行光（Direction Light）的光照效果。
## 使用
### 环境配置
   目前仅支持Visual Studio进行构建，以下说明均以Visual Studio 2015 Community版本为例
* 添加库

   工程所需的库文件均在lib文件夹中，不过需要手动添加到解决方案中。<br>
   首先在项目属性的VC++目录中的“库目录”中添加`lib`文件夹的路径<br>
   再在链接器输入的依赖项中添加以下几项：`opengl32.lib`,`glew32s.lib`,`glfw3dll.lib`,`glfw3.lib`,`SOIL.lib`,`assimp-vc140-mt.lib`<br>
    
* 引用目录

  工程用到的头文件均在include文件夹中。将其路径添加到引用路径中即可。
* 注意事项

  1.请确认您的显卡上支持Opengl 3.3版本；<br>
  2.使用的库均为32位，请确认目标平台正确；<br>
  3.如果在运行时出现`缺少glfw32.dll`的提示，请将lib文件夹中的该文件放到您生成的程序所在目录中<br>
  
### 应用编写
  您可以参照`apps/blockshooter.cpp`,`apps/treePlant`的例子编写您自己的游戏程序。

## 已知问题
* 当场景中碰撞体数目较多时，程序可能崩溃
* 物体相对静止接触时可能会发生上下“抖动”。这是由于使用了冲量来处理静止碰撞造成的
* 倾斜的物体碰撞后的旋转效果可能失真
* 缺乏足够的测试
* `SOIL.lib`,`assimp-vc140-mt.lib`可能需要重新编译

## 示例图片
![blockShooter](https://github.com/lvbaoq/CrystalEngine/raw/master/sampleImage/bs1.PNG)  
![treePlant](https://github.com/lvbaoq/CrystalEngine/raw/master/sampleImage/treePlant.PNG)  
----------

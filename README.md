<div align="center">

# 提瓦特大冒险

派蒙与野猪的小游戏练习，从一个文件理解动画、移动与碰撞。

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=flat-square) ![EasyX](https://img.shields.io/badge/EasyX-16a34a?style=flat-square) ![入门游戏](https://img.shields.io/badge/%E5%85%A5%E9%97%A8%E6%B8%B8%E6%88%8F-7c3aed?style=flat-square)

</div>

---

<p align="center"><a href="https://github.com/wek486/Adventure-in-Teyvat/tree/master">完整源码与资源 · master 分支</a></p>

## 玩法与学习内容

- 使用方向键控制角色，在场景中移动并应对敌人。
- 练习角色朝向、逐帧动画、敌人追踪与受伤反馈。
- 通过图集、动画类和角色类，了解游戏对象如何组织。
- 配合背景音乐与音效，串起完整的游戏主循环。

## 资源预览

下方图片直接引用本仓库已有资源，用于展示项目中的场景和角色，属于素材预览。角色、美术和音乐的权利归各自权利人所有；仓库提供素材不代表授予额外使用许可。


<p align="center"><img src="https://raw.githubusercontent.com/wek486/Adventure-in-Teyvat/master/%E6%8F%90%E7%93%A6%E7%89%B9%E5%B9%B8%E5%AD%98%E8%80%85/img/menu.png" alt="项目菜单素材" width="760"></p>
<p align="center"><img src="https://raw.githubusercontent.com/wek486/Adventure-in-Teyvat/master/%E6%8F%90%E7%93%A6%E7%89%B9%E5%B9%B8%E5%AD%98%E8%80%85/img/player_right_0.png" alt="玩家角色动画首帧素材" width="100">&nbsp;&nbsp;<img src="https://raw.githubusercontent.com/wek486/Adventure-in-Teyvat/master/%E6%8F%90%E7%93%A6%E7%89%B9%E5%B9%B8%E5%AD%98%E8%80%85/img/enemy_left_0.png" alt="敌人角色动画首帧素材" width="100"></p>

## 本地运行

环境：Windows、Visual Studio C++ 桌面开发工具、与目标架构匹配的 EasyX。工程使用的工具集以 `.vcxproj` 为准；本机版本不同可在项目属性中重定向。

```sh
git clone --branch master https://github.com/wek486/Adventure-in-Teyvat.git
cd Adventure-in-Teyvat
```

1. 打开 `yuanshen.slnx`，选择相应平台和编译配置。
2. 确认 EasyX 的头文件与库已配置，再生成项目。
3. 运行时保留 `提瓦特幸存者/img/` 和 `提瓦特幸存者/mus/`，工作目录指向仓库根目录。

以上是按仓库代码整理的运行步骤，未在此次文档整理中完成编译验证。

## 代码导览

| 文件 / 目录 | 内容 |
| --- | --- |
| `yuanshen.cpp` | 图集、动画、玩家与游戏主流程 |
| `提瓦特幸存者/img/` | 背景、角色动画与按钮 |
| `提瓦特幸存者/mus/` | 背景音乐与命中音效 |

阅读时可先追踪方向键输入如何改变坐标，再看动画与绘图如何使用角色状态。想继续练习模块拆分，可以参考 [植物大战双人版](https://github.com/wek486/plant_war_duo)。

---

如果你也在学习这个方向，欢迎通过 Issues 交流思路、反馈问题。遇到不理解的实现，可以结合代码与 AI 提问，先理解，再动手修改。

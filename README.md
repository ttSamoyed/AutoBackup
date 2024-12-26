# AutoBackup
## 简介
AutoBackup是我们为软工实验课程项目开发的软件，主要包括如下功能：
1. 备份文件
2. 还原文件
3. 自定义备份 
4. 加密文件
5. 解密文件 
6. 定时备份 
7. 实时备份  
8. 退出软件

## 安装CMake

在构建项目之前，请确保你已经安装了CMake。你可以通过以下方式安装CMake：

- **Windows**: 从[CMake官网](https://cmake.org/download/)下载并安装。
- **macOS**: 使用Homebrew安装：
    ```sh
    brew install cmake
    ```
- **Linux**: 使用包管理器安装，例如在Debian/Ubuntu上：
    ```sh
    sudo apt-get install cmake
    ```

## 构建项目

1. 创建构建目录：

    ```sh
    mkdir build
    cd build
    ```

2. 生成构建文件：

    ```sh
    cmake ..
    ```

3. 编译项目：

    ```sh
    make
    ```
    如果编译器为mingw，则使用：
    ```sh
    mingw32-make
    ```

## 运行项目

编译完成后，你可以运行生成的可执行文件：

```sh
./AutoBackup
```
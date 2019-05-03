# OJ-CLI
一个便捷的命令行OJ交题程序

### 用法
```bash
oj-cli [oj] [problemid] [file]
```

`oj` : `uoj/loj/darkbzoj` 无需考虑大小写问题      
`problemid` : 问题的ID，不带任何符号，请确定是您在`URL`中看到的ID         
`file` : 代码文件，为了供您实验，我已经在项目根目录放了一份[A + B Problem](./apb.cpp) 

代码可以自行编译，但您需确保自己的`curl`不出差错。    
```bash
g++ main.cpp -o oj-cli #Linux

g++ main.cpp -o oj-cli.exe #Windows
```

### 目前支持的 OJ

UOJ : 登录，交题        
LOJ : 登录，交题，获取 Submission 号              
DarkBZOJ : 登录，交题          
BZOJ : 因为没有权限号，暂时咕掉          


### 使用时需要准备啥

- `curl` with `OpenSSL` support
  - 如果您是 `Windows` 系统的话，我给您准备好了一个即开即用的编译好了的`curl`，您如果不放心也可以自己编译一个。
- `UOJ`系 OJ 的密钥目前需要**自行拼接**，详见[Wiki](https://git.io/fjZdT)


### 未来计划


[ ] 加入搜索题目功能
[ ] 无输入数据时加入用户交互功能
[ ] 将命令行模式的`curl`改为`libcurl`
[ ] 修改目前 UOJ 的请求模式
[ ] 将 `HmacMD5` 的计算放到本地


### 最后

本项目是本人现学先写，难免会有些意想不到的问题，敬请各位一起来改进。            

本项目的写成参考了网络上的一些项目，具体信息已经在注释中注明，在此一并感谢。
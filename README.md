

<h2>功能介绍</h2>

您可以把用户 exe 或其他窗体的屏幕录制下来，然后作为视频文件用于回看。适用于记录在线定损、互动课堂、远程庭审等场景。



## 开启录制

录制回看功能依托于腾讯云的**点播服务**支撑，如果您想要对接这个功能，首先需要在腾讯云的管理控制台[开通点播服务](http://console.cloud.tencent.com/video)。服务开通之后，新录制的文件就可以在点播控制台的 [视频管理](http://console.cloud.tencent.com/video/videolist) 里找到它们。

那么怎么开启屏幕录制呢？您可以简单地使用参数使用我们的录制小工具：

### 1. 配置工程

点击 [GitHub](https://github.com/TencentVideoCloudMLVBDev/exe_screen_record) 下载 TXCloudRecord 源代码，将 RecordSDK 放置于您的代码目录中，如下图：

![](https://main.qcloudimg.com/raw/2b9138f0a2953b6d412a43dfe4602fd0.png)

1）在您的附加包含目录中添加如下字段：

```
$(ProjectDir)RecordSDK\TXCloudRecord\include
$(ProjectDir)RecordSDK\jsoncpp
```

如下图：

![](https://main.qcloudimg.com/raw/b5b2e15a358cf5f31dd04444dad24db5.png) 

2）在工程文件中添加如下内容：

![](https://main.qcloudimg.com/raw/294f75e4a964d49b1659b9be26c4a42e.png)

3）将RecordSDK\dll中的文件拷贝到您的 exe 运行目录下，如图：

![](https://main.qcloudimg.com/raw/af44b40d600ff11a4ae68e1fbc272420.png)

这里有几点要特别说明：

- 目前录制功能默认使用的视频封装格式为 mp4（播放、下载及传输比较方便）。
- mp4 文件默认分片时长为2小时，只要中途不断流，2小时内的直播内容都将录制在同一个 mp4 文件中。若中途停止推流，以实际推流时长为准。



### 2.使用录屏接口

<h2 id = "API">接口列表</h2>

| 成员函数                          | 功能说明           |
| ----------------------------- | -------------- |
| [runAndRecord](#runAndRecord) | 启动录屏 exe 并开始录制 |
| [start](#start)               | 继续进行录屏         |
| [stop](#stop)                 | 暂时停止录屏         |
| [exit](#exit)                 | 退出录屏 exe 并结束录屏 |

<h2 id="runAndRecord">runAndRecord</h2>

- 接口定义：bool runAndRecord(ScreenRecordType recordType, std::string recordUrl, std::string recordExe, int winID = -1)
- 接口说明：启动录屏 exe 并开始录制
- 参数说明：

```c++
{
	recordType			     ScreenRecordType   录屏方式。暂时只支持RecordScreenToServer，即录制文件至云端
	recordUrl		         string             推流地址
	recordExe		         string             需要录制的exe名称
	winID        			 int                需要录制的窗口句柄
}
```

- 注意事项：

  1）recordUrl 为推流地址，可查阅  [doc](https://cloud.tencent.com/document/product/454/14551#URL) 里进行获取。

  2）recordExe 及 winID 只需要传入一个参数即可，若recordExe 不为空，则默认录屏 recordExe 指代的 exe 屏幕

- 返回值说明：启动是否成功

- 示例代码：

```c++
std::string url = "xxx";
TXCloudRecordCmd::instance().runAndRecord(RecordScreenToServer, url, "chrome.exe");
```



<h2 id="start">start</h2>

- 接口定义：void start()
- 接口说明：继续进行录屏


- 示例代码：

```c++
TXCloudRecordCmd::instance().start();
```



<h2 id="stop">stop</h2>

- 接口定义：void stop()
- 接口说明：暂时停止录屏


- 示例代码：

```c++
TXCloudRecordCmd::instance().stop();
```



<h2 id="start">exit</h2>

- 接口定义：void exit()
- 接口说明：退出录屏 exe 并结束录屏


- 示例代码：

```c++
TXCloudRecordCmd::instance().exit();
```



## 获取文件

一个新的录制视频文件生成后，会相应的生成一个观看地址，您可以结合自己的业务场景实现很多的扩展功能，比如：您可以将其追加到历史信息里，作为资料进行存档；或者将其放入回放列表中，经过专门的人工筛选，将优质的视频推荐给您的 App 用户。

那么怎样才能拿到文件的地址呢？有如下三种解决方案：

### 1. 被动监听通知

您可以使用腾讯云的**[事件通知服务](https://cloud.tencent.com/document/product/267/5957)**：您的服务器注册一个自己的**回调URL**给腾讯云，腾讯云会在一个新的录制文件生成时通过这个URL通知给您。

![](https://main.qcloudimg.com/raw/b50c901fb4d529daf3405e78bc69908d.png)

如下是一个典型的通知消息，它的含义是：一个id为9192487266581821586的新的flv录制文件已经生成，播放地址为：`http://200025724.vod.myqcloud.com/200025724_ac92b781a22c4a3e937c9e61c2624af7.f0.flv`。

```json
{
    "channel_id": "2121_15919131751",
    "end_time": 1473125627,
    "event_type": 100,
    "file_format": "flv",
    "file_id": "9192487266581821586",
    "file_size": 9749353,
    "sign": "fef79a097458ed80b5f5574cbc13e1fd",
    "start_time": 1473135647,
    "stream_id": "2121_15919131751",
    "t": 1473126233,
    "video_id": "200025724_ac92b781a22c4a3e937c9e61c2624af7",
    "video_url":        "http://200025724.vod.myqcloud.com/200025724_ac92b781a22c4a3e937c9e61c2624af7.f0.flv"
}
```

> 注意：来自 APP 客户端的时间信息很重要，如果您希望定义这段时间内的录制文件都属于同一次直播，那么只需要用直播码和时间信息检索收到的录制通知即可（每一条录制通知事件都会携带stream_id、start_time 和 end_time 等信息）。

### 2. 主动查询文件

您可以通过腾讯云的文件查询接口（**[Live_Tape_GetFilelist](https://cloud.tencent.com/document/product/267/5960)**）定时地查看是否有新的录制文件生成，不过这种方案在要查询的频道数特别多的时候，响应速度不理想，同时调用频率也不能太快（仅对刚结束的频道进行调用为宜），这种方案的实时性和可靠性不高，并不推荐频繁使用。

### 3. 查看视频管理中的文件

在点播控制台的 [视频管理](http://console.cloud.tencent.com/video/videolist) 里可以查询到所有的录制视频，适用于后续检索文件内容。可根据前缀搜索查询指定视频，如下图：

![](https://main.qcloudimg.com/raw/d1fe5bf5aecf7e3ee32fb2516e1029b6.png)

选择视频列表中对应文件后，可以在右侧看到具体视频内容，切换 tab 页至视频发布，点击选择显示源地址，即可得到文件地址。如下图：

![](https://main.qcloudimg.com/raw/a7b1e0543b222b8eaa028433a2527a33.png)



## 常见问题

### 1. 直播录制的原理是什么？

![](https://main.qcloudimg.com/raw/cbb2aae6b5e767db1d30cb51d147948d.png)

对于一条直播流，一旦开启录制，音视频数据就会被旁路到录制系统。主播的手机推上来的每一帧数据，都会被录制系统追加写入到录制文件中。

一旦直播流断中断，接入层会 立刻 通知录制服务器将正在写入的文件落地，将其转存到点播系统中，并为其生成索引，这样您在点播系统中就会看到这个新生成的录制文件了。同时，如果您配置了录制事件通知，录制系统会将该文件的 **索引ID** 和 **在线播放地址** 等信息通知给您之前配置的服务器上。

### 2. 一次直播会有几个录制文件？

- 如果一次直播过程非常短暂，比如只有不到 1 秒钟时间，那么可能是没有录制文件的。
- 如果一次直播时间不算长——小于7天（hls 格式）或者小于 mp4、flv 格式的分片时长，且中途没有推流中断的事情发生，那么通常只有一个文件。
- 如果一次直播时间很长——超过7天（hls 格式）或者超过 mp4、flv 格式的分片时长，那么会强制进行分片，分片的原因是避免过长的文件在分布式系统中流转的时间不确定性。
- 如果一次直播过程中发生推流中断（之后 SDK 会尝试重新推流），那么每次中断均会产生一个新的分片。

### 3. 如何把碎片拼接起来？

目前腾讯云支持使用云端 API 接口拼接视频分片，API 详细用法可以参考 [视频拼接](/document/product/266/7821) 。
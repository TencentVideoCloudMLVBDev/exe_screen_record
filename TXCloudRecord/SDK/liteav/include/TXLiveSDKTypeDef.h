#ifndef __TX_LIVE_SDK_TYPE_DEF_H__
#define __TX_LIVE_SDK_TYPE_DEF_H__

/**
* \brief：音频数据源类型
*/
enum TXEAudioCaptureSrcType
{
	TXE_AUDIO_SRC_SDK_DATA = 1,				//音频采集源来自LiteAvSDK的包括本地麦克风，本地播放器、系统等声音
	TXE_AUDIO_SRC_USER_PCM = 1001,				//音频采集源来自用户传入的PCM裸音频的数据
	TXE_AUDIO_SRC_USER_AAC = 1002,				//音频采集源来自用户传入AAC音频编码数据
	//TXE_AUDIO_SRC_USER_PCM_MIX_SDK = 1003,//[暂未支持]音频采集源来自用户传入PCM裸音频的数据+本地音频数据的混音
	//TXE_AUDIO_SRC_USER_AAC_MIX_SDK = 1004,//[暂未支持]音频采集源来自用户传入AAC音频编码数据+本地音频数据的混音
};

/**
* \brief：视频数据源类型
*/
enum TXEVideoCaptureSrcType
{
	TXE_VIDEO_SRC_UNDEFINED = 0,		//无视频采集源
	TXE_VIDEO_SRC_SDK_CAMERA = 1,		//视频采集源来自LiteAvSDK地摄像头数据
	TXE_VIDEO_SRC_SDK_SCREEN = 2,		//视频采集源来自LiteAvSDK屏幕录屏数据
	TXE_VIDEO_SRC_USER_DATA = 1001,	//视频采集源来自用户传入的视频数据，
};

/**
* \brief：用户导入的视频数据仅提供几种比较常见的场景编码参数
* \TXEVideoCaptureSrcType == TXE_VIDEO_SRC_USER_DATA时，此参数生效。
*/
enum TXEVideoUserDataFormat
{
	TXE_USER_DATA_FORMAT_UNDEFINED = 0,		//屏蔽用户输入的所有视频数据，
	TXE_USER_DATA_FORMAT_YUV420P,			//TXE_VIDEO_SRC_USER_DATA传入的数据是Yuv420P的视频数据
	TXE_USER_DATA_FORMAT_H264NAL,			//TXE_VIDEO_SRC_USER_DATA传入的数据是H264Nal的视频编码数据
};

/**
* \brief：推流视频比例
*/
enum TXEVideoRatio
{
	TXE_VIDEO_RATIO_4_3 = 0,
	TXE_VIDEO_RATIO_16_9 = 1,
	TXE_VIDEO_RATIO_9_16 = 2,
};

/**
* \brief：推流视频分辨率
*/
enum TXEVideoResolution
{
    // 普屏 4:3
    TXE_VIDEO_RESOLUTION_320x240 = 1,
	TXE_VIDEO_RESOLUTION_480x360 = 2,
    TXE_VIDEO_RESOLUTION_640x480 = 3,
	TXE_VIDEO_RESOLUTION_960x720 = 4,
	TXE_VIDEO_RESOLUTION_1280x960 = 5,

	// 普屏 3:4
	TXE_VIDEO_RESOLUTION_240x320 = 50,
	TXE_VIDEO_RESOLUTION_360x480 = 51,
	TXE_VIDEO_RESOLUTION_480x640 = 52,
	TXE_VIDEO_RESOLUTION_720x960 = 53,
	TXE_VIDEO_RESOLUTION_960x1280 = 54,

    // 宽屏16:9
	TXE_VIDEO_RESOLUTION_320x180 = 101,
    TXE_VIDEO_RESOLUTION_480x272 = 102,
    TXE_VIDEO_RESOLUTION_640x360 = 103,
	TXE_VIDEO_RESOLUTION_960x540 = 104,
	TXE_VIDEO_RESOLUTION_1280x720 = 105,

	// 宽屏9:16
	TXE_VIDEO_RESOLUTION_180x320 = 201,
	TXE_VIDEO_RESOLUTION_272x480 = 202,
	TXE_VIDEO_RESOLUTION_360x640 = 203,
	TXE_VIDEO_RESOLUTION_540x960 = 204,
	TXE_VIDEO_RESOLUTION_720x1280 = 205,
};

/**
* \brief：目前SDK支持的画面渲染模式，两种模式均会严格保持画面的原始宽高比
*/
enum TXERenderMode
{
    TXE_RENDER_MODE_ADAPT = 1,              // 适应，此模式下会显示整个画面的全部内容，但可能有黑边的存在
    TXE_RENDER_MODE_FILLSCREEN = 2,         // 填充，此模式下画面无黑边，但是会裁剪掉一部分超出渲染区域的部分，裁剪模式为居中裁剪
};

/**
* \brief：目前SDK支持的画面顺时针旋转角度
*/
enum TXEVideoRotation
{
    TXE_VIDEO_ROTATION_NONE = 1,            // 保持原图像的角度
    TXE_VIDEO_ROTATION_90 = 2,              // 顺时针旋转90度，最终图像的宽度和高度互换
    TXE_VIDEO_ROTATION_180 = 3,             // 顺时针旋转180度，最终图像颠倒
    TXE_VIDEO_ROTATION_270 = 4,             // 顺时针旋转270度，最终图像的宽度和高度互换
};

/**
* \brief：目前SDK支持的流控策略
*/
enum TXEAutoAdjustStrategy
{
	TXE_AUTO_ADJUST_NONE = -1,  ///< 无流控，恒定使用 setVideoBitRate 指定的视频码率
	TXE_AUTO_ADJUST_LIVEPUSH_STRATEGY = 0,  ///< 适用于普通直播推流的流控策略，该策略敏感度比较低，会缓慢适应带宽变化，有利于在带宽波动时保持画面的清晰度。
	TXE_AUTO_ADJUST_LIVEPUSH_RESOLUTION_STRATEGY = 1,  ///< 适用于普通直播推流的流控策略，是对 LIVEPUSH_STRATEGY 的升级版本，差别是该模式下 SDK 会根据当前码率自动调整出适合的分辨率
	TXE_AUTO_ADJUST_REALTIME_VIDEOCHAT_STRATEGY = 5,  ///< 适用于实时音视频通话的流控策略，也就是 VIDEO_QUALITY_REALTIME_VIDEOCHAT 所使用流控策略,                           
													  ///<该策略敏感度比较高，网络稍有风吹草动就会进行自适应调整
};


/**
*  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\  推流的画面质量预设选项  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*
*  - 1.2.0 版本开始引入推流画质接口 setVideoQualityParamPreset 用于傻瓜化的选择推流画质效果。
*  - TXLivePush::setVideoQualityParamPreset 内部通过预先配置不同的视频参数调整到对应的质量
*  - 目前支持的几种画质选项如下：
*
*  （1）标清 - 采用 640 * 360 级别分辨率，码率会在 400kbps - 800kbps 之间自适应，如果主播的网络条件不理想，
*              直播的画质会偏模糊，但总体卡顿率不会太高。
*              Android平台下这一档我们会选择采用软编码，软编码虽然更加耗电，但在运动画面的表现要优于硬编码。
*
*  （2）高清 - 采用 960 * 540 级别分辨率，码率会锁定在 1200kbps，如果主播的网络条件不理想，直播画质不会有变化，
*              但这段时间内会出现频繁的卡顿和跳帧。 两个平台下，这一档我们都会采用硬编码。
*
*  （3）超清 - 采用 1280 * 720 级别分辨率，码率会锁定在 1500kbps，对主播的上行带宽要求比较高，适合观看端是大屏的业务场景。
*
*  （4）大主播 - 顾名思义，连麦中大主播使用，因为是观众的主画面，追求清晰一些的效果，所以分辨率会优先选择 960 * 540。
*
*  （5）小主播 - 顾名思义，连麦中小主播使用，因为是小画面，画面追求流畅，分辨率采用 480 * 320， 码率 350kbps 固定。
*
*  （6）实时音视频通话 - 时延低，小画面，画面追求流畅，分辨率采用 480 * 320， 码率 350kbps 固定。
*
*  （7）静态画质场景   - 画面变动小，能保证非常低的码率下，视频相对清晰，如截屏正在播放的ppt，1080p，400kb左右码率。
* 【特别说明】
*  1. 使用 setVideoQualityParamPreset 之后，依然可以使用setVideoBitRate、setVideoFPS、setAutoAdjustStrategy等设置自定义画质，以最后一次的设置为准。
*/
/**
* \brief：SDK推流画质预设选项
*/
enum TXEVideoQualityParamPreset
{
	TXE_VIDEO_QUALITY_STANDARD_DEFINITION = 1,      //标清：建议追求流畅性的客户使用该选项
	TXE_VIDEO_QUALITY_HIGH_DEFINITION = 2,			//高清：建议对清晰度有要求的客户使用该选项
	TXE_VIDEO_QUALITY_SUPER_DEFINITION = 3,			//超清：如果不是大屏观看，不推荐使用
	TXE_VIDEO_QUALITY_LINKMIC_MAIN_PUBLISHER = 4,   //连麦大主播
	TXE_VIDEO_QUALITY_LINKMIC_SUB_PUBLISHER = 5,    //连麦小主播
	TXE_VIDEO_QUALITY_REALTIME_VIDEOCHAT = 6,       //实时音视频
	TXE_VIDEO_QUALITY_STILLIMAGE_DEFINITION = 7,	//静态画质场景，一般用户截屏推流，画面动态变化比较小。内部有自适应码率和分辨率算法，不建议做自定义设置。
};

/**
* \brief：设置输出的视频格式
*/
enum TXEOutputVideoFormat
{
    TXE_OUTPUT_VIDEO_WITHOUT_OUTPUT = 1,        // 不输出数据
    TXE_OUTPUT_VIDEO_FORMAT_YUV420 = 2,         // yuv420格式
    TXE_OUTPUT_VIDEO_FORMAT_RGBA = 3,           // RBGA格式
};

/**
* \brief：设置美颜风格
*/
enum TXEBeautyStyle
{
	TXE_BEAUTY_STYLE_SMOOTH = 0,        // 光滑
	TXE_BEAUTY_STYLE_NATURE = 1,        // 自然
	TXE_BEAUTY_STYLE_BLUR   = 2,        // 朦胧
};

/**
* \brief：设置播放类型
*/
enum TXEPlayType
{
    PLAY_TYPE_LIVE_RTMP = 0,          // RTMP直播
    //PLAY_TYPE_LIVE_FLV,               // FLV直播
    //PLAY_TYPE_VOD_FLV,                // FLV点播
    //PLAY_TYPE_VOD_HLS,                // HLS点播
    //PLAY_TYPE_VOD_MP4,                // MP4点播
    PLAY_TYPE_LIVE_RTMP_ACC,          // RTMP直播加速播放
    //PLAY_TYPE_LOCAL_VIDEO,            // 本地视频文件
};

enum TXEH264NalPacketType
{
	TXE_H264_NAL_TYPE_NONE = -1,		 //此种数据内部会丢弃
	TXE_H264_NAL_TYPE_IFRAME = 0,        //I帧数据
	TXE_H264_NAL_TYPE_PFRAME = 1,        //P帧数据
	TXE_H264_NAL_TYPE_BFRAME = 2,        //B帧数据
	TXE_H264_NAL_TYPE_SPS = 3,        //SPS数据
	TXE_H264_NAL_TYPE_PPS = 4,        //PPS数据
};



//回调状态定义 ,目前提供视频速度、音频速度、帧率、视频缓冲、音频缓冲、视频大小、服务器地址信息
#define TXE_STATUS_UPLOAD_EVENT         200001  //推流相关数据
#define TXE_STATUS_DOWNLOAD_EVENT       200002  //拉流相关数据

// 状态键名定义
#define NET_STATUS_CPU_USAGE         "CPU_USAGE"        // cpu使用率
#define NET_STATUS_CPU_USAGE_D       "CPU_USAGE_DEVICE" // 设备总CPU占用
#define NET_STATUS_VIDEO_BITRATE     "VIDEO_BITRATE"    // 当前视频编码器输出的比特率，也就是编码器每秒生产了多少视频数据，单位 kbps
#define NET_STATUS_AUDIO_BITRATE     "AUDIO_BITRATE"    // 当前音频编码器输出的比特率，也就是编码器每秒生产了多少音频数据，单位 kbps
#define NET_STATUS_VIDEO_FPS         "VIDEO_FPS"        // 当前视频帧率，也就是视频编码器每条生产了多少帧画面
#define NET_STATUS_VIDEO_GOP         "VIDEO_GOP"        // 当前视频I帧间隔 ，也就是视频编码器每个I帧之间的间隔，单位s
#define NET_STATUS_NET_SPEED         "NET_SPEED"        // 当前的发送速度
#define NET_STATUS_NET_JITTER        "NET_JITTER"       // 网络抖动情况，抖动越大，网络越不稳定
#define NET_STATUS_CACHE_SIZE        "CACHE_SIZE"       // 缓冲区大小，缓冲区越大，说明当前上行带宽不足以消费掉已经生产的视频数据
#define NET_STATUS_DROP_SIZE         "DROP_SIZE"
#define NET_STATUS_VIDEO_WIDTH       "VIDEO_WIDTH"
#define NET_STATUS_VIDEO_HEIGHT      "VIDEO_HEIGHT"
#define NET_STATUS_SERVER_IP         "SERVER_IP"
#define NET_STATUS_CODEC_CACHE       "CODEC_CACHE"      //编解码缓冲大小
#define NET_STATUS_CODEC_DROP_CNT    "CODEC_DROP_CNT"   //编解码队列DROPCNT
#define NET_STATUS_SET_VIDEO_BITRATE "SET_VIDEO_BITRATE"

#define EVT_MSG                      "EVT_MSG"
#define EVT_TIME                     "EVT_TIME"
#define EVT_PARAM1					 "EVT_PARAM1"
#define EVT_PARAM2					 "EVT_PARAM2"
#define EVT_PARAM3					 "EVT_PARAM3"
#define EVT_DESCRIPTION              "EVT_DESCRIPTION"
#define EVT_PLAY_PROGRESS            "EVT_PLAY_PROGRESS"
#define EVT_PLAY_DURATION            "EVT_PLAY_DURATION"
#define EVT_PLAYABLE_DURATION        "PLAYABLE_DURATION"   
#define EVT_REPORT_TOKEN             "EVT_REPORT_TOKEN"
#define EVT_PARAM_VIDEO_WIDTH		 "EVT_PARAM_VIDEO_WIDTH"
#define EVT_PARAM_VIDEO_HEIGHT		 "EVT_PARAM_VIDEO_HEIGHT"
#define EVT_PARAM_ENCODE_BITRATE	 "EVT_PARAM_ENCODE_BITRATE"

#define STREAM_ID                    "STREAM_ID"

#endif // __TX_LIVE_SDK_TYPE_DEF_H__

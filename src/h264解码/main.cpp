
#include <opencv2/highgui.hpp>
#include <iostream>
#include <windows.h>
extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib,"opencv_world320.lib")
using namespace std;
using namespace cv;


cv::Mat avframeToCvmat(const AVFrame* frame)
{
	int width = frame->width;
	int height = frame->height;
	cv::Mat image(height, width, CV_8UC3);
	int cvLinesizes[1];
	cvLinesizes[0] = image.step1();
	SwsContext* conversion = sws_getContext(width, height, (AVPixelFormat)frame->format, width, height, AVPixelFormat::AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cvLinesizes);
	sws_freeContext(conversion);
	return image;
}



int main() {
	const char* filename = "C:/Users/75846/Desktop/out.h264";
	int ret=0;
	int y_size = 0;

	AVFormatContext* ctx = NULL;
	AVCodec* codec = NULL;
	AVCodecContext* avctx = NULL;
	AVFrame* picture = NULL;
	AVPacket* pkt=NULL;


	av_register_all();
	//1.打开h264文件
	ret=avformat_open_input(&ctx, filename, NULL, NULL);
	if (ret != 0)
	{
		cout << "can 't open file" << endl;
	}
	//2.查找文件信息
	ret = avformat_find_stream_info(ctx,NULL);
	if (ret < 0)
	{
		cout << "can't find stream info" << endl;
	}
	//3.查找解码器
	codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (codec==NULL) 
	{
		cout << "can't find decoder" << endl;
	}

	//创建解码器上下文
	avctx = avcodec_alloc_context3(codec);
	if (avctx == NULL)
	{
		cout << "can' t alloc avctx" << endl;
	}

	//5.打开解码器
	ret = avcodec_open2(avctx, codec, NULL);
	if (ret != 0)
	{
		cout << "can't open codec" << endl;

	}
	//6.创建avpacket
	pkt = (AVPacket*)av_malloc(sizeof(AVPacket));
	y_size = avctx->width * avctx->height;
	av_new_packet(pkt, y_size);

	if (pkt == NULL)
	{
		cout << "pack error" << endl;
	}
	av_dump_format(ctx, 0, filename, 0);
	


	//7.分配avframe
	picture = av_frame_alloc();
	if (picture == NULL)
	{
		cout << "picture error" << endl;
	}
	Mat pic;

	while (av_read_frame(ctx, pkt) == 0)
	{

		ret=avcodec_send_packet(avctx, pkt);//将音视频解码步骤分为了两步，第一步avcodec_send_packet，第二步avcodec_receive_frame
		ret=avcodec_receive_frame(avctx, picture);//第一步是发送编码数据包，第二步是接收解码后数据
		if (ret < 0)
		{
			cout << "decode error" << endl;
		}
		pic = avframeToCvmat(picture);
		imshow("pic", pic);
		waitKey(20);
	
	}

	av_packet_unref(pkt);
	av_frame_unref(picture);
	avcodec_close(avctx);
	avformat_close_input(&ctx);
	

	

	return 0;
	
}



/*
	cout << avcodec_receive_frame(avctx, picture)<<endl;
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);  //获取当前时间 可精确到ms
		printf("%d-%02d-%02d %02d:%02d:%02d:%02d\n",
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond,
			st.wMilliseconds);
*/
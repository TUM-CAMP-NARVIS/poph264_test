#include <iostream>
#include <string>
#include <thread>
#include <cstdio>
#include <chrono>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

#include "PopH264.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>


// copied from:
// https://github.com/NewChromantics/SoyLib/blob/a32d484eb0e15575d2a9db7f5c02f2d2dfb4100c/src/SoyPixels.h
enum SoyPixelType {
	Invalid = 0,
	Greyscale,
	GreyscaleAlpha,  // png has this for 2 channel, so why not us!
	RGB,
	RGBA,
	ARGB,
	BGRA,
	BGR,
	KinectDepth,          //  16 bit. 13 bits of depth, 3 bits of user-index
	FreenectDepth10bit,   //  16 bit, 1 channel
	FreenectDepth11bit,   //  16 bit, 1 channel
	Depth16mm,            //  16 bit, 1 channel
	DepthFloatMetres,     //  32bit float
	DepthHalfMetres,      //  16bit float
	DepthDisparityFloat,  //  32bit float - apple true depth camera raw format
	DepthDisparityHalf,   //  16bit float - apple true depth camera raw format
	Yuv_8_88,             //  NV12
	Yvu_8_88,             //  NV21
	Uvy_8_88,             //  yuvs  //  gr: should this be Uvy_88_8?
	Yuv_8_8_8,  //  luma, u, v seperate planes (uv is half size... reflect
	//this somehow in the name!)
	YYuv_8888,
	uyvy_8888,
	ChromaUV_8_8,  // 8 bit plane, 8 bit plane
	ChromaUV_88,   // 16 bit interleaved plane
	ChromaU_8,     // single plane
	ChromaV_8,     // single plane
	Palettised_RGB_8,
	Palettised_RGBA_8,
	Float1,
	Float2,
	Float3,
	Float4,

	Count,
};


// helper from: https://github.com/opencv/opencv/issues/21727
// not efficient
cv::Mat BGR2YUV_NV12(const cv::Mat& src) {
	auto src_h = src.rows, src_w = src.cols;
	cv::Mat dst(src_h * 3 / 2, src_w, CV_8UC1);
	cv::cvtColor(src, dst, cv::COLOR_BGR2YUV_I420);
	auto n_y = src_h * src_w;
	auto n_uv = n_y / 2, n_u = n_y / 4;
	std::vector<uint8_t> uv(n_uv);
	std::copy(dst.data + n_y, dst.data + n_y + n_uv, uv.data());
	for (auto i = 0; i < n_u; i++) {
		dst.data[n_y + 2 * i] = uv[i];            // U
		dst.data[n_y + 2 * i + 1] = uv[n_u + i];  // V
	}
	return dst;
}


inline std::vector<std::string> splitLines(const std::string& str) {
	if (str.empty()) {
		return {};
	}

	std::vector<std::string> tokens;
	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while ((pos = str.find('\n', prev)) != std::string::npos) {
		auto v = str.substr(prev, pos - prev);
		if (!v.empty()) {
			tokens.push_back(v);
		}
		prev = pos + 1;
	}
	tokens.push_back(str.substr(prev));

	return tokens;
}




int main() {
	using namespace std::chrono;


	// setup opencv capture

	int device{ 1 };
	int width{ 1280 };
	int height{ 720 };
	int framerate{ 15 };

	int apiPreference = cv::CAP_ANY;
#ifdef __APPLE__
	apiPreference = cv::CAP_AVFOUNDATION;
#endif

	auto videocapture = cv::VideoCapture(device, apiPreference);
	auto ret = videocapture.set(cv::CAP_PROP_FRAME_WIDTH, width);
	ret &= videocapture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
	ret &= videocapture.set(cv::CAP_PROP_FPS, framerate);
	assert(ret);


	// configure poph264 encoder
	std::string encoder_cfg;
	{
		// don't know how to properly create a new Document
		rapidjson::Document rd;
		rd.Parse("{}");
		auto d = rd.GetObject();

		// d.AddMember(POPH264_ENCODER_KEY_ENCODERNAME, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_ENCODERNAME].SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.size()));
		// d.AddMember(POPH264_ENCODER_KEY_PROFILELEVEL, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_PROFILELEVEL].SetInt(profile_level.value());
		// d.AddMember(POPH264_ENCODER_KEY_AVERAGEKBPS, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_AVERAGEKBPS].SetInt(average_kbps.value());
		// d.AddMember(POPH264_ENCODER_KEY_MAXKBPS, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_MAXKBPS].SetInt(max_kbps.value());
		 d.AddMember(POPH264_ENCODER_KEY_REALTIME, rapidjson::Value(true), rd.GetAllocator());
		 d.AddMember(POPH264_ENCODER_KEY_VERBOSEDEBUG, rapidjson::Value(false), rd.GetAllocator());
		// d.AddMember(POPH264_ENCODER_KEY_MAXFRAMEBUFFERS, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_MAXFRAMEBUFFERS].SetInt(max_frame_buffers.value());
		// d.AddMember(POPH264_ENCODER_KEY_MAXSLICEBYTES, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_MAXSLICEBYTES].SetInt(max_slice_bytes.value());
		 d.AddMember(POPH264_ENCODER_KEY_MAXIMISEPOWEREFFICIENCY, rapidjson::Value(false), rd.GetAllocator());
		// d.AddMember(POPH264_ENCODER_KEY_KEYFRAMEFREQUENCY, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_KEYFRAMEFREQUENCY].SetInt(max_key_frame_frequency.value());
		// d.AddMember(POPH264_ENCODER_KEY_QUALITY, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_QUALITY].SetInt(quality.value());
		// d.AddMember(POPH264_ENCODER_KEY_ENCODERTHREADS, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_ENCODERTHREADS].SetInt(encoder_threads.value());
		// d.AddMember(POPH264_ENCODER_KEY_LOOKAHEADTHREADS, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_LOOKAHEADTHREADS].SetInt(lookahead_threads.value());
		// d.AddMember(POPH264_ENCODER_KEY_BSLICEDTHREADS, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_BSLICEDTHREADS].SetBool(bsliced_threads.value());
		// d.AddMember(POPH264_ENCODER_KEY_DETERMINISTIC, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_DETERMINISTIC].SetBool(deterministic.value());
		// d.AddMember(POPH264_ENCODER_KEY_CPUOPTIMISATIONS, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_CPUOPTIMISATIONS].SetBool(cpu_optimisations.value());
		// d.AddMember(POPH264_ENCODER_KEY_CONSTANTBITRATE, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_CONSTANTBITRATE].SetInt();
		// d.AddMember(POPH264_ENCODER_KEY_SLICELEVELENCODING, rapidjson::Value(), rd.GetAllocator());
		// d[POPH264_ENCODER_KEY_SLICELEVELENCODING].SetInt();

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		rd.Accept(writer);
		encoder_cfg = buffer.GetString();
	}


	// create encoder
	std::array<char, 1024> StringBuffer = { 0 };
	auto encoder_handle =
		PopH264_CreateEncoder(encoder_cfg.c_str(), StringBuffer.data(),
			static_cast<int32_t>(StringBuffer.size()));

	if (encoder_handle == 0) {
		for (auto& l : splitLines(StringBuffer.data())) {
			std::cout << "CreateEncoder: PopH264Error: %s" << l << std::endl;
		}
		exit(1);
	}


	// global encoder config / pointers - input to encoder is NV12
	bool is_key_frame = true;
	int format = SoyPixelType::Yuv_8_88;
	int luma_size = width * height;
	int chroma_u_size = luma_size / 4;
	int chroma_v_size = chroma_u_size;

	// start capturing/encoding
	cv::Mat frame;

	bool should_stop{ false };
	while (!should_stop) {

		// grap the image
		if (videocapture.grab()) {
			if (!videocapture.retrieve(frame)) {
				std::cout << "OpenCV: error retrieving frame" << std::endl;
				break;
			}
			// new image retrieved - CV works with BGR
			cv::Mat input_image = BGR2YUV_NV12(frame);

			// feed the input_image into the poph264 encoder
			std::string frame_cfg;
			{
				rapidjson::Document rd;
				rd.Parse("{}");
				auto d = rd.GetObject();

				d.AddMember("Width", rapidjson::Value(), rd.GetAllocator());
				d["Width"].SetInt(width);

				d.AddMember("Height", rapidjson::Value(), rd.GetAllocator());
				d["Height"].SetInt(height);

				d.AddMember("LumaSize", rapidjson::Value(), rd.GetAllocator());
				d["LumaSize"].SetInt(luma_size);

				d.AddMember("Format", rapidjson::Value(), rd.GetAllocator());
				d["Format"].SetInt(format);

				if (chroma_u_size > 0) {
					d.AddMember("ChromaUSize", rapidjson::Value(), rd.GetAllocator());
					d["ChromaUSize"].SetInt(chroma_u_size);
				}

				if (chroma_v_size > 0) {
					d.AddMember("ChromaVSize", rapidjson::Value(), rd.GetAllocator());
					d["ChromaVSize"].SetInt(chroma_v_size);
				}

				d.AddMember("KeyFrame", rapidjson::Value(), rd.GetAllocator());
				d["KeyFrame"].SetBool(is_key_frame);

				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				rd.Accept(writer);

				frame_cfg = (buffer.GetString());
			}

			// Push Frame to encoder

			// check if this pointer arithmetic is correct (only for NV12 + Grey  i guess)
			const uchar* y_plane = input_image.data;

			const uchar* u_plane = nullptr;
			const uchar* v_plane = nullptr;
			if (chroma_u_size > 0) {
				u_plane = y_plane + luma_size;
			}
			if (chroma_v_size > 0) {
				v_plane = u_plane + chroma_u_size;
			}

			std::array<char, 1024> JsonBuffer = { 0 };
			PopH264_EncoderPushFrame(encoder_handle, frame_cfg.c_str(), y_plane, u_plane,
				v_plane, JsonBuffer.data(),
				static_cast<int32_t>(JsonBuffer.size()));

			auto errors = splitLines(JsonBuffer.data());
			if (!errors.empty()) {
				for (auto& l : errors) {
					std::cout << "PushFrame: PopH264Error: %s" << l << std::endl;;
				}
				exit(1);
			}

			// try to get output for maximum of n ms
			bool all_data_fetched{ false };
			auto stop_t = system_clock::now() + milliseconds(200);
			while (stop_t > system_clock::now() && !all_data_fetched) {

				int encode_duration_ms{0};
				int delay_duration_ms{0};
				int data_size{ 0 };
				int output_queue_count{ 0 };
				std::string encoder_name;

				JsonBuffer = { 0 };
				PopH264_EncoderPeekData(encoder_handle, JsonBuffer.data(),
					static_cast<int32_t>(JsonBuffer.size()));
				//std::cout << "PeekData: " << JsonBuffer.data() << std::endl;
				{
					rapidjson::Document d;
					d.Parse(JsonBuffer.data());

					if (d.HasMember("DataSize")) {
						data_size = d["DataSize"].GetInt();
					}
					if (d.HasMember("EncodeDurationMs")) {
						encode_duration_ms = d["EncodeDurationMs"].GetInt();
					}
					if (d.HasMember("DelayDurationMs")) {
						delay_duration_ms = d["DelayDurationMs"].GetInt();
					}
					if (d.HasMember("OutputQueueCount")) {
						output_queue_count = d["OutputQueueCount"].GetInt();
					}
					if (d.HasMember("EncoderName")) {
						encoder_name = d["EncoderName"].GetString();
					}
					if (d.HasMember("EndOfStream") && d["EndOfStream"].GetBool()) {
						std::cout << "H264Encoder: received end-of-stream." << std::endl;
						if (output_queue_count > 0) {
							PopH264_EncoderPopData(encoder_handle, nullptr, 0);
						}
						exit(0);
					}
				}

				if (data_size == 0 || output_queue_count == 0) {
					// wait for 5ms before polling again
					std::this_thread::sleep_for(milliseconds(5));
					continue;
				}
				
				// retrieve encoded buffer
				auto buff = std::make_unique<std::byte[]>(data_size);
				auto bytes_written = PopH264_EncoderPopData(
					encoder_handle,
					reinterpret_cast<uint8_t*>(buff.get()),
					static_cast<int32_t>(data_size));
				if (bytes_written == 0) {
					std::cout <<
						"Encoder: popped 0 bytes for frame, but expected data." << std::endl;
					break;
				}
				else if (bytes_written < 0) {
					std::cout <<
						"Encoder: error while popping frame, bytes_written: " << bytes_written << ", meta: " << JsonBuffer.data() << std::endl;
					break;
				}

				// stop if we retrieved all frames
				all_data_fetched = output_queue_count == 1;

				// forward the data in buff/bytes_written to the application
				std::cout << "Got Encoded Frame with size: " << bytes_written << " queue-count: " << output_queue_count << " data-fetched: " << all_data_fetched << std::endl;
				buff.reset();

				if (all_data_fetched) {
					break;
				}
			}


			if (!all_data_fetched) {
				std::cout << "Encoder: encoder did not produce data within the required amount of time." << std::endl;
				//continue;
			}


			// also show the image to see that it's running
			cv::imshow("live", frame);
			cv::waitKey(5);
		}
		else {
			std::cout << "error grabbing frame" << std::endl;
			break;
		}
	}
	return 0;
}

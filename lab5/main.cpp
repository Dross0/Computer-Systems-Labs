#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <sys/times.h>
#include <unistd.h>

double get_time(struct tms start, struct tms end, unsigned long clocks_per_sec){
    return static_cast<double>(end.tms_utime - start.tms_utime) / static_cast<double>(clocks_per_sec);
}


int main(int argc,char *argv[]){
    cv::Mat frame;
    cv::VideoCapture cap;
    int deviceID = 0;
    int apiID = cv::CAP_ANY;
    cap.open(deviceID + apiID);
    if (!cap.isOpened()){
        std::cerr << "Cant open" << std::endl;
        return 1;
    }
    struct tms start_all, end_all, start_input, end_input, start_proc, end_proc, start_out, end_out;
    long clocks_per_sec = sysconf(_SC_CLK_TCK);
    unsigned long int frames_count = 0;
    double input_time = 0;
    double processing_time = 0;
    double output_time = 0;
    times(&start_all);
    while (1){
        times(&start_input);
        cap >> frame;
        times(&end_input);
        input_time += get_time(start_input, end_input, clocks_per_sec);

        if (frame.empty()){
            std::cerr << "Error frame is empty" << std::endl;
            break;
        }

        times(&start_proc);
        for (int y = 0; y < frame.cols; ++y){
            for (int x = 0; x < frame.rows; ++x){
                frame.at<cv::Vec3b>(x, y)[0] = 255 - frame.at<cv::Vec3b>(x, y)[0];
                frame.at<cv::Vec3b>(x, y)[1] = 255 - frame.at<cv::Vec3b>(x, y)[1];
                frame.at<cv::Vec3b>(x, y)[2] = 255 - frame.at<cv::Vec3b>(x, y)[2];
            }
        }
        times(&end_proc);
        processing_time += get_time(start_proc, end_proc, clocks_per_sec);

        times(&start_out);
        cv::imshow("Live", frame);
        times(&end_out);
        output_time += get_time(start_out, end_out, clocks_per_sec);
        frames_count++;
        if (cv::waitKey(5) >= 0){
            break;
        }
    }
    times(&end_all);
    double total_time = get_time(start_all, end_all, clocks_per_sec);
    std::cout << "Total time: " << total_time << " Fps: " << static_cast<double>(frames_count) / total_time << std::endl
                << "Input: " << input_time / total_time * 100 << "%" << std::endl
                << "Processing: " << processing_time / total_time * 100 << "%" << std::endl
                << "Output: " << output_time / total_time * 100 << "%" << std::endl;
    return 0;
}

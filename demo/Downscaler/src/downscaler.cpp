#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string_view>
#include <thread>
#include <chrono>
#include <queue>
#include <cmath>
#include <mutex>
#include <random>
#include <array>
#include <filesystem>
#include <vector>

#include <opencv2/opencv.hpp>

#include <mt/csemaphore.h>
#include <mt/cpipe.h>
#include <mt/posix_semaphore.h>
#include <mt/lbs_qpipe.h>
#include <mt/lbs_pipe.h>

#include <ns/ring.h>

struct NamedImage
{
    cv::Mat image;
    std::string fileName;
};

std::vector<std::string> findJpgs(const std::string_view dir)
{
    std::vector<std::string> result;

    for (const auto & entry : std::filesystem::directory_iterator(dir))
    {
        if (!entry.is_regular_file()) continue;
        const auto path = entry.path().generic_string();
        if (path.ends_with(".JPG")) result.push_back(path);
    }

    return result;
}

int main(int argc, char ** argv) 
{
    const std::string usageMsg = "./Downscaler [src dir] [output dir] [output width]";
    if (argc != 4)
    {
        std::cerr << usageMsg << "\n";
        return 1;
    }

    const std::string_view srcDir = argv[1];
    const std::string_view dstDir = argv[2];
    const int outWidth = std::stoi(argv[3]);

    const auto inFiles = findJpgs(srcDir);
    std::cout << "Found " << inFiles.size() << " JPGs\n";

    constexpr int capacity = 8;
    std::vector<NamedImage> inputImages(capacity), outputImages(capacity);
    mt::lbs_qpipe<NamedImage, mt::posix_semaphore, std::vector<NamedImage>> src2DsPipe(inputImages);
    mt::lbs_qpipe<NamedImage, mt::posix_semaphore, std::vector<NamedImage>> ds2DstPipe(outputImages);

    auto sourceF = [&]()
    {
        for (int i = 0; i < inFiles.size(); i++)
        {
            auto val = src2DsPipe.begin_write();
            val->fileName = inFiles[i];
            val->image = cv::imread(inFiles[i]);
            src2DsPipe.finish_write(val);
        }
        src2DsPipe.close_write();
    };

    auto dsF = [&]()
    {
        while (true)
        {
            auto inVal = src2DsPipe.begin_read();
            if (!inVal) break;
            auto outVal = ds2DstPipe.begin_write();

            const cv::Mat & inMat = inVal->image;
            cv::Mat & outMat = outVal->image;
            cv::resize(inMat, outMat, cv::Size(outWidth, inMat.rows * outWidth / inMat.cols), cv::INTER_AREA);
            auto path = std::filesystem::path(inVal->fileName).replace_extension("png");
            auto basename = path.filename().generic_string();
            outVal->fileName = std::string(dstDir) + "/" + basename;

            src2DsPipe.finish_read(inVal);
            ds2DstPipe.finish_write(outVal);
        }
        ds2DstPipe.close_write();
    };

    bool done = false;

    auto saveF = [&]()
    {
        while (true)
        {
            auto inVal = ds2DstPipe.begin_read();
            if (!inVal) break;

            cv::imwrite(inVal->fileName, inVal->image);
            ds2DstPipe.finish_read(inVal);
        }
        done = true;
    };

    std::thread tSrc(sourceF);
    std::thread tDs1(dsF);
    std::thread tDs2(dsF);
    std::thread tSave(saveF);

    using namespace std::chrono_literals;
    while (!done)
    {
        auto cnt1 = src2DsPipe.count();
        auto cnt2 = ds2DstPipe.count();
        std::cout << cnt1 << "\t" << cnt2 << "\n";
        std::this_thread::sleep_for(200ms);
    }

    tSrc.join();
    tDs1.join();
    tDs2.join();
    tSave.join();

    return 0;
}

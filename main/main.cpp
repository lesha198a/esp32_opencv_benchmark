// Benchmarks based on joachimBurket/esp32-opencv (github.com) examples for esp32

#include <setjmp.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/calib3d.hpp"
#include <esp_log.h>
#include <esp_err.h>
#include <esp_task_wdt.h>
#include <esp_timer.h>
#include <memory>
#include <string>
#include <chrono>
#include <iostream>
#include <sstream>
#include <sys/unistd.h>
#include <pthread.h>
#include "wdt_abstract.h"
#include "system.hpp"
#include "report.hpp"

using namespace cv;
using namespace std;

static const char *TAG = "opencv_tests";

// Required for ESP-IDF compatibility 
void __wrap_longjmp(jmp_buf env, int val) {
    longjmp(env, val);
}

extern "C" {
    void app_main(void);
}

/**
 * Reads and validates an image file
 */
Mat read_image_specific_res(const String &fileName) {
    Mat img;
    try {
        img = imread(fileName, IMREAD_GRAYSCALE);
        if (img.empty()) {
            ESP_LOGE(TAG, "Failed to load image: %s", fileName.c_str());
            exit(1);
        }
    } catch (cv::Exception &e) {
        ESP_LOGE(TAG, "Exception: %s", e.msg.c_str());
        exit(1);
    }

    ESP_LOGI(TAG, "Image read: %dx%dx%d, depth: %d", 
             img.rows, img.cols, img.channels(), img.depth());
    return img;
}

// Benchmark function declarations
int64_t BM_binThresh(const Mat &src) {
    Mat dst;
    return BENCHMARK(threshold, src, dst, 128, 255, THRESH_BINARY);
}

int64_t BM_triangleThresh(const Mat &src) {
    Mat dst;
    return BENCHMARK(threshold, src, dst, 0, 255, THRESH_BINARY | THRESH_TRIANGLE);
}

int64_t BM_OTSUThresh(const Mat &src) {
    Mat dst;
    return BENCHMARK(threshold, src, dst, 0, 255, THRESH_OTSU | THRESH_BINARY);
}

int64_t BM_toZeroThresh(const Mat &src) {
    Mat dst;
    return BENCHMARK(threshold, src, dst, 128, 255, THRESH_TOZERO);
}

int64_t BM_GaussianBlur3x3(const Mat &src) {
    Mat dst;
    return BENCHMARK(GaussianBlur, src, dst, Size(3, 3), 0, 0, BORDER_DEFAULT);
}

int64_t BM_medianBlur3x3(const Mat &src) {
    Mat dst;
    return BENCHMARK(medianBlur, src, dst, 3);
}

int64_t BM_bilateralFilter(const Mat &src) {
    Mat dst;
    return BENCHMARK(bilateralFilter, src, dst, 5, 18, 5, BORDER_DEFAULT);
}

int64_t BM_erode(const Mat &src) {
    Mat dst;
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(4, 4));
    return BENCHMARK(erode, src, dst, element, Point(-1,-1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
}

int64_t BM_dilate(const Mat &src) {
    Mat dst;
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(4, 4));
    return BENCHMARK(dilate, src, dst, element, Point(-1,-1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
}

int64_t BM_open(const Mat &src) {
    Mat dst;
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(4, 4));
    return BENCHMARK(morphologyEx, src, dst, MORPH_OPEN, element, Point(-1,-1), 1, BORDER_CONSTANT,
                     morphologyDefaultBorderValue());
}

int64_t BM_resizeLinear(const Mat &src) {
    Mat dst;
    return BENCHMARK(resize, src, dst, Size(), 0.75, 0.75, INTER_LINEAR);
}

int64_t BM_resizeCubic(const Mat &src) {
    Mat dst;
    return BENCHMARK(resize, src, dst, Size(), 0.75, 0.75, INTER_CUBIC);
}

int64_t BM_Sobel(const Mat &src) {
    Mat dst;
    return BENCHMARK(Sobel, src, dst, 2, 2, 1, 3, 1, 0, BORDER_DEFAULT);
}

int64_t BM_Canny(const Mat &src) {
    Mat dst;
    Canny(src, dst, 50, 200, 3, false); // Cache warm up
    auto start = chrono::system_clock::now();
    Canny(src, dst, 50, 200, 3, false);
    return chrono::duration_cast<chrono::microseconds>(
        chrono::system_clock::now() - start).count();
}

int64_t BM_HoughLines(const Mat &src) {
    Mat dst, blurred, edge;
    blur(src, blurred, Size(3, 3));
    Canny(blurred, edge, 50, 200, 3, false);
    vector<Vec2f> lines;
    return BENCHMARK(HoughLines, edge, lines, 1, CV_PI/180, 100, 0, 0, 0, CV_PI);
}

int64_t BM_HoughLinesP(const Mat &src) {
    Mat dst, blurred, edge;
    blur(src, blurred, Size(3, 3));
    Canny(blurred, edge, 50, 200, 3, false);
    vector<Vec4i> linesP;
    return BENCHMARK(HoughLinesP, edge, linesP, 1, CV_PI/180, 100, 80, 0);
}


#undef EPS
/**
 * Camera calibration benchmark
 */
int64_t BM_CalibCamera(const imagesList &images) {
    const cv::Size patternSize(9, 6);
    const float squareSize = 1.0f;
    
    // Generate object points for chessboard pattern
    vector<Point3f> obj;
    for (int i = 0; i < patternSize.height; ++i) {
        for (int j = 0; j < patternSize.width; ++j) {
            obj.emplace_back(j * squareSize, i * squareSize, 0);
        }
    }
    
    vector<vector<Point2f>> imagePoints;
    vector<vector<Point3f>> objectPoints;
    Size imageSize;

    TickType_t prepStartTick = xTaskGetTickCount();
    
    // Process each calibration image
    for (const auto &[imgName, img] : images) {
        if (img.empty()) continue;
        
        if (imageSize.empty()) {
            imageSize = img.size();
        }

        vector<Point2f> corners;
        bool found = findChessboardCorners(img, patternSize, corners,
            CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

        if (found) {
            Mat gray = (img.channels() == 3) ? Mat() : img;
            if (img.channels() == 3) {
                cvtColor(img, gray, COLOR_BGR2GRAY);
            }
            
            cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
                
            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
        }

        my_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    uint32_t prepDuration = pdTICKS_TO_MS(xTaskGetTickCount() - prepStartTick);

    if (objectPoints.size() < 3) {
        ESP_LOGE(TAG, "Insufficient calibration images: %zu", objectPoints.size());
        return 0;
    }

    Mat cameraMatrix, distCoeffs;
    vector<Mat> rvecs, tvecs;

    my_task_wdt_delete(NULL);
    
    TickType_t calibStartTick = xTaskGetTickCount();
    double rmsError = calibrateCamera(objectPoints, imagePoints, imageSize,
        cameraMatrix, distCoeffs, rvecs, tvecs, 0);
    uint32_t calibDuration = pdTICKS_TO_MS(xTaskGetTickCount() - calibStartTick);

    my_task_wdt_add(NULL);

    // Log calibration results
    ostringstream oss;
    oss << "Camera Matrix:\n" << cameraMatrix << "\nDistortion Coefficients:\n" << distCoeffs;
    ESP_LOGI(TAG, "Calibration completed in %lu ms, RMS: %f\n%s", 
             calibDuration, rmsError, oss.str().c_str());

    return prepDuration + calibDuration;
}

/**
 * Benchmark task that runs all tests
 */
void benchmarkTask(void *pvParameters) {
    my_task_wdt_add(NULL);

    const vector<string> images_res = {"160x120", "320x240", "640x480", "1024x768"};
    auto testImages = make_unique<imagesList>();
    
    for (const auto &res : images_res) {
        testImages->emplace_back(res, read_image_specific_res("/spiffs/" + res + ".png"));
    }

    Report report("BUILD_TYPE=Release", *testImages);
    
    // TestGroup &calibration = report.addGroup("Calibration");
    // calibration.addTestCase(make_unique<TestCaseOnAll>("calibrateCamera", BM_CalibCamera));

    TestGroup &thresholds = report.addGroup("Threshold");
    thresholds.addTestCase(make_unique<TestCase>("binaryThreshold", BM_binThresh));
    thresholds.addTestCase(make_unique<TestCase>("triangleThreshold", BM_triangleThresh));
    thresholds.addTestCase(make_unique<TestCase>("OTSUThreshold", BM_OTSUThresh));
    thresholds.addTestCase(make_unique<TestCase>("toZeroThreshold", BM_toZeroThresh));

    TestGroup &blurrings = report.addGroup("Blurring");
    blurrings.addTestCase(make_unique<TestCase>("GaussianBlur 3x3 kernel", BM_GaussianBlur3x3));
    blurrings.addTestCase(make_unique<TestCase>("medianBlur 3x3 kernel", BM_medianBlur3x3));
    blurrings.addTestCase(make_unique<TestCase>("bilateralFilter diameter=5", BM_bilateralFilter));

    TestGroup &morph = report.addGroup("Morphological transforms");
    morph.addTestCase(make_unique<TestCase>("erode 5x5 kernel", BM_erode));
    morph.addTestCase(make_unique<TestCase>("dilate 5x5 kernel", BM_dilate));
    morph.addTestCase(make_unique<TestCase>("open 5x5 kernel", BM_open));

    TestGroup &resize = report.addGroup("Resize image");
    resize.addTestCase(make_unique<TestCase>("resize linear interpolation", BM_resizeLinear));
    resize.addTestCase(make_unique<TestCase>("resize cubic interpolation", BM_resizeCubic));

    TestGroup &edgeDetect = report.addGroup("Edge detection");
    edgeDetect.addTestCase(make_unique<TestCase>("Sobel", BM_Sobel));
    edgeDetect.addTestCase(make_unique<TestCase>("Canny", BM_Canny));

    TestGroup &houghTransform = report.addGroup("Hough transformations");
    houghTransform.addTestCase(make_unique<TestCase>("HoughLines", BM_HoughLines));
    houghTransform.addTestCase(make_unique<TestCase>("HoughLines probabilistic", BM_HoughLinesP));

    report.startBenchmark();
    cout << report.getSummary() << endl;

    my_task_wdt_delete(nullptr);
    vTaskDelete(NULL);
}

extern "C" void app_main(void) {
    pthread_cond_t cond_test = PTHREAD_COND_INITIALIZER;
    pthread_cond_init(&cond_test, nullptr);

    ESP_LOGI(TAG, "Starting main");
    disp_mem_infos();
    init_spiffs();

    // Verify OpenCV initialization
    {
        Mat testMat(160, 160, CV_8UC3, Scalar(0));
        if (testMat.empty()) {
            ESP_LOGE(TAG, "OpenCV initialization failed!");
            return;
        }
    }

    xTaskCreatePinnedToCore(
        benchmarkTask,
        "BenchmarkTask",
        16384,
        nullptr,
        1,
        nullptr,
        1
    );

    ESP_LOGI(TAG, "End of main");
}

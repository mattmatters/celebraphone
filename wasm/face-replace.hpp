#include <dlib/clustering.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/string.h>
#include <opencv2/opencv.hpp>

class FaceReplace {
  cv::CascadeClassifier detector;
  dlib::shape_predictor landmarker;
  cv::Mat srcImg;
  std::vector<cv::Point2f> srcPoints;
  cv::CascadeClassifier faceDetector;
public:
  void DebugMode(bool debugMode);
  std::vector<cv::Rect> DetectFaces(cv::Mat &img);
  FaceReplace(std::vector<uint8_t> &baseImg, int width, int height);
  void MapToFace(std::vector<uint8_t> &src, int width, int height);
  std::vector<cv::Point2f> DetectLandmarks(cv::Mat &img, cv::Rect face);
};

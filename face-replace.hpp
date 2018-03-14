#include <dlib/clustering.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/string.h>
#include <opencv2/opencv.hpp>

class FaceReplace {
  dlib::frontal_face_detector detector;
  dlib::shape_predictor landmarker;
  cv::Mat srcImg;
  std::vector<std::vector<int>> srcTri;
  std::vector<cv::Point2f> srcPoints;
private:
  std::vector<cv::Point2f> detectPoints(cv::Mat &img, dlib::rectangle box);
public:
  int getPointCount();
  int getWidth();
  int getHeight();
  std::vector<cv::Point2f> getPoints();
  float getPoint(int point);
  int getFailingPoint();
  std::string tryTriangles();
  FaceReplace(std::vector<uint8_t> baseImg, int width, int height);
  std::vector<uint8_t> MapToFace(std::vector<uint8_t> src, int width, int height);
};

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
  std::vector<std::vector<int>> srcTri;
  std::vector<cv::Point2f> srcPoints;
  std::vector<uint8_t> stuff;
private:
  std::vector<cv::Point2f> detectPoints(dlib::cv_image<dlib::bgr_pixel> &img, dlib::rectangle box);
  std::vector<cv::Point2f> detectFace(cv::Mat &img);
  cv::Mat initImg(std::vector<uint8_t> img, int width, int height);
  void applyAffineTransform(cv::Mat &warpImage, cv::Mat &src, std::vector<cv::Point2f> &srcTri, std::vector<cv::Point2f> &dstTri);
public:
  int getPointCount();
  //  int detectpoop(int k);
  std::string showImg(std::string img);
  int getWidth();
  int getHeight();
  std::vector<cv::Point2f> getPoints();
  float getPoint(int point);
  int getFailingPoint();
  std::string tryTriangles();
  FaceReplace(std::vector<uint8_t> baseImg, int width, int height);
  std::vector<uint8_t> MapToFace(std::vector<uint8_t> src, int width, int height);
};

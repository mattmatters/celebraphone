#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>

using namespace cv;

class FaceReplace {
  CascadeClassifier detector;
  //  Ptr<Facemark> facemark;
  cv::Mat srcImg;
  std::vector<std::vector<int>> srcTri;
  std::vector<cv::Point2f> srcPoints;
private:
  std::vector<cv::Point2f> detectPoints(cv::Mat &img, cv::Rect box);
  std::vector<cv::Rect> detectFaces(cv::Mat &img);
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

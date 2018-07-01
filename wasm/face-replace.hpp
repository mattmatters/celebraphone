#ifndef FACE_REPLACE_HPP
#define FACE_REPLACE_HPP

#include <opencv2/opencv.hpp>
#include <face-detect.hpp>

class FaceReplace {
  FaceDetect faceDetector;
  cv::Mat srcImg;
  std::vector<cv::Point2f> srcPoints;
public:
  FaceReplace(FaceDetect faceDetector, std::vector<uint8_t> &baseImg, int width, int height);
  std::vector<cv::Rect> DetectFaces(cv::Mat &img);
  void MapToFace(std::vector<uint8_t> &src, int width, int height);
  std::vector<cv::Point2f> DetectLandmarks(cv::Mat &img, cv::Rect face);
};

#endif

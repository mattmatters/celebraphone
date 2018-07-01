#ifndef FACE_DETECT_HPP
#define FACE_DETECT_HPP

#include <dlib/clustering.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/string.h>
#include <opencv2/opencv.hpp>

class FaceDetect {
  cv::CascadeClassifier detector;
  dlib::shape_predictor landmarker;
  cv::CascadeClassifier faceDetector;
public:
  FaceDetect();
  FaceDetect(bool highQuality);
  std::vector<cv::Rect> DetectFaces(cv::Mat &img);
  std::vector<cv::Point2f> DetectLandmarks(cv::Mat &img, cv::Rect face);
};

#endif

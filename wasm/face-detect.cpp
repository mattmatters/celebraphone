#include <dlib/clustering.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/pixel.h>
#include <dlib/string.h>
#include <emscripten/bind.h>
#include <iostream>
#include <face-detect.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;         // OpenCV
using namespace emscripten; // WASM

static dlib::rectangle openCVRectToDlib(cv::Rect r)
{
  return dlib::rectangle((long)r.tl().x, (long)r.tl().y, (long)r.br().x - 1, (long)r.br().y - 1);
}

FaceDetect::FaceDetect()
{
  faceDetector.load("haarcascade_frontalface_alt2.xml");
  dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> landmarker;
}

FaceDetect::FaceDetect(bool highQuality)
{
  faceDetector.load("haarcascade_frontalface_alt2.xml");
  dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> landmarker;
}

std::vector<cv::Point2f> FaceDetect::DetectLandmarks(cv::Mat &img, cv::Rect face)
{
  std::vector<cv::Point2f> points;
  dlib::rectangle rec = openCVRectToDlib(face);

  // Create the dlib equivalent
  dlib::array2d<dlib::rgb_alpha_pixel> thing;
  dlib::assign_image(thing, dlib::cv_image<dlib::rgb_alpha_pixel>(img));

  dlib::full_object_detection shape = landmarker(thing, rec);

  for (int k=0; k<shape.num_parts(); k++) {
    cv::Point2f landmark(shape.part(k).x(), shape.part(k).y());
    points.push_back(landmark);
  }

  return points;
}

// std::vector<cv::Rect>
std::vector<cv::Rect> FaceDetect::DetectFaces(cv::Mat &img)
{
  std::vector<cv::Rect> faces;
  faceDetector.detectMultiScale(img, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
  return faces;
}

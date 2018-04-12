#include <dlib/clustering.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/pixel.h>
#include <dlib/string.h>
#include <emscripten/bind.h>
#include <face-replace.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;         // OpenCV
using namespace emscripten; // WASM

/**
 * This class is made to process uint8 clamped arrays being passed from the browser.
 *
 * A uint8 clamped array generated from a canvas images is a two dimensional array
 * made up of by y height and x width.
 *
 * Each pixel comes in rgba ([red, green, blue, alpha]) format. Opencv's equivalent is
 * CV_8UC4 and dlib's is rgb_alpha_pixel.
 */

int main(int argc, char **argv) {
  std::cout << "Hello, World!" << std::endl;
  return 0;
}

// Apply affine transform calculated using srcTri and dstTri to src
static void applyAffineTransform(Mat &warpImage, Mat &src, std::vector<Point2f> &srcTri, std::vector<Point2f> &dstTri) {
  // Given a pair of triangles, find the affine transform.
  Mat warpMat = cv::getAffineTransform(srcTri, dstTri);

  // Apply the Affine Transform just found to the src image
  warpAffine(src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
}

// Calculate Delaunay triangles for set of points
// Returns the vector of indices of 3 points for each triangle
static void calcDelaunayTriangles(std::vector<Point2f> &points, std::vector<std::vector<int>> &delaunayTri) {
  cv::Rect rect = cv::boundingRect(points);
  cv::Subdiv2D subdiv(rect);

    // Insert points into subdiv
  for (int i = 0; i < points.size(); i++) {
    subdiv.insert(points[i]);
  }

  //  for (std::vector<cv::Point2f>::iterator it = points.begin(); it !=
  // points.end(); it++)
  //   subdiv.insert(*it);

  std::vector<cv::Point2f> pt(3);
  std::vector<int> ind(3);
  std::vector<cv::Vec6f> triangleList;
  subdiv.getTriangleList(triangleList);

  for (size_t i = 0; i < triangleList.size(); i++) {
    cv::Vec6f t = triangleList[i];
    pt[0] = cv::Point2f(t[0], t[1]);
    pt[1] = cv::Point2f(t[2], t[3]);
    pt[2] = cv::Point2f(t[4], t[5]);

    if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])) {
      for (int j = 0; j < 3; j++) {
        for (size_t k = 0; k < points.size(); k++) {
          if (abs(pt[j].x - points[k].x) < 1.0 && abs(pt[j].y - points[k].y) < 1) {
            ind[j] = k;
          }
        }
      }

      delaunayTri.push_back(ind);
    }
  }
}

// Warps and alpha blends triangular regions from img1 and img2 to img
void warpTriangle(Mat &img1, Mat &img2, std::vector<Point2f> &t1, std::vector<Point2f> &t2) {
  cv::Rect r1 = boundingRect(t1);
  cv::Rect r2 = boundingRect(t2);

  // Offset points by left top corner of the respective rectangles
  std::vector<Point2f> t1Rect, t2Rect;
  std::vector<Point> t2RectInt;

  for (int i = 0; i < 3; i++) {
    t1Rect.push_back(Point2f(t1[i].x - r1.x, t1[i].y - r1.y));
    t2Rect.push_back(Point2f(t2[i].x - r2.x, t2[i].y - r2.y));
    t2RectInt.push_back(Point(t2[i].x - r2.x, t2[i].y - r2.y)); // for fillConvexPoly
  }

  // Get mask by filling triangle
  Mat mask = Mat::zeros(r2.height, r2.width, CV_32FC3); // should this be CV_8UC4?
  fillConvexPoly(mask, t2RectInt, cv::Scalar(1.0, 1.0, 1.0), 16, 0);

  // Apply warpImage to small rectangular patches
  Mat img1Rect;
  img1(r1).copyTo(img1Rect);

  Mat img2Rect = Mat::zeros(r2.height, r2.width, img1Rect.type());

  applyAffineTransform(img2Rect, img1Rect, t1Rect, t2Rect);

  multiply(img2Rect, mask, img2Rect);
  multiply(img2(r2), cv::Scalar(1.0, 1.0, 1.0) - mask, img2(r2));
  img2(r2) = img2(r2) + img2Rect;
}

FaceReplace::FaceReplace(std::vector<uint8_t> baseImg, int width, int height) {
  dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> landmarker;
  srcImg = cv::Mat(height, width, CV_8UC4, &baseImg);
  srcPoints = detectFace(srcImg);

  // Convert to bgr
  // cv::Mat grey;
  // cv::cvtColor(srcImg, grey, COLOR_RGBA2GRAY);

  calcDelaunayTriangles(srcPoints, srcTri);
}


std::vector<uint8_t> FaceReplace::MapToFace(std::vector<uint8_t> src, int width, int height) {
  cv::Mat img(height, width, CV_8UC4, &src);
  std::vector<cv::Point2f> landmarks = detectFace(img);
  std::vector<std::vector<int>> delaunayTri;

  calcDelaunayTriangles(landmarks, delaunayTri);

  // Apply affine transformation to Delaunay triangles
  for (int j = 0; j < delaunayTri.size(); j++) {
    std::vector<cv::Point2f> t1, t2;

    // Get corresponding points
    for (int k = 0; k < 3; k++) {
      t1.push_back(srcPoints[srcTri[j][k]]);
      t2.push_back(landmarks[delaunayTri[j][k]]);
    }

    warpTriangle(srcImg, img, t1, t2);
  }

  return src;
}

std::vector<cv::Point2f> FaceReplace::detectFace(cv::Mat &img) {
  std::vector<cv::Point2f> points;
  dlib::rectangle rec(0,0,img.cols,img.rows);

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

// Debug
int FaceReplace::getFailingPoint() {
  Subdiv2D subdiv(cv::boundingRect(srcPoints));

  // Insert points into subdiv
  for (int i = 0; i < srcPoints.size(); i++) {
    try {
      subdiv.insert(srcPoints[i]);
    } catch(std::exception e) {
      return i;
    }
  }
  return 88;
}

int FaceReplace::getWidth() { return srcImg.cols; }
int FaceReplace::getHeight() { return srcImg.rows; }
int FaceReplace::getPointCount() { return srcPoints.size(); }
std::vector<cv::Point2f> FaceReplace::getPoints() { return srcPoints; }
float FaceReplace::getPoint(int p) { return srcPoints.at(p).x; }

std::string FaceReplace::tryTriangles() {
  try {
    this->detectFace(srcImg);
  } catch (std::exception &e) {
    return e.what();
  }
  return "all good";
}

// std::string dofuckingerror(std::vector<uint8_t> baseImg, int width, int height) {
//   cv::Mat srcImg;
//   cv::Mat testConv = cv::Mat(height, width, CV_8UC4, &baseImg);
//   cv::cvtColor(testConv, srcImg, cv::COLOR_RGBA2BGR);

//   try {
//     dlib::cv_image<dlib::bgr_pixel> cow(srcImg);
//   } catch (dlib::error e) {
//     return e.what();
//   }
//   return "all good";
// }

// std::vector<uint8_t> drawBox(std::vector<uint8_t> baseImg, int width, int height) {
//   cv::Mat srcImg(height, width, CV_8UC4, &baseImg);
//   cv::Point pt(10, 8);
//   cv::Point pt2(1000, 2222);
//   int thickness = 80;
//   int lineType = LINE_8;
//   cv::line(srcImg, pt, pt2, cv::Scalar(255, 255, 255), thickness, lineType);
//   return baseImg;
// }

// int FaceReplace::detectpoop(int k) {
//   dlib::cv_image<dlib::bgr_pixel> im(srcImg);
//   std::vector<dlib::rectangle> dets = detector(im);

//   dlib::full_object_detection shape = landmarker(im, dets[0]);
//   std::vector<std::vector<int>> landmarks;

//   return dets[0].bottom();
//   // // Append points and proceed with warping image
//   // for (int k = 0; k < shape.num_parts(); k++) {
//   //   std::vector<int> tpoint;
//   //   tpoint.push_back(shape.part(k).x());
//   //   tpoint.push_back(shape.part(k).y());
//   //   landmarks.push_back(tpoint);
//   // }

//   // return landmarks;
// }

EMSCRIPTEN_BINDINGS(c) {
  register_vector<uint8_t>("VectorInt");
  //  function("dofuckingerror", &dofuckingerror);
  //  function("drawBox", &drawBox);
  class_<FaceReplace>("FaceReplace")
    .constructor<std::vector<uint8_t>, int, int>()
    .function("MapToFace", &FaceReplace::MapToFace)
    //    .function("detectpoop", &FaceReplace::detectpoop)
    .function("tryTriangles", &FaceReplace::tryTriangles)
    .function("getPointCount", &FaceReplace::getPointCount)
    .function("getPoints", &FaceReplace::getPoints)
    .function("getPoint", &FaceReplace::getPoint)
    .function("getWidth", &FaceReplace::getWidth)
    .function("getHeight", &FaceReplace::getHeight)
    //    .function("showImg", &FaceReplace::showImg)
    .function("getFailingPoint", &FaceReplace::getFailingPoint);
}

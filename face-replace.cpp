#include <emscripten/bind.h>
#include <face-replace.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>

using namespace cv;         // OpenCV
using namespace emscripten; // WASM

/**
 * This class is made to process uint8 clamped arrays being passed from the browser.
 *
 * A uint8 clamped array generated from a canvas images is a two dimensional array
 * made up of by y height and x width.
 *
 * Each pixel comes in rgba ([red, green, blue, alpha]) format. Opencv's equivelent is
 * CV_8UC4 and dlib's is rgb_alpha_pixel.
 */

int main(int argc, char **argv) {
  std::cout << "Hello, World!" << std::endl;
  return 0;
}

// Apply affine transform calculated using srcTri and dstTri to src
void applyAffineTransform(Mat &warpImage, Mat &src, std::vector<Point2f> &srcTri, std::vector<Point2f> &dstTri) {
  // Given a pair of triangles, find the affine transform.
  Mat warpMat = cv::getAffineTransform(srcTri, dstTri);

  // Apply the Affine Transform just found to the src image
  warpAffine(src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
}

// Calculate Delaunay triangles for set of points
// Returns the vector of indices of 3 points for each triangle
static void calcDelaunayTriangles(std::vector<Point2f> &points, std::vector<std::vector<int>> &delaunayTri) {
  Subdiv2D subdiv(cv::boundingRect(points));

    // Insert points into subdiv
  for (int i = 0; i < points.size(); i++) {
    subdiv.insert(points[i]);
  }

  //  for (std::vector<cv::Point2f>::iterator it = points.begin(); it !=
  // points.end(); it++)
  //   subdiv.insert(*it);

  //  std::vector<cv::Vec6f> triangleList;
  // subdiv.getTriangleList(triangleList);
  // std::vector<cv::Point2f> pt(3);
  // std::vector<int> ind(3);

  // for (size_t i = 0; i < triangleList.size(); i++) {
  //   cv::Vec6f t = triangleList[i];
  //   pt[0] = cv::Point2f(t[0], t[1]);
  //   pt[1] = cv::Point2f(t[2], t[3]);
  //   pt[2] = cv::Point2f(t[4], t[5]);

  //   if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
  //   {
  //     for (int j = 0; j < 3; j++) {
  //       for (size_t k = 0; k < points.size(); k++) {
  //         if (abs(pt[j].x - points[k].x) < 1.0 && abs(pt[j].y - points[k].y)
  //         < 1) {
  //           ind[j] = k;
  //         }
  //       }
  //     }

  //     delaunayTri.push_back(ind);
  //   }
  // }
}

// Warps and alpha blends triangular regions from img1 and img2 to img
void warpTriangle(Mat &img1, Mat &img2, std::vector<Point2f> &t1, std::vector<Point2f> &t2) {
  Rect r1 = boundingRect(t1);
  Rect r2 = boundingRect(t2);

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

std::vector<cv::Rect> FaceReplace::detectFaces(cv::Mat &img) {
  Mat gray;
  std::vector<cv::Rect> faces;
  cvtColor(srcImg, gray, COLOR_RGBA2GRAY);
  detector.detectMultiScale(gray, faces);

  return faces;
}

FaceReplace::FaceReplace(std::vector<uint8_t> baseImg, int width, int height) {
  // Load Face Detector
  detector = CascadeClassifier("./haarcascade_frontalface_alt2.xml");

  // Load landmark detector
  // landmarker = FacemarkLBF::create();
  // landmarker->loadModel("lbfmodel.yaml");

  // Load image
  srcImg = cv::Mat(height, width, CV_8UC4, &baseImg);

  // Detect faces
  std::vector<cv::Rect> faces = detectFaces(srcImg);

  // Detect image
  // srcPoints = detectPoints(srcImg, cow[0]);

  // std::vector<std::vector<Point2f>> landmarks;
  // facemark->fit(frame,faces,landmarks);
  // srcPoints = landmarks[0];

  //  calcDelaunayTriangles(cv::boundingRect(srcPoints), srcPoints, srcTri);
}

int FaceReplace::getWidth() {
  return srcImg.cols;
}

int FaceReplace::getHeight() {
  return srcImg.rows;
}

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

int FaceReplace::getPointCount() { return srcPoints.size(); }
std::vector<Point2f> FaceReplace::getPoints() { return srcPoints; }
float FaceReplace::getPoint(int p) { return srcPoints[p].x; }

std::string FaceReplace::tryTriangles() {
  try {
    calcDelaunayTriangles(srcPoints, srcTri);
  } catch (cv::Exception &e) {
    return e.what();
  }
  return "all good";
}

// Things are breaking around here
// It's something going on here, we get extremely large numbers for point coordinates
// I think it might be point types and things getting improperly converted
std::vector<Point2f> FaceReplace::detectPoints(Mat &img, cv::Rect box) {
  std::vector<Point2f> todo;
  return todo;
}

std::vector<uint8_t> FaceReplace::MapToFace(std::vector<uint8_t> src, int width, int height) {
  cv::Mat img = cv::Mat(height, width, CV_8UC4, &src);

  // Detect faces
  std::vector<cv::Rect> dets = detectFaces(img);

  // std::vector<dlib::full_object_detection> shapes;

  // // Iterate over detected faces and apply face swap
  // for (unsigned long i = 0; i < dets.size(); ++i) {
  //   std::vector<std::vector<int>> delaunayTri;
  //   std::vector<cv::Point2f> landmarks = detectPoints(img, dets[i]);
  //   //    cv::Rect boundingBox = cv::boundingRect(landmarks);

  //   calcDelaunayTriangles(landmarks, delaunayTri);

  //   // Apply affine transformation to Delaunay triangles
  //   for (int j = 0; j < delaunayTri.size(); j++) {
  //     std::vector<cv::Point2f> t1, t2;

  //     // Get corresponding points
  //     for (int k = 0; k < 3; k++) {
  //       t1.push_back(srcPoints[srcTri[j][k]]);
  //       t2.push_back(landmarks[delaunayTri[j][k]]);
  //     }

  //     warpTriangle(srcImg, img, t1, t2);
  //   }
  // }

  return src;
}

EMSCRIPTEN_BINDINGS(c) {
  register_vector<uint8_t>("VectorInt");
  //  function("dofuckingerror", &dofuckingerror);
  //  function("drawBox", &drawBox);
  class_<FaceReplace>("FaceReplace")
      .constructor<std::vector<uint8_t>, int, int>()
      .function("MapToFace", &FaceReplace::MapToFace)
      .function("tryTriangles", &FaceReplace::tryTriangles)
      .function("getPointCount", &FaceReplace::getPointCount)
      .function("getPoints", &FaceReplace::getPoints)
      .function("getPoint", &FaceReplace::getPoint)
      .function("getWidth", &FaceReplace::getWidth)
      .function("getHeight", &FaceReplace::getHeight)
      .function("getFailingPoint", &FaceReplace::getFailingPoint);
}

#include <emscripten/bind.h>
#include <face-detect.hpp>
#include <face-replace.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;         // OpenCV
using namespace emscripten; // WASM

// Apply affine transform calculated using srcTri and dstTri to src
static void applyAffineTransform(Mat &warpImage, Mat &src, std::vector<Point2f> &srcTri, std::vector<Point2f> &dstTri)
{
  // Given a pair of triangles, find the affine transform.
  Mat warpMat = cv::getAffineTransform(srcTri, dstTri);
  cv::warpAffine(src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
}

// Calculate Delaunay triangles for set of points
// Returns the vector of indices of 3 points for each triangle
static void calcDelaunayTriangles(std::vector<Point2f> points, std::vector<std::vector<int>> &delaunayTri)
{
  cv::Rect rect = cv::boundingRect(points);
  cv::Subdiv2D subdiv(rect);

  // Insert points into subdiv
  for (std::vector<cv::Point2f>::iterator it = points.begin(); it != points.end(); it++) {
    subdiv.insert(*it);
  }

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
void warpTriangle(Mat &img1, Mat &img2, std::vector<Point2f> t1, std::vector<Point2f> t2)
{
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
  Mat mask = Mat::zeros(r2.height, r2.width, CV_8UC4); // should this be CV_8UC4?
  fillConvexPoly(mask, t2RectInt, cv::Scalar(1.0, 1.0, 1.0, 1.0), 16, 0);

  // Apply warpImage to small rectangular patches
  Mat img1Rect;
  img1(r1).copyTo(img1Rect);

  Mat img2Rect = Mat::zeros(r2.height, r2.width, CV_8UC4); // img1Rect.type());

  applyAffineTransform(img2Rect, img1Rect, t1Rect, t2Rect);

  multiply(img2Rect, mask, img2Rect);
  multiply(img2(r2), cv::Scalar(1.0, 1.0, 1.0, 1.0) - mask, img2(r2));
  img2(r2) = img2(r2) + img2Rect;
}

/**
 * This class is made to process uint8 clamped arrays being passed from the browser.
 *
 * A uint8 clamped array generated from a canvas images is a two dimensional array
 * made up of by y height and x width.
 *
 * Each pixel comes in rgba ([red, green, blue, alpha]) format. Opencv's equivalent is
 * CV_8UC4 and dlib's is rgb_alpha_pixel.
 */
FaceReplace::FaceReplace(FaceDetect faceDetector, std::vector<uint8_t> &baseImg, int width, int height)
{
  srcImg = cv::Mat(height, width, CV_8UC(4), baseImg.data());
  std::vector<cv::Rect> faces = faceDetector.DetectFaces(srcImg);
  srcPoints = faceDetector.DetectLandmarks(srcImg, faces[0]);
}

void FaceReplace::MapToFace(std::vector<uint8_t> &src, int width, int height)
{
  cv::Mat img(height, width, CV_8UC(4), src.data());
  std::vector<cv::Rect> faces = faceDetector.DetectFaces(img);
  std::vector<std::vector<int>> delaunayTri;

  if (faces.size() == 0) {
    return;
  }

  std::vector<cv::Point2f> landmarks = faceDetector.DetectLandmarks(img, faces[0]);
  calcDelaunayTriangles(landmarks, delaunayTri);

  // Apply affine transformation to Delaunay triangles
  for (int j = 0; j < delaunayTri.size(); j++) {
    std::vector<cv::Point2f> t1, t2;

    // Get corresponding points
    for (int k = 0; k < 3; k++) {
      t1.push_back(srcPoints[delaunayTri[j][k]]);
      t2.push_back(landmarks[delaunayTri[j][k]]);
    }

    warpTriangle(srcImg, img, t1, t2);
  }
}

// make a cv mat function
// having that will create a decent common language to share amongst functions
cv::Mat initMat(std::vector<uint8_t> &src, int width, int height)
{
  return cv::Mat(height, width, CV_8UC(4), src.data());
}

EMSCRIPTEN_BINDINGS(c) {
  class_<cv::Rect>("cvRect");
  class_<cv::Point>("cvPoint");
  class_<cv::Point2f>("cvPointf");
  class_<cv::Mat>("Mat");

  register_vector<cv::Rect>("vectRect");
  register_vector<uint8_t>("VectorInt");

  function("initMat", &initMat);

  class_<FaceDetect>("FaceDetect")
    .constructor()
    .constructor<bool>()
    .function("DetectFaces", &FaceDetect::DetectFaces)
    .function("DetectLandmarks", &FaceDetect::DetectLandmarks);

  class_<FaceReplace>("FaceReplace")
    .constructor<FaceDetect&, std::vector<uint8_t>&, int, int>()
    .function("MapToFace", &FaceReplace::MapToFace);
}

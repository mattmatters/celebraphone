#include <dlib/clustering.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <dlib/string.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

// Apply affine transform calculated using srcTri and dstTri to src
void applyAffineTransform(cv::Mat &warpImage, cv::Mat &src,
                          std::vector<cv::Point2f> &srcTri,
                          std::vector<cv::Point2f> &dstTri) {

  // Given a pair of triangles, find the affine transform.
  cv::Mat warpMat = cv::getAffineTransform(srcTri, dstTri);

  // Apply the Affine Transform just found to the src image
  warpAffine(src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR,
             cv::BORDER_REFLECT_101);
}

// Calculate Delaunay triangles for set of points
// Returns the vector of indices of 3 points for each triangle
static void calculateDelaunayTriangles(cv::Rect rect, std::vector<cv::Point2f> &points,
                                       std::vector<std::vector<int>> &delaunayTri) {

  // Create an instance of Subdiv2D
  cv::Subdiv2D subdiv(rect);

  // Insert points into subdiv
  for (std::vector<cv::Point2f>::iterator it = points.begin();
       it != points.end(); it++)
    subdiv.insert(*it);

  std::vector<cv::Vec6f> triangleList;
  subdiv.getTriangleList(triangleList);
  std::vector<cv::Point2f> pt(3);
  std::vector<int> ind(3);

  for (size_t i = 0; i < triangleList.size(); i++) {
    cv::Vec6f t = triangleList[i];
    pt[0] = cv::Point2f(t[0], t[1]);
    pt[1] = cv::Point2f(t[2], t[3]);
    pt[2] = cv::Point2f(t[4], t[5]);

    if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])) {
      for (int j = 0; j < 3; j++)
        for (size_t k = 0; k < points.size(); k++)
          if (abs(pt[j].x - points[k].x) < 1.0 &&
              abs(pt[j].y - points[k].y) < 1)
            ind[j] = k;

      delaunayTri.push_back(ind);
    }
  }
}

// Warps and alpha blends triangular regions from img1 and img2 to img
void warpTriangle(cv::Mat &img1, cv::Mat &img2, std::vector<cv::Point2f> &t1,
                  std::vector<cv::Point2f> &t2) {

  cv::Rect r1 = boundingRect(t1);
  cv::Rect r2 = boundingRect(t2);

  // Offset points by left top corner of the respective rectangles
  std::vector<cv::Point2f> t1Rect, t2Rect;
  std::vector<cv::Point> t2RectInt;

  for (int i = 0; i < 3; i++) {
    t1Rect.push_back(cv::Point2f(t1[i].x - r1.x, t1[i].y - r1.y));
    t2Rect.push_back(cv::Point2f(t2[i].x - r2.x, t2[i].y - r2.y));
    t2RectInt.push_back(
        cv::Point(t2[i].x - r2.x, t2[i].y - r2.y)); // for fillConvexPoly
  }

  // Get mask by filling triangle
  cv::Mat mask = cv::Mat::zeros(r2.height, r2.width, CV_32FC3);
  cv::fillConvexPoly(mask, t2RectInt, cv::Scalar(1.0, 1.0, 1.0), 16, 0);

  // Apply warpImage to small rectangular patches
  cv::Mat img1Rect;
  img1(r1).copyTo(img1Rect);

  cv::Mat img2Rect = cv::Mat::zeros(r2.height, r2.width, img1Rect.type());

  applyAffineTransform(img2Rect, img1Rect, t1Rect, t2Rect);

  multiply(img2Rect, mask, img2Rect);
  multiply(img2(r2), cv::Scalar(1.0, 1.0, 1.0) - mask, img2(r2));
  img2(r2) = img2(r2) + img2Rect;
}

int main(int argc, char **argv) {
  std::cout << "Hello, World!" << std::endl;
  return 0;
}

class FaceReplace {
  dlib::frontal_face_detector detector;
  dlib::shape_predictor landmarker;
  std::vector<cv::Point2f> srcPoints;
  cv::Mat srcImg;
  std::vector<std::vector<int>> srcTri;

private:
  std::vector<cv::Point2f> detectPoints(cv::Mat &img, dlib::rectangle box) {
    dlib::full_object_detection shape = landmarker(dlib::cv_image<dlib::bgr_pixel>(img), box);
    std::vector<cv::Point2f> landmarks;

    // Append points and proceed with warping image
    for (int k=0; k<shape.num_parts(); k++) {
      landmarks.push_back(cv::Point2f(shape.part(k).x(), shape.part(k).y()));
    }

    return landmarks;
  }

public:
  FaceReplace(std::vector<uint> baseImg, int width, int height) {
      this->detector = dlib::get_frontal_face_detector();
      this->srcImg = cv::Mat(height, width, CV_8UC4, &baseImg);

      dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> this->landmarker;

      this->srcPoints = this->detectPoints(this->srcImg, this->detector(dlib::cv_image<dlib::bgr_pixel>(this->srcImg))[0]);
      calculateDelaunayTriangles(cv::boundingRect(this->srcPoints), this->srcPoints, this->srcTri);
  }

  cv::Mat MapToFace(std::vector<uint> src, int width, int height) {
    cv::Mat img = cv::Mat(height, width, CV_8UC4, &src);

    // Detect faces
    std::vector<dlib::rectangle> dets = detector(dlib::cv_image<dlib::bgr_pixel>(img));
    std::vector<dlib::full_object_detection> shapes;

    // Iterate over detected faces and appy face swap
    for (unsigned long i = 0; i < dets.size(); ++i) {
      std::vector<std::vector<int>> delaunayTri;
      std::vector<cv::Point2f> landmarks = this->detectPoints(img, dets[i]);
      cv::Rect boundingBox = cv::boundingRect(landmarks);

      calculateDelaunayTriangles(boundingBox, landmarks, delaunayTri);

      // Apply affine transformation to Delaunay triangles
      for(int j = 0; j < delaunayTri.size(); j++) {
        std::vector<cv::Point2f> t1, t2;

        // Get corresponding points
        for(int k = 0; k < 3; k++) {
          t1.push_back(srcPoints[srcTri[j][k]]);
          t2.push_back(landmarks[delaunayTri[j][k]]);
        }

        warpTriangle(srcImg, img, t1, t2);
      }
    }

    return img;
  }
};

// FaceReplace::FaceReplace(std::vector<uint>, widthcv::Mat src) {
//   detector = dlib::get_frontal_face_detector();
//   dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> landmarker;

//   // Initialize source image
//   srcImg = src;
//   srcPoints = this->detectPoints(src, detector(dlib::cv_image<dlib::bgr_pixel>(src))[0]);
//   calculateDelaunayTriangles(cv::boundingRect(srcPoints), srcPoints, srcTri);
// }

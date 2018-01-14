#include <dlib/clustering.h>
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/string.h>
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
  std::cout << "Hello WASM!" << std::endl;
  dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
  dlib::shape_predictor pose_model;
  dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

  cv::Mat im;
  std::cout << "Hello, World!" << std::endl;
  return 0;
}

class FaceReplace {
  dlib::frontal_face_detector detector;
  dlib::shape_predictor landmarker;
  std::vector<cv::Point2f> srcPoints;
  cv::Mat srcImg;
public:
  FaceReplace();
  std::vector<T> MapToFace(std::vector<T> buffer) {
    cv::Mat img = cv::imdecode(buffer);
    std::vector<rectangle> dets = FaceReplace::detector(img);
    std::vector<dlib::full_object_detection> shapes;

    for (unsigned long j = 0; j < dets.size(); ++j) {
      dlib::full_object_detection shape = sp(img, dets[j]);
      std::cout << "number of parts: " << shape.num_parts() << std::endl;
      std::cout << "pixel position of first part:  " << shape.part(0)
                << std::endl;
      std::cout << "pixel position of second part: " << shape.part(1)
                << std::endl;

      // You get the idea, you can get all the face part locations if
      // you want them.  Here we just store them in shapes so we can
      // put them on the screen.
      shapes.push_back(shape);
    }
  }
}

FaceReplace::FaceReplace() {
  detector = dlib::get_frontal_face_detector();
  dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> landmarker;
}

// Apply affine transform calculated using srcTri and dstTri to src
void applyAffineTransform(cv::Mat &warpImage, Mat &src, std::vector<cv::Point2f> &srcTri,
                          std::vector<Point2f> &dstTri) {
  // Given a pair of triangles, find the affine transform.
  cv::Mat warpMat = getAffineTransform(srcTri, dstTri);

  // Apply the Affine Transform just found to the src image
  warpAffine(src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR,
             BORDER_REFLECT_101);
}

// Calculate Delaunay triangles for set of points
// Returns the vector of indices of 3 points for each triangle
static void calculateDelaunayTriangles(cv::Rect rect, std::vector<cv::Point2f> &points,
                                       std::vector<std::vector<int>> &delaunayTri) {

  // Create an instance of Subdiv2D
  cv::Subdiv2D subdiv(rect);

  // Insert points into subdiv
  for (std::vector<cv::Point2f>::iterator it = points.begin(); it != points.end();
       it++)
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

    t1Rect.push_back(Point2f(t1[i].x - r1.x, t1[i].y - r1.y));
    t2Rect.push_back(Point2f(t2[i].x - r2.x, t2[i].y - r2.y));
    t2RectInt.push_back(
        Point(t2[i].x - r2.x, t2[i].y - r2.y)); // for fillConvexPoly
  }

  // Get mask by filling triangle
  cv::Mat mask = cv::Mat::zeros(r2.height, r2.width, cv::CV_32FC3);
  cv::fillConvexPoly(mask, t2RectInt, cv::Scalar(1.0, 1.0, 1.0), 16, 0);

  // Apply warpImage to small rectangular patches
  cv::Mat img1Rect;
  img1(r1).copyTo(img1Rect);

  cv::Mat img2Rect = cv::Mat::zeros(r2.height, r2.width, img1Rect.type());

  applyAffineTransform(img2Rect, img1Rect, t1Rect, t2Rect);

  multiply(img2Rect, mask, img2Rect);
  multiply(img2(r2), Scalar(1.0, 1.0, 1.0) - mask, img2(r2));
  img2(r2) = img2(r2) + img2Rect;
}

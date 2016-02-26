#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/** @function main */
int main(int argc, char** argv) {
  Mat src, src_gray, dst;
  int kernel_size = 3;
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  char* window_name = "Laplace Demo";

  int c;

  /// Load an image
  //src = imread(argv[1]);
  VideoCapture vid(argv[1]);

  //if (!src.data) {
  //  return -1;
  //}

  while (true) {
  Mat image;
  if (!vid.read(image)) {
    waitKey(0);
    return 0;
  }
  /// Remove noise by blurring with a Gaussian filter
  //GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

  /// Convert the image to grayscale
  //cvtColor(src, src_gray, CV_BGR2GRAY);

  Mat hsv_src;
  cvtColor(image, hsv_src, COLOR_BGR2HSV);

  std::vector<cv::Mat> channels;
  cv::split(hsv_src, channels);

  cv::Mat H = channels[0];
  cv::Mat S = channels[1];
  cv::Mat V = channels[2];
  //imshow("Hue", H);
  //imshow("Saturation", S);
  //imshow("Value", V);

//  inRange(src, cv::Scalar(0, 0, 150), cv::Scalar(255, 255, 255), src);
//  imshow("thresholds!", src);
//  waitKey(0);

  // Set up the detector with default parameters.
  SimpleBlobDetector::Params params;
  params.filterByColor = false;
  //params.blobColor = 255;
  params.filterByArea = true;
  params.maxArea = 5000;
  params.minArea = 5;
  params.filterByConvexity = true;
  params.minConvexity = 0.5;
  params.maxConvexity = 1.0;
  params.filterByInertia = false;
  params.filterByCircularity = true;
  params.maxCircularity = 1.0;
  params.minCircularity = 0.6;
  SimpleBlobDetector detector(params);

  // Detect blobs.
  Mat blob_detect = S;
  std::vector<KeyPoint> keypoints;
  detector.detect(blob_detect, keypoints);

  // Draw detected blobs as red circles.
  // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
  Mat im_with_keypoints;
  drawKeypoints(blob_detect, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

  // Show blobs
  imshow("keypoints", im_with_keypoints );
  waitKey(1);

  // Set up the detector with default parameters.

  /// Create window
  //namedWindow(window_name, CV_WINDOW_AUTOSIZE);
  continue;

  /// Apply Laplace function
  Mat abs_dst;

  Laplacian(S, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
  convertScaleAbs(dst, abs_dst);

  /// Show what you got
  //imshow(window_name, abs_dst);

  vector<Vec3f> circles;
  HoughCircles(S, circles, CV_HOUGH_GRADIENT, 1, S.rows / 8, 200, 100, 0,
               0);

  /// Draw the circles detected
  for (size_t i = 0; i < circles.size(); i++) {
    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
    // circle center
    circle(S, center, 3, Scalar(0, 255, 0), -1, 8, 0);
    // circle outline
    circle(S, center, radius, Scalar(0, 0, 255), 3, 8, 0);
  }

  /// Show your results
  //namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
  //imshow("Hough Circle Transform Demo", S);

  //waitKey(0);
  }

  return 0;
}

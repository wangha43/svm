#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>
using namespace std;
using namespace cv;
using namespace cv::ml;


int main()
{    
    Ptr<ml::SVM> svm = ml::SVM::create();
    svm->setCoef0(0.0);
        svm->setDegree(3);
        svm->setTermCriteria(TermCriteria( CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000, 1e-3 ));
        svm->setGamma(0);
        svm->setKernel(SVM::LINEAR);
        svm->setNu(0.5);
        svm->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
        svm->setC(0.01); // From paper, soft classifier
        svm->setType(SVM::EPS_SVR);
        const DecisionFunc
    Mat trainData;
    Mat lables;
    Ptr<TrainData> td = TrainData::create(trainingDataMat, ROW_SAMPLE, labelsMat);
        svmOld->train(td);
        //same svm
        svmOld->save("trainedSVM.xml");


        //Initialize SVM object
        Ptr<SVM> svmNew = SVM::create();
        //Load Previously saved SVM from XML

        svmNew = SVM::load<SVM>("trainedSVM.xml");



        Vec3b green(0, 255, 0), blue(255, 0, 0);
        // Show the decision regions given by the SVM
        for (int i = 0; i < image.rows; ++i)
            for (int j = 0; j < image.cols; ++j)
            {
                Mat sampleMat = (Mat_<float>(1, 2) << j, i);
                float response = svmNew->predict(sampleMat);

                if (response == 1)
                    image.at<Vec3b>(i, j) = green;
                else if (response == -1)
                    image.at<Vec3b>(i, j) = blue;
            }

        // Show the training data
        int thickness = -1;
        int lineType = 8;
        circle(image, Point(501, 10), 5, Scalar(0, 0, 0), thickness, lineType);
        circle(image, Point(255, 10), 5, Scalar(255, 255, 255), thickness, lineType);
        circle(image, Point(501, 255), 5, Scalar(255, 255, 255), thickness, lineType);
        circle(image, Point(10, 501), 5, Scalar(255, 255, 255), thickness, lineType);

        // Show support vectors
        thickness = 2;
        lineType = 8;
        Mat sv = svmNew->getSupportVectors();

        for (int i = 0; i < sv.rows; ++i)
        {
            const float* v = sv.ptr<float>(i);
            circle(image, Point((int)v[0], (int)v[1]), 6, Scalar(128, 128, 128), thickness, lineType);
        }

        imwrite("result.png", image);        // save the image

        imshow("SVM Simple Example", image); // show it to the user
        waitKey(0);
        return(0);
    }
}

// svm.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <fstream>  
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


#define PosSamNO 2416    //����������  
#define NegSamNO 453    //����������  
  
#define TRAIN false    //�Ƿ����ѵ��,true��ʾ����ѵ����false��ʾ��ȡxml�ļ��е�SVMģ��  
#define CENTRAL_CROP true   //true:ѵ��ʱ����96*160��INRIA������ͼƬ���ó��м��64*128��С����  
  
//HardExample�����������������HardExampleNO����0����ʾ�������ʼ���������󣬼�������HardExample����������  
//��ʹ��HardExampleʱ��������Ϊ0����Ϊ������������������������ά����ʼ��ʱ�õ����ֵ  
#define HardExampleNO 0   


class MySVM : public CvSVM  
{  
public:  
    //���SVM�ľ��ߺ����е�alpha����  
    double * get_alpha_vector()  
    {  
        return this->decision_func->alpha;  
    }  
  
    //���SVM�ľ��ߺ����е�rho����,��ƫ����  
    float get_rho()  
    {  
        return this->decision_func->rho;  
    }  
};  

int main()
{
	HOGDescriptor hog(Size(64,128),Size(16,16),Size(8,8),Size(8,8),9);
    int DescriptorDim;
    MySVM svm;
	 string ImgName;//ͼƬ��(����·��)  
    ifstream finPos("C:\\QMDownload\\INRIAPerson\\96X160H96\\Train\\posi.txt");

    ifstream finNeg("C:\\QMDownload\\INRIAPerson\\Test\\nega.txt");

	Mat sampleFeatureMat;//����ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��      
    Mat sampleLabelMat;

	if(TRAIN)  
    {  
	//���ζ�ȡ������ͼƬ������HOG������  
        for(int num=0; num<PosSamNO && getline(finPos,ImgName); num++)  
        {  
            cout<<"������"<<ImgName<<endl;  
            //ImgName = "D:\\DataSet\\PersonFromVOC2012\\" + ImgName;//������������·����  
            ImgName = "C:\\QMDownload\\INRIAPerson\\96X160H96\\Train\\pos\\" + ImgName;//������������·����  
            Mat src = imread(ImgName);//��ȡͼƬ  
            if(CENTRAL_CROP)  
                src = src(Rect(16,16,64,128));//��96*160��INRIA������ͼƬ����Ϊ64*128������ȥ�������Ҹ�16������  
            //resize(src,src,Size(64,128));  
  
            vector<float> descriptors;//HOG����������  
            hog.compute(src,descriptors,Size(8,8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)  
            //cout<<"������ά����"<<descriptors.size()<<endl;  
  
            //������һ������ʱ��ʼ�����������������������Ϊֻ��֪��������������ά�����ܳ�ʼ��������������  
            if( 0 == num )  
            {  
                DescriptorDim = descriptors.size();//HOG�����ӵ�ά��  
                //��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��sampleFeatureMat  
                sampleFeatureMat = Mat::zeros(PosSamNO+NegSamNO+HardExampleNO, DescriptorDim, CV_32FC1);  
                //��ʼ��ѵ����������������������������������ĸ�������������1��1��ʾ���ˣ�0��ʾ����  
                sampleLabelMat = Mat::zeros(PosSamNO+NegSamNO+HardExampleNO, 1, CV_32FC1);  
            }  
  
            //������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
            for(int i=0; i<DescriptorDim; i++)  
                sampleFeatureMat.at<float>(num,i) = descriptors[i];//��num�����������������еĵ�i��Ԫ��  
            sampleLabelMat.at<float>(num,0) = 1;//���������Ϊ1������  
        }  
  
        //���ζ�ȡ������ͼƬ������HOG������  
        for(int num=0; num<NegSamNO && getline(finNeg,ImgName); num++)  
        {  
            cout<<"������"<<ImgName<<endl;  
            ImgName = "C:\\QMDownload\\INRIAPerson\\Test\\neg\\" + ImgName;//���ϸ�������·����  
            Mat src = imread(ImgName);//��ȡͼƬ  
            //resize(src,img,Size(64,128));  
  
            vector<float> descriptors;//HOG����������  
            hog.compute(src,descriptors,Size(8,8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)  
            //cout<<"������ά����"<<descriptors.size()<<endl;  
  
            //������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
            for(int i=0; i<DescriptorDim; i++)  
                sampleFeatureMat.at<float>(num+PosSamNO,i) = descriptors[i];//��PosSamNO+num�����������������еĵ�i��Ԫ��  
            sampleLabelMat.at<float>(num+PosSamNO,0) = -1;//���������Ϊ-1������  
        }  
  
        //����HardExample������  
        if(HardExampleNO > 0)  
        {  
            ifstream finHardExample("HardExample_2400PosINRIA_12000NegList.txt");//HardExample���������ļ����б�  
            //���ζ�ȡHardExample������ͼƬ������HOG������  
            for(int num=0; num<HardExampleNO && getline(finHardExample,ImgName); num++)  
            {  
                cout<<"������"<<ImgName<<endl;  
                ImgName = "D:\\DataSet\\HardExample_2400PosINRIA_12000Neg\\" + ImgName;//����HardExample��������·����  
                Mat src = imread(ImgName);//��ȡͼƬ  
                //resize(src,img,Size(64,128));  
  
                vector<float> descriptors;//HOG����������  
                hog.compute(src,descriptors,Size(8,8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)  
                //cout<<"������ά����"<<descriptors.size()<<endl;  
  
                //������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
                for(int i=0; i<DescriptorDim; i++)  
                    sampleFeatureMat.at<float>(num+PosSamNO+NegSamNO,i) = descriptors[i];//��PosSamNO+num�����������������еĵ�i��Ԫ��  
                sampleLabelMat.at<float>(num+PosSamNO+NegSamNO,0) = -1;//���������Ϊ-1������  
            }  
        }  
		CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000, FLT_EPSILON);  
        //SVM������SVM����ΪC_SVC�����Ժ˺������ɳ�����C=0.01  
        CvSVMParams param(CvSVM::C_SVC, CvSVM::LINEAR, 0, 1, 0, 0.01, 0, 0, 0, criteria);  
        cout<<"��ʼѵ��SVM������"<<endl;  
        svm.train(sampleFeatureMat, sampleLabelMat, Mat(), Mat(), param);//ѵ��������  
        cout<<"ѵ�����"<<endl;  
        svm.save("SVM_HOG.xml");//��ѵ���õ�SVMģ�ͱ���Ϊxml�ļ� 
	
	}  
		else //��TRAINΪfalse����XML�ļ���ȡѵ���õķ�����  
		{  
			svm.load("C:\\Users\\wxc\\Documents\\Visual Studio 2010\\Projects\\svmp\\svmp\\SVM_HOG.xml");//��XML�ļ���ȡѵ���õ�SVMģ��  
		}  

	/********************************************************************************************************************/
	DescriptorDim = svm.get_var_count();//����������ά������HOG�����ӵ�ά��  
    int supportVectorNum = svm.get_support_vector_count();//֧�������ĸ���  
    cout<<"֧������������"<<supportVectorNum<<endl;  
  
    Mat alphaMat = Mat::zeros(1, supportVectorNum, CV_32FC1);//alpha���������ȵ���֧����������  
    Mat supportVectorMat = Mat::zeros(supportVectorNum, DescriptorDim, CV_32FC1);//֧����������  
    Mat resultMat = Mat::zeros(1, DescriptorDim, CV_32FC1);//alpha��������֧����������Ľ��  
  
    //��֧�����������ݸ��Ƶ�supportVectorMat������  
    for(int i=0; i<supportVectorNum; i++)  
    {  
        const float * pSVData = svm.get_support_vector(i);//���ص�i��֧������������ָ��  
        for(int j=0; j<DescriptorDim; j++)  
        {  
            //cout<<pData[j]<<" ";  
            supportVectorMat.at<float>(i,j) = pSVData[j];  
        }  
    }  
  
    //��alpha���������ݸ��Ƶ�alphaMat��  
    double * pAlphaData = svm.get_alpha_vector();//����SVM�ľ��ߺ����е�alpha����  
    for(int i=0; i<supportVectorNum; i++)  
    {  
        alphaMat.at<float>(0,i) = pAlphaData[i];  
    }  
  
    //����-(alphaMat * supportVectorMat),����ŵ�resultMat��  
    //gemm(alphaMat, supportVectorMat, -1, 0, 1, resultMat);//��֪��Ϊʲô�Ӹ��ţ�  
    resultMat = -1 * alphaMat * supportVectorMat;  
  
    //�õ����յ�setSVMDetector(const vector<float>& detector)�����п��õļ����  
    vector<float> myDetector;  
    //��resultMat�е����ݸ��Ƶ�����myDetector��  
    for(int i=0; i<DescriptorDim; i++)  
    {  
        myDetector.push_back(resultMat.at<float>(0,i));  
    }  
    //�������ƫ����rho���õ������  
    myDetector.push_back(svm.get_rho());  
    cout<<"�����ά����"<<myDetector.size()<<endl;  
    //����HOGDescriptor�ļ����  
    HOGDescriptor myHOG;  
    myHOG.setSVMDetector(myDetector);  
    //myHOG.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());  
  
    //�������Ӳ������ļ�  
    ofstream fout("HOGDetectorForOpenCV.txt");  
    for(int i=0; i<myDetector.size(); i++)  
    {  
        fout<<myDetector[i]<<endl;  
    }  

	 /**************����ͼƬ����HOG���˼��******************/  
    //Mat src = imread("00000.jpg");  
    //Mat src = imread("2007_000423.jpg");  
    Mat src = imread("C:\\QMDownload\\INRIAPerson\\70X134H96\\Test\\pos\\crop_000001a.png");  
    vector<Rect> found, found_filtered;//���ο�����  
    cout<<"���ж�߶�HOG������"<<endl;  
    myHOG.detectMultiScale(src, found, 0, Size(8,8), Size(32,32), 1.05, 2);//��ͼƬ���ж�߶����˼��  
    cout<<"�ҵ��ľ��ο������"<<found.size()<<endl;  
  
    //�ҳ�����û��Ƕ�׵ľ��ο�r,������found_filtered��,�����Ƕ�׵Ļ�,��ȡ���������Ǹ����ο����found_filtered��  
    for(int i=0; i < found.size(); i++)  
    {  
        Rect r = found[i];  
        int j=0;  
        for(; j < found.size(); j++)  
            if(j != i && (r & found[j]) == r)  
                break;  
        if( j == found.size())  
            found_filtered.push_back(r);  
    }  
  
    //�����ο���Ϊhog�����ľ��ο��ʵ�������Ҫ��΢��Щ,����������Ҫ��һЩ����  
    for(int i=0; i<found_filtered.size(); i++)  
    {  
        Rect r = found_filtered[i];  
        r.x += cvRound(r.width*0.1);  
        r.width = cvRound(r.width*0.8);  
        r.y += cvRound(r.height*0.07);  
        r.height = cvRound(r.height*0.8);  
        rectangle(src, r.tl(), r.br(), Scalar(0,255,0), 3);  
    }  
  
    imwrite("ImgProcessed.jpg",src);  
    namedWindow("src",0);  
    imshow("src",src);  
    waitKey();
      
  
    /******************���뵥��64*128�Ĳ���ͼ������HOG�����ӽ��з���*********************/  
    ////��ȡ����ͼƬ(64*128��С)����������HOG������  
    ////Mat testImg = imread("person014142.jpg");  
    //Mat testImg = imread("noperson000026.jpg");  
    //vector<float> descriptor;  
    //hog.compute(testImg,descriptor,Size(8,8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)  
    //Mat testFeatureMat = Mat::zeros(1,3780,CV_32FC1);//����������������������  
    ////������õ�HOG�����Ӹ��Ƶ�testFeatureMat������  
    //for(int i=0; i<descriptor.size(); i++)  
    //  testFeatureMat.at<float>(0,i) = descriptor[i];  
  
    ////��ѵ���õ�SVM�������Բ���ͼƬ�������������з���  
    //int result = svm.predict(testFeatureMat);//�������  
    //cout<<"��������"<<result<<endl;  		

	system("pause"); 
	return 0;
}

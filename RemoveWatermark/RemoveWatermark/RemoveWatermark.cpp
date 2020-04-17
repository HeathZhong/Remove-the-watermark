#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void TemplateMatch(Mat &InputImage, Mat &TempImage);
void SplitRGB2SingleGray(Mat &Image, Mat &OutImage);
void Filter_G_Channel(Mat MainImage, Mat AssistantImage, Mat &OutImage);

int main()
{
	Mat SrcImage, GrayImage, MaskImage;
	SrcImage = imread("origin2.png");
	if (SrcImage.data == nullptr)
	{
		cout << "Image load error ...\n" << endl;
		return -1;
	}
	/*************处理模板************/
// 	Mat ProcessImage;
// 	SplitRGB2SingleGray(SrcImage, ProcessImage);
 	/****************END**************/
// 	cvtColor(ProcessImage, GrayImage, COLOR_BGR2GRAY);
 	//mask
// 	inRange(GrayImage, Scalar(200, 200, 200), Scalar(230, 230, 230), MaskImage);
//	imwrite("MaskImage.jpg", MaskImage);
	cvtColor(SrcImage, GrayImage, COLOR_BGR2GRAY);
	MaskImage = imread("MaskImage.jpg",COLOR_BGR2GRAY);
	resize(MaskImage, MaskImage, Size(GrayImage.cols, GrayImage.rows));
	Mat MorImage = MaskImage.clone();
	//erode(MorImage, MorImage, Mat());
	//dilate(MorImage, MorImage, Mat(), Point(-1, -1), 1);
	imshow("mask", MorImage);
	//TemplateMatch(GrayImage, MorImage);

	// 修复
	Mat RepairImage;
	inpaint(SrcImage, MorImage, RepairImage, 3, INPAINT_TELEA);
	//imshow
	imshow("SrcImage", SrcImage);
	imshow("RepairImage", RepairImage);
	waitKey(0);
	return 0;
}

void SplitRGB2SingleGray(Mat &Image, Mat &OutImage)
{
	vector<Mat>RGB_Channels(3);
	split(Image, RGB_Channels);
	for (int row = 0; row < Image.rows; row++)
	{
		for (int col = 0; col < Image.cols; col++)
			if (RGB_Channels[0].at<uchar>(row, col) - RGB_Channels[1].at<uchar>(row, col) > 20 || 
				RGB_Channels[2].at<uchar>(row, col) - RGB_Channels[1].at<uchar>(row, col) > 20)
			{
				RGB_Channels[0].at<uchar>(row, col) = 255;
				RGB_Channels[1].at<uchar>(row, col) = 255;
				RGB_Channels[2].at<uchar>(row, col) = 255;
			}
	};
	merge(RGB_Channels, OutImage);
}

//过滤掉红色细胞中绿色部分干扰
void Filter_G_Channel(Mat MainImage, Mat AssistantImage, Mat &OutImage)
{
	for (int row = 0; row < MainImage.rows; row++)
	{
		for (int col = 0; col < MainImage.cols; col++)
		{
			if (MainImage.at<uchar>(row, col) - AssistantImage.at<uchar>(row, col) > 0)
				OutImage.at<uchar>(row, col) = MainImage.at<uchar>(row, col);
			else
				OutImage.at<uchar>(row, col) = 0;
		}
	}
}

//模板匹配
void TemplateMatch(Mat &InputImage, Mat &TempImage)
{
	Mat ResultImage;
	matchTemplate(InputImage, TempImage, ResultImage, TM_CCOEFF_NORMED);
	normalize(ResultImage, ResultImage, -1, 1, NORM_MINMAX);
	double MinVal = 0, MaxVal = 0;
	Point MinLoc(0, 0), MaxLoc(0, 0);
	minMaxLoc(ResultImage, &MinVal, &MaxVal, &MinLoc, &MaxLoc);
	circle(InputImage, Point(MaxLoc.x + TempImage.cols / 2, MaxLoc.y + TempImage.rows / 2), 30, Scalar(0, 0, 255), 2, 8);

	imshow("src", InputImage);
	imshow("Template", TempImage);
}